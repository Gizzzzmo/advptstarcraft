#include "Entity.h"
#include "json.hpp"
using json = nlohmann::json;
#include <map>
#include <vector>
#include <iostream>

typedef ProductionEntry* (*build_and_check)();


std::map<int, std::vector<AbstractEntity*>*> entitymap;
std::map<std::string, build_and_check> build_map;
std::map<int, std::string> name_map;
std::vector<ProductionEntry*> production_list; 
unsigned int time_tick = 1;
unsigned int minerals = 50;
unsigned int gas = 0;
unsigned int supply;
unsigned int supply_used = 12;
unsigned int workers_available = 12;
unsigned int mineral_worker = 12;
unsigned int gas_worker = 0;

template<Race race, int clss_id, int mins, int gs, int sppl, int sppl_p, int max_nrg, int start_nrg, int ablty_cost, long prd_mask, Destiny prd_d, long req_mask, int max_occ, int bldtime, bool is_wrkr, bool prd_larva>
ProductionEntry* makeEntity(){
    return Entity<race, clss_id, mins, gs, sppl, sppl_p, max_nrg, start_nrg, ablty_cost, prd_mask, prd_d, req_mask, max_occ, bldtime, is_wrkr, prd_larva>::check_and_build(entitymap, minerals, gas, supply_used, supply, workers_available);
}

inline std::vector<ProductionEntry*> process_production_list(){
    std::vector<ProductionEntry*> productionlist;
    for(ProductionEntry* entry : production_list) {
        if(entry->time_done == time_tick) {
            entitymap[(entry->producee)->class_id()]->push_back(entry->producee);
            //TODO:: KIll if producer killed at end and remove supply_provided as well as supply
            //TODO: Remove from production_list
            //TODO: If producer occupied, make available
            //TODO: Add to enity list
            //TODO: add supply_p
        }
    }
    return production_list;
}

int main(){
    int worker_id = 0;
    int gas_id = 10;

    //keep in mind minerals and gas are in hundredths
    build_map["Hatchery"] = makeEntity<Race::Terran, 0, 30000, 0, 0, 6, 0, 0, 0, 0, Destiny::consumed_at_start, 0, 1, 60, false, true>;
    //TODO @Thomas also initialize all the vectors and put them into the entitymap i.e. entitymap[id] = new std::vector<AbstractEntity*>(); for all ids 

    bool built = true;
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
        json message;
        message["time"] = time_tick;

        std::vector<json> events;
        bool generate_json = false;
        if(built){
            if(next_line == lines.size())break;
            line = lines[next_line++];
        }
        std::vector <ProductionEntry*> finished_list = process_production_list();
        if (!finished_list.empty()) {
            generate_json = true;
            for(ProductionEntry* entry : finished_list){
                json event;
                event["type"] = "build-end";
                event["name"] = name_map[entry->producee->class_id()];
                event["producerID"] = entry->producer->id();
                event["producedIDs"] = {entry->producee->id()};
                events.push_back(event);
            }
        }


        
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