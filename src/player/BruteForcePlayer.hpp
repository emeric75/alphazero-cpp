#include "Player.hpp"
#include "MCTree.hpp"

class BruteForcePlayer : public Player{
private:
    MCTree tree;
    MCTree *cur_tree;
public:
    BruteForcePlayer(State *init_state);
    int play(State *s) override;
    void register_action(int action) override;
    void reset() override;
    void update() override {};
    int DEBUG_nb_states();
};