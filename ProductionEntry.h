#pragma once
#include "Entity.h"
#include <memory>

class ProductionEntry{
public:
    std::shared_ptr<Entity> producee;
    std::shared_ptr<Entity> producer;
    unsigned int time_done;
    ProductionEntry(std::shared_ptr<Entity> producee, std::shared_ptr<Entity> producer, GameState& state) :
    producee(producee), producer(producer), time_done(state.time_tick + producee->build_time()){}
};
