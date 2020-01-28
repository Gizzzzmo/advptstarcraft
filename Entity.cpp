#include "Entity.h"

Entity::Entity(const std::array<EntityMeta, 64>& metamap, int class_id, unsigned int time_tick) :
    occupied(metamap[class_id].max_occupation),
    energy(metamap[class_id].start_energy),
    clss_id(class_id),
    metamap(metamap),
    chrono_boosted_until(time_tick)
{
    obj_id = getCounter()++;
    std::stringstream ss;
    ss << clss_id << "." << obj_id;
    ss >> idd;

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

bool Entity::cast_if_possible(){
    if(energy >= ability_cost()){
        energy -= ability_cost();
        return true;
    }
    return false;
}

// use only for occupied producers
void Entity::make_available(GameState& state){
    if(is_worker())state.workers_available++;
    occupied++;
}
