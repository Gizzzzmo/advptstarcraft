#include "Simulator.h"
#include "Node.h"

#include <string>
#include <iostream>
#include <map>
#include <array>

enum Scenario{Rush, Push};

int main(int argc, char** argv){
    std::string scenario(argv[1]), target_unit(argv[2]);
    
    int leaf_qualifier = std::stoi(argv[3]);
    Race gamerace;
    std::array<EntityMeta, 64> meta_map;
    std::map<std::string, int> name_map;

    int worker_id;
    int gas_id;
    int super_id;
    unsigned int supply = 15;
    std::vector<unsigned int> base_ids;
    std::vector<unsigned int> building_ids = {};

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

    #include "unit_map_protoss.h"
    if(name_map.count(target_unit)){
        gamerace = Race::Protoss;
        worker_id = 5;
        gas_id = 7;
        base_ids = {0};
        super_id = 0;
        building_ids = {0, 1, 2, 7, 8, 9, 10, 12, 14, 15, 16, 18, 22, 24};
        for(int i = 0;i < 12;i++){
            std::shared_ptr<Entity> a(new Entity(meta_map, 5, 0));
            entitymap[5]->push_back(a);
        }
        std::shared_ptr<Entity> a(new Entity(meta_map, 0, 0));
        entitymap[0]->push_back(a);
    }
    else{
        name_map.clear();
        #include "unit_map_terran.h"
        if(name_map.count(target_unit)){
            gamerace = Race::Terran;
            worker_id = 4;
            gas_id = 25;
            base_ids = {0, 1, 2};
            super_id = 1;
            for(int i = 0;i < 12;i++){
            	std::shared_ptr<Entity> a(new Entity(meta_map, 4, 0));
                entitymap[4]->push_back(a);
            }
            std::shared_ptr<Entity> a(new Entity(meta_map, 0, 0));
            entitymap[0]->push_back(a);
        }
        else{
            name_map.clear();
            #include "unit_map_zerg.h"
            gamerace = Race::Zerg;
            worker_id = 9;
            gas_id = 20;
            base_ids = {0, 3, 17};
            super_id = 1;
            for(int i = 0;i < 12;i++){
            	std::shared_ptr<Entity> a(new Entity(meta_map, 9, 0));
                entitymap[9]->push_back(a);
            }
            std::shared_ptr<Entity> hatch(new Entity(meta_map, 0, 0));
            entitymap[0]->push_back(hatch);
            std::shared_ptr<Entity> ovi(new Entity(meta_map, 16, 0));
            entitymap[16]->push_back(ovi);
            supply = 14;
        }
    }
    const GameState initialState(0, 5000, 0, supply, 12, 12, 12, 0, 0, 12, true, entitymap, {}, {});

    switch(gamerace){
        case Protoss:
            if(!scenario.compare("rush")){
                optimize<Race::Protoss, Scenario::Rush>(meta_map, initialState, gas_id, worker_id, base_ids, building_ids, super_id, leaf_qualifier);
            }
            else if(!scenario.compare("push")){
                optimize<Race::Protoss, Scenario::Push>(meta_map, initialState, gas_id, worker_id, base_ids, building_ids, super_id, leaf_qualifier);
            }
            break;
        case Terran:
            if(!scenario.compare("rush")){
                optimize<Race::Terran, Scenario::Rush>(meta_map, initialState, gas_id, worker_id, base_ids, building_ids, super_id, leaf_qualifier);
            }
            else if(!scenario.compare("push")){
                optimize<Race::Terran, Scenario::Push>(meta_map, initialState, gas_id, worker_id, base_ids, building_ids, super_id, leaf_qualifier);
            }
            break;
        case Zerg:
            if(!scenario.compare("rush")){
                optimize<Race::Zerg, Scenario::Rush>(meta_map, initialState, gas_id, worker_id, base_ids, building_ids, super_id, leaf_qualifier);
            }
            else if(!scenario.compare("push")){
                optimize<Race::Zerg, Scenario::Push>(meta_map, initialState, gas_id, worker_id, base_ids, building_ids, super_id, leaf_qualifier);
            }
            break;
    }
    return 0;
}

template<Race gamerace, Scenario scenario>
void optimize(const std::array<EntityMeta, 64>& meta_map,
    const GameState& initialState,
    const int gas_id,
    const int worker_id,
    const std::vector<unsigned int>& base_ids,
    const std::vector<unsigned int>& building_ids,
    const int super_id,
    const int leaf_qualifier)
{
    Simulator<gamerace> sim(meta_map, initialState, gas_id, worker_id, base_ids, building_ids, super_id);
    std::unique_ptr<GameState> rootState = new GameState(initialState);
    Node root = {-1, -1, rootState, nullptr, {}};
    dfs(sim, root);

    std::cout << sim.currentState << "\n";
    sim.step(0, nullptr, nullptr);
    std::cout << sim.currentState << "\n";
    while(sim.worker_distribution_well_defined()){
        sim.step(-1, nullptr, nullptr);
        std::cout << sim.currentState << "\n";
    }
}

template<Race gamerace>
void dfs(Simulator<gamerace>& sim, Node& currentNode){
    
}
