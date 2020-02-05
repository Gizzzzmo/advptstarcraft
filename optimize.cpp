#include "Simulator.h"
#include "Entity.h"
#include "Action.h"
#include "Node.h"

#include <string>
#include <iostream>
#include <map>
#include <array>
#include <cstdlib>
#include <chrono>
#include <cmath>

enum Scenario{Rush, Push};

std::array<EntityMeta, 64> meta_map;

int gas_id;
int worker_id;
int target_id;
std::vector<unsigned int> base_ids;
std::vector<unsigned int> building_ids = {};
int super_id;
int best_score;
int leaf_qualifier;
std::array<bool, 64> requirements;
std::array<bool, 64> producers;
bool gas_required = false;
std::list<Action> best_build;
long nodes_visited = 0;
std::vector<std::shared_ptr<Entity>> test;
std::vector<std::shared_ptr<ProductionEntry>> test1;

template<Scenario scenario, Race gamerace>
bool is_option_viable(int option_id, GameState& state){
    if(option_id == gas_id)return gas_required || target_id == gas_id;
    if(option_id == -1)return true;
    if(scenario == Scenario::Push){
        if(1 + state.time_tick + meta_map[option_id].build_time * (gamerace == Race::Protoss ? 2 : 3) / 3 > leaf_qualifier)return false;
    }
    int bases = 0;
    for(int base_id : base_ids){
        bases += state.entity_count[base_id];
    }
    if(option_id == 0)return bases < 5;
    int max_workers = bases * 16;
    max_workers += state.entity_count[gas_id] * 3;
    if(option_id == worker_id && state.entity_count[worker_id] > max_workers + 10)return false;
    if(option_id == target_id ||
        option_id == worker_id  ||
        (meta_map[option_id].supply_provided > 0 && state.final_supply < 14))
        return true;
    EntityMeta& option = meta_map[option_id];
    EntityMeta& target = meta_map[target_id];
    int num_of_producers = 0;
    for(int prd : mask_to_vector(target.production_mask)){
        num_of_producers += state.entity_count[prd] * meta_map[prd].units_produced;
    }
    if(producers[option_id] && 
        (scenario != Scenario::Rush || (leaf_qualifier > num_of_producers && leaf_qualifier > state.entity_count[option_id])))
        return true;
    if(requirements[option_id] && state.entity_count[option_id] < 1)return true;

    return false;
}

template<Scenario scenario, Race gamerace>
int random_play_through(Simulator<gamerace>& sim, int leaf_qualifier){
    GameState& currentState = sim.currentState;
    while(true){
        if (scenario == Scenario::Push) {
            if(sim.currentState.time_tick >= leaf_qualifier){
                break;
            }
        }
        else if(scenario == Scenario::Rush){
            if(sim.currentState.entitymap[target_id]->size() >= leaf_qualifier){
                break;
            }
        }

        std::cout << "time " << currentState.time_tick<<", options:\n";
        std::shared_ptr<Entity> caster = gamerace == Race::Protoss ? sim.get_caster() : nullptr;
        std::array<int, 64> options = {-1};
        if(!sim.worker_distribution_well_defined() && 
            (scenario != Scenario::Rush || currentState.entity_count[target_id] < leaf_qualifier)){
            options = sim.getOptions();
        }

        std::vector<int> viable_options;
        std::vector<double> weights;
        for(int option : options){
            std::cout << " option: " << (option == -1 ? "-1": meta_map[option].name) << ", is viable? " << is_option_viable<scenario, gamerace>(option, currentState) << "\n";
            if(is_option_viable<scenario, gamerace>(option, currentState)){
                viable_options.push_back(option);
                weights.push_back(1);
            }
            if(option == -1)break;
        }
        double sum = weights.size();
        //weigh options
        double r;
        double untilnow = 0;
        
        int chosen_target_class = -1;
        int chosen_target_id = -1;
        if(caster){
            std::vector<std::pair<int, int>> possible_targets = sim.get_chrono_targets();
            r = (possible_targets.size()+1) * ((double) std::rand() / (RAND_MAX));
            for(std::pair<int, int> possible_target : possible_targets){
                untilnow +=1;
                if(r <= untilnow){
                    chosen_target_class = possible_target.first;
                    chosen_target_id = possible_target.second;
                }
            }
        }

        r = sum * ((double) std::rand() / (RAND_MAX));
        untilnow = 0;
        int chosen_option = -1;
        if(chosen_target_class == -1 && weights.size() > 1)weights[weights.size()-2] += weights.back();
        for(int i = 0;i < viable_options.size();i++){
            untilnow += weights[i];
            if(r <= untilnow){
                chosen_option = viable_options[i];
                std::cout << " chosen option:" <<(chosen_option == -1 ? "-1": meta_map[chosen_option].name) << "\n";
                break;
            }
        }
        if(chosen_target_class != -1 || chosen_option != -1)sim.step(chosen_option, chosen_target_class, chosen_target_id);
        else{
            sim.step(-1, -1, -1);
        }
        if(currentState.time_tick > 500)break;
    }
    if(scenario == Scenario::Rush)return currentState.time_tick;
    else return currentState.entitymap[target_id]->size();
}

template<Scenario scenario, Race gamerace>
int mcts(Simulator<gamerace>& sim){
    Node* startNode = new Node{nullptr, {}, {-1, -1, -1}, 0, 0, 0};
    GameState rootState = sim.currentState;
    Node* currentRoot = startNode;

    int k = 0;
    while(true){
        k++;
        Node* currentNode = currentRoot;
        std::cout <<"selection " << k << ", time " << rootState.time_tick<< ", looking for next move:\n";
        for(int i = 0;i < 1500;i++){
            int currentdepth = 0;
            currentNode = currentRoot;
            std::cout << "  starting iteration number " << i <<", searching for leaf:\n";
            if(currentNode->visits == -1)break;
            while(currentNode->visits > 0){
                currentdepth++;
                currentNode->visits++;
                double max_ucb = 0;
                double maxScore = 0;
                bool nonleaf_found = false;
                std::cout << "    " << currentNode << " "<< currentNode->visits <<"\n";
                std::cout << "    number of possible children: " << currentNode->children.size() << "\n";
                for(Node* child : currentNode->children){
                    if(child->visits != -1)nonleaf_found = true;
                    if(child->avg_score > maxScore)maxScore = child->avg_score;
                }
                if(!nonleaf_found){
                    currentNode->visits = -1;
                    currentNode->avg_score = maxScore;
                    break;
                }
                Node* next_node = nullptr;
                if(maxScore == 0){
                    std::cout << "    randomly selecting child\n";
                    double r = currentNode->children.size() * ((double) std::rand() / (RAND_MAX));
                    int j = 1;
                    for(Node* child : currentNode->children){
                        if(r <= j){
                            next_node = child;
                            break;
                        }
                        j++;
                    }
                }
                else{
                    for(Node* child : currentNode->children){
                        std::cout << "      " << currentNode << " " << currentNode->visits<<"\n";
                        double ucb = 0.2*currentdepth*(child->avg_score/maxScore) +
                                std::sqrt(std::log(currentNode->visits)/std::max((double)child->visits, 0.01));
                        std::cout << "      child " << child << ", visits : " << child->visits << ", ucb: " << ucb << "\n";
                        std::cout << "      child " << std::log(currentNode->visits) << ", visits : " << std::log(currentNode->visits)/std::max((double)child->visits, 0.1) << ", ucb: " << child->avg_score << "\n";
                        if(ucb > max_ucb){
                            max_ucb = ucb;
                            next_node = child;
                        }
                    }
                }
                currentNode = next_node;
                std::cout << "    " << next_node << "\n";
                std::cout << "    selected move " << currentNode->action.entity_to_be_built_id << ", with ucb " << max_ucb <<"\n";
                if(currentNode->visits != -1){
                    sim.step(currentNode->action.entity_to_be_built_id,
                            currentNode->action.chrono_target_class,
                            currentNode->action.chrono_target_id);
                    for(int i = 0;i < currentNode->skips;i++){
                        sim.step(-1, -1, -1);
                    }
                }
            }
            currentNode->visits++;
            std::cout << "  found leaf, checking if actual game leaf:\n";
            double score = 0;
            if(currentNode->visits == -1)score = currentNode->avg_score;
            else{
                std::shared_ptr<Entity> caster = sim.get_caster();
                if((scenario == Scenario::Push && sim.currentState.time_tick == leaf_qualifier)
                    || scenario == Scenario::Rush && sim.currentState.entitymap[target_id]->size() == leaf_qualifier){
                    score = scenario == Scenario::Push ? sim.currentState.entitymap[target_id]->size()
                                                        : 1/((double)sim.currentState.time_tick + 1);
                    currentNode->visits = -1;
                    std::cout << "  yes! setting score " << score <<"\n";
                }
                else{
                    std::cout << "  no, expanding leaf, new leaves:\n";
                    GameState simStart = sim.currentState;
                    std::array<int, 64> options = {-1};
                    if(!sim.worker_distribution_well_defined())options = sim.getOptions();
                    bool sthhappened = false;
                    for(int option : options){
                        std::cout << "    is viable? " << option << "\n";
                        if(is_option_viable<scenario, gamerace>(option, sim.currentState)){
                            if(caster){
                                std::vector<std::pair<int, int>> targets = sim.get_chrono_targets();
                                for(std::pair<int, int>& target : targets){
                                    sim.currentState = simStart;
                                    sthhappened = true;
                                    std::cout << "    " << option << ", " << target.first << " " << target.second << "\n";
                                    sim.step(option, target.first, target.second);
                                    int skips = 0;
                                    while(sim.worker_distribution_well_defined() && 
                                        (sim.get_caster() == nullptr || sim.get_chrono_targets().size() == 0)){
                                        if((scenario == Scenario::Push && sim.currentState.time_tick == leaf_qualifier)
                                            || scenario == Scenario::Rush && sim.currentState.entitymap[target_id]->size() == leaf_qualifier){
                                            break;
                                        }
                                        skips++;
                                        sim.step(-1, -1, -1);
                                    }
                                    currentNode->children.push_back(new Node{currentNode, {}, {option, target.first, target.second}, 0, 0, skips});
                                }
                            }
                            if(option == -1 && sthhappened)break;
                            sim.currentState = simStart;
                            sthhappened = true;
                            sim.step(option, -1, -1);
                            int skips = 0;
                            while(sim.worker_distribution_well_defined() && 
                                (sim.get_caster() == nullptr || sim.get_chrono_targets().size() == 0)){
                                if((scenario == Scenario::Push && sim.currentState.time_tick == leaf_qualifier)
                                    || scenario == Scenario::Rush && sim.currentState.entitymap[target_id]->size() == leaf_qualifier){
                                    break;
                                }
                                skips++;
                                sim.step(-1, -1, -1);
                            }
                            std::cout << "    " << option << ", " << -1 << " " << -1 << "\n";
                            currentNode->children.push_back(new Node{currentNode, {}, {option, -1, -1}, 0, 0, skips});
                        }
                        if(option == -1)break;
                    }
                    std::cout << "  doing random playouts:\n";
                    for(int j = 0;j < 2;j++){
                        sim.currentState = simStart;
                        double ss = random_play_through<scenario>(sim, leaf_qualifier);
                        std::cout << "    score: " << ss << "\n";
                        score += ss;
                    }
                    score /= 2;
                    std::cout << "  average score: " << score << "\n";
                    if(scenario == Scenario::Rush)score =1/(score +1);
                }
                currentNode->avg_score = score;
            }
            std::cout << "  backpropagating:\n";
            if(currentRoot != currentNode)currentNode = currentNode->parent;
            while(currentNode != currentRoot){
                std::cout << "    " << currentNode << " " << currentRoot << "\n";
                currentNode->avg_score = (currentNode->avg_score*currentNode->visits + score)/(currentNode->visits+1);
                currentNode = currentNode->parent;
            }
            sim.currentState = rootState;
        }
        std::cout << "selecting child with best score:\n";
        double max_score = 0;
        for(Node* child : currentRoot->children){
            if(child->avg_score > max_score){
                max_score = child->avg_score;
                currentRoot = child;
            }
        }
        std::cout << "time: " << sim.currentState << "\n";
        std::cout << "selected move " << currentRoot->action.entity_to_be_built_id<<" "<<
                    currentRoot->action.chrono_target_class << " " <<
                    currentRoot->action.chrono_target_id << " with score " << max_score << "\n";

        sim.step(currentRoot->action.entity_to_be_built_id,
            currentRoot->action.chrono_target_class,
            currentRoot->action.chrono_target_id);
        if((scenario == Scenario::Push && sim.currentState.time_tick == leaf_qualifier)
            || scenario == Scenario::Rush && sim.currentState.entitymap[target_id]->size() == leaf_qualifier){
            return scenario == Scenario::Push ? sim.currentState.entitymap[target_id]->size()
                                                : sim.currentState.time_tick;
        }
        for(int i = 0;i < currentRoot->skips;i++){
            sim.step(-1, -1, -1);
            if((scenario == Scenario::Push && sim.currentState.time_tick == leaf_qualifier)
                || scenario == Scenario::Rush && sim.currentState.entitymap[target_id]->size() == leaf_qualifier){
                return scenario == Scenario::Push ? sim.currentState.entitymap[target_id]->size()
                                                    : sim.currentState.time_tick;
            }
        }
        rootState = sim.currentState;
    }
}

template<Scenario scenario, Race gamerace>
void dfs(Simulator<gamerace>& sim, std::list<Action>& build){
    if(nodes_visited++ % 100 == 0)std::cout << "nodes visited: " << nodes_visited << "\n";
    if (scenario == Scenario::Push) {
        if(sim.currentState.time_tick >= leaf_qualifier){
            if(sim.currentState.entitymap[target_id]->size() > best_score){
                best_score = sim.currentState.entitymap[target_id]->size();
                best_build = build;
            }
            return;
        }
    }
    else if(scenario == Scenario::Rush){
        if(sim.currentState.entitymap[target_id]->size() >= leaf_qualifier){
            if(sim.currentState.time_tick < best_score){
                best_score = sim.currentState.time_tick;
                best_build = build;
            }
            return;
        }
    }

    GameState currentState(sim.currentState);
    std::shared_ptr<Entity> caster = gamerace == Race::Protoss ? sim.get_caster() : nullptr;
    std::array<int, 64> options = {-1};
    if(!sim.worker_distribution_well_defined() && 
        (scenario != Scenario::Rush || currentState.entity_count[target_id] < leaf_qualifier))options = sim.getOptions();

    //std::cout << sim.currentState << "\n";
    //while(sim.worker_distribution_well_defined()) {
    //	GameState currentState = sim.currentState;
        //Cast options
    //}
    bool something_happened = false;
    /*std::cout << "current build:\n";
    for(Action& a : build){
        std::cout << "  " <<a.entity_to_be_built_id << ", " << a.chrono_target_class << "\n";
    }
    std::cout << "options:\n";
    for(int op : options){
        std::cout << "  " << op << "\n";
        if(op == -1)break;
    }*/
    //Build options
    for(int ent_id : options) {
        /*std::cout << "trying option " << ent_id << "\n";
        std::cout << "is viable: " << is_option_viable<scenario, gamerace>(ent_id, currentState) << "\n";*/
        if(is_option_viable<scenario, gamerace>(ent_id, currentState)){
            if(caster) {
                std::vector<std::pair<int, int>> targets = sim.get_chrono_targets();
                for(std::pair<int, int>& target : targets) {
                    something_happened = true;
                    sim.currentState = currentState;
                    sim.step(ent_id, target.first, target.second);

                    build.push_back({ent_id, target.first, target.second});
                    dfs<scenario>(sim, build);
                    build.pop_back();
                }
            }
            if(ent_id == -1)break;

            something_happened = true;

            sim.currentState = currentState;
            sim.step(ent_id, -1, -1);
            build.push_back({ent_id, -1});
            dfs<scenario>(sim, build);
            build.pop_back();
        }
    }

    //Cast options
    if(!something_happened && (sim.worker_distribution_well_defined() || sim.currentState.entity_count[target_id] == leaf_qualifier || scenario == Scenario::Push)) {
        do{
            sim.step(-1, -1, -1);
            if(scenario == Scenario::Push && sim.currentState.time_tick == leaf_qualifier)break;
            if(scenario == Scenario::Rush && sim.currentState.entitymap[target_id]->size() == leaf_qualifier)break;
        }
        while((sim.worker_distribution_well_defined() || sim.currentState.entity_count[target_id] == leaf_qualifier) && sim.get_caster() == nullptr);
        dfs<scenario>(sim, build);
    }
    else return;
}

template<Race gamerace>
void construct_producers(int entity_id);

template<Race gamerace>
void construct_requirements(int entity_id){
    const EntityMeta& info = meta_map[entity_id];
    if(info.gas > 0)gas_required = true;
    if(info.requirement_mask == 0)return;
    for(int req : mask_to_vector(info.requirement_mask)){
        if(requirements[req] || 
            (gamerace == Race::Terran && 
                (req == 12 || req == 16 || req == 21 || req == 20 || req == 32 || req == 33)))
            continue;
        requirements[req] = true;
        construct_requirements<gamerace>(req);
        construct_producers<gamerace>(req);
        requirements[req] = true;
    }
}

template<Race gamerace>
void construct_producers(int entity_id){
    const EntityMeta& info = meta_map[entity_id];
    if(info.gas > 0)gas_required = true;
    for(int prd : mask_to_vector(info.production_mask)){
        if(producers[prd])continue;
        producers[prd] = true;
        construct_producers<gamerace>(prd);
        construct_requirements<gamerace>(prd);
        producers[prd] = true;
    }
}

template<Race gamerace, Scenario scenario>
void optimize(const GameState& initialState)
{
    Simulator<gamerace> sim(meta_map, initialState, gas_id, worker_id, base_ids, building_ids, super_id);
    if(scenario == Scenario::Push)best_score = 0;
    else best_score = 1001;
    std::list<Action> empty_build = {};
    requirements.fill(false);
    producers.fill(false);
    construct_requirements<gamerace>(target_id);
    construct_producers<gamerace>(target_id);

    if(gamerace == Race::Terran){
        if(target_id != 2 && target_id != 5 && target_id != 18){
            requirements[3] = false;//engineering bay is only ever required when targeting either a turret, planetary, or sensor tower
        }
        requirements[2] = false;
        producers[2] = false;
        producers[3] = false;
    }
    else if(gamerace == Race::Zerg){
        requirements[28] = false;
        producers[28] = false;
    }
    std::chrono::milliseconds ms = std::chrono::duration_cast< std::chrono::milliseconds >(
        std::chrono::system_clock::now().time_since_epoch()
    );
    std::srand(ms.count());
    double score = 0;
    std::cout << score/1000 <<" ???\n";
    GameState simStart = sim.currentState;
    for(int i = 0;i < 1000;i++){
        double ss = random_play_through<scenario>(sim, leaf_qualifier);
        //std::cout << " " << ss << "\n";
        score += ss;
        sim.currentState = simStart;
    }
    sim.currentState = initialState;
    std::cout << score/1000 <<" ???\n";
    score = 0;
    simStart = sim.currentState;
    for(int i = 0;i < 1000;i++){
        double ss = random_play_through<scenario>(sim, leaf_qualifier);
        //std::cout << " " << ss << "\n";
        score += ss;
        sim.currentState = simStart;
    }
    std::cout << score/1000 <<" ???\n";
    sim.currentState = initialState;
    best_score = mcts<scenario>(sim);


    std::cout << "best score: " << best_score << "\n with build:\n";
    for(Action& a : best_build){
        std::cout << a.entity_to_be_built_id << ", chrono target: " << a.chrono_target_id << "\n";
    }

}

int main(int argc, char** argv){
    std::string scenario(argv[1]), target_unit(argv[2]);

    leaf_qualifier = std::stoi(argv[3]);
    Race gamerace;
    std::map<std::string, int> name_map;

    unsigned int supply = 15;
    unsigned int max_id = 0;

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
            std::shared_ptr<Entity> a(new Entity(meta_map, 5, 0, max_id));
            entitymap[5]->push_back(a);
        }
        std::shared_ptr<Entity> a(new Entity(meta_map, 0, 0, max_id));
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
            	std::shared_ptr<Entity> a(new Entity(meta_map, 4, 0, max_id));
                entitymap[4]->push_back(a);
            }
            std::shared_ptr<Entity> a(new Entity(meta_map, 0, 0, max_id));
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
            	std::shared_ptr<Entity> a(new Entity(meta_map, 9, 0, max_id));
                entitymap[9]->push_back(a);
            }
            std::shared_ptr<Entity> hatch(new Entity(meta_map, 0, 0, max_id));
            entitymap[0]->push_back(hatch);
            std::shared_ptr<Entity> ovi(new Entity(meta_map, 16, 0, max_id));
            entitymap[16]->push_back(ovi);
            supply = 14;
        }
    }

    target_id = name_map[target_unit];
    const GameState initialState(0, 5000, 0, supply, 12, 12, 12, 0, 0, 3, true, max_id, entitymap, {}, {});

    switch(gamerace){
        case Protoss:
            if(!scenario.compare("rush")){
                optimize<Race::Protoss, Scenario::Push>(initialState);
            }
            else if(!scenario.compare("push")){
                optimize<Race::Protoss, Scenario::Rush>(initialState);
            }
            break;
        case Terran:
            if(!scenario.compare("rush")){
                optimize<Race::Terran, Scenario::Push>(initialState);
            }
            else if(!scenario.compare("push")){
                optimize<Race::Terran, Scenario::Rush>(initialState);
            }
            break;
        case Zerg:
            if(!scenario.compare("rush")){
                optimize<Race::Zerg, Scenario::Push>(initialState);
            }
            else if(!scenario.compare("push")){
                optimize<Race::Zerg, Scenario::Rush>(initialState);
            }
            break;
    }
    return 0;
}

