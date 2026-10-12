#define _POSIX_C_SOURCE 199309L

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "functions.h"

#ifdef _WIN32
#include <windows.h>
#endif

#define RUNS 7
#define MIN_SAMPLE_MS 20.0
#define MAX_REPS 100000

static const char* PUZZLES[] = {
    "TO + GO = OUT",
    "ONE + ONE = TWO",
    "TWO + TWO = FOUR",
    "BASE + BALL = GAMES",
    "SEND + MORE = MONEY",
    "BIG + CAT = LION",
    "CROSS + ROADS = DANGER",
    "A + B + C + D + E + F + G = HI",
    "ELEVEN + NINE + FIVE + FIVE = THIRTY",
    "SATURN + URANUS + NEPTUNE + PLUTO = PLANETS",
};
static const int PUZZLE_COUNT = (int)(sizeof PUZZLES / sizeof PUZZLES[0]);

static double now_ms(void)
{
#ifdef _WIN32
    static LARGE_INTEGER freq;
    LARGE_INTEGER t;
    if (freq.QuadPart == 0)
        QueryPerformanceFrequency(&freq);
    QueryPerformanceCounter(&t);
    return (double)t.QuadPart * 1000.0 / (double)freq.QuadPart;
#else
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec * 1000.0 + ts.tv_nsec / 1e6;
#endif
}

static int cmp_double(const void* a, const void* b)
{
    double x = *(const double*)a, y = *(const double*)b;
    return (x > y) - (x < y);
}

static double measure(solver_fn fn, const char* puzzle, char* out, size_t out_size,
                      long long* nodes, int* found)
{
    double samples[RUNS];
    char scratch[256];

    double t0 = now_ms();
    *found = fn(puzzle, out, out_size, nodes);
    double one = now_ms() - t0;

    int reps = 1;
    if (one < MIN_SAMPLE_MS) {
        reps = (one > 1e-4) ? (int)(MIN_SAMPLE_MS / one) + 1 : MAX_REPS;
        if (reps > MAX_REPS)
            reps = MAX_REPS;
    }

    for (int r = 0; r < RUNS; r++) {
        t0 = now_ms();
        for (int i = 0; i < reps; i++)
            fn(puzzle, scratch, sizeof scratch, NULL);
        samples[r] = (now_ms() - t0) / reps;
    }

    qsort(samples, RUNS, sizeof samples[0], cmp_double);
    return samples[RUNS / 2];
}

int main(int argc, char** argv)
{
    const char* csv_path = (argc > 1) ? argv[1] : "results.csv";
    FILE* csv = fopen(csv_path, "w");
    int failures = 0;

    if (!csv) {
        perror(csv_path);
        return 1;
    }
    fprintf(csv, "version;puzzle;letters;addends;time_ms;nodes;ok\n");

    printf("%-11s %-44s %7s %12s %12s  %s\n", "version", "puzzle", "letters", "time, ms", "nodes",
           "check");

    for (int v = 0; v < SOLVER_COUNT; v++) {
        for (int i = 0; i < PUZZLE_COUNT; i++) {
            Puzzle p;
            char answer[256];
            long long nodes = 0;
            int found = 0;

            if (parse_puzzle(PUZZLES[i], &p) != 0) {
                fprintf(stderr, "Bad puzzle in the list: %s\n", PUZZLES[i]);
                fclose(csv);
                return 1;
            }

            double ms = measure(SOLVERS[v].fn, PUZZLES[i], answer, sizeof answer, &nodes, &found);
            int ok = found && check_solution(PUZZLES[i], answer);
            if (!ok)
                failures++;

            printf("%-11s %-44s %7d %12.4f %12lld  %s\n", SOLVERS[v].name, PUZZLES[i],
                   p.letter_count, ms, nodes, ok ? "OK" : "FAIL");

            char ms_text[64];
            snprintf(ms_text, sizeof ms_text, "%.6f", ms);
            for (char* c = ms_text; *c; c++)
                if (*c == '.')
                    *c = ',';

            fprintf(csv, "%s;%s;%d;%d;%s;%lld;%d\n", SOLVERS[v].name, PUZZLES[i], p.letter_count,
                    p.word_count -1, ms_text, nodes, ok);
            fflush(stdout);
        }
        printf("\n");
    }

    fclose(csv);
    printf("Results saved to %s\n", csv_path);
    if (failures)
        printf("WARNING: %d wrong answers\n", failures);
    return failures ? 1 : 0;
}