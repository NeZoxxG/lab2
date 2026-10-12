cmake_minimum_required(VERSION 3.16)
project(Laba2 C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
    add_compile_options(-Wall -Wextra)
endif()

set(SOLVER_SOURCES
        general/functions.c
        general/source_program.c
        general/optimize_weights.c
        general/optimize_hotloop.c
        general/optimize_backtrack.c
        general/optimize_heuristics.c
)

add_executable(rebus general/main.c ${SOLVER_SOURCES})

add_executable(benchmark general/benchmark.c ${SOLVER_SOURCES})