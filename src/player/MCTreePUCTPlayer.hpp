#include "InitPredictor.hpp"
#include "Player.hpp"
#include "MCTreePUCT.hpp"
#include <vector>

class MCTreePUCTPlayer : public Player{
private:
    MCTreePUCT tree;
    MCTreePUCT *cur_tree;
    std::vector<std::pair<State *, ProbasAndValue>> past_predictions;
public:
    MCTreePUCTPlayer(State *init_state, InitPredictor &pred);
    int play(State *s) override;
    void register_action(int action) override;
    void reset() override;
    void update() override;
    void DEBUGprint_average_past_predictions();
};