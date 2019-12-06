#pragma once

#include <vector>
#include <map>
#include <string>
#include <sstream>
#include <exception>

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
public:
    virtual int class_id() const = 0;
    virtual int minerals() const = 0;
    virtual int gas() const = 0;
    virtual int supply() const = 0;
    virtual int supply_provided() const = 0;
    virtual int producer() const = 0;
    virtual Destiny producer_destiny() const = 0;
    virtual int requirement() const = 0;
    virtual std::string id() const = 0;

    bool check_and_occupy(){
        if(occupied){
            --occupied;
            return true;
        }
        else return false;
    }
    
    void make_available(){
        occupied++;
    }
};


class ProductionEntry{
public:
    AbstractEntity* producee;
    AbstractEntity* producer;
    unsigned int time_done;
    ProductionEntry(AbstractEntity* producee, AbstractEntity* producer, unsigned int time_done) : 
    producee(producee), producer(producer), time_done(time_done){}
    void addTime(unsigned int time){
        time_done += time;
    }
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

template<Race race, int clss_id, int mins, int gs, int sppl, int sppl_p, long prd_mask, Destiny prd_d, long req_mask, int maxOcc, int bldtime>
class Entity : public AbstractEntity{
private:
    static auto getCounter() -> unsigned int& {
        static unsigned int counter = 0;
        return counter;
    }

public:
    Entity(){
        occupied = maxOcc;
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
    int producer() const override{
        return prd_mask;
    }
    Destiny producer_destiny() const override{
        return prd_d;
    }
    int requirement() const override{
        return req_mask;
    }
    std::string id() const override{
        std::stringstream ss;
        ss << clss_id << "." << obj_id;
        std::string s;
        ss >> s;
        return s;
    }

    static ProductionEntry* check_and_build(std::map<int , std::vector<AbstractEntity*>*> &entities_done, unsigned int& minerals, unsigned int& gas, unsigned int& supply_used, unsigned int& supply){
        if(minerals < mins)throw noMineralsException();
        if(gas < gs) throw noGasException();
        if(sppl < supply-supply_used) throw noSupplyException();
        for(int req_id : mask_to_vector<req_mask>()){
            if(!(entities_done[req_id]->empty()))goto req_fulfilled;
        }
        throw requirementNotFulfilledException();

        req_fulfilled:
        //possibly search for chronoboosted producers?
        for(int prd_id : mask_to_vector<prd_mask>()){
            std::vector<AbstractEntity*> possible_producers = *(entities_done[prd_id]);
            for(std::vector<AbstractEntity*>::iterator it = possible_producers.begin();it != possible_producers.end();++it){
                AbstractEntity* producer = *it;
                if(producer->check_and_occupy()){
                    if(prd_d == Destiny::consumed_at_start)possible_producers.erase(it);
                    AbstractEntity* producee = new Entity<race, clss_id, mins, gs, sppl, sppl_p, prd_mask, prd_d, req_mask, maxOcc, bldtime>();
                    ProductionEntry* e = new ProductionEntry(producee, producer, bldtime);
                    return e;
                }
            }
        }
        throw noProducerAvailableException();
        return nullptr;
    }
};