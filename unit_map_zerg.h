
meta_map[0] = {
                            30000, //mins
                            0,  //gas
                            100, //build_time
                            -1, //supply_delta
                            6, //provided_supply_delta
                            0, //max_energy
                            0, //start_energy
                            0, //ablty_cost
                            0x0000000000000200, //producer_id
                            Destiny::consumed_at_start, //destiny
                            0x0000000000000000, //requirements
                            1, //maximum_occupation
                            0, //is_worker
                            1, //produces_larva
                            1, //units_produced
                            "Hatchery" //name
};
name_map["Hatchery"] = 0;

meta_map[1] = {
                            15000, //mins
                            0,  //gas
                            50, //build_time
                            2, //supply_delta
                            0, //provided_supply_delta
                            20000000, //max_energy
                            2500000, //start_energy
                            2500000, //ablty_cost
                            0x0000000000020009, //producer_id
                            Destiny::occupied, //destiny
                            0x0000000000000010, //requirements
                            1, //maximum_occupation
                            0, //is_worker
                            0, //produces_larva
                            1, //units_produced
                            "Queen" //name
};
name_map["Queen"] = 1;

meta_map[2] = {
                            7500, //mins
                            0,  //gas
                            35, //build_time
                            -1, //supply_delta
                            0, //provided_supply_delta
                            0, //max_energy
                            0, //start_energy
                            0, //ablty_cost
                            0x0000000000000200, //producer_id
                            Destiny::consumed_at_start, //destiny
                            0x0000000000020009, //requirements
                            1, //maximum_occupation
                            0, //is_worker
                            0, //produces_larva
                            1, //units_produced
                            "EvolutionChamber" //name
};
name_map["EvolutionChamber"] = 2;

meta_map[3] = {
                            15000, //mins
                            10000,  //gas
                            80, //build_time
                            0, //supply_delta
                            0, //provided_supply_delta
                            0, //max_energy
                            0, //start_energy
                            0, //ablty_cost
                            0x0000000000000001, //producer_id
                            Destiny::consumed_at_end, //destiny
                            0x0000000000000010, //requirements
                            1, //maximum_occupation
                            0, //is_worker
                            1, //produces_larva
                            1, //units_produced
                            "Lair" //name
};
name_map["Lair"] = 3;

meta_map[4] = {
                            20000, //mins
                            0,  //gas
                            65, //build_time
                            -1, //supply_delta
                            0, //provided_supply_delta
                            0, //max_energy
                            0, //start_energy
                            0, //ablty_cost
                            0x0000000000000200, //producer_id
                            Destiny::consumed_at_start, //destiny
                            0x0000000000020009, //requirements
                            1, //maximum_occupation
                            0, //is_worker
                            0, //produces_larva
                            1, //units_produced
                            "SpawningPool" //name
};
name_map["SpawningPool"] = 4;

meta_map[5] = {
                            5000, //mins
                            0,  //gas
                            24, //build_time
                            1, //supply_delta
                            0, //provided_supply_delta
                            0, //max_energy
                            0, //start_energy
                            0, //ablty_cost
                            0x0000000010000000, //producer_id
                            Destiny::consumed_at_start, //destiny
                            0x0000000000000010, //requirements
                            1, //maximum_occupation
                            0, //is_worker
                            0, //produces_larva
                            2, //units_produced
                            "Zergling" //name
};
name_map["Zergling"] = 5;

meta_map[6] = {
                            10000, //mins
                            10000,  //gas
                            50, //build_time
                            -1, //supply_delta
                            0, //provided_supply_delta
                            0, //max_energy
                            0, //start_energy
                            0, //ablty_cost
                            0x0000000000000200, //producer_id
                            Destiny::consumed_at_start, //destiny
                            0x0000000000020008, //requirements
                            1, //maximum_occupation
                            0, //is_worker
                            0, //produces_larva
                            1, //units_produced
                            "InfestationPit" //name
};
name_map["InfestationPit"] = 6;

meta_map[7] = {
                            2500, //mins
                            2500,  //gas
                            17, //build_time
                            0, //supply_delta
                            0, //provided_supply_delta
                            0, //max_energy
                            0, //start_energy
                            0, //ablty_cost
                            0x0000000000010000, //producer_id
                            Destiny::consumed_at_start, //destiny
                            0x0000000000020008, //requirements
                            1, //maximum_occupation
                            0, //is_worker
                            0, //produces_larva
                            1, //units_produced
                            "OverlordTransport" //name
};
name_map["OverlordTransport"] = 7;

meta_map[8] = {
                            5000, //mins
                            5000,  //gas
                            17, //build_time
                            0, //supply_delta
                            0, //provided_supply_delta
                            20000000, //max_energy
                            5000000, //start_energy
                            0, //ablty_cost
                            0x0000000000010000, //producer_id
                            Destiny::consumed_at_start, //destiny
                            0x0000000000020008, //requirements
                            1, //maximum_occupation
                            0, //is_worker
                            0, //produces_larva
                            1, //units_produced
                            "Overseer" //name
};
name_map["Overseer"] = 8;

meta_map[9] = {
                            5000, //mins
                            0,  //gas
                            17, //build_time
                            1, //supply_delta
                            0, //provided_supply_delta
                            0, //max_energy
                            0, //start_energy
                            0, //ablty_cost
                            0x0000000010000000, //producer_id
                            Destiny::consumed_at_start, //destiny
                            0x0000000000000000, //requirements
                            1, //maximum_occupation
                            1, //is_worker
                            0, //produces_larva
                            1, //units_produced
                            "Drone" //name
};
name_map["Drone"] = 9;

meta_map[10] = {
                            10000, //mins
                            5000,  //gas
                            60, //build_time
                            -1, //supply_delta
                            0, //provided_supply_delta
                            0, //max_energy
                            0, //start_energy
                            0, //ablty_cost
                            0x0000000000000200, //producer_id
                            Destiny::consumed_at_start, //destiny
                            0x0000000000000010, //requirements
                            1, //maximum_occupation
                            0, //is_worker
                            0, //produces_larva
                            1, //units_produced
                            "BanelingNest" //name
};
name_map["BanelingNest"] = 10;

meta_map[11] = {
                            10000, //mins
                            10000,  //gas
                            40, //build_time
                            -1, //supply_delta
                            0, //provided_supply_delta
                            0, //max_energy
                            0, //start_energy
                            0, //ablty_cost
                            0x0000000000000200, //producer_id
                            Destiny::consumed_at_start, //destiny
                            0x0000000000020008, //requirements
                            1, //maximum_occupation
                            0, //is_worker
                            0, //produces_larva
                            1, //units_produced
                            "HydraliskDen" //name
};
name_map["HydraliskDen"] = 11;

meta_map[12] = {
                            15000, //mins
                            0,  //gas
                            55, //build_time
                            -1, //supply_delta
                            0, //provided_supply_delta
                            0, //max_energy
                            0, //start_energy
                            0, //ablty_cost
                            0x0000000000000200, //producer_id
                            Destiny::consumed_at_start, //destiny
                            0x0000000000000010, //requirements
                            1, //maximum_occupation
                            0, //is_worker
                            0, //produces_larva
                            1, //units_produced
                            "RoachWarren" //name
};
name_map["RoachWarren"] = 12;

meta_map[13] = {
                            20000, //mins
                            20000,  //gas
                            100, //build_time
                            -1, //supply_delta
                            0, //provided_supply_delta
                            0, //max_energy
                            0, //start_energy
                            0, //ablty_cost
                            0x0000000000000200, //producer_id
                            Destiny::consumed_at_start, //destiny
                            0x0000000000020008, //requirements
                            1, //maximum_occupation
                            0, //is_worker
                            0, //produces_larva
                            1, //units_produced
                            "Spire" //name
};
name_map["Spire"] = 13;

meta_map[14] = {
                            10000, //mins
                            0,  //gas
                            50, //build_time
                            -1, //supply_delta
                            0, //provided_supply_delta
                            0, //max_energy
                            0, //start_energy
                            0, //ablty_cost
                            0x0000000000000200, //producer_id
                            Destiny::consumed_at_start, //destiny
                            0x0000000000000010, //requirements
                            1, //maximum_occupation
                            0, //is_worker
                            0, //produces_larva
                            1, //units_produced
                            "SpineCrawler" //name
};
name_map["SpineCrawler"] = 14;

meta_map[15] = {
                            2500, //mins
                            2500,  //gas
                            20, //build_time
                            0, //supply_delta
                            0, //provided_supply_delta
                            0, //max_energy
                            0, //start_energy
                            0, //ablty_cost
                            0x0000000000000020, //producer_id
                            Destiny::consumed_at_start, //destiny
                            0x0000000000000400, //requirements
                            1, //maximum_occupation
                            0, //is_worker
                            0, //produces_larva
                            1, //units_produced
                            "Baneling" //name
};
name_map["Baneling"] = 15;

meta_map[16] = {
                            10000, //mins
                            0,  //gas
                            25, //build_time
                            0, //supply_delta
                            8, //provided_supply_delta
                            0, //max_energy
                            0, //start_energy
                            0, //ablty_cost
                            0x0000000010000000, //producer_id
                            Destiny::consumed_at_start, //destiny
                            0x0000000000000000, //requirements
                            1, //maximum_occupation
                            0, //is_worker
                            0, //produces_larva
                            1, //units_produced
                            "Overlord" //name
};
name_map["Overlord"] = 16;

meta_map[17] = {
                            20000, //mins
                            15000,  //gas
                            100, //build_time
                            0, //supply_delta
                            0, //provided_supply_delta
                            0, //max_energy
                            0, //start_energy
                            0, //ablty_cost
                            0x0000000000000008, //producer_id
                            Destiny::consumed_at_end, //destiny
                            0x0000000000000040, //requirements
                            1, //maximum_occupation
                            0, //is_worker
                            1, //produces_larva
                            1, //units_produced
                            "Hive" //name
};
name_map["Hive"] = 17;

meta_map[18] = {
                            10000, //mins
                            7500,  //gas
                            40, //build_time
                            3, //supply_delta
                            0, //provided_supply_delta
                            0, //max_energy
                            0, //start_energy
                            0, //ablty_cost
                            0x0000000010000000, //producer_id
                            Destiny::consumed_at_start, //destiny
                            0x0000000000000040, //requirements
                            1, //maximum_occupation
                            0, //is_worker
                            0, //produces_larva
                            1, //units_produced
                            "SwarmHostMP" //name
};
name_map["SwarmHostMP"] = 18;

meta_map[19] = {
                            10000, //mins
                            20000,  //gas
                            40, //build_time
                            3, //supply_delta
                            0, //provided_supply_delta
                            20000000, //max_energy
                            5000000, //start_energy
                            0, //ablty_cost
                            0x0000000010000000, //producer_id
                            Destiny::consumed_at_start, //destiny
                            0x0000000000020000, //requirements
                            1, //maximum_occupation
                            0, //is_worker
                            0, //produces_larva
                            1, //units_produced
                            "Viper" //name
};
name_map["Viper"] = 19;

meta_map[20] = {
                            2500, //mins
                            0,  //gas
                            30, //build_time
                            -1, //supply_delta
                            0, //provided_supply_delta
                            0, //max_energy
                            0, //start_energy
                            0, //ablty_cost
                            0x0000000000000200, //producer_id
                            Destiny::consumed_at_start, //destiny
                            0x0000000000000000, //requirements
                            1, //maximum_occupation
                            0, //is_worker
                            0, //produces_larva
                            1, //units_produced
                            "Extractor" //name
};
name_map["Extractor"] = 20;

meta_map[21] = {
                            10000, //mins
                            15000,  //gas
                            100, //build_time
                            0, //supply_delta
                            0, //provided_supply_delta
                            0, //max_energy
                            0, //start_energy
                            0, //ablty_cost
                            0x0000000000002000, //producer_id
                            Destiny::consumed_at_end, //destiny
                            0x0000000000020000, //requirements
                            1, //maximum_occupation
                            0, //is_worker
                            0, //produces_larva
                            1, //units_produced
                            "GreaterSpire" //name
};
name_map["GreaterSpire"] = 21;

meta_map[22] = {
                            10000, //mins
                            5000,  //gas
                            33, //build_time
                            2, //supply_delta
                            0, //provided_supply_delta
                            0, //max_energy
                            0, //start_energy
                            0, //ablty_cost
                            0x0000000010000000, //producer_id
                            Destiny::consumed_at_start, //destiny
                            0x0000000000000800, //requirements
                            1, //maximum_occupation
                            0, //is_worker
                            0, //produces_larva
                            1, //units_produced
                            "Hydralisk" //name
};
name_map["Hydralisk"] = 22;

meta_map[23] = {
                            10000, //mins
                            15000,  //gas
                            50, //build_time
                            2, //supply_delta
                            0, //provided_supply_delta
                            20000000, //max_energy
                            5000000, //start_energy
                            0, //ablty_cost
                            0x0000000010000000, //producer_id
                            Destiny::consumed_at_start, //destiny
                            0x0000000000000040, //requirements
                            1, //maximum_occupation
                            0, //is_worker
                            0, //produces_larva
                            1, //units_produced
                            "Infestor" //name
};
name_map["Infestor"] = 23;

meta_map[24] = {
                            10000, //mins
                            10000,  //gas
                            33, //build_time
                            2, //supply_delta
                            0, //provided_supply_delta
                            0, //max_energy
                            0, //start_energy
                            0, //ablty_cost
                            0x0000000010000000, //producer_id
                            Destiny::consumed_at_start, //destiny
                            0x0000000000202000, //requirements
                            1, //maximum_occupation
                            0, //is_worker
                            0, //produces_larva
                            1, //units_produced
                            "Mutalisk" //name
};
name_map["Mutalisk"] = 24;

meta_map[25] = {
                            7500, //mins
                            2500,  //gas
                            27, //build_time
                            2, //supply_delta
                            0, //provided_supply_delta
                            0, //max_energy
                            0, //start_energy
                            0, //ablty_cost
                            0x0000000010000000, //producer_id
                            Destiny::consumed_at_start, //destiny
                            0x0000000000001000, //requirements
                            1, //maximum_occupation
                            0, //is_worker
                            0, //produces_larva
                            1, //units_produced
                            "Roach" //name
};
name_map["Roach"] = 25;

meta_map[26] = {
                            7500, //mins
                            0,  //gas
                            30, //build_time
                            -1, //supply_delta
                            0, //provided_supply_delta
                            0, //max_energy
                            0, //start_energy
                            0, //ablty_cost
                            0x0000000000000200, //producer_id
                            Destiny::consumed_at_start, //destiny
                            0x0000000000000010, //requirements
                            1, //maximum_occupation
                            0, //is_worker
                            0, //produces_larva
                            1, //units_produced
                            "SporeCrawler" //name
};
name_map["SporeCrawler"] = 26;

meta_map[27] = {
                            15000, //mins
                            20000,  //gas
                            65, //build_time
                            -1, //supply_delta
                            0, //provided_supply_delta
                            0, //max_energy
                            0, //start_energy
                            0, //ablty_cost
                            0x0000000000000200, //producer_id
                            Destiny::consumed_at_start, //destiny
                            0x0000000000020000, //requirements
                            1, //maximum_occupation
                            0, //is_worker
                            0, //produces_larva
                            1, //units_produced
                            "UltraliskCavern" //name
};
name_map["UltraliskCavern"] = 27;

meta_map[28] = {
                            0, //mins
                            0,  //gas
                            0, //build_time
                            0, //supply_delta
                            0, //provided_supply_delta
                            0, //max_energy
                            0, //start_energy
                            0, //ablty_cost
                            0x0000000000000000, //producer_id
                            Destiny::occupied, //destiny
                            0x0000000000000000, //requirements
                            1, //maximum_occupation
                            0, //is_worker
                            0, //produces_larva
                            1, //units_produced
                            "Larva" //name
};
name_map["Larva"] = 28;

meta_map[29] = {
                            15000, //mins
                            15000,  //gas
                            50, //build_time
                            -1, //supply_delta
                            0, //provided_supply_delta
                            0, //max_energy
                            0, //start_energy
                            0, //ablty_cost
                            0x0000000000000200, //producer_id
                            Destiny::consumed_at_start, //destiny
                            0x0000000000020008, //requirements
                            1, //maximum_occupation
                            0, //is_worker
                            0, //produces_larva
                            1, //units_produced
                            "NydusNetwork" //name
};
name_map["NydusNetwork"] = 29;

meta_map[30] = {
                            2500, //mins
                            7500,  //gas
                            12, //build_time
                            1, //supply_delta
                            0, //provided_supply_delta
                            0, //max_energy
                            0, //start_energy
                            0, //ablty_cost
                            0x0000000002000000, //producer_id
                            Destiny::consumed_at_start, //destiny
                            0x0000000000001000, //requirements
                            1, //maximum_occupation
                            0, //is_worker
                            0, //produces_larva
                            1, //units_produced
                            "Ravager" //name
};
name_map["Ravager"] = 30;

meta_map[31] = {
                            5000, //mins
                            10000,  //gas
                            25, //build_time
                            1, //supply_delta
                            0, //provided_supply_delta
                            0, //max_energy
                            0, //start_energy
                            0, //ablty_cost
                            0x0000000000400000, //producer_id
                            Destiny::consumed_at_start, //destiny
                            0x0000000200000000, //requirements
                            1, //maximum_occupation
                            0, //is_worker
                            0, //produces_larva
                            1, //units_produced
                            "LurkerMP" //name
};
name_map["LurkerMP"] = 31;

meta_map[32] = {
                            5000, //mins
                            5000,  //gas
                            20, //build_time
                            0, //supply_delta
                            0, //provided_supply_delta
                            0, //max_energy
                            0, //start_energy
                            0, //ablty_cost
                            0x0000000020000000, //producer_id
                            Destiny::occupied, //destiny
                            0x0000000020000000, //requirements
                            1, //maximum_occupation
                            0, //is_worker
                            0, //produces_larva
                            1, //units_produced
                            "NydusCanal" //name
};
name_map["NydusCanal"] = 32;

meta_map[33] = {
                            10000, //mins
                            15000,  //gas
                            120, //build_time
                            -1, //supply_delta
                            0, //provided_supply_delta
                            0, //max_energy
                            0, //start_energy
                            0, //ablty_cost
                            0x0000000000000200, //producer_id
                            Destiny::consumed_at_start, //destiny
                            0x0000000000000800, //requirements
                            1, //maximum_occupation
                            0, //is_worker
                            0, //produces_larva
                            1, //units_produced
                            "LurkerDenMP" //name
};
name_map["LurkerDenMP"] = 33;

meta_map[34] = {
                            15000, //mins
                            10000,  //gas
                            40, //build_time
                            2, //supply_delta
                            0, //provided_supply_delta
                            0, //max_energy
                            0, //start_energy
                            0, //ablty_cost
                            0x0000000010000000, //producer_id
                            Destiny::consumed_at_start, //destiny
                            0x0000000000202000, //requirements
                            1, //maximum_occupation
                            0, //is_worker
                            0, //produces_larva
                            1, //units_produced
                            "Corruptor" //name
};
name_map["Corruptor"] = 34;

meta_map[35] = {
                            30000, //mins
                            20000,  //gas
                            55, //build_time
                            6, //supply_delta
                            0, //provided_supply_delta
                            0, //max_energy
                            0, //start_energy
                            0, //ablty_cost
                            0x0000000010000000, //producer_id
                            Destiny::consumed_at_start, //destiny
                            0x0000000008000000, //requirements
                            1, //maximum_occupation
                            0, //is_worker
                            0, //produces_larva
                            1, //units_produced
                            "Ultralisk" //name
};
name_map["Ultralisk"] = 35;

meta_map[36] = {
                            15000, //mins
                            15000,  //gas
                            34, //build_time
                            2, //supply_delta
                            0, //provided_supply_delta
                            0, //max_energy
                            0, //start_energy
                            0, //ablty_cost
                            0x0000000400000000, //producer_id
                            Destiny::consumed_at_start, //destiny
                            0x0000000000200000, //requirements
                            1, //maximum_occupation
                            0, //is_worker
                            0, //produces_larva
                            1, //units_produced
                            "BroodLord" //name
};
name_map["BroodLord"] = 36;

meta_map[37] = {2};