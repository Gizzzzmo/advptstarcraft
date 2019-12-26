#pragma once
#include "json.hpp"
using json = nlohmann::json;
#include <map>
#include <vector>
#include <list>
#include <iostream>
#include <algorithm>

#include "Entity.h"
#include "AbstractEntity.h"
#include "StarcraftExceptions.h"

typedef struct GameState{
    unsigned int time_tick;
    unsigned int minerals;
    unsigned int gas;
    unsigned int supply;
    unsigned int supply_used;
    unsigned int workers_available;
    unsigned int mineral_worker;
    unsigned int gas_worker;
    std::array<std::list<AbstractEntity*>*, 64> entitymap;
    std::list<ProductionEntry*> production_list;
} GameState;

typedef ProductionEntry* (*build_and_check)(GameState& state);

class Simulator{
private:
    const std::map<std::string, build_and_check>& build_map;
    const std::map<int, std::string>& name_map;
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
                std::list<AbstractEntity*>* producers = currentState.entitymap[(entry->producer)->class_id()];
                switch(entry->producee->producer_destiny()) {
                    case consumed_at_end:
                        for(std::list<AbstractEntity*>::iterator it = producers->begin();it != producers->end();++it){
                            if(*it == entry->producer){
                                producers->erase(it);
                                break;
                            }
                        }
                        break;
                    case occupied:
                        entry->producer->make_available(currentState.workers_available);
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
public:

Simulator(const std::map<std::string, build_and_check>& build_map,
            const std::map<int, std::string>& name_map,
            const GameState initialState,
            const unsigned int gas_id,
            const unsigned int worker_id) :
    build_map(build_map), name_map(name_map), initialState(initialState), gas_id(gas_id), worker_id(worker_id){}

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
                event["name"] = name_map.find(entry->producee->class_id())->second;
                event["producerID"] = entry->producer->id();
                event["producedIDs"] = {entry->producee->id()};
                events.push_back(event);
                if(entry->producee->producer_destiny() == Destiny::consumed_at_end || 
                    entry->producee->producer_destiny() == Destiny::consumed_at_start){
                    std::cout << "deleting consumed producer" << entry->producer->class_id() << "\n";
                    delete entry->producer;
                }
                std::cout << "deleting prod entry "<< entry->producee->class_id() <<"\n";
                delete entry;
            }
        }

        if(!nomorebuilding){
            auto fff = build_map.find(line);
            if(build_map.find(line) == build_map.end()){
                error_exit(line + " Entity not found", output);
                return output;
            }
            build_and_check f = fff->second;
            ProductionEntry* entry;
            try{
                entry = (*f)(currentState);
                generate_json = true;
                entry->addTime(currentState.time_tick);
                json build_start_event;
                build_start_event["type"] = "build-start";
                build_start_event["name"] = name_map.find(entry->producee->class_id())->second;
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