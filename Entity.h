#pragma once

#include <vector>
#include <map>
#include <string>
#include <sstream>

enum Destiny {consumed_at_start, consumed_at_end, occupied, freed};


class AbstractEntity{
protected:
    unsigned int occupied;
    unsigned int obj_id;
public:
    virtual int minerals() const = 0;
    virtual int gas() const = 0;
    virtual int supply() const = 0;
    virtual int supply_provided() const = 0;
    virtual int producer() const = 0;
    virtual Destiny producer_destiny() const = 0;
    virtual long requirements() const = 0;
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
};

template<int class_id, int mins, int gs, int sppl, int sppl_p, int prd, Destiny prd_d, long req, int maxOcc, int bldtime>
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
        return prd;
    }
    Destiny producer_destiny() const override{
        return prd_d;
    }
    long requirements() const override{
        return req;
    }
    std::string id() const override{
        std::stringstream ss;
        ss << class_id << "." << obj_id;
        std::string s;
        ss >> s;
        return s;
    }

    static ProductionEntry* check_and_build(std::map<int , std::vector<AbstractEntity*>> &entities_done){
        std::vector<AbstractEntity> possible_producers = entities_done.find(prd);
        for(AbstractEntity* producer : possible_producers){
            if(producer->check_and_occupy()){
                AbstractEntity* producee = new Entity<class_id, mins, gs, sppl, sppl_p, prd, prd_d, req, maxOcc, bldtime>;
                ProductionEntry* e = new ProductionEntry(producee, producer, bldtime);
                return e;
            }
        }
        return nullptr;
    }
};