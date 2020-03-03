#include "Simulator.h"
#include "json.hpp"
#include "Entity.h"
#include <string>
#include <list>
#include <vector>
#include <array>
#include <memory>
#include <iostream>
#include "parameters.h"
//https://thispointer.com/learning-shared_ptr-part-1-usage-details/


int main(int argc, char** argv){
	if(DEBUG)
		std::cout << "Init forward\n";
    std::string racearg(argv[1]);
    Race race = !racearg.compare("terran") ? Race::Terran : !racearg.compare("zerg") ? Race::Zerg : Race::Protoss;
    
    int worker_id;
    int gas_id;
    int super_id;
    unsigned int supply = 15;
    unsigned int max_id = 0;

    std::map<std::string, int> name_map;

    //keep in mind minerals and gas are in hundredths
    std::array<std::shared_ptr<std::list<std::shared_ptr<Entity>>>, 64> entitymap;
    std::list<std::shared_ptr<ProductionEntry>> production_list;
    if(DEBUG)
    	std::cout << "Init entities\n";
    for (unsigned int i = 0; i<entitymap.size(); i++){
        //entitymap[i] = new std::list<Entity*>();
    	std::shared_ptr<std::list<std::shared_ptr<Entity>>> a(new std::list<std::shared_ptr<Entity>>());
    	entitymap[i] = a;
    }
    std::array<EntityMeta, 64> meta_map;

    std::vector<unsigned int> base_ids;
    if(DEBUG)
    	std::cout << "Switch race\n";
    switch(race){
        case Terran:
        {
            #include "unit_map_terran.h"
            worker_id = 4;
            gas_id = 25;
            base_ids = {0, 1, 2};
            super_id = 1;
            for(int i = 0;i < 12;i++){
            	std::shared_ptr<Entity> a(new Entity(meta_map, 4, 0, max_id));
                entitymap[4]->push_back(a);
            }
            std::shared_ptr<Entity> a(new Entity(meta_map, 0, 0, max_id));
            entitymap[0]->push_back(a);
            break;
        }
        case Zerg:
        {
            #include "unit_map_zerg.h"
            worker_id = 9;
            gas_id = 20;
            base_ids = {0, 3, 17};
            super_id = 1;
            for(int i = 0;i < 12;i++){
            	std::shared_ptr<Entity> a(new Entity(meta_map, 9, 0, max_id));
                entitymap[9]->push_back(a);
            }
            std::shared_ptr<Entity> hatch(
                    new Entity( meta_map, 0, 1u - meta_map[name_map["Hatchery"]].build_time - LARVA_START*LARVA_DURATION, max_id));
            entitymap[0]->push_back(hatch);
            std::shared_ptr<Entity> ovi(new Entity( meta_map, 16, 0, max_id));
            entitymap[16]->push_back(ovi);
            supply = 14;
            break;
        }
        case Protoss:
        {
            #include "unit_map_protoss.h"
            worker_id = 5;
            gas_id = 7;
            base_ids = {0};
            super_id = 0;
            for(int i = 0;i < 12;i++){
            	std::shared_ptr<Entity> a(new Entity(meta_map, 5, 0, max_id));
            	entitymap[5]->push_back(a);
            }
            std::shared_ptr<Entity> a(new Entity(meta_map, 0, 0, max_id));
            entitymap[0]->push_back(a);
            break;
        }
    }

    std::vector<std::string> lines;
    json initial_units;

    if(DEBUG)
    	std::cout << "Read into entitymap\n";
    for (std::size_t i = 0; i < 64; ++i) {
        if (entitymap[i]->empty())
            continue;
        std::list<std::string> l;
        for( std::list<std::shared_ptr<Entity>>::iterator jt = entitymap[i]->begin(); jt != entitymap[i]->end(); ++jt){
            l.push_back((*jt)->id());
        }
        initial_units[meta_map[i].name] = l;
    }
    if(DEBUG)
    	std::cout << "Read lines from cin\n";
    while(std::cin){
        std::string line; 
        std::getline(std::cin, line);
        if(line != "")lines.push_back(line);
        if(line == "") break;
    }
    std::vector<int> build_list(lines.size(), 0);
    for(size_t i = 0;i < lines.size();++i){
        build_list[i] = name_map[lines[i]];
    }
    if(DEBUG)
    	std::cout << "Initialize Game State\n";
    const GameState initialState(0, 5000, 0, supply, 12, 12, 12, 0, 0, 12, true, max_id, entitymap, {}, {});
    json output;
    switch(race){
        case Terran:
        {
            Simulator<Terran> sim(meta_map, initialState, gas_id, worker_id, base_ids, {}, super_id);
            output = sim.run(build_list);
            break;
        }
        case Zerg:
        {
            Simulator<Zerg> sim(meta_map, initialState, gas_id, worker_id, base_ids, {}, super_id);
            output = sim.run(build_list);
            break;
        }
        case Protoss:
        {
            Simulator<Protoss> sim(meta_map, initialState, gas_id, worker_id, base_ids, {}, super_id);
            output = sim.run(build_list);
            break;
        }
    }
    output["game"] = race == Race::Terran ? "Terr" : race == Race::Zerg ? "Zerg" : "Prot";
    output["initialUnits"] = initial_units;
    if(DEBUG)
    	std::cout << "\n\n\n\n\n";
    std::cout << output << std::endl;
    return 0;
}
