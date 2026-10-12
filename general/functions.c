#include "functions.h"

#include <ctype.h>
#include <string.h>

static int is_letter(int c)
{
    return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z');
}

static int is_digit_char(int c)
{
    return c >= '0' && c <= '9';
}

static int split_expression(const char* s, char words[MAX_WORDS][MAX_WORD_LEN + 1],
                            int (*is_word_char)(int))
{
    int count = 0;
    int len = 0;
    int word_closed = 0;
    int seen_eq = 0;

    for (;; s++) {
        int c = (unsigned char)*s;

        if (is_word_char(c)) {
            if (word_closed)
                return -1;
            if (count >= MAX_WORDS || len >= MAX_WORD_LEN)
                return -1;
            words[count][len++] = (char)toupper(c);
        } else if (isspace(c)) {
            if (len > 0)
                word_closed = 1;
        } else if (c == '+' || c == '=' || c == '\0') {
            if (len == 0)
                return -1;
            if (c == '+' && seen_eq)
                return -1;
            if (c == '=') {
                if (seen_eq)
                    return -1;
                seen_eq = 1;
            }
            words[count][len] = '\0';
            count++;
            len = 0;
            word_closed = 0;
            if (c == '\0')
                break;
        } else {
            return -1;
        }
    }

    if (!seen_eq)
        return -1;
    if (count - 1 < MIN_ADDENDS || count - 1 > MAX_ADDENDS)
        return -1;
    return count;
}

int parse_puzzle(const char* input, Puzzle* p)
{
    int seen[26] = { 0 };
    int n = split_expression(input, p->words, is_letter);
    if (n < 0)
        return -1;

    p->word_count = n;
    p->letter_count = 0;

    for (int w = 0; w < n; w++) {
        for (const char* c = p->words[w]; *c; c++) {
            int i = *c - 'A';
            if (!seen[i]) {
                if (p->letter_count == MAX_LETTERS)
                    return -1;
                seen[i] = 1;
                p->letters[p->letter_count++] = *c;
            }
        }
    }
    return 0;
}

static int append(char* out, size_t out_size, size_t* pos, const char* s)
{
    size_t len = strlen(s);
    if (*pos + len + 1 > out_size)
        return -1;
    memcpy(out + *pos, s, len + 1);
    *pos += len;
    return 0;
}

int format_solution(const Puzzle* p, const int digit_of[26], char* out, size_t out_size)
{
    size_t pos = 0;
    char number[MAX_WORD_LEN + 1];

    if (out_size == 0)
        return -1;
    out[0] = '\0';

    for (int w = 0; w < p->word_count; w++) {
        const char* sep = (w == 0) ? "" : (w == p->word_count - 1) ? " = " : " + ";
        size_t len = strlen(p->words[w]);

        for (size_t i = 0; i < len; i++) {
            int d = digit_of[p->words[w][i] - 'A'];
            if (d < 0 || d > 9)
                return -1;
            number[i] = (char)('0' + d);
        }
        number[len] = '\0';

        if (append(out, out_size, &pos, sep) != 0)
            return -1;
        if (append(out, out_size, &pos, number) != 0)
            return -1;
    }
    return 0;
}

int check_solution(const char* puzzle, const char* solution)
{
    Puzzle p;
    char nums[MAX_WORDS][MAX_WORD_LEN + 1];
    int digit_of[26];
    int letter_of[10];
    long long sum = 0, result = 0;

    if (parse_puzzle(puzzle, &p) != 0)
        return 0;
    if (split_expression(solution, nums, is_digit_char) != p.word_count)
        return 0;

    for (int i = 0; i < 26; i++)
        digit_of[i] = -1;
    for (int i = 0; i < 10; i++)
        letter_of[i] = -1;

    for (int w = 0; w < p.word_count; w++) {
        size_t len = strlen(p.words[w]);
        long long value = 0;

        if (strlen(nums[w]) != len)
            return 0;
        if (len > 1 && nums[w][0] == '0')
            return 0;

        for (size_t i = 0; i < len; i++) {
            int l = p.words[w][i] - 'A';
            int d = nums[w][i] - '0';
            if (digit_of[l] == -1 && letter_of[d] == -1) {
                digit_of[l] = d;
                letter_of[d] = l;
            } else if (digit_of[l] != d || letter_of[d] != l) {
                return 0;
            }
            value = value * 10 + d;
        }

        if (w < p.word_count - 1)
            sum += value;
        else
            result = value;
    }
    return sum == result;
}

const SolverInfo SOLVERS[] = {
    { "source", solve_source },
    { "weights", solve_weights },
    { "hotloop", solve_hotloop },
    { "backtrack", solve_backtrack },
    { "heuristics", solve_heuristics },
};

const int SOLVER_COUNT = (int)(sizeof SOLVERS / sizeof SOLVERS[0]);