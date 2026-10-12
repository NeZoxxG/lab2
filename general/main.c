#include <stdio.h>
#include <string.h>

#include "functions.h"

int main(int argc, char** argv)
{
    char puzzle[512] = "";
    char answer[256];
    long long checked = 0;
    int version = SOLVER_COUNT - 1;
    int first = 1;

    if (argc > 1 && argv[1][0] >= '0' && argv[1][0] <= '9' && argv[1][1] == '\0') {
        version = argv[1][0] - '0';
        if (version >= SOLVER_COUNT) {
            fprintf(stderr, "Unknown version %d (0..%d)\n", version, SOLVER_COUNT - 1);
            return 1;
        }
        first = 2;
    }

    if (argc > first) {
        size_t len = 0;
        for (int i = first; i < argc; i++) {
            size_t part = strlen(argv[i]);
            if (len + part + 2 > sizeof puzzle) {
                fprintf(stderr, "Puzzle is too long\n");
                return 1;
            }
            if (i > first)
                puzzle[len++] = ' ';
            memcpy(puzzle + len, argv[i], part + 1);
            len += part;
        }
    } else {
        printf("Enter puzzle: ");
        fflush(stdout);
        if (!fgets(puzzle, sizeof puzzle, stdin))
            return 1;
        puzzle[strcspn(puzzle, "\r\n")] = '\0';
    }

    if (!SOLVERS[version].fn(puzzle, answer, sizeof answer, &checked)) {
        printf("Invalid puzzle or no solution found\n");
        return 1;
    }

    printf("Version:  %s\n", SOLVERS[version].name);
    printf("Puzzle:   %s\n", puzzle);
    printf("Solution: %s\n", answer);
    printf("Nodes:    %lld\n", checked);
    printf("Verify:   %s\n", check_solution(puzzle, answer) ? "OK" : "FAIL");
    return 0;
}