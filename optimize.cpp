#include "Simulator.h"
#include "Action.h"

#include <string>
#include <iostream>
#include <map>
#include <array>

enum Scenario{Rush, Push};

int target_id;
int best_score;
int leaf_qualifier;
std::list<Action> best_build;
long nodes_visited = 0;
std::vector<std::shared_ptr<Entity>> test;



template<Scenario scenario, Race gamerace>
void dfs(Simulator<gamerace>& sim, std::list<Action>& build){
    std::cout << "nodes visited: " << nodes_visited++ << "\n";
    if (scenario == Scenario::Push) {
        if(sim.currentState.time_tick == leaf_qualifier){
            std::cout << sim.currentState.time_tick << ", score:" << sim.currentState.entitymap[target_id]->size() << "\n";
            if(sim.currentState.entitymap[target_id]->size() > best_score){
                best_score = sim.currentState.entitymap[target_id]->size();
                best_build = build;
            }
            return;
        }
    }
    else if(scenario == Scenario::Rush){
        if(sim.currentState.entity_count[target_id] == leaf_qualifier){
            if(sim.currentState.time_tick < best_score){
                best_score = sim.currentState.time_tick;
                best_build = build;
            }
            return;
        }
    }

    std::shared_ptr<Entity> caster = gamerace == Race::Protoss ? sim.get_caster() : nullptr;
    GameState currentState(sim.currentState);
    for(int i = 0;i < 64;i++){
        for(auto& entry : *currentState.entitymap[i]){
            std::cout << "use coutn of " << entry->id() << " afterwards: " << entry.use_count() << "\n";
            test.push_back(entry);
        }
    }
    std::array<int, 64> options = {-1};
    if(!sim.worker_distribution_well_defined())options = sim.getOptions();

    std::cout << sim.currentState << "\n";
    //while(sim.worker_distribution_well_defined()) {
    //	GameState currentState = sim.currentState;
        //Cast options
    //}
    bool something_happened = false;

    //Build options
    for(int ent_id : options) {
        if(caster) {
            std::list<std::shared_ptr<Entity>> targets = sim.get_chrono_targets();
            for(std::shared_ptr<Entity> target : targets) {
                something_happened = true;
                sim.currentState = currentState;
                for(int i = 0;i < 64;i++){
                    for(auto& entity : *sim.currentState.entitymap[i]){
                        std::cout << " use count of entity "<< entity->id() << " "<< entity.use_count() << "\n";
                        for(auto& entry : entity->producees){
                            std::cout << "  use count of associated production entry " << entry->producee->id() << " " << entry.use_count() << "\n";
                        }
                    }
                }
                for(auto& entry : sim.currentState.production_list){
                    std::cout << " use count of production entry" << entry->producee->id() << " " << entry.use_count() << "\n";
                }
                sim.step(ent_id, target, caster);
                build.push_back({ent_id, target->obj_id});
                dfs<scenario>(sim, build);
                build.pop_back();
            }
        }
        if(ent_id == -1)break;

        something_happened = true;

        sim.currentState = currentState;
        for(int i = 0;i < 64;i++){
            for(auto& entity : *sim.currentState.entitymap[i]){
                std::cout << " use count of entity "<< entity->id() << " "<< entity.use_count() << "\n";
            }
        }
        for(auto& entry : sim.currentState.production_list){
            std::cout << " use count of production entry " << entry->producee->id() << " " << entry.use_count() << "\n";
            std::cout << "  and of producer " << entry->producer->id() << " ??? " << entry->producer.use_count() << "\n";
        }
        sim.step(ent_id, nullptr, caster);
        build.push_back({ent_id, -1});
        dfs<scenario>(sim, build);
        build.pop_back();
    }

    //Cast options
    if(!something_happened) {
        sim.step(-1, nullptr, nullptr);
        dfs<scenario>(sim, build);
    }
    
}

template<Race gamerace, Scenario scenario>
void optimize(const std::array<EntityMeta, 64>& meta_map,
    const GameState& initialState,
    const int gas_id,
    const int worker_id,
    const std::vector<unsigned int>& base_ids,
    const std::vector<unsigned int>& building_ids,
    const int super_id)
{
    Simulator<gamerace> sim(meta_map, initialState, gas_id, worker_id, base_ids, building_ids, super_id);
    if(scenario == Scenario::Push)best_score = 0;
    else best_score = 1001;
    std::list<Action> empty_build = {};
    dfs<scenario>(sim, empty_build);

    std::cout << "best score: " << best_score << "\n";

    for(auto& entry : test){
        if(entry.use_count() > 1)std::cout << "blub " << entry << ": " << entry->id() << ", use count: " << entry.use_count() <<"\n";
    }

    /*
    std::cout << sim.currentState << "\n";
    std::array<int, 64> options = sim.getOptions();
    for(int i = 0;i < 64;i++){
        std::cout << "option: " << options[i] << "\n";
        if(options[i] == -1)break;
        std::cout << "        " << meta_map[options[i]].name << "\n";
    }
    sim.step(0, nullptr, nullptr);
    std::cout << sim.currentState << "\n";
    while(sim.worker_distribution_well_defined()){
        sim.step(-1, nullptr, nullptr);
        std::cout << sim.currentState << "\n";
    }*/
}

int main(int argc, char** argv){
    std::string scenario(argv[1]), target_unit(argv[2]);

    leaf_qualifier = std::stoi(argv[3]);
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

    target_id = name_map[target_unit];
    const GameState initialState(0, 5000, 0, supply, 12, 12, 12, 0, 0, 12, true, entitymap, {}, {});

    switch(gamerace){
        case Protoss:
            if(!scenario.compare("rush")){
                optimize<Race::Protoss, Scenario::Rush>(meta_map, initialState, gas_id, worker_id, base_ids, building_ids, super_id);
            }
            else if(!scenario.compare("push")){
                optimize<Race::Protoss, Scenario::Push>(meta_map, initialState, gas_id, worker_id, base_ids, building_ids, super_id);
            }
            break;
        case Terran:
            if(!scenario.compare("rush")){
                optimize<Race::Terran, Scenario::Rush>(meta_map, initialState, gas_id, worker_id, base_ids, building_ids, super_id);
            }
            else if(!scenario.compare("push")){
                optimize<Race::Terran, Scenario::Push>(meta_map, initialState, gas_id, worker_id, base_ids, building_ids, super_id);
            }
            break;
        case Zerg:
            if(!scenario.compare("rush")){
                optimize<Race::Zerg, Scenario::Rush>(meta_map, initialState, gas_id, worker_id, base_ids, building_ids, super_id);
            }
            else if(!scenario.compare("push")){
                optimize<Race::Zerg, Scenario::Push>(meta_map, initialState, gas_id, worker_id, base_ids, building_ids, super_id);
            }
            break;
    }
    return 0;
}

