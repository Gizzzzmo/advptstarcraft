#pragma once
#include "Entity.h"

class ProductionEntry{
public:
    Entity* producee;
    Entity* producer;
    unsigned int time_done;
    ProductionEntry(Entity* producee, Entity* producer, GameState& state) :
    producee(producee), producer(producer), time_done(state.time_tick + producee->build_time()){}
};
