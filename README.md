# advptstarcraft
game_state:
*  minerals (in Hundertstel) : int
*  gas (in Hundertstel) : int
*  worker\_mining_minerals : int
*  worker\_mining_gas : int
*  entities_done : hashmap<int, collection<Entity>> (vector/list/heap?)
*  max_supply : int
*  productions/ entities\_in\_production : collection<Triple(Entity, Entity, int)> (producee, producer, time\_alive, bzw. neue Klasse für die Entries anstatt Tripel)


Entity:

static: (bzw ich glaub static vererbung funktioniert so nicht, stattdessen vielleicht konstante virtual Funktionen, oder die dinger alle zu template argumenten machen?)
*  minerals (in Hundertstel?) : int
*  gas (in Hundertstel?) : int
*  supply : int
*  supply_provided : int
*  class_id : int
*  productionTime : int
*  producer\_destiny : enum(consumed\_at\_start/consumed\_at\_end/occupied/freed)
*  producer : int (id des producer)
*  requirements : collection<int> (disjunktive liste der ids der Entities die erlauben die entsprechende Entity zu bauen)




non-static:
*  occupied : int (semaphore, 0 entspricht occupied, n>0 entrspricht n slots zur verarbeitung verfügbar)
    

