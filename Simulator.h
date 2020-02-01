#pragma once
#include "json.hpp"
using json = nlohmann::json;
#include <map>
#include <vector>
#include <list>
#include <iostream>
#include <algorithm>
#include <memory>

#include "ProductionEntry.h"
#include "Entity.h"
#include "StarcraftExceptions.h"
#include "GameState.h"

#ifdef NDEBUG
#define DEBUG 0
#else
#define DEBUG 1
#endif

template<Race gamerace>
class Simulator{
private:
    const unsigned int worker_id;
    const unsigned int gas_id;
    const std::array<EntityMeta, 64>& meta_map;
    const GameState initialState;
    const std::vector<unsigned int>& base_ids;
    const std::vector<unsigned int>& building_ids;
    const int super_id;

    inline std::list<std::shared_ptr<ProductionEntry>> process_production_list();

    void update_resources();

    void error_exit(std::string message, json& output);

    std::shared_ptr<ProductionEntry> check_and_build(int class_id);

    inline json make_json_message(const GameState &currentGameState, const std::vector<json> events);

	inline void generate_json_build_end(std::vector<json> &events, std::list <std::shared_ptr<ProductionEntry>> &finished_list);

	inline void generate_json_build_start(std::vector<json> &events, const std::shared_ptr<ProductionEntry> &entry);

    inline bool update_worker_distribution(std::vector<int>& lines, int current_line);

    inline bool update_worker_distribution();

    unsigned int number_of_bases();

    void update_energy();

public:
    GameState currentState;

Simulator(const std::array<EntityMeta, 64>& meta_map,
            const GameState &initialState,
            const unsigned int gas_id,
            const unsigned int worker_id, 
            const std::vector<unsigned int>& base_ids,
            const std::vector<unsigned int>& building_ids,
            const unsigned int super_id);

    json run(std::vector<int> build_list);

    std::shared_ptr<Entity> get_caster();
    //If someone can cast, return caster, else nullptr

    std::list<std::shared_ptr<Entity>> get_chrono_targets();
    //Protoss -> Buildings that can produce something

    std::array<int, 64> getOptions();
    //Possible entities

    void step(int entity_id, std::shared_ptr<Entity> target, std::shared_ptr<Entity> caster);
//Propagates game one time step

    inline bool worker_distribution_well_defined();
};
