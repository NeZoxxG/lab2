#include "functions.h"

#include <string.h>

typedef struct {
    int add[MAX_ADDENDS];
    int add_count;
    int res;
} Column;

typedef struct {
    Column cols[MAX_WORD_LEN];
    int col_count;
    int k;
    int leading[MAX_LETTERS];
    int digit[MAX_LETTERS];
    int used[10];
    long long checked;
} BtState;

static void prepare(const Puzzle* p, BtState* st)
{
    int idx_of[26];
    int addends = p->word_count - 1;
    const char* result = p->words[addends];
    int res_len = (int)strlen(result);

    st->k = p->letter_count;
    for (int i = 0; i < st->k; i++) {
        idx_of[p->letters[i] - 'A'] = i;
        st->digit[i] = -1;
        st->leading[i] = 0;
    }
    for (int d = 0; d < 10; d++)
        st->used[d] = 0;

    st->col_count = 0;
    for (int w = 0; w < p->word_count; w++) {
        int len = (int)strlen(p->words[w]);
        if (len > st->col_count)
            st->col_count = len;
        if (len > 1)
            st->leading[idx_of[p->words[w][0] - 'A']] = 1;
    }

    for (int c = 0; c < st->col_count; c++) {
        Column* col = &st->cols[c];
        col->add_count = 0;
        for (int w = 0; w < addends; w++) {
            int len = (int)strlen(p->words[w]);
            if (c < len)
                col->add[col->add_count++] = idx_of[p->words[w][len - 1 - c] - 'A'];
        }
        col->res = (c < res_len) ? idx_of[result[res_len - 1 - c] - 'A'] : -1;
    }
}

static int search(BtState* st, int c, int pos, int s)
{
    if (c == st->col_count) {
        if (s != 0)
            return 0;
        for (int i = 0; i < st->k; i++)
            if (st->leading[i] && st->digit[i] == 0)
                return 0;
        return 1;
    }

    const Column* col = &st->cols[c];

    if (pos < col->add_count) {
        int L = col->add[pos];

        if (st->digit[L] >= 0)
            return search(st, c, pos + 1, s + st->digit[L]);

        for (int d = 0; d <= 9; d++) {
            if (st->used[d])
                continue;
            st->digit[L] = d;
            st->used[d] = 1;
            st->checked++;
            if (search(st, c, pos + 1, s + d))
                return 1;
            st->used[d] = 0;
        }
        st->digit[L] = -1;
        return 0;
    }

    int d = s % 10;
    int carry = s / 10;
    int R = col->res;

    if (R < 0)
        return (d == 0) ? search(st, c + 1, 0, carry) : 0;

    if (st->digit[R] >= 0)
        return (st->digit[R] == d) ? search(st, c + 1, 0, carry) : 0;

    if (st->used[d])
        return 0;

    st->digit[R] = d;
    st->used[d] = 1;
    st->checked++;
    if (search(st, c + 1, 0, carry))
        return 1;
    st->used[d] = 0;
    st->digit[R] = -1;
    return 0;
}

int solve_backtrack(const char* puzzle, char* out, size_t out_size, long long* checked)
{
    Puzzle p;
    BtState st;
    int digit_of[26];

    if (checked)
        *checked = 0;
    if (parse_puzzle(puzzle, &p) != 0)
        return 0;

    prepare(&p, &st);
    st.checked = 0;

    int found = search(&st, 0, 0, 0);
    if (checked)
        *checked = st.checked;
    if (!found)
        return 0;

    for (int i = 0; i < 26; i++)
        digit_of[i] = -1;
    for (int i = 0; i < p.letter_count; i++)
        digit_of[p.letters[i] - 'A'] = st.digit[i];

    return format_solution(&p, digit_of, out, out_size) == 0;
}