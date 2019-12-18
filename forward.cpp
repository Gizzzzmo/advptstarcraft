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

typedef ProductionEntry* (*build_and_check)();


std::array<std::list<AbstractEntity*>*, 64> entitymap;
std::map<std::string, build_and_check> build_map;

std::map<int, std::string> name_map;
std::list<ProductionEntry*> production_list;
unsigned int time_tick = 1;
unsigned int minerals = 5000;
unsigned int gas = 0;
unsigned int supply = 15;
unsigned int supply_used = 12;
unsigned int workers_available = 12;
unsigned int mineral_worker = 12;
unsigned int gas_worker = 0;

template<Race race, int clss_id, int mins, int gs, int sppl, int sppl_p, int max_nrg, int start_nrg, int ablty_cost, ulong prd_mask, Destiny prd_d, ulong req_mask, int max_occ, int bldtime, bool is_wrkr, bool prd_larva, int units_produced>
ProductionEntry* makeEntity(){
    return Entity<race, clss_id, mins, gs, sppl, sppl_p, max_nrg, start_nrg, ablty_cost, prd_mask, prd_d, req_mask, max_occ, bldtime, is_wrkr, prd_larva, units_produced>::check_and_build(entitymap, minerals, gas, supply_used, supply, workers_available);
}

inline std::list<ProductionEntry*> process_production_list(){
    std::list<ProductionEntry*> result;
    for(std::list<ProductionEntry*>::iterator it = production_list.begin(); it != production_list.end();) {
        ProductionEntry* entry = *it;
        auto prev = it;
        ++it;
        if(entry->time_done == time_tick) {
            //TODO: Add to enity list
            entitymap[(entry->producee)->class_id()]->push_back(entry->producee);
            switch(entry->producee->producer_destiny()) {
                case consumed_at_end:
                    entitymap[(entry->producee)->class_id()]->erase(entry->it); //TODO: Delete at correct place
                    break;
                case occupied:
                    entry->producer->make_available(workers_available);
                    break;
            }
            production_list.erase(prev);
            supply += entry->producee->supply_provided();
            result.push_back(entry);
        }
    }
    return result;
}

void update_resources(){
    minerals += 70*mineral_worker;
    gas += 63*gas_worker;
}

void error_exit(char* message, json output) {
    output["buildlistValid"] = 0;
    std::cout << message << "\n";
    std::cout << output << std::endl;
    exit(1);
}

int main(int argc, char **argv){
    std::string racearg(argv[1]);
    Race race = !racearg.compare("terran") ? Race::Terran : !racearg.compare("zerg") ? Race::Zerg : Race::Protoss;
    int worker_id;
    int gas_id;

    //keep in mind minerals and gas are in hundredths
    for (int i = 0; i<entitymap.size(); i++){
        entitymap[i] = new std::list<AbstractEntity*>();
    }
    switch(race){
        case Terran:
            #include "unit_map_terran.h"
            worker_id = 4;
            gas_id = 25;
            for(int i = 0;i < 12;i++){
                entitymap[4]->push_back(new Entity<Race::Terran, 4, 5000, 0, 1, 0, 0, 0, 0, 0x0000000000000007, Destiny::occupied, 0x0000000000000000, 1, 17, 1, 0, 1>);
            }
            entitymap[0]->push_back(new Entity<Race::Terran, 0, 40000, 0, 0, 15, 0, 0, 0, 0x0000000000000010, Destiny::occupied, 0x0000000000000000, 1, 100, 0, 0, 1>);
            break;
        case Zerg:
            #include "unit_map_zerg.h"
            worker_id = 9;
            gas_id = 20;
            for(int i = 0;i < 12;i++){
                entitymap[9]->push_back(new Entity<Race::Zerg, 9, 5000, 0, 1, 0, 0, 0, 0, 0x0000000010000000, Destiny::consumed_at_start, 0x0000000000000000, 1, 17, 1, 0, 1>);
            }
            entitymap[0]->push_back(new Entity<Race::Zerg, 0, 30000, 0, 0, 6, 0, 0, 0, 0x0000000000000200, Destiny::consumed_at_start, 0x0000000000000000, 1, 100, 0, 1, 1>);
            entitymap[16]->push_back(new Entity<Race::Zerg, 16, 10000, 0, 0, 8, 0, 0, 0, 0x0000000010000000, Destiny::consumed_at_start, 0x0000000000000000, 1, 25, 0, 0, 1>);
            supply = 14;
            break;
        case Protoss:
            #include "unit_map_protoss.h"
            worker_id = 5;
            gas_id = 7;
            for(int i = 0;i < 12;i++){
                entitymap[5]->push_back(new Entity<Race::Protoss, 5, 5000, 0, 1, 0, 0, 0, 0, 0x0000000000000001, Destiny::occupied, 0x0000000000000000, 1, 17, 1, 0, 1>);
            }
            entitymap[0]->push_back(new Entity<Race::Protoss, 0, 40000, 0, 0, 15, 200, 50, 50, 0x0000000000000020, Destiny::freed, 0x0000000000000000, 1, 100, 0, 0, 1>);
            break;
    }

    bool built = true;
    bool nomorebuilding = false;
    std::vector<json> messages;
    std::vector<std::string> lines;
    while(std::cin){
        std::string line; 
        std::getline(std::cin, line);
        if(line != "")lines.push_back(line);
    }
    int next_line = 0;
    std::string line;

    json output;

    for(;1;++time_tick){
        if(time_tick > 1000) error_exit("Exceeded max time", output);
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
            if(build_map.find(line) == build_map.end()) error_exit("Build map is empty", output);
            build_and_check f = build_map[line];
            ProductionEntry* entry;
            try{
                entry = (*f)();
                generate_json = true;
                entry->addTime(time_tick);
                json build_start_event;
                build_start_event["type"] = "build-start";
                build_start_event["name"] = name_map[entry->producee->class_id()];
                build_start_event["producerID"] = entry->producer->id();
                events.push_back(build_start_event);
                production_list.push_back(entry);
                built = true;
            }catch(noMineralsException& e){
                if(entitymap[worker_id]->empty() && production_list.empty()) error_exit("No Minerals", output);
                built = false;
            }catch(noGasException& e){
                if(entitymap[gas_id]->empty() && production_list.empty()) error_exit("No Gas", output);
                built = false;
            }catch(noSupplyException& e){
                if(production_list.empty()) error_exit("No supply", output);
                built = false;
            }catch(noProducerAvailableException& e){
                if(production_list.empty()) error_exit("No Producer Available", output);
                built = false;
            }catch(requirementNotFulfilledException& e){
                if(production_list.empty()) error_exit("Requirement Not Fullfilled", output);
                built = false;
            }
        }
        unsigned int max_gas_worker = std::min(entitymap[0]->size()*6, entitymap[gas_id]->size()*3);
        unsigned int new_gas_worker = std::min(max_gas_worker, workers_available/2);
        unsigned int new_mineral_worker = workers_available - gas_worker;
        if(new_gas_worker != gas_worker || new_mineral_worker != mineral_worker){
            generate_json = true;
            gas_worker = new_gas_worker;
            mineral_worker = new_mineral_worker;
        }
        //TODO recalculate worker distribution
        if(generate_json){
            message["status"] = {};
            message["status"]["workers"] = {};
            message["status"]["workers"]["minerals"] = mineral_worker;
            message["status"]["workers"]["vespene"] = gas_worker;
            message["status"]["resources"] = {};
            message["status"]["resources"]["minerals"] = minerals/100;
            message["status"]["resources"]["vespene"] = gas/100;
            message["status"]["resources"]["supply-used"] = supply_used;
            message["status"]["resources"]["supply"] = supply;
            message["events"] = events;
            messages.push_back(message);
        }
    }
    output["buildlistValid"] = 1;
    output["game"] = race == Race::Terran ? "Terr" : race == Race::Zerg ? "Zerg" : "Prot";
    output["messages"] = messages;
    std::cout << output << std::endl;
    return 0;
}
