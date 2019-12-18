#pragma once
#include "Entity.h"

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
