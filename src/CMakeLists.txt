cmake_minimum_required(VERSION 3.20)
project(PPSE_TP2)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

add_compile_options("-Wall" "-g" "-O3" "-march=native")

add_executable(test main.c transmitter.c receiver.c)
add_executable(simulator simulator.c transmitter.c receiver.c)
add_executable(simulator_thread simulator_thread.c transmitter.c receiver.c)

target_include_directories(test PRIVATE ../include)
target_include_directories(simulator PRIVATE ../include)
target_include_directories(simulator_thread PRIVATE ../include)

target_compile_definitions(simulator PRIVATE ENABLE_STATS _REENTRANT)
target_compile_definitions(simulator_thread PRIVATE ENABLE_STATS _REENTRANT)

# This tells CMake: "Link the math library (m) to the target"
target_link_libraries(test PRIVATE m)
target_link_libraries(simulator PRIVATE m)
target_link_libraries(simulator_thread PRIVATE m)
target_link_libraries(simulator_thread PRIVATE pthread)