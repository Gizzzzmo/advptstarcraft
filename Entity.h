#pragma once

#include <vector>
#include <map>
#include <string>
#include <sstream>
#include <exception>
#include <list>

enum Destiny {consumed_at_start, consumed_at_end, occupied, freed};
enum Race {Protoss, Zerg, Terran};

//turns a bitmask into a vector of those indices of the mask, where the bit is one
//the mask is a template argument and the function is cached 
template<int bitmask>
inline std::vector<int>& mask_to_vector(){
    static bool already_calculated = false;
    static std::vector<int> v;
    if(!already_calculated){
        already_calculated = true;
        int mask = bitmask;
        for(size_t i = 0;i < 64;++i){
            if(mask%2 == 1)v.push_back(i);
            mask = mask>>1;
        }
    }
    return v;
}

class AbstractEntity{
protected:
    unsigned int occupied;
    unsigned int obj_id;
    unsigned int energy;
public:
    virtual int class_id() const = 0;
    virtual int minerals() const = 0;
    virtual int gas() const = 0;
    virtual int supply() const = 0;
    virtual int supply_provided() const = 0;
    virtual unsigned long producer() const = 0;
    virtual Destiny producer_destiny() const = 0;
    virtual unsigned long requirement() const = 0;
    virtual std::string id() const = 0;
    virtual bool is_worker() const = 0;
    virtual unsigned int max_energy() const = 0;

    inline void updateEnergy(){
        energy += 56525;
        unsigned int max_nrg = max_energy();
        if(energy > max_nrg)energy = max_nrg;
    }

    bool check_and_occupy(){
        if(occupied){
            --occupied;
            return true;
        }
        else return false;
    }

    virtual bool cast_if_possible() = 0;
    
    void make_available(){
        occupied++;
    }
};


class ProductionEntry{
public:
    AbstractEntity* producee;
    AbstractEntity* producer;
    unsigned int time_done;
    ProductionEntry(AbstractEntity* producee, AbstractEntity* producer, unsigned int time_done, std::list<AbstractEntity *>::iterator itt) :
    producee(producee), producer(producer), time_done(time_done), it(itt){}
    void addTime(unsigned int time){
        time_done += time;
    }
    std::list<AbstractEntity *>::iterator it;
};

class noMineralsException : public std::exception {
    const char * what () const throw () {
      return "noMins";
   }
};

class noGasException : public std::exception {
    const char * what () const throw () {
      return "noGas";
   }
};

class noSupplyException : public std::exception {
    const char * what () const throw () {
      return "noSupply";
   }
};

class noProducerAvailableException : public std::exception {
    const char * what () const throw () {
      return "noProducer";
   }
};

class requirementNotFulfilledException : public std::exception {
    const char * what () const throw () {
      return "noProducer";
   }
};

template<Race race, int clss_id, int mins, int gs, int sppl, int sppl_p, int max_nrg, int start_nrg, int ablty_cost, ulong prd_mask, Destiny prd_d, ulong req_mask, int max_occ, int bldtime, bool is_wrkr, bool prd_larva, int units_produced>
class Entity : public AbstractEntity{
private:
    static auto getCounter() -> unsigned int& {
        static unsigned int counter = 0;
        return counter;
    }

public:
    Entity(){
        occupied = max_occ;
        obj_id = getCounter()++;
        energy = start_nrg;
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

    static ProductionEntry* check_and_build(std::array<std::list<AbstractEntity*>*, 64> &entities_done, unsigned int& minerals, unsigned int& gas, unsigned int& supply_used, unsigned int supply, unsigned int& available_workers){
        if(sppl > supply-supply_used) throw noSupplyException();
        //bitmask of 0 is interpreted as there not being any requirements, since all entities should be buildable somehow
        if(req_mask == 0)goto req_fulfilled;
        for(int req_id : mask_to_vector<req_mask>()){
            if(!(entities_done[req_id]->empty()))goto req_fulfilled;
        }
        throw requirementNotFulfilledException();
        req_fulfilled:
        if(gas < gs) throw noGasException();
        if(minerals < mins)throw noMineralsException();

        //possibly search for chronoboosted producers?
        for(int prd_id : mask_to_vector<prd_mask>()){
            std::list<AbstractEntity*> possible_producers = *(entities_done[prd_id]);
            for(std::list<AbstractEntity*>::iterator it = possible_producers.begin();it != possible_producers.end();++it){
                AbstractEntity* producer = *it;
                if(producer->check_and_occupy()){
                    if(producer->is_worker())available_workers--;
                    if(prd_d == Destiny::consumed_at_start)possible_producers.erase(it);
                    minerals -= mins;
                    gas -= gs;
                    supply_used += sppl;
                    AbstractEntity* producee = new Entity<race, clss_id, mins, gs, sppl, sppl_p, max_nrg, start_nrg, ablty_cost, prd_mask, prd_d, req_mask, max_occ, bldtime, is_wrkr, prd_larva, units_produced>();
                    ProductionEntry* e = new ProductionEntry(producee, producer, bldtime, it);
                    return e;
                }
            }
        }
        throw noProducerAvailableException();
        return nullptr;
    }
};
