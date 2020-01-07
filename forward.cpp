#include "Simulator.h"
#include "json.hpp"
#include "Entity.h"
using json = nlohmann::json;
#include <string>
#include <list>
#include <vector>
#include <array>
#include <memory>
//https://thispointer.com/learning-shared_ptr-part-1-usage-details/


int main(int argc, char** argv){
    std::string racearg(argv[1]);
    Race race = !racearg.compare("terran") ? Race::Terran : !racearg.compare("zerg") ? Race::Zerg : Race::Protoss;
    
    int worker_id;
    int gas_id;
    int super_id;
    unsigned int supply = 15;

    std::map<std::string, int> name_map;

    //keep in mind minerals and gas are in hundredths
    std::array<std::shared_ptr<std::list<std::shared_ptr<Entity>>>, 64> entitymap;
    std::list<std::shared_ptr<ProductionEntry>> production_list;
    for (unsigned int i = 0; i<entitymap.size(); i++){
        //entitymap[i] = new std::list<Entity*>();
    	std::shared_ptr<std::list<std::shared_ptr<Entity>>> a(new std::list<std::shared_ptr<Entity>>());
    	entitymap[i] = a;
    }
    std::array<EntityMeta, 64> meta_map;

    std::vector<unsigned int> base_ids;
    switch(race){
        case Terran:
        {
            #include "unit_map_terran.h"
            worker_id = 4;
            gas_id = 25;
            base_ids = {0, 1, 2};
            super_id = 1;
            for(int i = 0;i < 12;i++){
            	std::shared_ptr<Entity> a(new Entity(meta_map, 4));
                entitymap[4]->push_back(a);
            }
            std::shared_ptr<Entity> a(new Entity(meta_map, 0));
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
            	std::shared_ptr<Entity> a(new Entity(meta_map, 9));
                entitymap[9]->push_back(a);
            }
            std::shared_ptr<Entity> a(new Entity( meta_map, 0));
            std::shared_ptr<Entity> b(new Entity( meta_map, 0));
            entitymap[0]->push_back(a);
            entitymap[16]->push_back(b);
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
            	std::shared_ptr<Entity> a(new Entity( meta_map, 5));
            	entitymap[5]->push_back(a);
            }
            std::shared_ptr<Entity> a(new Entity( meta_map, 0));
            entitymap[0]->push_back(a);
            break;
        }
    }

    const GameState initialState{0, 5000, 0, supply, 12, 12, 12, 0, entitymap, {}, {}, race};

    std::vector<std::string> lines;
    json initial_units;

    for (std::size_t i = 0; i < 64; ++i) {
        if (entitymap[i]->empty())
            continue;
        std::list<std::string> l;
        for( std::list<std::shared_ptr<Entity>>::iterator jt = entitymap[i]->begin(); jt != entitymap[i]->end(); ++jt){
            l.push_back((*jt)->id());
        }
        initial_units[meta_map[i].name] = l;
    }
    while(std::cin){
        std::string line; 
        std::getline(std::cin, line);
        if(line != "")lines.push_back(line);
        if(line == "") break;
    }
    Simulator sim(meta_map, name_map, initialState, gas_id, worker_id, base_ids, super_id);
    json output = sim.run(lines);
    output["game"] = race == Race::Terran ? "Terr" : race == Race::Zerg ? "Zerg" : "Prot";
    output["initialUnits"] = initial_units;
    std::cout << output << std::endl;
    return 0;
}
