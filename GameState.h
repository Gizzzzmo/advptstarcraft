#pragma once

#include <array>
#include <map>
#include <list>
#include <memory>
#include <iostream>
#include "ProductionEntry.h"


class Entity;
enum Race{Terran, Zerg, Protoss};

typedef struct GameState{
private:

public:
    GameState(uint time_tick, uint minerals, uint gas, uint supply, uint supply_used, uint workers_available, uint mineral_worker, uint gas_worker, int gas_geysers_available, int final_supply, bool built, uint max_id,
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
            timeout_mule(timeout_mule),
            gas_geysers_available(gas_geysers_available),
            final_supply(final_supply),
            built(built),
            max_id(max_id){
                for(int i = 0;i < 64;i++){
                    entity_count[i] = entitymap[i]->size();
                }
            };
    GameState(const GameState& state);
    ~GameState();
    GameState& operator=(const GameState& state);
    friend std::ostream &operator<<(std::ostream  &outstream, GameState& state);
    unsigned int time_tick;
    unsigned int minerals;
    unsigned int gas;
    unsigned int supply;
    unsigned int supply_used;
    unsigned int workers_available;
    unsigned int mineral_worker;
    unsigned int gas_worker;
    unsigned int max_id;
    bool built;
    std::array<std::shared_ptr<std::list<std::shared_ptr<Entity>>>, 64> entitymap;
    std::list<std::shared_ptr<ProductionEntry>> production_list;

    //Terran:
    std::list<unsigned int> timeout_mule;
    std::list<int> build_list;
    int gas_geysers_available; // gas_available once current build_list is completed
    int final_supply; // supply available once current build_list is completed
    std::array<int, 64> entity_count;
} GameState;
