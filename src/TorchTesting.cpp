#include <torch/torch.h>
#include <iostream>

int main() {
  for(int i =0; i<10000; ++i){
    torch::Tensor tensor = torch::rand({2, 3, 4});
    std::cout << tensor << std::endl;
  }
}