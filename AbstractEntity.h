#pragma once

enum Destiny {consumed_at_start, consumed_at_end, occupied, freed};
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
    
    // use only for occupied producers
    void make_available(unsigned int& available_worker){
        if(is_worker())available_worker++;
        occupied++;
    }
};