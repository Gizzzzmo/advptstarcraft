#pragma once
#include <vector>

#include "Action.h"

struct Node;

typedef struct Node{
    Node* parent;
    std::vector<struct Node*> children;
    Action action;
    double avg_score;
    int visits;
    const int skips;
} Node;