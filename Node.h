#include "GameState.h"
#include <vector>

class Node{
public:
    Node(const int entity_to_be_built_id, const int chrono_target_id, const GameState& state, const Node* parent) :
        entity_to_be_built_id(entity_to_be_built_id), chrono_target_id(chrono_target_id), state(new GameState(state)), parent(parent){}
    const int entity_to_be_built_id;
    const int chrono_target_id;
    const std::unique_ptr<GameState> state;
    const Node* parent;
    std::vector<Node> children;
};