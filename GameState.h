#pragma once

#include <array>
#include <list>
#include <memory>

class ProductionEntry;
class Entity;

typedef struct GameState{
    unsigned int time_tick;
    unsigned int minerals;
    unsigned int gas;
    unsigned int supply;
    unsigned int supply_used;
    unsigned int workers_available;
    unsigned int mineral_worker;
    unsigned int gas_worker;
    std::array<std::shared_ptr<std::list<std::shared_ptr<Entity>>>, 64> entitymap;
    std::list<std::shared_ptr<ProductionEntry>> production_list;
} GameState;
