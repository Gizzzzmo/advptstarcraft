#pragma once
#include "json.hpp"
using json = nlohmann::json;
#include <map>
#include <vector>
#include <list>
#include <iostream>
#include <algorithm>
#include <memory>
#include <utility>

#include "ProductionEntry.h"
#include "Entity.h"
#include "StarcraftExceptions.h"
#include "GameState.h"

#ifdef NDEBUG
#define DEBUG 0
#else
#define DEBUG 1
#endif
// Helper Method
//turns a bitmask into a vector of those indices of the mask, where the bit is one
//TODO: for performance improvements, one might buffer theses vectors
inline std::vector<unsigned> mask_to_vector(unsigned long mask) {
    std::vector<unsigned> v;
    for(size_t i = 0;i < 64;++i){
        if(mask%2 == 1) v.push_back(i);
        mask = mask>>1;
    }
    return v;
}

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
    bool is_recording;

    inline std::list<std::shared_ptr<ProductionEntry>> process_production_list();

    void update_resources();

    void error_exit(std::string message, json& output);

    std::shared_ptr<ProductionEntry> check_and_build(int class_id);

    inline json make_json_message(const GameState &currentGameState, const std::vector<json> events);

	inline void generate_json_build_end(std::vector<json> &events, std::list <std::shared_ptr<ProductionEntry>> &finished_list);

	inline void generate_json_build_start(std::vector<json> &events, const std::shared_ptr<ProductionEntry> &entry);

    inline bool update_worker_distribution(std::vector<int>& lines, int current_line);

    inline bool update_worker_distribution();

    unsigned int number_of_future_bases();

    unsigned int number_of_bases();

    bool needs_larva(int class_id);

    bool is_zergling(int class_id);

    void update_energy();

public:
    GameState currentState;
    std::vector<json> recording;

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

    std::vector<std::pair<int, int>> get_chrono_targets();
    //Protoss -> Buildings that can produce something

    std::array<int, 64> getOptions();
    //Possible entities

    void record();

    void step(int entity_id, int cast_target_class_id, int cast_target_obj_id);
//Propagates game one time step

    bool worker_distribution_well_defined();
};
