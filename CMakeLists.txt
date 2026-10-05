cmake_minimum_required(VERSION 4.2)
project(_2 C)

set(CMAKE_C_STANDARD 11)

add_executable(_2 "general/source_program.c"
        general/optimize_weights.c
        general/functions.c
        general/functions.h
        general/main.c
        general/benchmark.c
        general/optimize_hotloop.c
        general/optimize_backtrack.c
        general/optimize_heuristics.c)
