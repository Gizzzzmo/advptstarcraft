#include "ProductionEntry.h"
#include "GameState.h"
#include "Entity.h"

ProductionEntry::ProductionEntry(std::shared_ptr<Entity> producee, std::shared_ptr<Entity> producer, GameState& state) :
producee(producee), producer(producer), time_done((state.time_tick + producee->build_time())*6), chrono_boosted_until(state.time_tick){}

ProductionEntry::ProductionEntry(const ProductionEntry& entry, const std::map<std::shared_ptr<Entity>, std::shared_ptr<Entity>> translation_map):
    time_done(entry.time_done),
    chrono_boosted_until(entry.chrono_boosted_until),
    producee(new Entity(*entry.producee)){

        if(entry.second_producee)
            second_producee = std::shared_ptr<Entity>(new Entity(*entry.producee));

        auto kv = translation_map.find(entry.producer);
        if(kv != translation_map.end())
            producer = std::shared_ptr<Entity>(kv->second);
        else{
            producer = std::shared_ptr<Entity>(new Entity(*entry.producer));
        }
    }

void ProductionEntry::chrono_boost(GameState& state, unsigned int until){
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
