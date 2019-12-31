#pragma once
#include <string>
#include <array>
#include <sstream>

#include "GameState.h"

enum Destiny {consumed_at_start, consumed_at_end, occupied, freed};

typedef struct EntityMeta{
    int minerals;
    int gas;
    unsigned int build_time;
    int supply;
    int supply_provided;
    int max_energy;
    int start_energy;
    int ability_cost;
    unsigned long production_mask;
    Destiny producer_destiny;
    unsigned long requirement_mask;
    int max_occupation;
    bool is_worker;
    bool produces_larva;
    int units_produced;
    std::string name;
} EntityMeta;

class Entity{
private:
    unsigned int occupied;
    unsigned int obj_id;
    unsigned int energy;
    int clss_id;
    std::string idd;
    const std::array<EntityMeta, 64>& metamap;
    
    static auto getCounter() -> unsigned int& {
        static unsigned int counter = 0;
        return counter;
    }

public:
    Entity(const std::array<EntityMeta, 64>& metamap, int class_id) :
        occupied(metamap[class_id].max_occupation),
        energy(metamap[class_id].max_occupation),
        clss_id(class_id),
        metamap(metamap)
    {
        obj_id = getCounter()++;
        std::stringstream ss;
        ss << clss_id << "." << obj_id;
        ss >> idd;

    }
    inline int class_id() const{
        return clss_id;
    }
    inline int minerals() const{
        return metamap[class_id()].minerals;
    }
    inline int gas() const{
        return metamap[class_id()].gas;
    }
    inline int supply() const{
        return metamap[class_id()].supply;
    }
    inline int supply_provided() const{
        return metamap[class_id()].supply_provided;
    }
    inline unsigned long producer() const{
        return metamap[class_id()].production_mask;
    }
    inline Destiny producer_destiny() const{
        return metamap[class_id()].producer_destiny;
    }
    inline unsigned long requirement() const{
        return metamap[class_id()].requirement_mask;
    }
    inline std::string id() const{
        return idd;
    }
    inline bool is_worker() const{
        return metamap[class_id()].is_worker;
    }
    inline unsigned int max_energy() const{
        return metamap[class_id()].max_energy;
    }
    inline unsigned int start_energy() const{
        return metamap[class_id()].start_energy;
    }
    inline unsigned int ability_cost() const{
        return metamap[class_id()].ability_cost;
    }
    inline unsigned int build_time() const{
        return metamap[class_id()].build_time;
    }
    inline std::string name() const{
        return metamap[class_id()].name;
    }
    inline int max_occupation() const{
        return metamap[class_id()].max_occupation;
    }

    inline void updateEnergy(){
        energy += 56525;
        unsigned int max_nrg = max_energy();
        if(energy > max_nrg)energy = max_nrg;
    }

    inline bool check_and_occupy(){
        if(occupied){
            --occupied;
            return true;
        }
        else return false;
    }

    bool cast_if_possible(){
        if(energy >= ability_cost()){
            energy -= ability_cost();
            return true;
        }
        return false;
    }
    
    // use only for occupied producers
    inline void make_available(GameState& state){
        if(is_worker())state.workers_available++;
        occupied++;
    }
};
