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
    int lo[MAX_LETTERS];
    int hi[MAX_LETTERS];
    int digit[MAX_LETTERS];
    int used[10];
    long long checked;
} HeurState;

static void assign(HeurState* st, int L, int d)
{
    st->digit[L] = d;
    st->used[d] = 1;
    st->checked++;
}

static void unassign(HeurState* st, int L)
{
    st->used[st->digit[L]] = 0;
    st->digit[L] = -1;
}

static void prepare(const Puzzle* p, HeurState* st)
{
    int idx_of[26];
    int addends = p->word_count - 1;
    const char* result = p->words[addends];
    int res_len = (int)strlen(result);
    int max_add_len = 0;

    st->k = p->letter_count;
    st->checked = 0;
    for (int i = 0; i < st->k; i++) {
        idx_of[p->letters[i] - 'A'] = i;
        st->digit[i] = -1;
        st->lo[i] = 0;
        st->hi[i] = 9;
    }
    for (int d = 0; d < 10; d++)
        st->used[d] = 0;

    st->col_count = res_len;
    for (int w = 0; w < p->word_count; w++) {
        int len = (int)strlen(p->words[w]);
        if (len > st->col_count)
            st->col_count = len;
        if (w < addends && len > max_add_len)
            max_add_len = len;
        if (len > 1)
            st->lo[idx_of[p->words[w][0] - 'A']] = 1;
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

    if (res_len > max_add_len) {
        int top = idx_of[result[0] - 'A'];
        if (st->hi[top] > addends - 1)
            st->hi[top] = addends - 1;
        if (st->lo[top] == st->hi[top])
            assign(st, top, st->lo[top]);
    }
}

static int rest_assigned(const HeurState* st, const Column* col, int pos, int* rest_sum)
{
    int sum = 0;
    for (int j = pos + 1; j < col->add_count; j++) {
        int d = st->digit[col->add[j]];
        if (d < 0)
            return 0;
        sum += d;
    }
    *rest_sum = sum;
    return 1;
}

static int search(HeurState* st, int c, int pos, int s)
{
    if (c == st->col_count)
        return s == 0;

    const Column* col = &st->cols[c];

    if (pos < col->add_count) {
        int L = col->add[pos];
        int R = col->res;
        int rest;

        if (st->digit[L] >= 0)
            return search(st, c, pos + 1, s + st->digit[L]);

        if (R >= 0 && R != L && st->digit[R] >= 0 && rest_assigned(st, col, pos, &rest)) {
            int d = ((st->digit[R] - s - rest) % 10 + 10) % 10;
            if (d < st->lo[L] || d > st->hi[L] || st->used[d])
                return 0;
            assign(st, L, d);
            if (search(st, c, pos + 1, s + d))
                return 1;
            unassign(st, L);
            return 0;
        }

        for (int d = st->lo[L]; d <= st->hi[L]; d++) {
            if (st->used[d])
                continue;
            assign(st, L, d);
            if (search(st, c, pos + 1, s + d))
                return 1;
            unassign(st, L);
        }
        return 0;
    }

    int d = s % 10;
    int carry = s / 10;
    int R = col->res;

    if (R < 0)
        return (d == 0) ? search(st, c + 1, 0, carry) : 0;

    if (st->digit[R] >= 0)
        return (st->digit[R] == d) ? search(st, c + 1, 0, carry) : 0;

    if (d < st->lo[R] || d > st->hi[R] || st->used[d])
        return 0;

    assign(st, R, d);
    if (search(st, c + 1, 0, carry))
        return 1;
    unassign(st, R);
    return 0;
}

int solve_heuristics(const char* puzzle, char* out, size_t out_size, long long* checked)
{
    Puzzle p;
    HeurState st;
    int digit_of[26];

    if (checked)
        *checked = 0;
    if (parse_puzzle(puzzle, &p) != 0)
        return 0;

    prepare(&p, &st);

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