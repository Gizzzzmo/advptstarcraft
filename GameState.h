#pragma once

#include <array>
#include <map>
#include <list>
#include <memory>
#include "ProductionEntry.h"


class Entity;
enum Race{Terran, Zerg, Protoss};

typedef struct GameState{
private:

public:
    GameState(uint time_tick, uint minerals, uint gas, uint supply, uint supply_used, uint workers_available, uint mineral_worker, uint gas_worker,
        std::array<std::shared_ptr<std::list<std::shared_ptr<Entity>>>, 64> entitymap,
        std::list<unsigned int> timeout_mule, 
        std::list<std::shared_ptr<ProductionEntry>> production_list) :
            time_tick(time_tick),
            minerals(minerals),
            gas(gas),
            supply(supply),
            supply_used(supply_used),
            workers_available(workers_available),
            mineral_worker(mineral_worker),
            gas_worker(gas_worker),
            entitymap(entitymap),
            production_list(production_list),
            timeout_mule(timeout_mule){};
    GameState(const GameState& state);
    GameState& operator=(const GameState& state);
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

    //Terran:
    std::list<unsigned int> timeout_mule;
} GameState;
