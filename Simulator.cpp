#include "Simulator.h"

// Helper Method
//turns a bitmask into a vector of those indices of the mask, where the bit is one
//TODO: for performance improvements, one might buffer theses vectors
inline std::vector<unsigned> mask_to_vector(unsigned long mask) {
    std::vector<unsigned> v;
    for(size_t i = 0;i < 64;++i){
        if(mask%2 == 1) v.push_back(i);
        mask = mask>>1;
    }
    return v;
}

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
            for(auto it = entry->producer->producees.begin(); it != entry->producer->producees.end();++it){
                if(*it == entry){
                    entry->producer->producees.erase(it);
                }
                break;
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
                std::shared_ptr<Entity> producee(new Entity(meta_map, class_id, currentState.time_tick));
                std::shared_ptr<ProductionEntry> e(new ProductionEntry(producee, producer, currentState));

                if (is_zergling(class_id)){
                    std::shared_ptr<Entity> second_producee(new Entity(meta_map, class_id, currentState.time_tick));
                    e->second_producee = second_producee;
                }

                if(producer->is_chrono_boosted(currentState))e->chrono_boost(currentState, producer->get_chrono_until());
                producer->producees.push_back(e);
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
    if(DEBUG) std::cout << "line: "<< current_line << "\n";
    int cost_mins = 0, cost_gas = 0;
    int gas = (int) currentState.gas;
    int mins = (int) currentState.minerals;
    if(DEBUG) std::cout << mins << " " << gas << "\n";
    do{
        if(current_line >= lines.size())break;
        cost_mins += meta_map[lines[current_line]].minerals;
        cost_gas += meta_map[lines[current_line]].gas;
        
        if(DEBUG) std::cout << " " << meta_map[lines[current_line]].name << " " << current_line <<" "<<cost_mins << " "<< cost_gas << "\n";
        current_line++;
    }while(mins >= cost_mins && gas >= cost_gas);
    int missing_mins = std::max(0, cost_mins - mins);
    int missing_gas = std::max(0, cost_gas - gas);
    unsigned int ideal_mineral_worker;
    unsigned int ideal_gas_worker;
    if(missing_mins == 0 && missing_gas == 0){
        ideal_gas_worker = 0;
        ideal_mineral_worker = currentState.workers_available;
    }
    else{
        ideal_mineral_worker = currentState.workers_available * 63 * missing_mins/(missing_mins * 63 + missing_gas * 70);
        ideal_gas_worker = currentState.workers_available - ideal_mineral_worker;
    }
    if(DEBUG) std::cout << ideal_mineral_worker << " " << ideal_gas_worker << "\n";
    unsigned int max_gas_worker = static_cast<unsigned int>(currentState.entitymap[gas_id]->size()*3);
    unsigned int max_mineral_worker = number_of_bases()*16;
    unsigned int new_gas_worker = std::min(max_gas_worker, ideal_gas_worker);
    unsigned int new_mineral_worker = std::min(max_mineral_worker, ideal_mineral_worker);
    new_gas_worker = 
        std::min(new_gas_worker + currentState.workers_available - new_gas_worker - new_mineral_worker, max_gas_worker);
    new_mineral_worker = 
        std::min(new_mineral_worker + currentState.workers_available - new_gas_worker - new_mineral_worker, max_mineral_worker);
    if(DEBUG) std::cout << new_mineral_worker << " " << new_gas_worker << "\n";
    if(new_gas_worker != currentState.gas_worker || new_mineral_worker != currentState.mineral_worker){
        currentState.gas_worker = new_gas_worker;
        currentState. mineral_worker = new_mineral_worker;
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
void Simulator<gamerace>::update_energy(){
    for(std::shared_ptr<Entity> special_unit : *currentState.entitymap[super_id]){
        special_unit->updateEnergy();
    }
}

// Simulator
// public object methods

template<Race gamerace>
Simulator<gamerace>::Simulator(const std::array<EntityMeta, 64>& meta_map,
            const GameState &initialState,
            const unsigned int gas_id,
            const unsigned int worker_id, 
            const std::vector<unsigned int>& base_ids,
            const unsigned int super_id) :
    worker_id(worker_id), gas_id(gas_id), meta_map(meta_map), initialState(initialState), base_ids(base_ids), super_id(super_id){}

template<Race gamerace>
json Simulator<gamerace>::run(std::vector<int> build_list){
	if(DEBUG)
		std::cout << "Init Simulator";
    currentState = initialState;

    bool built = true;
    bool generate_json;
    std::vector<json> messages;
    unsigned int next_line = 0;
    int next_entity;

    json output;

    // 1. Liste abarbeiten
    if(DEBUG) std::cout << "Finished Initalization. Continue with simulation";
    while(next_line != build_list.size() || !built) {
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
        if(built)next_entity = build_list[next_line++];

        std::shared_ptr<ProductionEntry> entry;
        try{
            entry = check_and_build(next_entity);
            built = true;
        }catch(noMineralsException& e){
            //std::cout << "no mins\n";
            if(currentState.entitymap[worker_id]->empty() && currentState.production_list.empty()){
                error_exit("No Minerals", output);
                return output;
            }
            built = false;
        }catch(noGasException& e){
            //std::cout << "no gas\n";
            if(currentState.entitymap[gas_id]->empty() && currentState.production_list.empty()){
                error_exit("No Gas", output);
                return output;
            }
            built = false;
        }catch(noSupplyException& e){
            //std::cout << "no supply\n";
            if(currentState.production_list.empty()){
                error_exit("No supply", output);
                return output;
            }
            built = false;
        }catch(noProducerAvailableException& e){
            //std::cout << "no prod\n";
            if(currentState.production_list.empty()){
                error_exit("No Producer Available", output);
                return output;
            }
            built = false;
        }catch(requirementNotFulfilledException& e){
            //std::cout << "missing req\n";
            if(currentState.production_list.empty()){
                error_exit("Requirement not fulfilled", output);
                return output;
            }
            built = false;
        }catch(noGasGeyserAllowedException& e){
            if(currentState.production_list.empty()){
                error_exit("Requirement not fulfilled", output);
                return output;
            }
            built = false;
        }catch(noLarvaAvailableException& e){
            built = false;
        }
        if(built) {
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

        if(update_worker_distribution(build_list, next_line - (built ? 0 : 1)))
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
