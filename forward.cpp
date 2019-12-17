#include "Entity.h"
#include "json.hpp"
using json = nlohmann::json;
#include <map>
#include <vector>
#include <list>
#include <iostream>

typedef ProductionEntry* (*build_and_check)();


std::map<int, std::list<AbstractEntity*>*> entitymap;
std::map<std::string, build_and_check> build_map;
std::map<int, std::string> name_map;
std::list<ProductionEntry*> production_list; 
unsigned int time_tick = 1;
unsigned int minerals = 50;
unsigned int gas = 0;
unsigned int supply;
unsigned int supply_used = 12;
unsigned int workers_available = 12;
unsigned int mineral_worker = 12;
unsigned int gas_worker = 0;

template<Race race, int clss_id, int mins, int gs, int sppl, int sppl_p, int max_nrg, int start_nrg, int ablty_cost, long prd_mask, Destiny prd_d, long req_mask, int max_occ, int bldtime, bool is_wrkr, bool prd_larva, int units_produced>
ProductionEntry* makeEntity(){
    return Entity<race, clss_id, mins, gs, sppl, sppl_p, max_nrg, start_nrg, ablty_cost, prd_mask, prd_d, req_mask, max_occ, bldtime, is_wrkr, prd_larva, units_produced>::check_and_build(entitymap, minerals, gas, supply_used, supply, workers_available);
}

inline std::list<ProductionEntry*> process_production_list(){
    std::list<ProductionEntry*> result;
    for(std::list<ProductionEntry*>::iterator it = production_list.begin(); it != production_list.end(); ++it) {
        ProductionEntry* entry = *it;
        if(entry->time_done == time_tick) {
            //TODO: Add to enity list
            entitymap[(entry->producee)->class_id()]->push_back(entry->producee);
            switch(entry->producee->producer_destiny()) {
                case consumed_at_end:
                    entitymap[(entry->producee)->class_id()]->erase(entry->it); //TODO: Delete at correct place
                    break;
                case occupied:
                    entry->producee->make_available();
                    break;
            production_list.erase(it);
            supply += entry->producee->supply_provided();
            result.push_back(entry);
            }
        }
    }
    return result;
}

void update_resources(){
    minerals += 70*mineral_worker;
    gas += 63*gas_worker;
}

int main(){
    int worker_id = 0;
    int gas_id = 10;

    //keep in mind minerals and gas are in hundredths
#include "unit_map.h"
    //TODO @Thomas also initialize all the vectors and put them into the entitymap i.e. entitymap[id] = new std::vector<AbstractEntity*>(); for all ids 

    bool built = true;
    bool nomorebuilding = false;
    std::vector<json> messages;
    std::vector<std::string> lines;
    while(std::cin){
        std::string line; 
        std::getline(std::cin, line);
        lines.push_back(line);
    }
    int next_line = 0;
    std::string line;

    json output;

    for(;1;++time_tick){
        update_resources();

        json message;
        message["time"] = time_tick;

        std::vector<json> events;
        bool generate_json = false;
        if(built){
            if(next_line != lines.size())
                line = lines[next_line++];
            else if(production_list.empty())break;
            else nomorebuilding = true;
        }
        std::list <ProductionEntry*> finished_list = process_production_list();
        if (!finished_list.empty()) {
            generate_json = true;
            for(ProductionEntry* entry : finished_list){
                json event;
                event["type"] = "build-end";
                event["name"] = name_map[entry->producee->class_id()];
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
            build_and_check f = build_map[line];
            ProductionEntry* entry;
            try{
                entry = (*f)();
                entry->addTime(time_tick);
                json build_start_event;
                build_start_event["type"] = "build-start";
                build_start_event["name"] = name_map[entry->producee->class_id()];
                build_start_event["producerID"] = entry->producer->id();
                events.push_back(build_start_event);
                production_list.push_back(entry);
                built = true;
            }catch(noMineralsException& e){
                if(entitymap[worker_id]->empty() && production_list.empty())goto list_invalid;
                built = false;
            }catch(noGasException& e){
                if(entitymap[gas_id]->empty() && production_list.empty())goto list_invalid;
                built = false;
            }catch(noSupplyException& e){
                if(production_list.empty())goto list_invalid;
                built = false;
            }catch(noProducerAvailableException& e){
                if(production_list.empty())goto list_invalid;
                built = false;
            }catch(requirementNotFulfilledException& e){
                if(production_list.empty())goto list_invalid;
                built = false;
            }
        }
        //TODO recalculate worker distribution
        message["status"] = {};
        message["status"]["workers"] = {};
        message["status"]["workers"]["minerals"] = mineral_worker;
        message["status"]["workers"]["vespene"] = gas_worker;
        message["status"]["resources"] = {};
        message["status"]["resources"]["minerals"] = minerals;
        message["status"]["resources"]["vespene"] = gas;
        message["status"]["resources"]["supply-used"] = supply_used;
        message["status"]["resources"]["supply"] = supply;
        message["events"] = events;
        messages.push_back(message);
        
    }
    output["buildlistValid"] = 1;
    output["game"] = "Race";//TODO determine race
    output["messages"] = messages;
    std::cout << output << std::endl;
    return 0;
    list_invalid:
    output["buildlistValid"] = 0;
    std::cout << output << std::endl;
}
