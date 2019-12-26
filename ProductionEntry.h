#pragma once
#include "Entity.h"

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
