#pragma once
#include "json.hpp"
using json = nlohmann::json;
#include <map>
#include <vector>
#include <list>
#include <iostream>
#include <algorithm>
#include <memory>

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
    const unsigned int worker_id;
    const unsigned int gas_id;
    const std::array<EntityMeta, 64>& meta_map;
    const std::map<std::string, int>& name_map;
    const GameState& initialState;
    const std::vector<unsigned int> base_ids;
    GameState currentState;


    inline std::list<std::shared_ptr<ProductionEntry>> process_production_list(){
        std::list<std::shared_ptr<ProductionEntry>> result;
        for(std::list<std::shared_ptr<ProductionEntry>>::iterator it = currentState.production_list.begin(); it != currentState.production_list.end();) {
            std::shared_ptr<ProductionEntry> entry = *it;
            auto prev = it;
            ++it;
            if(entry->time_done == currentState.time_tick) {
                //TODO: Add to enity list
                if(entry->producee->is_worker())currentState.workers_available++;
                currentState.entitymap[(entry->producee)->class_id()]->push_back(entry->producee);
                std::shared_ptr<std::list<std::shared_ptr<Entity>>> producers = currentState.entitymap[(entry->producer)->class_id()];
                //std::list<Entity*>* producers = currentState.entitymap[(entry->producer)->class_id()];
                switch(entry->producee->producer_destiny()) {
                    case consumed_at_end:
                    	for(std::list<std::shared_ptr<Entity>>::iterator it = producers->begin();it != producers->end();++it){
                            if(*it == entry->producer){
                                producers->erase(it);
                                break;
                            }
                        }
                        break;
                    case occupied:
                        entry->producer->make_available(currentState);
                        break;
                    case freed:
                    	break;
                    case consumed_at_start:
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

    std::shared_ptr<ProductionEntry> check_and_build(int class_id){
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
            std::shared_ptr<std::list<std::shared_ptr<Entity>>> possible_producers = currentState.entitymap[prd_id];
            for(std::list<std::shared_ptr<Entity>>::iterator it = possible_producers->begin();it != possible_producers->end();++it){
                std::shared_ptr<Entity> producer = *it;
                if(producer->check_and_occupy()){
                    if(producer->is_worker())currentState.workers_available--;
                    if(meta_map[class_id].producer_destiny == Destiny::freed)producer->make_available(currentState);
                    if(meta_map[class_id].producer_destiny == Destiny::consumed_at_start)possible_producers->erase(it);
                    currentState.minerals -= meta_map[class_id].minerals;
                    currentState.gas -= meta_map[class_id].gas;
                    currentState.supply_used += meta_map[class_id].supply;
                    std::shared_ptr<Entity> producee(new Entity(meta_map, class_id));
                    std::shared_ptr<ProductionEntry> e(new ProductionEntry(producee, producer, currentState));
                    return e;
                }
            }
        }
        throw noProducerAvailableException();
        return nullptr;
    }

    inline json make_json_message(const GameState &currentGameState, const std::vector<json> events) {
    	json message;
        message["time"] = currentState.time_tick;
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
        return message;
    }


	inline void generate_json_build_end(std::vector<json> &events, std::list <std::shared_ptr<ProductionEntry>> &finished_list) {
		for(std::shared_ptr<ProductionEntry> entry : finished_list){
			json event;
			event["type"] = "build-end";
			event["name"] = meta_map[entry->producee->class_id()].name;
			event["producerID"] = entry->producer->id();
			event["producedIDs"] = {entry->producee->id()};
			events.push_back(event);
		}
	}


	inline void generate_json_build_start(std::vector<json> &events, const std::shared_ptr<ProductionEntry> &entry) {
        json build_start_event;
        build_start_event["type"] = "build-start";
        build_start_event["name"] = meta_map[entry->producee->class_id()].name;
        build_start_event["producerID"] = entry->producer->id();
        events.push_back(build_start_event);
	}

	inline bool update_worker_distribution() {
        unsigned int max_gas_worker = std::min(number_of_bases()*6, static_cast<unsigned int>(currentState.entitymap[gas_id]->size()*3));
        unsigned int new_gas_worker = std::min(max_gas_worker, currentState.workers_available/2);
        unsigned int new_mineral_worker = std::min(currentState.workers_available - new_gas_worker, number_of_bases()*16);
        //std::cout << currentState.workers_available - new_gas_worker << " " << new_mineral_worker << "\n";
        if(new_gas_worker != currentState.gas_worker || new_mineral_worker != currentState.mineral_worker){
            currentState.gas_worker = new_gas_worker;
            currentState.mineral_worker = new_mineral_worker;
            return true;
        }
        return false;
	}


    unsigned int number_of_bases(){
        unsigned int sum = 0;
        for(unsigned int id : base_ids){
            //std::cout << meta_map[id].name << "exists " << currentState.entitymap[id]->size() << " many times\n";
            sum += currentState.entitymap[id]->size();
        }
        //std::cout << "total number of bases: " << sum << "\n";
        return sum;
    }
public:

Simulator(const std::array<EntityMeta, 64>& meta_map,
            const std::map<std::string, int>& name_map,
            const GameState initialState,
            const unsigned int gas_id,
            const unsigned int worker_id, 
            const std::vector<unsigned int>& base_ids) :
    meta_map(meta_map), name_map(name_map), initialState(initialState), gas_id(gas_id), worker_id(worker_id), base_ids(base_ids){}

json run(std::vector<std::string> lines){
    currentState = initialState;

    bool built = true;
    bool generate_json;
    std::vector<json> messages;
    unsigned int next_line = 0;
    std::string line;

    json output;

    // 1. Liste abarbeiten
    while(next_line != lines.size() || !built) {
        //std::cout << currentState.time_tick << " " << line << "\n";
    	//Init timestep
    	if((++currentState.time_tick) > 1000){
            error_exit("Exceeded max time", output);
            return output;
        }
    	generate_json = false;
        std::vector<json> events;

    	update_resources();

    	//Check finished buildings
        std::list <std::shared_ptr<ProductionEntry>> finished_list = process_production_list();
    	if (!finished_list.empty()) {
    		generate_json = true;
			generate_json_build_end(events, finished_list);
    	}

    	//Start new buildings
        if(built) {
        	line = lines[next_line++];
        }
        auto fff = name_map.find(line);
        if(name_map.find(line) == name_map.end()){
            error_exit(line + " Entity not found", output);
            return output;
        }

        std::shared_ptr<ProductionEntry> entry;
        try{
            entry = check_and_build(fff->second);
            built = true;
        }catch(noMineralsException& e){
            //std::cout << "no mins\n";
            if(currentState.entitymap[worker_id]->empty() && currentState.production_list.empty()){
                error_exit("No Minerals", output);
                return output;
            }
            built = false;
        }catch(noGasException& e){
            //std::cout << "no gas\n";
            if(currentState.entitymap[gas_id]->empty() && currentState.production_list.empty()){
                error_exit("No Gas", output);
                return output;
            }
            built = false;
        }catch(noSupplyException& e){
            //std::cout << "no supply\n";
            if(currentState.production_list.empty()){
                error_exit("No supply", output);
                return output;
            }
            built = false;
        }catch(noProducerAvailableException& e){
            //std::cout << "no prod\n";
            if(currentState.production_list.empty()){
                error_exit("No Producer Available", output);
                return output;
            }
            built = false;
        }catch(requirementNotFulfilledException& e){
            //std::cout << "missing req\n";
            if(currentState.production_list.empty()){
                error_exit("Requirement not fulfilled", output);
                return output;
            }
            built = false;
        }
        if(built) {
        	currentState.production_list.push_back(entry);
			generate_json = true;
			generate_json_build_start(events, entry);
        }

        if(update_worker_distribution())
        	generate_json = true;

        if(generate_json){
            messages.push_back(make_json_message(currentState, events));
        }
    }

    // 2. Warte auf Fertigstellung
    while(!currentState.production_list.empty()){
        if((++currentState.time_tick) > 1000){
            error_exit("Exceeded max time", output);
            return output;
        }
        generate_json = false;
        std::vector<json> events;

        update_resources();

        std::list <std::shared_ptr<ProductionEntry>> finished_list = process_production_list();
    	if (!finished_list.empty()) {
			generate_json = true;
			generate_json_build_end(events, finished_list);
    	}

        if(update_worker_distribution())
        	generate_json = true;

        if(generate_json){
            messages.push_back(make_json_message(currentState, events));
        }
    }
    output["buildlistValid"] = 1;
    output["messages"] = messages;
    return output;
}
};
