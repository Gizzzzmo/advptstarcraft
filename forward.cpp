#include "Entity.h"
#include <map>
#include <vector>
#include <iostream>

typedef ProductionEntry* (*build_and_check)();


std::map<int, std::vector<AbstractEntity*>> entitymap;
std::map<std::string, build_and_check> build_map;
std::vector<ProductionEntry*> production_list; 
unsigned int time = 0;
unsigned int minerals;
unsigned int gas;
unsigned int supply;
unsigned int supply_used;

ProductionEntry* makeHatchery(){
    return Entity<Race::Zerg, 0, 300, 0, 0, 6, 0, Destiny::consumed_at_start, -1, 1, 60>::check_and_build(entitymap, minerals, gas, supply_used, supply);
}

inline std::vector<ProductionEntry*> process_production_list(){
    std::vector<ProductionEntry*> productionlist;
    for(ProductionEntry* entry : production_list) {
        if(entry->time_done == time) {
            entitymap[(entry->producee)->id].push_back(entry->producee);
            //TODO:: KIll if producer killed at end
            //TODO: Remove from production_list
            //TODO: If producer occupied, make available
            //TODO: Add to production list
            //TODO: add supply_p
        }
    }
    return production_list;
}

int main(){
    int gas_id = 10;

    
    build_map["Hatchery"] = makeHatchery;

    bool built = false;
    std::string line;
    for(;1;++time){
        bool generate_json = false;
        if(built){
            std::getline(std::cin, line);
        }
        std::vector <ProductionEntry*> finished_list = process_production_list();
        if (!finished_list.empty()) {
            generate_json = true;
            //TODO generate productionfinishedjson
        }

        
        build_and_check f = build_map[line];
        ProductionEntry* entry;
        try{
            entry = (*f)();
            entry->time_done += time;
            //TODO genrate produtionbeingjson
            production_list.push_back(entry);
        }catch(noMineralsException& e){
            built = false;
        }catch(noGasException& e){
            if(entitymap[gas_id].empty() && production_list.empty())return 1;
            built = false;
        }catch(noSupplyException& e){
            if(production_list.empty())return 1;
            built = false;
        }catch(noProdcuerAvailableException& e){
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