#include "InitPredictor.hpp"
#include "State.hpp"
#include "TicTacToeState.hpp"

#include <ATen/core/TensorBody.h>
#include <ATen/ops/_shape_as_tensor.h>
#include <ATen/ops/flatten.h>
#include <ATen/ops/hstack.h>
#include <ATen/ops/index_select.h>
#include <ATen/ops/randint.h>
#include <ATen/ops/relu.h>
#include <ATen/ops/softmax.h>
#include <ATen/ops/zeros.h>
#include <c10/core/TensorOptions.h>
#include <string>
#include <torch/data.h>
#include <torch/data/datasets/tensor.h>
#include <torch/enum.h>
#include <torch/nn/module.h>
#include <torch/nn/modules/batchnorm.h>
#include <torch/nn/modules/container/sequential.h>
#include <torch/nn/modules/conv.h>
#include <torch/nn/modules/linear.h>
#include <torch/nn/options/conv.h>
#include <torch/optim/sgd.h>
#include <torch/torch.h>
#include <torch/types.h>

namespace nn = torch::nn;
//namespace F = torch::nn::functional;

struct ResBlock : nn::Module {
    ResBlock(int channels){
        conv1 = register_module("conv1",
                                nn::Conv2d(nn::Conv2dOptions(channels, channels, 3)
                                                 .bias(false)
                                                 .stride(1)
                                                 .padding(torch::kSame)
                                          )
                                );
        conv2 = register_module("conv2",
                                nn::Conv2d(nn::Conv2dOptions(channels, channels, 3)
                                                 .bias(false)
                                                 .stride(1)
                                                 .padding(torch::kSame)
                                          )
                                );
        
        bn1 = register_module("bn1",
                              nn::BatchNorm2d(channels)
                             );
        bn2 = register_module("bn2",
                              nn::BatchNorm2d(channels)
                             );
    }
    torch::Tensor forward(torch::Tensor x){
        // Use one of many tensor manipulation functions.
        torch::Tensor res;
        res = conv1->forward(x);
        res = torch::relu(bn1->forward(res));
        res = conv2->forward(res);
        res = bn2->forward(res);

        res += x;
        res = torch::relu(res);
        return res;
    }
    nn::Conv2d conv1{nullptr}, conv2{nullptr};
    nn::BatchNorm2d bn1{nullptr}, bn2{nullptr};
};

struct GameNet : nn::Module{
    //expected input shape (*, init_channels, boardshape1, boardshape2)
    //output shape (N, out_policy_shape+1)
    GameNet(int init_channels, int boardshape1, int boardshape2, int mid_channels, int nb_blocks, int out_policy_shape){
        //Main layers
        conv1 = register_module("conv1",
                                nn::Conv2d(nn::Conv2dOptions(init_channels, mid_channels, 3)
                                                 .bias(false)
                                                 .stride(1)
                                                 .padding(torch::kSame)
                                          )
                                );
        bn1 = register_module("bn1",
                              nn::BatchNorm2d(mid_channels)
                             );
        res_blocks = register_module("res_blocks", nn::Sequential());
        for(int i = 0; i<nb_blocks; ++i)
            res_blocks->push_back(std::string("block") + std::to_string(i),
                                  ResBlock(mid_channels) 
                                 );
        //Policy head
        policy_conv = register_module("policy_conv",
                                      nn::Conv2d(nn::Conv2dOptions(mid_channels, mid_channels, 1)
                                                 .bias(false)
                                                 .stride(1)
                                                 .padding(torch::kSame)
                                      )
                                     );
        policy_bn = register_module("policy_bn",
                              nn::BatchNorm2d(mid_channels)
                             );
        policy_fc = register_module("policy_fc",
                                    nn::Linear(mid_channels*boardshape1*boardshape2, out_policy_shape)
                                   );
        //Value head
        value_conv = register_module("value_conv",
                                      nn::Conv2d(nn::Conv2dOptions(mid_channels, mid_channels, 1)
                                                 .bias(false)
                                                 .stride(1)
                                                 .padding(torch::kSame)
                                      )
                                     );
        value_bn = register_module("value_bn",
                              nn::BatchNorm2d(mid_channels)
                             );
        //value to add as param
        int mid_shape_fc = 64;
        value_fc1 = register_module("value_fc1",
                                    nn::Linear(mid_channels*boardshape1*boardshape2, mid_shape_fc)
                                   );
        value_fc2 = register_module("value_fc2",
                                    nn::Linear(mid_shape_fc, 1)
                                   );
    }
    torch::Tensor forward(torch::Tensor x){
        // Use one of many tensor manipulation functions.
        torch::Tensor main_res, policy_out, value_out;
        main_res = conv1->forward(x);
        main_res = torch::relu(bn1->forward(main_res));
        main_res = res_blocks->forward(main_res);
        
        //policy computation
        policy_out = policy_conv->forward(main_res);
        policy_out = torch::relu(policy_bn->forward(policy_out));
        policy_out = torch::flatten(policy_out, 1);
        policy_out = policy_fc->forward(policy_out);
        policy_out = torch::softmax(policy_out, 1);
        
        //value computation
        value_out = value_conv->forward(main_res);
        value_out = torch::relu(value_bn->forward(value_out));
        value_out = torch::flatten(value_out, 1);
        value_out = value_fc1->forward(value_out);
        value_out = value_fc2->forward(value_out);
        value_out = torch::tanh(value_out);
        return torch::hstack({policy_out, value_out});
    } 

    nn::Conv2d conv1{nullptr}, policy_conv{nullptr}, value_conv{nullptr};
    nn::BatchNorm2d bn1{nullptr}, policy_bn{nullptr}, value_bn{nullptr};
    nn::Sequential res_blocks{nullptr};
    nn::Linear policy_fc{nullptr}, value_fc1{nullptr}, value_fc2{nullptr};
};

//TODO handle devices cpu vs gpu
//moving tensors correctly
class TicTacToeInitPredictor : public InitPredictor{
private:
    GameNet net {3,3,3,16,8,nb_actions};
    
    //about loss function
    constexpr static float EPS = 1e-5;

    //avoid issues with 0 probabilities
    static torch::Tensor rescale(torch::Tensor other){
        return (other + EPS)/(1 + other.size(1) * EPS);
    };
    static torch::Tensor gameloss(torch::Tensor outputs, torch::Tensor labels){
         
        torch::Tensor prob_outputs, prob_labels, value_outputs, value_labels;
        prob_outputs = rescale(outputs.slice(1,0,-1));
        prob_labels = rescale(labels.slice(1,0,-1));
        value_outputs = outputs.select(1,-1);
        value_labels = labels.select(1,-1);
        //std::cout << outputs << prob_outputs << labels << prob_labels;

        torch::Tensor CE_losses = -(prob_labels * torch::log(prob_outputs)).sum(1);
        torch::Tensor value_losses = (value_outputs - value_labels).float_power(2);
        return torch::mean(CE_losses + value_losses);
    };
public:
    const static int nb_actions = 9;
    int DEBUG_nb_gets = 0;

    inline torch::Tensor stateToTensor(State &s){
        TicTacToeState &tts = dynamic_cast<TicTacToeState&>(s);
        array<char, 9> board = tts.getBoard();
        torch::Tensor res = torch::zeros({1, 3,3,3});
        for(int i = 0; i<9; ++i){
            if(board[i] != 0){
                if(board[i] == tts.player)
                    res[0][0][i/3][i%3] = 1.0;
                else
                    res[0][1][i/3][i%3] = 1.0;
            }
            if(tts.player == 2) //give info on cur_player
                res[0][2][i/3][i%3] = 1.0;
        }
        return res;
    };

    inline torch::Tensor probasAndValueToTensor(ProbasAndValue pv){
        torch::Tensor res = torch::zeros(10);
        for(int i = 0; i<9; ++i)
            res[i] = pv.probas[i];
        res[9] = pv.value;
        return res;
    };

    ProbasAndValue get(State &s) override {
        net.eval();
        torch::Tensor res = net.forward(stateToTensor(s));
        //std::cout << res << std::endl;
        const float *probas = res[0].slice(0, 0, 9).const_data_ptr<float>();
        
        std::vector<float> proba_vec {probas, probas+9};
        float value = res[0][9].item<float>();
        DEBUG_nb_gets += 1;
        ProbasAndValue out = ProbasAndValue{.probas=proba_vec, .value=value};
        //out.print();
        return out;
    }

    //TODO add parameters, learning rate, batchsize, ...
    void update(std::vector<std::pair<State*, ProbasAndValue>> results) override {
        net.train();
        int n = results.size();
        
        int batchsize = 32;
        int n_batches = n;

        std::vector<torch::Tensor> ins_vec, outs_vec;
        for(int i = 0; i<n; ++i){
            ins_vec.push_back(stateToTensor(*results[i].first));
            outs_vec.push_back(probasAndValueToTensor(results[i].second));
        }
        torch::Tensor ins = torch::vstack(ins_vec);
        torch::Tensor outs = torch::vstack(outs_vec);
        
        torch::optim::SGD optimizer(net.parameters(), torch::optim::SGDOptions(5e-3));

        for(int batch = 0; batch < n_batches; ++batch){
            torch::Tensor indices = torch::randint(0,results.size()-1,batchsize);
            torch::Tensor cur_ins = ins.index_select(0,indices);
            torch::Tensor cur_outs = outs.index_select(0, indices);
            optimizer.zero_grad();

            torch::Tensor pred = net.forward(cur_ins);
            torch::Tensor loss = gameloss(pred, cur_outs);
            loss.backward();
            optimizer.step();
            std::cout << "Batch: " << batch
                  << " | Loss: " << loss.item<float>() << std::endl;
        }
    };
    void print(){
        std::cout << net << std::endl;
        int nb_parameters = 0;
        for (auto param : net.parameters())
            nb_parameters += param.numel();
        std::cout << "No. parameters " << nb_parameters << std::endl;
        std::cout << "No. of gets : " << DEBUG_nb_gets << std::endl;

    }
};