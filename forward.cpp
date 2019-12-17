#include "Entity.h"
#include <map>
#include <vector>
#include <list>
#include <iostream>

typedef ProductionEntry* (*build_and_check)();


std::map<int, std::list<AbstractEntity*>*> entitymap;
std::map<std::string, build_and_check> build_map;
std::list<ProductionEntry*> production_list; 
unsigned int time_tick = 0;
unsigned int minerals;
unsigned int gas;
unsigned int supply;
unsigned int supply_used;
unsigned int workers_available;

template<Race race, int clss_id, int mins, int gs, int sppl, int sppl_p, int max_nrg, int start_nrg, int ablty_cost, long prd_mask, Destiny prd_d, long req_mask, int max_occ, int bldtime, bool is_wrkr, bool prd_larva>
ProductionEntry* makeEntity(){
    return Entity<race, clss_id, mins, gs, sppl, sppl_p, max_nrg, start_nrg, ablty_cost, prd_mask, prd_d, req_mask, max_occ, bldtime, is_wrkr, prd_larva>::check_and_build(entitymap, minerals, gas, supply_used, supply, workers_available);
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

int main(){
    int worker_id = 0;
    int gas_id = 10;

    //keep in mind minerals and gas are in hundredths
    build_map["Hatchery"] = makeEntity<Race::Terran, 0, 30000, 0, 0, 6, 0, 0, 0, 0, Destiny::consumed_at_start, 0, 1, 60, false, true>;
    //TODO @Thomas also initialize all the vectors and put them into the entitymap i.e. entitymap[id] = new std::vector<AbstractEntity*>(); for all ids 

    bool built = true;
    std::string line;
    for(;1;++time_tick){
        bool generate_json = false;
        if(built){
            std::getline(std::cin, line);
        }
        std::list <ProductionEntry*> finished_list = process_production_list();
        if (!finished_list.empty()) {
            generate_json = true;
            //TODO generate productionfinishedjson
        }

        
        build_and_check f = build_map[line];
        ProductionEntry* entry;
        try{
            entry = (*f)();
            entry->addTime(time_tick);
            //TODO genrate produtionbeingjson
            production_list.push_back(entry);
        }catch(noMineralsException& e){
            if(entitymap[worker_id]->empty() && production_list.empty())return 1;
            built = false;
        }catch(noGasException& e){
            if(entitymap[gas_id]->empty() && production_list.empty())return 1;
            built = false;
        }catch(noSupplyException& e){
            if(production_list.empty())return 1;
            built = false;
        }catch(noProducerAvailableException& e){
            if(production_list.empty())return 1;
            built = false;
        }catch(requirementNotFulfilledException& e){
            if(production_list.empty())return 1;
            built = false;
        }
        if(generate_json){
            //TODO print json
        }
    }
    
}