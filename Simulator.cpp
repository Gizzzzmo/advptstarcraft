#include "Simulator.h"
// Simulator
// private object methods

template<Race gamerace>
inline std::list<std::shared_ptr<ProductionEntry>> Simulator<gamerace>::process_production_list(){
    std::list<std::shared_ptr<ProductionEntry>> result;
    for(std::list<std::shared_ptr<ProductionEntry>>::iterator it = currentState.production_list.begin(); it != currentState.production_list.end();) {
        std::shared_ptr<ProductionEntry> entry = *it;
        auto prev = it;
        ++it;
        if((entry->time_done + 5)/6 == currentState.time_tick) {
            //TODO: Add to enity list
            if(entry->producee->is_worker())currentState.workers_available++;
            currentState.entitymap[(entry->producee)->class_id()]->push_back(entry->producee);
            if(is_zergling(entry->producee->class_id())) 
                currentState.entitymap[(entry->producee)->class_id()]->push_back(entry->second_producee);
            std::shared_ptr<std::list<std::shared_ptr<Entity>>> producers = currentState.entitymap[(entry->producer)->class_id()];
            //std::list<Entity*>* producers = currentState.entitymap[(entry->producer)->class_id()];
            switch(entry->producee->producer_destiny()) {
                case consumed_at_end:
                    for(std::list<std::shared_ptr<Entity>>::iterator it = producers->begin();it != producers->end();++it){
                        if(*it == entry->producer){
                            if (gamerace == Zerg) entry->producee->finalize(entry->producer);
                            producers->erase(it);
                            break;
                        }
                    }
                    break;
                case occupied:
                    entry->producer->make_available(currentState);
                    break;
                case freed:
                    break;
                case consumed_at_start:
                    break;
            }
            currentState.production_list.erase(prev);
            if(gamerace == Race::Protoss){
                for(auto it = entry->producer->producees.begin(); it != entry->producer->producees.end();++it){
                    if(*it == entry){
                        entry->producer->producees.erase(it);
                        break;
                    }
                }
            }
            currentState.supply += entry->producee->supply_provided();
            result.push_back(entry);
        }
    }
    return result;
}

template<Race gamerace>
void Simulator<gamerace>::update_resources(){
    unsigned int current_mineral_workers = 10*currentState.mineral_worker;
    if(gamerace == Terran) {
        if(!currentState.timeout_mule.empty()) {
            for (int timeout : currentState.timeout_mule) {
                if(timeout >= currentState.time_tick)
                /* MULEs require no other resources or supply and do not conflict
                 * with harvesting SCVs, but provide the yield of 3.8 SCVs. */
                    current_mineral_workers += 38; //TODO: Unit?
                //else
                    //currentState.timeout_mule.remove(timeout); //Segmentation fault
            }
        }
    }
    currentState.minerals += 7*current_mineral_workers;
    currentState.gas += 63*currentState.gas_worker;
}

template<Race gamerace>
bool Simulator<gamerace>::needs_larva(int class_id){
    return gamerace == Race::Zerg && meta_map[class_id].production_mask == 0x0000000010000000;
}

template<Race gamerace>
bool Simulator<gamerace>::is_zergling(int class_id){
    return gamerace == Zerg && ( meta_map[class_id].name == "Zergling" );
}

template<Race gamerace>
void Simulator<gamerace>::error_exit(std::string message, json& output) {
    output["buildlistValid"] = 0;
    if(DEBUG){
        std::cerr << message << "\n";
    }
}

template<Race gamerace>
std::shared_ptr<ProductionEntry> Simulator<gamerace>::check_and_build(int class_id){
    if(class_id == gas_id && currentState.entitymap[class_id]->size() == number_of_bases()*2)throw noGasGeyserAllowedException(); //TODO: Fix dis
    if(meta_map[class_id].supply > static_cast<int>(currentState.supply-currentState.supply_used)) throw noSupplyException();
    //bitmask of 0 is interpreted as there not being any requirements, since all entities should be buildable somehow
    if(meta_map[class_id].requirement_mask == 0)goto req_fulfilled;
    for(int req_id : mask_to_vector(meta_map[class_id].requirement_mask)){
        if(!(currentState.entitymap[req_id]->empty()))goto req_fulfilled;
    }
    throw requirementNotFulfilledException();
    req_fulfilled:
    if(currentState.gas < meta_map[class_id].gas) throw noGasException();
    if(currentState.minerals < meta_map[class_id].minerals)throw noMineralsException();

    std::vector<unsigned> producer_types;
    if(needs_larva(class_id))
        producer_types = base_ids;
    else
        producer_types = mask_to_vector(meta_map[class_id].production_mask);

    //possibly search for chronoboosted producers?
    for(int prd_id : producer_types){
        std::shared_ptr<std::list<std::shared_ptr<Entity>>> possible_producers = currentState.entitymap[prd_id];
        for(std::list<std::shared_ptr<Entity>>::iterator it = possible_producers->begin();it != possible_producers->end();++it){
            //std::cout << meta_map[prd_id].name << " is valid producer for " << meta_map[class_id].name << "\n";
            std::shared_ptr<Entity> producer = *it;
            if((needs_larva(class_id) && producer->decrement_larva(currentState)) 
                    || (!needs_larva(class_id) && producer->check_and_occupy())){
                if(producer->is_worker())
                    currentState.workers_available--;
                if(meta_map[class_id].producer_destiny == Destiny::freed)
                    producer->make_available(currentState);
                if(meta_map[class_id].producer_destiny == Destiny::consumed_at_start && !needs_larva(class_id))
                    possible_producers->erase(it);
                currentState.minerals -= meta_map[class_id].minerals;
                currentState.gas -= meta_map[class_id].gas;
                currentState.supply_used += meta_map[class_id].supply;
                std::shared_ptr<Entity> producee(new Entity(meta_map, class_id, currentState.time_tick, currentState.max_id));
                std::shared_ptr<ProductionEntry> e(new ProductionEntry(producee, producer, currentState));

                if (is_zergling(class_id)){
                    std::shared_ptr<Entity> second_producee(new Entity(meta_map, class_id, currentState.time_tick, currentState.max_id));
                    e->second_producee = second_producee;
                }

                if(producer->is_chrono_boosted(currentState))e->chrono_boost(currentState, producer->get_chrono_until());
                if(gamerace == Race::Protoss)producer->producees.push_back(e);
                return e;
            }
        }
    }
    if(needs_larva(class_id))
        throw noLarvaAvailableException();
    else
        throw noProducerAvailableException();
    return nullptr;
}

template<Race gamerace>
inline json Simulator<gamerace>::make_json_message(const GameState &currentGameState, const std::vector<json> events) {
    json message;
    message["time"] = currentState.time_tick;
    message["status"] = {};
    message["status"]["workers"] = {};
    message["status"]["workers"]["minerals"] = currentState.mineral_worker;
    message["status"]["workers"]["vespene"] = currentState.gas_worker;
    message["status"]["resources"] = {};
    message["status"]["resources"]["minerals"] = currentState.minerals/100;
    message["status"]["resources"]["vespene"] = currentState.gas/100;
    message["status"]["resources"]["supply-used"] = currentState.supply_used;
    message["status"]["resources"]["supply"] = currentState.supply;
    message["events"] = events;
    return message;
}

template<Race gamerace>
inline void Simulator<gamerace>::generate_json_build_end(std::vector<json> &events, std::list <std::shared_ptr<ProductionEntry>> &finished_list) {
    for(std::shared_ptr<ProductionEntry> entry : finished_list){
        json event;
        event["type"] = "build-end";
        event["name"] = meta_map[entry->producee->class_id()].name;
        event["producerID"] = entry->producer->id();
        event["producedIDs"] = {entry->producee->id()};

        if(is_zergling(entry->producee->class_id()))
            event["producedIDs"].push_back(entry->second_producee->id());

        events.push_back(event);
    }
}

template<Race gamerace>
inline void Simulator<gamerace>::generate_json_build_start(std::vector<json> &events, const std::shared_ptr<ProductionEntry> &entry) {
    json build_start_event;
    build_start_event["type"] = "build-start";
    build_start_event["name"] = meta_map[entry->producee->class_id()].name;
    build_start_event["producerID"] = entry->producer->id();
    events.push_back(build_start_event);
}

template<Race gamerace>
inline bool Simulator<gamerace>::update_worker_distribution(std::vector<int>& lines, int current_line) {
    
    int cost_mins = 0, cost_gas = 0;
    int gas = (int) currentState.gas;
    int mins = (int) currentState.minerals;
    
    do{
        if(current_line >= lines.size())break;
        cost_mins += meta_map[lines[current_line]].minerals;
        cost_gas += meta_map[lines[current_line]].gas;
        
        current_line++;
    }while(mins >= cost_mins && gas >= cost_gas);
    int missing_mins = std::max(0, cost_mins - mins);
    int missing_gas = std::max(0, cost_gas - gas);
    unsigned int ideal_mineral_worker = missing_mins + missing_gas == 0 ? currentState.workers_available : 
        currentState.workers_available * 63 * missing_mins/(missing_mins * 63 + missing_gas * 70);
    unsigned int ideal_gas_worker = currentState.workers_available - ideal_mineral_worker;

    unsigned int max_gas_worker = static_cast<unsigned int>(currentState.entitymap[gas_id]->size()*3);
    unsigned int max_mineral_worker = number_of_bases()*16;
    unsigned int new_gas_worker = std::min(max_gas_worker, ideal_gas_worker);
    unsigned int new_mineral_worker = std::min(max_mineral_worker, ideal_mineral_worker);
    new_gas_worker = 
        std::min(new_gas_worker + currentState.workers_available - new_gas_worker - new_mineral_worker, max_gas_worker);
    new_mineral_worker = 
        std::min(new_mineral_worker + currentState.workers_available - new_gas_worker - new_mineral_worker, max_mineral_worker);
    
    if(new_gas_worker != currentState.gas_worker || new_mineral_worker != currentState.mineral_worker){
        currentState.gas_worker = new_gas_worker;
        currentState.mineral_worker = new_mineral_worker;
        return true;
    }
    else return false;
}

template<Race gamerace>
inline bool Simulator<gamerace>::worker_distribution_well_defined(){
    int cost_mins = 0, cost_gas = 0;
    int gas = (int) currentState.gas;
    int mins = (int) currentState.minerals;
    auto it = currentState.build_list.begin();
    
    do{
        if(it == currentState.build_list.end())return false;
        int next_entity = *it;
        cost_mins += meta_map[next_entity].minerals;
        cost_gas += meta_map[next_entity].gas;
        
        ++it;
    }while(mins >= cost_mins && gas >= cost_gas);
    return true;
}

template<Race gamerace>
inline bool Simulator<gamerace>::update_worker_distribution() {
    int cost_mins = 0, cost_gas = 0;
    int gas = (int) currentState.gas;
    int mins = (int) currentState.minerals;
    auto it = currentState.build_list.begin();
    
    do{
        if(it == currentState.build_list.end())return false;
        int next_entity = *it;
        cost_mins += meta_map[next_entity].minerals;
        cost_gas += meta_map[next_entity].gas;
        
        ++it;
    }while(mins >= cost_mins && gas >= cost_gas);
    int missing_mins = std::max(0, cost_mins - mins);
    int missing_gas = std::max(0, cost_gas - gas);
    unsigned int ideal_mineral_worker = currentState.workers_available * 63 * missing_mins/(missing_mins * 63 + missing_gas * 70);
    unsigned int ideal_gas_worker = currentState.workers_available - ideal_mineral_worker;
    
    unsigned int max_gas_worker = static_cast<unsigned int>(currentState.entitymap[gas_id]->size()*3);
    unsigned int max_mineral_worker = number_of_bases()*16;
    unsigned int new_gas_worker = std::min(max_gas_worker, ideal_gas_worker);
    unsigned int new_mineral_worker = std::min(max_mineral_worker, ideal_mineral_worker);
    new_gas_worker = 
        std::min(new_gas_worker + currentState.workers_available - new_gas_worker - new_mineral_worker, max_gas_worker);
    new_mineral_worker = 
        std::min(new_mineral_worker + currentState.workers_available - new_gas_worker - new_mineral_worker, max_mineral_worker);
    
    if(new_gas_worker != currentState.gas_worker || new_mineral_worker != currentState.mineral_worker){
        currentState.gas_worker = new_gas_worker;
        currentState.mineral_worker = new_mineral_worker;
        return true;
    }
    else return false;
}



template<Race gamerace>
unsigned int Simulator<gamerace>::number_of_bases(){
    unsigned int sum = 0;
    for(unsigned int id : base_ids){
        //std::cout << meta_map[id].name << "exists " << currentState.entitymap[id]->size() << " many times\n";
        sum += currentState.entitymap[id]->size();
    }
    //std::cout << "total number of bases: " << sum << "\n";
    return sum;
}

template<Race gamerace>
unsigned int Simulator<gamerace>::number_of_future_bases(){
    unsigned int sum = 0;
    for(unsigned int id : base_ids){
        //std::cout << meta_map[id].name << "exists " << currentState.entitymap[id]->size() << " many times\n";
        sum += currentState.entity_count[id];
    }
    //std::cout << "total number of bases: " << sum << "\n";
    return sum;
}

template<Race gamerace>
void Simulator<gamerace>::update_energy(){
    for(std::shared_ptr<Entity> special_unit : *currentState.entitymap[super_id]){
        special_unit->updateEnergy();
    }
}

// Simulator
// public object methods
template<Race gamerace>
std::vector<std::pair<int, int>> Simulator<gamerace>::get_chrono_targets(){
    std::vector<std::pair<int, int>> targets;
    for(unsigned int building_id : building_ids){
        for(auto building : *currentState.entitymap[building_id]){
            if(!building->is_chrono_boosted(currentState) && building->occupied < building->max_occupation()){
                targets.push_back({building_id, building->obj_id});
            }
        }
    }
    return targets;
}

template<Race gamerace>
std::shared_ptr<Entity> Simulator<gamerace>::get_caster(){
    for(std::shared_ptr<Entity> specialunit : *currentState.entitymap[super_id]){
        if(specialunit->can_cast()){
            return specialunit;
        }
    }
    return nullptr;
}

template<Race gamerace>
std::array<int, 64> Simulator<gamerace>::getOptions(){
    std::array<int, 64> result_list;
    int j = 0;
    for(int i = 0;i < meta_map.size();i++){
        const EntityMeta& meta = meta_map[i];
        if(meta.minerals == 2)break;
        bool req_fulfilled = false;
        if(meta.requirement_mask == 0)req_fulfilled = true;
        else{
            for(int req_id : mask_to_vector(meta.requirement_mask)){
                if(currentState.entity_count[req_id] > 0){
                    req_fulfilled = true;
                    break;
                }
            }
        }

        bool producer_available = false;
        if(meta.production_mask == 0 || (gamerace == Race::Zerg && meta.production_mask == 0x0000000010000000))
            producer_available = true;
        else{
            for(int prd_id : mask_to_vector(meta.production_mask)){
                if(currentState.entity_count[prd_id] > 0){
                    producer_available = true;
                    break;
                }
            }
        }
        if((meta.gas == 0 || currentState.gas_geysers_available > 0)
                && (meta.supply <= currentState.final_supply)
                && req_fulfilled
                && producer_available
                && (i != gas_id || currentState.gas_geysers_available < number_of_future_bases()*2))
        {
            result_list[j++] = i;
        }
    }
    if(j < result_list.size())result_list[j] = -1;
    return result_list;
}

template<Race gamerace>
void Simulator<gamerace>::step(int entity_id, int cast_target_class_id, int cast_target_obj_id){
    currentState.time_tick++;
    if(entity_id > -1){
        if(entity_id == gas_id)currentState.gas_geysers_available++;
        currentState.final_supply = currentState.final_supply - meta_map[entity_id].supply + meta_map[entity_id].supply_provided;
        currentState.entity_count[entity_id] += meta_map[entity_id].units_produced;
        currentState.build_list.push_back(entity_id);
        if(meta_map[entity_id].producer_destiny == Destiny::consumed_at_end || meta_map[entity_id].producer_destiny == Destiny::consumed_at_start){
            currentState.entity_count[mask_to_vector(meta_map[entity_id].production_mask).front()]--;
        }
    }

    if(DEBUG) std::cout << "  time: " << currentState.time_tick << "\n";
    bool generate_json = false;
    std::vector<json> events;

    update_resources();
    update_energy();


    //Check finished buildings
    std::list<std::shared_ptr<ProductionEntry>> finished_list = process_production_list();
    if (is_recording && !finished_list.empty()){
        generate_json = true;
        generate_json_build_end(events, finished_list);
    }

    //Start new buildings

    std::shared_ptr<ProductionEntry> entry = nullptr;
    if(gamerace == Race::Protoss && cast_target_class_id >= 0){
        std::shared_ptr<Entity> target;
        for(auto& potential_target : *currentState.entitymap[cast_target_class_id]){
            if(potential_target->obj_id == cast_target_obj_id)target = potential_target;
        }
        if(target){
            auto caster = get_caster();
            caster->cast_if_possible();
            if(is_recording){
                generate_json = true;
                json special_event;
                special_event["type"] = "special";
                special_event["triggeredBy"] = caster->id();
                special_event["name"] = "chronoboost";
                special_event["targetBuilding"] = target->id();
            }
            target->chrono_boost(currentState);
            for(std::shared_ptr<ProductionEntry>& entry : target->producees){
                entry->chrono_boost(currentState, target->get_chrono_until());
            }
        }
            
        //special_event["name"] = "chronoboost";
        //special_event["targetBuilding"] = currentState.entitymap[0]->front()->id();
    }
    else if(!currentState.build_list.empty()){
        try{
            entry = check_and_build(currentState.build_list.front());
            currentState.build_list.erase(currentState.build_list.begin());
            currentState.built = true;
        }catch(noMineralsException& e){
            //std::cout << "no mins\n";
            currentState.built = false;
        }catch(noGasException& e){
            //std::cout << "no gas\n";
            currentState.built = false;
        }catch(noSupplyException& e){
            //std::cout << "no supply\n";
            currentState.built = false;
        }catch(noProducerAvailableException& e){
            //std::cout << "no prod\n";
            currentState.built = false;
        }catch(requirementNotFulfilledException& e){
            //std::cout << "missing req\n";
            currentState.built = false;
        }
        catch(noGasGeyserAllowedException& e){
            currentState.built = false;
        }
        catch(noLarvaAvailableException& e){
            currentState.built = false;
        }
    }
    if(entry) {
        currentState.production_list.push_back(entry);
        if(is_recording){
            generate_json = true;
            generate_json_build_start(events, entry);
        }
    }
    else if(gamerace != Race::Protoss){
        for(std::shared_ptr<Entity>& specialunit : *currentState.entitymap[super_id]){
            if(specialunit->cast_if_possible()){
                json special_event;
                if(is_recording){
                    special_event["type"] = "special";
                    special_event["triggeredBy"] = specialunit->id();
                }
                if(gamerace == Race::Terran){
                    currentState.timeout_mule.insert(currentState.timeout_mule.end(), currentState.time_tick + 64);
                    if(is_recording){
                        generate_json = true;
                        special_event["name"] = "mule";
                        events.push_back(special_event);
                    }
                }
                else{
                    for(int i : base_ids){
                        for(std::shared_ptr<Entity> base : *currentState.entitymap[i]){
                            if(base->inject_larva(currentState)){
                                if(is_recording){
                                    special_event["targetBuilding"] = base->id();
                                    special_event["name"] = "injectlarvae";
                                    generate_json = true;
                                    events.push_back(special_event);
                                }
                                goto injected1;
                            }
                        }
                    }
                    // Couldn't actually cast. Undo it.
                    specialunit->undo_cast();
                    injected1:;
                }
                    
            }
        }
        
    }

    if(update_worker_distribution() && is_recording)
        generate_json = true;

    if(generate_json){
        recording.push_back(make_json_message(currentState, events));
    }
}

template<Race gamerace>
void Simulator<gamerace>::record(){
    recording = {};
    is_recording = true;
}

template<Race gamerace>
Simulator<gamerace>::Simulator(const std::array<EntityMeta, 64>& meta_map,
            const GameState &initialState,
            const unsigned int gas_id,
            const unsigned int worker_id, 
            const std::vector<unsigned int>& base_ids,
            const std::vector<unsigned int>& building_ids,
            const unsigned int super_id) :
    worker_id(worker_id), gas_id(gas_id), meta_map(meta_map), base_ids(base_ids), super_id(super_id), initialState(initialState), currentState(initialState), building_ids(building_ids), is_recording(false){}

template<Race gamerace>
json Simulator<gamerace>::run(std::vector<int> build_list){
	if(DEBUG)
		std::cout << "Init Simulator";

    currentState.built = true;
    bool generate_json;
    std::vector<json> messages;
    unsigned int next_line = 0;
    int next_entity;

    json output;

    // 1. Liste abarbeiten
    if(DEBUG) std::cout << "Finished Initalization. Continue with simulation";
    while(next_line != build_list.size() || !currentState.built) {
    	//Init timestep
    	if((++currentState.time_tick) > 1000){
            error_exit("Exceeded max time", output);
            return output;
        }
        if(DEBUG) std::cout << "  time: " << currentState.time_tick << "\n";
    	generate_json = false;
        std::vector<json> events;

    	update_resources();
        update_energy();


    	//Check finished buildings
        std::list <std::shared_ptr<ProductionEntry>> finished_list = process_production_list();
    	if (!finished_list.empty()){
    		generate_json = true;
			generate_json_build_end(events, finished_list);
    	}

    	//Start new buildings
        if(currentState.built)next_entity = build_list[next_line++];

        std::shared_ptr<ProductionEntry> entry;
        try{
            entry = check_and_build(next_entity);
            currentState.built = true;
        }catch(noMineralsException& e){
            //std::cout << "no mins\n";
            if(currentState.entitymap[worker_id]->empty() && currentState.production_list.empty()){
                error_exit("No Minerals", output);
                return output;
            }
            currentState.built = false;
        }catch(noGasException& e){
            //std::cout << "no gas\n";
            if(currentState.entitymap[gas_id]->empty() && currentState.production_list.empty()){
                error_exit("No Gas", output);
                return output;
            }
            currentState.built = false;
        }catch(noSupplyException& e){
            //std::cout << "no supply\n";
            if(currentState.production_list.empty()){
                error_exit("No supply", output);
                return output;
            }
            currentState.built = false;
        }catch(noProducerAvailableException& e){
            //std::cout << "no prod\n";
            if(currentState.production_list.empty()){
                error_exit("No Producer Available", output);
                return output;
            }
            currentState.built = false;
        }catch(requirementNotFulfilledException& e){
            //std::cout << "missing req\n";
            if(currentState.production_list.empty()){
                error_exit("Requirement not fulfilled", output);
                return output;
            }
            currentState.built = false;
        }catch(noGasGeyserAllowedException& e){
            if(currentState.production_list.empty()){
                error_exit("Requirement not fulfilled", output);
                return output;
            }
            currentState.built = false;
        }catch(noLarvaAvailableException& e){
            currentState.built = false;
        }
        if(currentState.built) {
        	currentState.production_list.push_back(entry);
			generate_json = true;
			generate_json_build_start(events, entry);
        }
        else{
            for(std::shared_ptr<Entity> specialunit : *currentState.entitymap[super_id]){
                if(specialunit->cast_if_possible()){
                    json special_event;
                    special_event["type"] = "special";
                    special_event["triggeredBy"] = specialunit->id();
                    switch(gamerace){
                        case Terran:
                            if(DEBUG){
                                std::cout << "Energy: " << specialunit->get_energy() << "\n";
                                std::cout << "Ability cost: " << specialunit->ability_cost() << "\n";
                            }
                        	currentState.timeout_mule.insert(currentState.timeout_mule.end(), currentState.time_tick + 64);
                            special_event["name"] = "mule";
                            generate_json = true;
                        break;
                        case Zerg:
                            for(int i : base_ids){
                                for(std::shared_ptr<Entity> base : *currentState.entitymap[i]){
                                    if(base->inject_larva(currentState)){
                                        special_event["targetBuilding"] = base->id();
                                        special_event["name"] = "injectlarvae";
                                        generate_json = true;
                                        goto injected;
                                    }
                                }
                            }
                            // Couldn't actually cast. Undo it.
                            specialunit->undo_cast();
                            injected:
                        break;
                        case Protoss:
                            std::shared_ptr<Entity> target = currentState.entitymap[0]->front();
                            target->chrono_boost(currentState);
                            for(std::shared_ptr<ProductionEntry> entry : target->producees){
                                entry->chrono_boost(currentState, target->get_chrono_until());
                            }
                            special_event["name"] = "chronoboost";
                            special_event["targetBuilding"] = currentState.entitymap[0]->front()->id();
                            generate_json = true;
                        break;
                    }
                    events.push_back(special_event);
                    break;
                }
            }
        }

        if(update_worker_distribution(build_list, next_line - (currentState.built ? 0 : 1)))
        	generate_json = true;

        if(generate_json){
            messages.push_back(make_json_message(currentState, events));
        }
    }

    // 2. Warte auf Fertigstellung
    while(!currentState.production_list.empty()){
        if((++currentState.time_tick) > 1000){
            error_exit("Exceeded max time", output);
            return output;
        }
        generate_json = false;
        std::vector<json> events;

        update_resources();

        std::list <std::shared_ptr<ProductionEntry>> finished_list = process_production_list();
    	if (!finished_list.empty()) {
			generate_json = true;
			generate_json_build_end(events, finished_list);
    	}

        if(generate_json){
            messages.push_back(make_json_message(currentState, events));
        }
    }
    output["buildlistValid"] = 1;
    output["messages"] = messages;
    return output;
}

template class Simulator<Terran>;
template class Simulator<Protoss>;
template class Simulator<Zerg>;
