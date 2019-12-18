
build_map["Hatchery"] = makeEntity<Race::Zerg, 0, 30000, 0, -1, 6, 0, 0, 0, 0x0000000000000200, Destiny::consumed_at_start, 0x0000000000000000, 1, 100, 0, 1, 1>;
name_map[0] = "Hatchery";

build_map["Queen"] = makeEntity<Race::Zerg, 1, 15000, 0, 2, 0, 200, 25, 25, 0x0000000000020009, Destiny::occupied, 0x0000000000000010, 1, 50, 0, 0, 1>;
name_map[1] = "Queen";

build_map["EvolutionChamber"] = makeEntity<Race::Zerg, 2, 7500, 0, -1, 0, 0, 0, 0, 0x0000000000000200, Destiny::consumed_at_start, 0x0000000000020009, 1, 35, 0, 0, 1>;
name_map[2] = "EvolutionChamber";

build_map["Lair"] = makeEntity<Race::Zerg, 3, 15000, 10000, 0, 0, 0, 0, 0, 0x0000000000000001, Destiny::consumed_at_end, 0x0000000000000010, 1, 80, 0, 1, 1>;
name_map[3] = "Lair";

build_map["SpawningPool"] = makeEntity<Race::Zerg, 4, 20000, 0, -1, 0, 0, 0, 0, 0x0000000000000200, Destiny::consumed_at_start, 0x0000000000020009, 1, 65, 0, 0, 1>;
name_map[4] = "SpawningPool";

build_map["Zergling"] = makeEntity<Race::Zerg, 5, 5000, 0, 1, 0, 0, 0, 0, 0x0000000010000000, Destiny::consumed_at_start, 0x0000000000000010, 1, 24, 0, 0, 2>;
name_map[5] = "Zergling";

build_map["InfestationPit"] = makeEntity<Race::Zerg, 6, 10000, 10000, -1, 0, 0, 0, 0, 0x0000000000000200, Destiny::consumed_at_start, 0x0000000000020008, 1, 50, 0, 0, 1>;
name_map[6] = "InfestationPit";

build_map["OverlordTransport"] = makeEntity<Race::Zerg, 7, 2500, 2500, 0, 0, 0, 0, 0, 0x0000000000010000, Destiny::consumed_at_start, 0x0000000000020008, 1, 17, 0, 0, 1>;
name_map[7] = "OverlordTransport";

build_map["Overseer"] = makeEntity<Race::Zerg, 8, 5000, 5000, 0, 0, 200, 50, 0, 0x0000000000010000, Destiny::consumed_at_start, 0x0000000000020008, 1, 17, 0, 0, 1>;
name_map[8] = "Overseer";

build_map["Drone"] = makeEntity<Race::Zerg, 9, 5000, 0, 1, 0, 0, 0, 0, 0x0000000010000000, Destiny::consumed_at_start, 0x0000000000000000, 1, 17, 1, 0, 1>;
name_map[9] = "Drone";

build_map["BanelingNest"] = makeEntity<Race::Zerg, 10, 10000, 5000, -1, 0, 0, 0, 0, 0x0000000000000200, Destiny::consumed_at_start, 0x0000000000000010, 1, 60, 0, 0, 1>;
name_map[10] = "BanelingNest";

build_map["HydraliskDen"] = makeEntity<Race::Zerg, 11, 10000, 10000, -1, 0, 0, 0, 0, 0x0000000000000200, Destiny::consumed_at_start, 0x0000000000020008, 1, 40, 0, 0, 1>;
name_map[11] = "HydraliskDen";

build_map["RoachWarren"] = makeEntity<Race::Zerg, 12, 15000, 0, -1, 0, 0, 0, 0, 0x0000000000000200, Destiny::consumed_at_start, 0x0000000000000010, 1, 55, 0, 0, 1>;
name_map[12] = "RoachWarren";

build_map["Spire"] = makeEntity<Race::Zerg, 13, 20000, 20000, -1, 0, 0, 0, 0, 0x0000000000000200, Destiny::consumed_at_start, 0x0000000000020008, 1, 100, 0, 0, 1>;
name_map[13] = "Spire";

build_map["SpineCrawler"] = makeEntity<Race::Zerg, 14, 10000, 0, -1, 0, 0, 0, 0, 0x0000000000000200, Destiny::consumed_at_start, 0x0000000000000010, 1, 50, 0, 0, 1>;
name_map[14] = "SpineCrawler";

build_map["Baneling"] = makeEntity<Race::Zerg, 15, 2500, 2500, -1, 0, 0, 0, 0, 0x0000000000000020, Destiny::consumed_at_start, 0x0000000000000400, 1, 20, 0, 0, 1>;
name_map[15] = "Baneling";

build_map["Overlord"] = makeEntity<Race::Zerg, 16, 10000, 0, 0, 8, 0, 0, 0, 0x0000000010000000, Destiny::consumed_at_start, 0x0000000000000000, 1, 25, 0, 0, 1>;
name_map[16] = "Overlord";

build_map["Hive"] = makeEntity<Race::Zerg, 17, 20000, 15000, 0, 0, 0, 0, 0, 0x0000000000000008, Destiny::consumed_at_end, 0x0000000000000040, 1, 100, 0, 1, 1>;
name_map[17] = "Hive";

build_map["SwarmHostMP"] = makeEntity<Race::Zerg, 18, 10000, 7500, 3, 0, 0, 0, 0, 0x0000000010000000, Destiny::consumed_at_start, 0x0000000000000040, 1, 40, 0, 0, 1>;
name_map[18] = "SwarmHostMP";

build_map["Viper"] = makeEntity<Race::Zerg, 19, 10000, 20000, 3, 0, 200, 50, 0, 0x0000000010000000, Destiny::consumed_at_start, 0x0000000000020000, 1, 40, 0, 0, 1>;
name_map[19] = "Viper";

build_map["Extractor"] = makeEntity<Race::Zerg, 20, 2500, 0, -1, 0, 0, 0, 0, 0x0000000000000200, Destiny::consumed_at_start, 0x0000000000000000, 1, 30, 0, 0, 1>;
name_map[20] = "Extractor";

build_map["GreaterSpire"] = makeEntity<Race::Zerg, 21, 10000, 15000, 0, 0, 0, 0, 0, 0x0000000000002000, Destiny::consumed_at_end, 0x0000000000020000, 1, 100, 0, 0, 1>;
name_map[21] = "GreaterSpire";

build_map["Hydralisk"] = makeEntity<Race::Zerg, 22, 10000, 5000, 2, 0, 0, 0, 0, 0x0000000010000000, Destiny::consumed_at_start, 0x0000000000000800, 1, 33, 0, 0, 1>;
name_map[22] = "Hydralisk";

build_map["Infestor"] = makeEntity<Race::Zerg, 23, 10000, 15000, 2, 0, 200, 50, 0, 0x0000000010000000, Destiny::consumed_at_start, 0x0000000000000040, 1, 50, 0, 0, 1>;
name_map[23] = "Infestor";

build_map["Mutalisk"] = makeEntity<Race::Zerg, 24, 10000, 10000, 2, 0, 0, 0, 0, 0x0000000010000000, Destiny::consumed_at_start, 0x0000000000202000, 1, 33, 0, 0, 1>;
name_map[24] = "Mutalisk";

build_map["Roach"] = makeEntity<Race::Zerg, 25, 7500, 2500, 2, 0, 0, 0, 0, 0x0000000010000000, Destiny::consumed_at_start, 0x0000000000001000, 1, 27, 0, 0, 1>;
name_map[25] = "Roach";

build_map["SporeCrawler"] = makeEntity<Race::Zerg, 26, 7500, 0, -1, 0, 0, 0, 0, 0x0000000000000200, Destiny::consumed_at_start, 0x0000000000000010, 1, 30, 0, 0, 1>;
name_map[26] = "SporeCrawler";

build_map["UltraliskCavern"] = makeEntity<Race::Zerg, 27, 15000, 20000, -1, 0, 0, 0, 0, 0x0000000000000200, Destiny::consumed_at_start, 0x0000000000020000, 1, 65, 0, 0, 1>;
name_map[27] = "UltraliskCavern";

build_map["Larva"] = makeEntity<Race::Zerg, 28, 0, 0, 0, 0, 0, 0, 0, 0x0000000000000000, Destiny::occupied, 0x0000000000000000, 1, 0, 0, 0, 1>;
name_map[28] = "Larva";

build_map["NydusNetwork"] = makeEntity<Race::Zerg, 29, 15000, 15000, -1, 0, 0, 0, 0, 0x0000000000000200, Destiny::consumed_at_start, 0x0000000000020008, 1, 50, 0, 0, 1>;
name_map[29] = "NydusNetwork";

build_map["Ravager"] = makeEntity<Race::Zerg, 30, 2500, 7500, 1, 0, 0, 0, 0, 0x0000000002000000, Destiny::consumed_at_start, 0x0000000000001000, 1, 12, 0, 0, 1>;
name_map[30] = "Ravager";

build_map["LurkerMP"] = makeEntity<Race::Zerg, 31, 5000, 10000, 1, 0, 0, 0, 0, 0x0000000000400000, Destiny::consumed_at_start, 0x0000000200000000, 1, 25, 0, 0, 1>;
name_map[31] = "LurkerMP";

build_map["NydusCanal"] = makeEntity<Race::Zerg, 32, 5000, 5000, 0, 0, 0, 0, 0, 0x0000000020000000, Destiny::occupied, 0x0000000020000000, 1, 20, 0, 0, 1>;
name_map[32] = "NydusCanal";

build_map["LurkerDenMP"] = makeEntity<Race::Zerg, 33, 10000, 15000, -1, 0, 0, 0, 0, 0x0000000000000200, Destiny::consumed_at_start, 0x0000000000000800, 1, 120, 0, 0, 1>;
name_map[33] = "LurkerDenMP";

build_map["Corruptor"] = makeEntity<Race::Zerg, 34, 15000, 10000, 2, 0, 0, 0, 0, 0x0000000010000000, Destiny::consumed_at_start, 0x0000000000202000, 1, 40, 0, 0, 1>;
name_map[34] = "Corruptor";

build_map["Ultralisk"] = makeEntity<Race::Zerg, 35, 30000, 20000, 6, 0, 0, 0, 0, 0x0000000010000000, Destiny::consumed_at_start, 0x0000000008000000, 1, 55, 0, 0, 1>;
name_map[35] = "Ultralisk";

build_map["BroodLord"] = makeEntity<Race::Zerg, 36, 15000, 15000, 2, 0, 0, 0, 0, 0x0000000400000000, Destiny::consumed_at_start, 0x0000000000200000, 1, 34, 0, 0, 1>;
name_map[36] = "BroodLord";
