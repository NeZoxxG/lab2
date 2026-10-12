#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <stddef.h>

#define MIN_ADDENDS 2
#define MAX_ADDENDS 7
#define MAX_WORDS (MAX_ADDENDS + 1)
#define MAX_WORD_LEN 17
#define MAX_LETTERS 10

typedef struct {
    char words[MAX_WORDS][MAX_WORD_LEN + 1];
    int word_count;
    char letters[MAX_LETTERS];
    int letter_count;
} Puzzle;

typedef int (*solver_fn)(const char* puzzle, char* out, size_t out_size, long long* checked);

int parse_puzzle(const char* input, Puzzle* p);
int format_solution(const Puzzle* p, const int digit_of[26], char* out, size_t out_size);
int check_solution(const char* puzzle, const char* solution);

int solve_source(const char* puzzle, char* out, size_t out_size, long long* checked);
int solve_weights(const char* puzzle, char* out, size_t out_size, long long* checked);
int solve_hotloop(const char* puzzle, char* out, size_t out_size, long long* checked);
int solve_backtrack(const char* puzzle, char* out, size_t out_size, long long* checked);
int solve_heuristics(const char* puzzle, char* out, size_t out_size, long long* checked);

typedef struct {
    const char* name;
    solver_fn fn;
} SolverInfo;

extern const SolverInfo SOLVERS[];
extern const int SOLVER_COUNT;

#endif