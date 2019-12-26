#pragma once

#include <vector>
#include <map>
#include <string>
#include <sstream>
#include <list>
#include <iostream>

#include "AbstractEntity.h"
#include "StarcraftExceptions.h"
#include "ProductionEntry.h"
#include "Simulator.h"

enum Race {Protoss, Zerg, Terran};

//turns a bitmask into a vector of those indices of the mask, where the bit is one
//the mask is a template argument and the function is cached 
template<unsigned long bitmask>
inline std::vector<int>& mask_to_vector() {
    static bool already_calculated = false;
    static std::vector<int> v;
    if(!already_calculated){
        already_calculated = true;
        unsigned long mask = bitmask;
        for(size_t i = 0;i < 64;++i){
            if(mask%2 == 1)v.push_back(i);
            mask = mask>>1;
        }
    }
    return v;
}


template<Race race, int clss_id, int mins, int gs, int sppl, int sppl_p, int max_nrg, int start_nrg, int ablty_cost, ulong prd_mask, Destiny prd_d, ulong req_mask, int max_occ, int bldtime, bool is_wrkr, bool prd_larva, int units_produced>
class Entity : public AbstractEntity{
private:
    static auto getCounter() -> unsigned int& {
        static unsigned int counter = 0;
        return counter;
    }

public:
    Entity()
    {
        occupied = max_occ;
        energy = start_nrg;
        obj_id = getCounter()++;
    }

    int class_id() const override{
        return clss_id;
    }
    int minerals() const override{
        return mins;
    }
    int gas() const override{
        return gs;
    }
    int supply() const override{
        return sppl;
    }
    int supply_provided() const override{
        return sppl_p;
    }
    unsigned long producer() const override{
        return prd_mask;
    }
    bool is_worker() const override{
        return is_wrkr;
    }
    unsigned int max_energy() const override{
        return max_nrg;
    }
    Destiny producer_destiny() const override{
        return prd_d;
    }
    unsigned long requirement() const override{
        return req_mask;
    }
    std::string id() const override{
        std::stringstream ss;
        ss << clss_id << "." << obj_id;
        std::string s;
        ss >> s;
        return s;
    }

    bool cast_if_possible() override{
        if(energy >= ablty_cost){
            energy -= ablty_cost;
            return true;
        }
        return false;
    }

    static ProductionEntry* check_and_build(struct GameState& currentState){
        if(sppl > static_cast<int>(currentState.supply-currentState.supply_used)) throw noSupplyException();
        //bitmask of 0 is interpreted as there not being any requirements, since all entities should be buildable somehow
        if(req_mask == 0)goto req_fulfilled;
        for(int req_id : mask_to_vector<req_mask>()){
            if(!(currentState.entitymap[req_id]->empty()))goto req_fulfilled;
        }
        throw requirementNotFulfilledException();
        req_fulfilled:
        if(currentState.gas < gs) throw noGasException();
        if(currentState.minerals < mins)throw noMineralsException();

        //possibly search for chronoboosted producers?
        for(int prd_id : mask_to_vector<prd_mask>()){
            std::list<AbstractEntity*> possible_producers = *(currentState.entitymap[prd_id]);
            for(std::list<AbstractEntity*>::iterator it = possible_producers.begin();it != possible_producers.end();++it){
                AbstractEntity* producer = *it;
                if(producer->check_and_occupy()){
                    if(producer->is_worker())currentState.workers_available--;
                    if(prd_d == Destiny::freed)producer->make_available(currentState.workers_available);
                    if(prd_d == Destiny::consumed_at_start)possible_producers.erase(it);
                    currentState.minerals -= mins;
                    currentState.gas -= gs;
                    currentState.supply_used += sppl;
                    AbstractEntity* producee = new Entity<race, clss_id, mins, gs, sppl, sppl_p, max_nrg, start_nrg, ablty_cost, prd_mask, prd_d, req_mask, max_occ, bldtime, is_wrkr, prd_larva, units_produced>();
                    ProductionEntry* e = new ProductionEntry(producee, producer, bldtime);
                    return e;
                }
            }
        }
        throw noProducerAvailableException();
        return nullptr;
    }
};
