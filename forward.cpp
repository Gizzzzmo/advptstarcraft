#include "Simulator.h"
#include "json.hpp"
using json = nlohmann::json;
#include <string>
#include <list>
#include <vector>

template<Race race, int clss_id, int mins, int gs, int sppl, int sppl_p, int max_nrg, int start_nrg, int ablty_cost, ulong prd_mask, Destiny prd_d, ulong req_mask, int max_occ, int bldtime, bool is_wrkr, bool prd_larva, int units_produced>
ProductionEntry* makeEntity(GameState& state){
    return Entity<race, clss_id, mins, gs, sppl, sppl_p, max_nrg, start_nrg, ablty_cost, prd_mask, prd_d, req_mask, max_occ, bldtime, is_wrkr, prd_larva, units_produced>::check_and_build(state);
}

int main(int argc, char** argv){
    std::string racearg(argv[1]);
    Race race = !racearg.compare("terran") ? Race::Terran : !racearg.compare("zerg") ? Race::Zerg : Race::Protoss;
    int worker_id;
    int gas_id;
    unsigned int supply = 15;
    std::map<std::string, build_and_check> build_map;

    std::map<int, std::string> name_map;

    //keep in mind minerals and gas are in hundredths
    std::array<std::list<AbstractEntity*>*, 64> entitymap;
    std::list<ProductionEntry*> production_list;
    for (int i = 0; i<entitymap.size(); i++){
        name_map[i] = "";
        entitymap[i] = new std::list<AbstractEntity*>();
    }
    switch(race){
        case Terran:
            #include "unit_map_terran.h"
            worker_id = 4;
            gas_id = 25;
            for(int i = 0;i < 12;i++){
                entitymap[4]->push_back(new Entity<Race::Terran, 4, 5000, 0, 1, 0, 0, 0, 0, 0x0000000000000007, Destiny::occupied, 0x0000000000000000, 1, 17, 1, 0, 1>);
            }
            entitymap[0]->push_back(new Entity<Race::Terran, 0, 40000, 0, 0, 15, 0, 0, 0, 0x0000000000000010, Destiny::occupied, 0x0000000000000000, 1, 100, 0, 0, 1>);
            break;
        case Zerg:
            #include "unit_map_zerg.h"
            worker_id = 9;
            gas_id = 20;
            for(int i = 0;i < 12;i++){
                entitymap[9]->push_back(new Entity<Race::Zerg, 9, 5000, 0, 1, 0, 0, 0, 0, 0x0000000010000000, Destiny::consumed_at_start, 0x0000000000000000, 1, 17, 1, 0, 1>);
            }
            entitymap[0]->push_back(new Entity<Race::Zerg, 0, 30000, 0, 0, 6, 0, 0, 0, 0x0000000000000200, Destiny::consumed_at_start, 0x0000000000000000, 1, 100, 0, 1, 1>);
            entitymap[16]->push_back(new Entity<Race::Zerg, 16, 10000, 0, 0, 8, 0, 0, 0, 0x0000000010000000, Destiny::consumed_at_start, 0x0000000000000000, 1, 25, 0, 0, 1>);
            supply = 14;
            break;
        case Protoss:
            #include "unit_map_protoss.h"
            worker_id = 5;
            gas_id = 7;
            for(int i = 0;i < 12;i++){
                entitymap[5]->push_back(new Entity<Race::Protoss, 5, 5000, 0, 1, 0, 0, 0, 0, 0x0000000000000001, Destiny::occupied, 0x0000000000000000, 1, 17, 1, 0, 1>);
            }
            entitymap[0]->push_back(new Entity<Race::Protoss, 0, 40000, 0, 0, 15, 200, 50, 50, 0x0000000000000020, Destiny::freed, 0x0000000000000000, 1, 100, 0, 0, 1>);
            break;
    }
    const GameState initialState{1, 5000, 0, supply, 12, 12, 12, 0, entitymap, {}};

    std::vector<std::string> lines;
    json initial_units;

    for (std::size_t i = 0; i < 64; ++i) {
        if (entitymap[i]->empty())
            continue;
        std::list<std::string> l;
        for( std::list<AbstractEntity*>::iterator jt = entitymap[i]->begin(); jt != entitymap[i]->end(); ++jt){
            l.push_back((*jt)->id());
        }
        initial_units[name_map[i]] = l;
    }

    while(std::cin){
        std::string line; 
        std::getline(std::cin, line);
        if(line != "")lines.push_back(line);
    }
    Simulator sim(build_map, name_map, initialState, gas_id, worker_id);
    json output = sim.run(lines);
    output["game"] = race == Race::Terran ? "Terr" : race == Race::Zerg ? "Zerg" : "Prot";
    output["initialUnits"] = initial_units;
    std::cout << output << std::endl;
    return 0;
}
