#pragma once
#include <memory>

class Entity;
struct GameState;

class ProductionEntry{
public:
    std::shared_ptr<Entity> producee;
    std::shared_ptr<Entity> second_producee;
    std::shared_ptr<Entity> producer;
    unsigned int time_done;
    unsigned int chrono_boosted_until;

    ProductionEntry(std::shared_ptr<Entity> producee, std::shared_ptr<Entity> producer, GameState& state);
    void chrono_boost(GameState& state, unsigned int until);
};
