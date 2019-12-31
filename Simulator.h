#pragma once
#include "json.hpp"
using json = nlohmann::json;
#include <map>
#include <vector>
#include <list>
#include <iostream>
#include <algorithm>

#include "ProductionEntry.h"
#include "Entity.h"
#include "StarcraftExceptions.h"
#include "GameState.h"

//turns a bitmask into a vector of those indices of the mask, where the bit is one
//TODO: for performance improvements, one might buffer theses vectors
inline std::vector<int> mask_to_vector(unsigned long mask) {
    std::vector<int> v;
    for(size_t i = 0;i < 64;++i){
        if(mask%2 == 1)v.push_back(i);
        mask = mask>>1;
    }
    return v;
}

class Simulator{
private:
    const std::array<EntityMeta, 64>& meta_map;
    const std::map<std::string, int>& name_map;
    const GameState& initialState;
    const unsigned int worker_id;
    const unsigned int gas_id;
    GameState currentState;


    inline std::list<ProductionEntry*> process_production_list(){
        std::list<ProductionEntry*> result;
        for(std::list<ProductionEntry*>::iterator it = currentState.production_list.begin(); it != currentState.production_list.end();) {
            ProductionEntry* entry = *it;
            auto prev = it;
            ++it;
            if(entry->time_done == currentState.time_tick) {
                //TODO: Add to enity list
                currentState.entitymap[(entry->producee)->class_id()]->push_back(entry->producee);
                std::list<Entity*>* producers = currentState.entitymap[(entry->producer)->class_id()];
                switch(entry->producee->producer_destiny()) {
                    case consumed_at_end:
                        for(std::list<Entity*>::iterator it = producers->begin();it != producers->end();++it){
                            if(*it == entry->producer){
                                producers->erase(it);
                                break;
                            }
                        }
                        break;
                    case occupied:
                        entry->producer->make_available(currentState);
                        break;
                }
                currentState.production_list.erase(prev);
                currentState.supply += entry->producee->supply_provided();
                result.push_back(entry);
            }
        }
        return result;
    }

    void update_resources(){
        currentState.minerals += 70*currentState.mineral_worker;
        currentState.gas += 63*currentState.gas_worker;
    }

    void error_exit(std::string message, json& output) {
        output["buildlistValid"] = 0;
        std::cerr << message << "\n";
    }

    ProductionEntry* check_and_build(int class_id){
        if(meta_map[class_id].supply > static_cast<int>(currentState.supply-currentState.supply_used)) throw noSupplyException();
        //bitmask of 0 is interpreted as there not being any requirements, since all entities should be buildable somehow
        if(meta_map[class_id].requirement_mask == 0)goto req_fulfilled;
        for(int req_id : mask_to_vector(meta_map[class_id].requirement_mask)){
            if(!(currentState.entitymap[req_id]->empty()))goto req_fulfilled;
        }
        throw requirementNotFulfilledException();
        req_fulfilled:
        if(currentState.gas < meta_map[class_id].gas) throw noGasException();
        if(currentState.minerals < meta_map[class_id].minerals)throw noMineralsException();

        //possibly search for chronoboosted producers?
        for(int prd_id : mask_to_vector(meta_map[class_id].production_mask)){
            std::list<Entity*> possible_producers = *(currentState.entitymap[prd_id]);
            for(std::list<Entity*>::iterator it = possible_producers.begin();it != possible_producers.end();++it){
                Entity* producer = *it;
                if(producer->check_and_occupy()){
                    if(producer->is_worker())currentState.workers_available--;
                    if(meta_map[class_id].producer_destiny == Destiny::freed)producer->make_available(currentState);
                    if(meta_map[class_id].producer_destiny == Destiny::consumed_at_start)possible_producers.erase(it);
                    currentState.minerals -= meta_map[class_id].minerals;
                    currentState.gas -= meta_map[class_id].gas;
                    currentState.supply_used += meta_map[class_id].supply;
                    Entity* producee = new Entity(meta_map, class_id);
                    ProductionEntry* e = new ProductionEntry(producee, producer, currentState);
                    return e;
                }
            }
        }
        throw noProducerAvailableException();
        return nullptr;
    }
public:

Simulator(const std::array<EntityMeta, 64>& meta_map,
            const std::map<std::string, int>& name_map,
            const GameState initialState,
            const unsigned int gas_id,
            const unsigned int worker_id) :
    meta_map(meta_map), name_map(name_map), initialState(initialState), gas_id(gas_id), worker_id(worker_id){}

json run(std::vector<std::string> lines){
    currentState = initialState;

    bool built = true;
    bool nomorebuilding = false;
    std::vector<json> messages;
    int next_line = 0;
    std::string line;

    json output;

    for(;1;++currentState.time_tick){
        if(currentState.time_tick > 1000){
            error_exit("Exceeded max time", output);
            return output;
        }
        update_resources();
        json message;
        message["time"] = currentState.time_tick;

        std::vector<json> events;
        bool generate_json = false;
        if(built){
            if(next_line != lines.size())
                line = lines[next_line++];
            else if(currentState.production_list.empty())break;
            else nomorebuilding = true;
        }
        std::list <ProductionEntry*> finished_list = process_production_list();
        if (!finished_list.empty()) {
            generate_json = true;
            for(ProductionEntry* entry : finished_list){
                json event;
                event["type"] = "build-end";
                event["name"] = meta_map[entry->producee->class_id()].name;
                event["producerID"] = entry->producer->id();
                event["producedIDs"] = {entry->producee->id()};
                events.push_back(event);
                if(entry->producee->producer_destiny() == Destiny::consumed_at_end || 
                    entry->producee->producer_destiny() == Destiny::consumed_at_start){
                    delete entry->producer;
                }
                delete entry;
            }
        }

        if(!nomorebuilding){
            auto fff = name_map.find(line);
            if(name_map.find(line) == name_map.end()){
                error_exit(line + " Entity not found", output);
                return output;
            }
            ProductionEntry* entry;
            try{
                entry = check_and_build(fff->second);
                generate_json = true;
                json build_start_event;
                build_start_event["type"] = "build-start";
                build_start_event["name"] = meta_map[entry->producee->class_id()].name;
                build_start_event["producerID"] = entry->producer->id();
                events.push_back(build_start_event);
                currentState.production_list.push_back(entry);
                built = true;
            }catch(noMineralsException& e){
                if(currentState.entitymap[worker_id]->empty() && currentState.production_list.empty()){
                    error_exit("No Minerals", output);
                    return output;
                }
                built = false;
            }catch(noGasException& e){
                if(currentState.entitymap[gas_id]->empty() && currentState.production_list.empty()){
                    error_exit("No Gas", output);
                    return output;
                }
                built = false;
            }catch(noSupplyException& e){
                if(currentState.production_list.empty()){
                    error_exit("No supply", output);
                    return output;
                }
                built = false;
            }catch(noProducerAvailableException& e){
                if(currentState.production_list.empty()){
                    error_exit("No Producer Available", output);
                    return output;
                }
                built = false;
            }catch(requirementNotFulfilledException& e){
                if(currentState.production_list.empty()){
                    error_exit("Requirement Not Fullfilled", output);
                    return output;
                }
                built = false;
            }
        }
        unsigned int max_gas_worker = std::min(currentState.entitymap[0]->size()*6, currentState.entitymap[gas_id]->size()*3);
        unsigned int new_gas_worker = std::min(max_gas_worker, currentState.workers_available/2);
        unsigned int new_mineral_worker = currentState.workers_available - new_gas_worker;
        if(new_gas_worker != currentState.gas_worker || new_mineral_worker != currentState.mineral_worker){
            generate_json = true;
            currentState.gas_worker = new_gas_worker;
            currentState.mineral_worker = new_mineral_worker;
        }
        //TODO recalculate worker distribution
        if(generate_json){
            message["status"] = {};
            message["status"]["workers"] = {};
            message["status"]["workers"]["minerals"] = currentState.mineral_worker;
            message["status"]["workers"]["vespene"] = currentState.gas_worker;
            message["status"]["resources"] = {};
            message["status"]["resources"]["minerals"] = currentState.minerals/100;
            message["status"]["resources"]["vespene"] = currentState.gas/100;
            message["status"]["resources"]["supply-used"] = currentState.supply_used;
            message["status"]["resources"]["supply"] = currentState.supply;
            message["events"] = events;
            messages.push_back(message);
        }
    }
    output["buildlistValid"] = 1;
    output["messages"] = messages;
    return output;
}
};