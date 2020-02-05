#pragma once
#include <memory>
#include <map>

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
    ProductionEntry(const ProductionEntry& entry, const std::map<std::shared_ptr<Entity>, std::shared_ptr<Entity>> translation_map);
    void chrono_boost(GameState& state, unsigned int until);
};
