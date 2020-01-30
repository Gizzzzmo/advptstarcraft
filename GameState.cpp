#include "Entity.h"
#include "ProductionEntry.h"
#include "GameState.h"
#include <iostream>

GameState::GameState(const GameState& state) : 
    time_tick(state.time_tick),
    minerals(state.minerals),
    gas(state.gas),
    supply(state.supply),
    supply_used(state.supply_used),
    workers_available(state.workers_available),
    mineral_worker(state.mineral_worker),
    gas_worker(state.gas_worker),
    timeout_mule(state.timeout_mule) {
        std::map<std::shared_ptr<Entity>, std::shared_ptr<Entity>> entity_translation_map;
        for(int i = 0;i < 64;i++){
            std::shared_ptr<std::list<std::shared_ptr<Entity>>> list(new std::list<std::shared_ptr<Entity>>);
            entitymap[i] = list;
            for(auto entity : *state.entitymap[i]){
                std::shared_ptr<Entity> copied_entity(new Entity(*entity));
                entitymap[i]->push_back(copied_entity);
                entity_translation_map[entity] = copied_entity;
            }
        }

        std::map<std::shared_ptr<ProductionEntry>, std::shared_ptr<ProductionEntry>> entry_translation_map;
        for(auto entry : state.production_list){
            std::shared_ptr<ProductionEntry> copied_entry(new ProductionEntry(*entry, entity_translation_map));
            production_list.push_back(copied_entry);
            entry_translation_map[entry] = copied_entry;
        }
        for(int i = 0;i < 64;i++){
            for(auto entity : *entitymap[i]){
                for(auto it = entity->producees.begin(); it != entity->producees.end();++it){
                    std::shared_ptr<ProductionEntry> new_entry = entry_translation_map[*it];
                    it = entity->producees.insert(entity->producees.erase(it), new_entry);
                }
            }
        }
}

GameState& GameState::operator=(const GameState& state){
    if(this != &state){

        time_tick = state.time_tick;
        minerals = state.minerals;
        gas = state.gas;
        supply = state.supply;
        supply_used = state.supply_used;
        workers_available = state.workers_available;
        mineral_worker = state.mineral_worker;
        gas_worker = state.gas_worker;
        timeout_mule = state.timeout_mule;

        std::map<std::shared_ptr<Entity>, std::shared_ptr<Entity>> entity_translation_map;
        for(int i = 0;i < 64;i++){
            std::shared_ptr<std::list<std::shared_ptr<Entity>>> list(new std::list<std::shared_ptr<Entity>>);
            entitymap[i]->erase(entitymap[i]->begin(), entitymap[i]->end());
            for(auto entity : *state.entitymap[i]){
                std::shared_ptr<Entity> copied_entity(new Entity(*entity));
                entitymap[i]->push_back(copied_entity);
                entity_translation_map[entity] = copied_entity;
            }
        }
        std::map<std::shared_ptr<ProductionEntry>, std::shared_ptr<ProductionEntry>> entry_translation_map;
        
        production_list.erase(state.production_list.begin(), state.production_list.end());
        for(auto entry : state.production_list){
            std::shared_ptr<ProductionEntry> copied_entry(new ProductionEntry(*entry, entity_translation_map));
            production_list.push_back(copied_entry);
            entry_translation_map[entry] = copied_entry;
        }
        for(int i = 0;i < 64;i++){
            for(auto entity : *entitymap[i]){
                for(auto it = entity->producees.begin(); it != entity->producees.end();++it){
                    std::shared_ptr<ProductionEntry> new_entry = entry_translation_map[*it];
                    it = entity->producees.insert(entity->producees.erase(it), new_entry);
                }
            }
        }
    }
    return *this;
}