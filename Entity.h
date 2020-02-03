#pragma once
#include <string>
#include <list>
#include <array>
#include <sstream>
#include <memory>

#include "GameState.h"

enum Destiny {consumed_at_start, consumed_at_end, occupied, freed};

typedef struct{
    unsigned int minerals;
    unsigned int gas;
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
    unsigned int chrono_boosted_until;
    unsigned int larva_spawn_start_time;
    unsigned int current_larva;
    unsigned int injected_until;            //0 means not injected
    int clss_id;
    std::string idd;
    const std::array<EntityMeta, 64>& metamap;
    
    static auto getCounter() -> unsigned int& {
        static unsigned int counter = 0;
        return counter;
    }
    void update_larva(unsigned now);

public:
    std::list<std::shared_ptr<ProductionEntry>> producees;
    Entity(const std::array<EntityMeta, 64>& metamap, int class_id, unsigned int time_tick);
    Entity(const std::array<EntityMeta, 64>& metamap, int class_id, unsigned int time_tick, Entity producer);

    int class_id() const{
        return clss_id;
    }
    int minerals() const{
        return metamap[class_id()].minerals;
    }
    int gas() const{
        return metamap[class_id()].gas;
    }
    int supply() const{
        return metamap[class_id()].supply;
    }
    int supply_provided() const{
        return metamap[class_id()].supply_provided;
    }
    unsigned long producer() const{
        return metamap[class_id()].production_mask;
    }
    Destiny producer_destiny() const{
        return metamap[class_id()].producer_destiny;
    }
    unsigned long requirement() const{
        return metamap[class_id()].requirement_mask;
    }
    std::string id() const{
        return idd;
    }
    bool is_worker() const{
        return metamap[class_id()].is_worker;
    }
    unsigned int max_energy() const{
        return metamap[class_id()].max_energy;
    }
    unsigned int start_energy() const{
        return metamap[class_id()].start_energy;
    }
    unsigned int ability_cost() const{
        return metamap[class_id()].ability_cost;
    }
    unsigned int build_time() const{
        return metamap[class_id()].build_time;
    }
    std::string name() const{
        return metamap[class_id()].name;
    }
    int max_occupation() const{
        return metamap[class_id()].max_occupation;
    }

    void updateEnergy();

    int get_energy(){
    	return energy;
    }
    bool check_and_occupy(){
        if(occupied){
            --occupied;
            return true;
        }
        else return false;
    }

    bool is_chrono_boosted(GameState& currentState);

    int get_chrono_until();

    void chrono_boost(GameState& currentState);

    // Called to give down properties of the producer to the
    // producee at the end of production. (consumed_at_end)
    void finalize(std::shared_ptr<Entity> producer);

    bool inject_larva(GameState& currentState);

    bool decrement_larva(GameState& currentState);

    bool cast_if_possible();
    
    // use only for occupied producers
    void make_available(GameState& state);
};
