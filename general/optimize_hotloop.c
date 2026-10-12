#include "functions.h"

#include <string.h>

typedef struct {
    int k;
    long long weight[MAX_LETTERS];
    int lead_list[MAX_LETTERS];
    int lead_count;
    int digits[MAX_LETTERS];
    int used[10];
    long long checked;
} HotState;

static void prepare(const Puzzle* p, HotState* st)
{
    int idx_of[26];
    int is_lead[MAX_LETTERS] = { 0 };

    st->k = p->letter_count;
    for (int i = 0; i < st->k; i++) {
        idx_of[p->letters[i] - 'A'] = i;
        st->weight[i] = 0;
    }

    for (int w = 0; w < p->word_count; w++) {
        int sign = (w == p->word_count - 1) ? -1 : 1;
        size_t len = strlen(p->words[w]);
        long long place = 1;

        for (size_t i = len; i-- > 0;) {
            st->weight[idx_of[p->words[w][i] - 'A']] += sign * place;
            place *= 10;
        }
        if (len > 1)
            is_lead[idx_of[p->words[w][0] - 'A']] = 1;
    }

    st->lead_count = 0;
    for (int i = 0; i < st->k; i++)
        if (is_lead[i])
            st->lead_list[st->lead_count++] = i;

    for (int d = 0; d < 10; d++)
        st->used[d] = 0;
}

static int search(HotState* st, int idx, long long sum)
{
    if (idx == st->k) {
        if (sum != 0)
            return 0;
        for (int j = 0; j < st->lead_count; j++)
            if (st->digits[st->lead_list[j]] == 0)
                return 0;
        return 1;
    }

    const long long w = st->weight[idx];
    long long wd = 0;

    for (int d = 0; d <= 9; d++, wd += w) {
        if (st->used[d])
            continue;

        st->used[d] = 1;
        st->digits[idx] = d;
        st->checked++;
        if (search(st, idx + 1, sum + wd))
            return 1;
        st->used[d] = 0;
    }
    return 0;
}

int solve_hotloop(const char* puzzle, char* out, size_t out_size, long long* checked)
{
    Puzzle p;
    HotState st;
    int digit_of[26];

    if (checked)
        *checked = 0;
    if (parse_puzzle(puzzle, &p) != 0)
        return 0;

    prepare(&p, &st);
    st.checked = 0;

    int found = search(&st, 0, 0);
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