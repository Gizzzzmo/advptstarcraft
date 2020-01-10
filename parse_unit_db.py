#!/usr/bin/python3

from sys import argv

line_string = """
meta_map[{class_id}] = {{{mins}, {gas}, {build_time}, {supply_delta}, {provided_supply_delta}, {max_energy}, {start_energy}, {ablty_cost}, 0x{producer_id:0>16X}, {destiny}, 0x{requirements:0>16X}, {maximum_occupation}, {is_worker}, {produces_larva}, {units_produced}, "{name}"}};
name_map["{name}"] = {class_id};
"""

def make_bitmap(dict_of_entities, entity_names):
    if entity_names == ['']:
        return 0
    return sum(map(int, map(lambda x : 1 << dict_of_entities[x]["class_id"], entity_names)))

if len(argv) < 2:
    print("Usage ",argv[0]," unitfile")
    exit(1)

db = open(argv[1], "r")

zergs   = {}
zerg_c = 0
prot  = {}
prot_c = 0
terr  = {}
terr_c = 0

for i, line in enumerate(db):

    line = line.split('#')[0].rstrip()
    if line == '' or i == 0:
        continue
    values = line.split(",")
    e = {}

    e["name"] = values[0]
    e["destiny"] = values[1]
    e["mins"] = int(values[2]) * 100
    e["gas"] = int(values[3]) * 100
    e["units_produced"] = int(values[9])
    e["supply"] = int(float(values[5]) * e["units_produced"])
    e["provided_supply"] = int(float(values[6]))
    e["build_time"] = int(values[7])
    e["maximum_occupation"] = values[8]
    e["producer"] = values[10]
    e["dependencies"] = values[11]
    #e["produces"] = values[12]
    e["start_energy"] = int(values[13]) * 100000
    e["max_energy"] = int(values[14]) * 100000
    e["structure"] = bool(values[15])
    e["race"] = values[16]
    
    e["is_worker"] =  1 if values[0] in ("Drone", "SCV", "Probe") else 0
    if values[16] == "Zerg":
        e["produces_larva"] = 1 if values[0] in ("Hatchery", "Lair", "Hive") else 0
        e["class_id"] = zerg_c
        e["ablty_cost"] = 2500000 if values[0] == "Queen" else 0
        zerg_c +=1
        zergs[e["name"]] = e
    if values[16] == "Prot":
        e["produces_larva"] = 0
        e["class_id"] = prot_c
        e["ablty_cost"] = 5000000 if values[0] == "Nexus" else 0
        prot_c +=1
        prot[e["name"]] = e
    if values[16] == "Terr":
        e["produces_larva"] = 0
        e["class_id"] = terr_c
        e["ablty_cost"] = 5000000 if values[0] == "OrbitalCommand" else 0
        terr_c +=1
        terr[e["name"]] = e

for entities in (zergs, prot, terr):
    for name, e in entities.items():
        e["destiny"] = {
                "producer_consumed_at_start":   "Destiny::consumed_at_start",
                "producer_consumed_at_end":     "Destiny::consumed_at_end",
                "producer_occupied":            "Destiny::occupied",
                "producer_not_occupied":        "Destiny::freed"
                }[e["destiny"]]
        e["race"] = {
                "Zerg": "Race::Zerg",
                "Prot": "Race::Protoss",
                "Terr": "Race::Terran"
                }[e["race"]]
        e["requirements"] = make_bitmap(entities, e["dependencies"].split("/"))
        if e["producer"] != "" and e["destiny"] in ("Destiny::consumed_at_start", "Destiny::consumed_at_end"):
            e["provided_supply_delta"] = e["provided_supply"] - entities[e["producer"].split("/")[0]]["provided_supply"]
        else:
            e["provided_supply_delta"] = e["provided_supply"]
        if e["producer"] != "" and e["destiny"] in ("Destiny::consumed_at_start", "Destiny::consumed_at_end"):
            e["supply_delta"] = e["supply"] - entities[e["producer"].split("/")[0]]["supply"]
        else:
            e["supply_delta"] = e["supply"]
        e["producer_id"] = make_bitmap(entities, e["producer"].split("/"))


unit_map_zerg = open("unit_map_zerg.h", "w")
for k,e in zergs.items():
    unit_map_zerg.write(line_string.format(**e))

unit_map_protoss = open("unit_map_protoss.h", "w")
for k,e in prot.items():
    unit_map_protoss.write(line_string.format(**e))

unit_map_terran = open("unit_map_terran.h", "w")
for k,e in terr.items():
    unit_map_terran.write(line_string.format(**e))

