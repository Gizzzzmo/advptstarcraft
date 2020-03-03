#include <algorithm>
#include <assert.h>

#include "parameters.h"
#include "Entity.h"

Entity::Entity(const std::array<EntityMeta, 64>& metamap, int class_id, unsigned int time_tick, unsigned int& id) :
    occupied(metamap[class_id].max_occupation),
    energy(metamap[class_id].start_energy),
    chrono_boosted_until(0),
    larva_spawn_start_time(time_tick+100),
    current_larva(0),
    injected_until(-1u),
    clss_id(class_id),
    metamap(metamap)
{
    obj_id = id++;
    std::stringstream ss;
    ss << clss_id << "." << obj_id;
    ss >> idd;

}

void Entity::finalize(std::shared_ptr<Entity> producer){
    current_larva = producer->current_larva;
    larva_spawn_start_time = producer->larva_spawn_start_time;
    injected_until = producer->injected_until;
}

void Entity::updateEnergy(){
    energy += 56250;
    unsigned int max_nrg = max_energy();
    if(energy > max_nrg)energy = max_nrg;
}

bool Entity::is_chrono_boosted(GameState& currentState){
    return chrono_boosted_until > currentState.time_tick;
}

int Entity::get_chrono_until(){
    return chrono_boosted_until;
}

void Entity::chrono_boost(GameState& currentState){
    chrono_boosted_until = currentState.time_tick + 20;
}

void Entity::update_larva(unsigned now){
    unsigned added_larva;

    if(now < injected_until){
        if(current_larva < 3){
            added_larva = current_larva < 3 ? 
                std::min((now - larva_spawn_start_time)/LARVA_DURATION, MAX_LARVA_PER_BUILDING-current_larva) 
                : 0;
            larva_spawn_start_time += added_larva * LARVA_DURATION;
            if(larva_spawn_start_time > now)std::cerr << larva_spawn_start_time << " " << now << "\n";
        }
        else
            added_larva = 0;
    }
    else{
        if(current_larva < 3){
            added_larva = current_larva < 3 ? 
                std::min((injected_until - larva_spawn_start_time)/LARVA_DURATION, MAX_LARVA_PER_BUILDING-current_larva) 
                : 0;
            added_larva += INJECTLARVAE_AMOUNT;
            injected_until = -1u;
            if(larva_spawn_start_time > now)std::cerr << larva_spawn_start_time << " " << now << "\n";
        }
        else {
            added_larva = INJECTLARVAE_AMOUNT;
            injected_until = -1u;
        }
    }

    current_larva += added_larva;
}

bool Entity::inject_larva(GameState& currentState){
    update_larva(currentState.time_tick);
    if (injected_until != -1u)
        return false;
    injected_until = currentState.time_tick + INJECTLARVAE_DURATION;

    return true;
}

bool Entity::decrement_larva(GameState& currentState){
    unsigned now = currentState.time_tick;

    update_larva(now);

    if(current_larva == 0){
        return false;
    }
    else if(current_larva == MAX_LARVA_PER_BUILDING){
        larva_spawn_start_time = now;
        current_larva--;
    }
    else{
        current_larva--;
    }
    return true;
}

bool Entity::cast_if_possible(){
    if(energy >= ability_cost()){
        energy -= ability_cost();
        return true;
    }
    return false;
}

bool Entity::can_cast() {
    return (energy >= ability_cost());
}
void Entity::undo_cast(){
    energy += ability_cost();
}

// use only for occupied producers
void Entity::make_available(GameState& state){
    if(is_worker())state.workers_available++;
    occupied++;
}
