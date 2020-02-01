#include "GameState.h"
#include <vector>

class Node{
public:
	Node(	const int entity_to_be_built_id,
			const int chrono_target_id,
			const std::unique_ptr<GameState> state,
			const Node* const parent,) :
				this.entity_to_be_built_id = entity_to_be_built_id;
				this.chrono_target_id = chrono_target_id;
				this.state = state;
				this.parent = parent;
				{}
    const int entity_to_be_built_id;
    const int chrono_target_id;
    const std::unique_ptr<GameState> state;
    const Node* parent;
    std::vector<Node> children;
};
