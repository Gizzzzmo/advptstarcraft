#pragma once

#include <array>
#include <list>

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
    std::array<std::list<Entity*>*, 64> entitymap;
    std::list<ProductionEntry*> production_list;
} GameState;