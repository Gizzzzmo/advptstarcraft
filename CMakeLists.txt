SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -flto")
SET(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3")
SET(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0")
add_executable(forward forward.cpp Simulator.cpp ProductionEntry.cpp Entity.cpp GameState.cpp)
add_executable(optimize optimize.cpp Simulator.cpp ProductionEntry.cpp Entity.cpp GameState.cpp)
