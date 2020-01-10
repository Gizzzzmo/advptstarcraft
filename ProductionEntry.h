#pragma once
#include "Entity.h"
#include <memory>

class ProductionEntry{
public:
    std::shared_ptr<Entity> producee;
    std::shared_ptr<Entity> producer;
    unsigned int time_done;
    unsigned int chrono_boosted_until;
    ProductionEntry(std::shared_ptr<Entity> producee, std::shared_ptr<Entity> producer, GameState& state) :
    producee(producee), producer(producer), time_done((state.time_tick + producee->build_time())*6), chrono_boosted_until(state.time_tick){}
    void chrono_boost(GameState& state, unsigned int until){
        if(chrono_boosted_until > time_done)return;
        until = until * 6;
        unsigned int time_tick = state.time_tick * 6;
        unsigned int duration = until - time_tick;
        unsigned int extra_duration = chrono_boosted_until > time_tick ? duration - (chrono_boosted_until - time_tick) : duration;
        unsigned int unchronoed_time = chrono_boosted_until > time_tick ? time_done - chrono_boosted_until : time_done - time_tick;
        chrono_boosted_until = until;
        if(extra_duration >= unchronoed_time*2/3){
            time_done -= unchronoed_time/3;
        }
        else{
            time_done -= extra_duration/2;
        }
    }
};
