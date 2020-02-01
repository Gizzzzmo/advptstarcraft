#include "GameState.h"
#include <vector>
#include <memory>

class Node{
public:
    Node(const int entity_to_be_built_id, const int chrono_target_id, const std::shared_ptr<Node> parent) :
        entity_to_be_built_id(entity_to_be_built_id), chrono_target_id(chrono_target_id), parent(parent){}
    const int entity_to_be_built_id;
    const int chrono_target_id;
    const std::shared_ptr<Node> parent;
    std::vector<std::shared_ptr<Node>> children;
};
