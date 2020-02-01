#include "GameState.h"
#include <vector>

class Node{
public:
    const int entity_to_be_built_id;
    const int chrono_target_id;
    const std::unique_ptr<GameState> state;
    const Node* parent;
    std::vector<Node> children;
};