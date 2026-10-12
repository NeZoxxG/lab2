#include "functions.h"

#include <string.h>

typedef struct {
    int k;
    long long weight[MAX_LETTERS];
    int leading[MAX_LETTERS];
    int digits[MAX_LETTERS];
    long long checked;
} WeightsState;

static int letter_index(const Puzzle* p, char c)
{
    for (int i = 0; i < p->letter_count; i++)
        if (p->letters[i] == c)
            return i;
    return -1;
}

static void compute_weights(const Puzzle* p, WeightsState* st)
{
    st->k = p->letter_count;
    for (int i = 0; i < st->k; i++) {
        st->weight[i] = 0;
        st->leading[i] = 0;
    }

    for (int w = 0; w < p->word_count; w++) {
        int sign = (w == p->word_count - 1) ? -1 : 1;
        size_t len = strlen(p->words[w]);
        long long place = 1;

        for (size_t i = len; i-- > 0;) {
            st->weight[letter_index(p, p->words[w][i])] += sign * place;
            place *= 10;
        }
        if (len > 1)
            st->leading[letter_index(p, p->words[w][0])] = 1;
    }
}

static int check_assignment(const WeightsState* st)
{
    long long sum = 0;
    for (int i = 0; i < st->k; i++) {
        if (st->leading[i] && st->digits[i] == 0)
            return 0;
        sum += st->weight[i] * st->digits[i];
    }
    return sum == 0;
}

static int search(WeightsState* st, int idx)
{
    if (idx == st->k)
        return check_assignment(st);

    for (int d = 0; d <= 9; d++) {
        int used = 0;
        for (int j = 0; j < idx; j++) {
            if (st->digits[j] == d) {
                used = 1;
                break;
            }
        }
        if (used)
            continue;

        st->digits[idx] = d;
        st->checked++;
        if (search(st, idx + 1))
            return 1;
    }
    return 0;
}

int solve_weights(const char* puzzle, char* out, size_t out_size, long long* checked)
{
    Puzzle p;
    WeightsState st;
    int digit_of[26];

    if (checked)
        *checked = 0;
    if (parse_puzzle(puzzle, &p) != 0)
        return 0;

    compute_weights(&p, &st);
    st.checked = 0;

    int found = search(&st, 0);
    if (checked)
        *checked = st.checked;
    if (!found)
        return 0;

    for (int i = 0; i < 26; i++)
        digit_of[i] = -1;
    for (int i = 0; i < p.letter_count; i++)
        digit_of[p.letters[i] - 'A'] = st.digits[i];

    return format_solution(&p, digit_of, out, out_size) == 0;
}