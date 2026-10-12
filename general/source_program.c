#include "functions.h"

#include <stdlib.h>
#include <string.h>

typedef struct {
    const Puzzle* p;
    int digits[MAX_LETTERS];
    long long checked;
} SourceState;

static int letter_index(const Puzzle* p, char c)
{
    for (int i = 0; i < p->letter_count; i++)
        if (p->letters[i] == c)
            return i;
    return -1;
}

static int check_assignment(SourceState* st)
{
    const Puzzle* p = st->p;
    long long sum = 0, result = 0;
    char buf[MAX_WORD_LEN + 1];

    for (int w = 0; w < p->word_count; w++) {
        size_t len = strlen(p->words[w]);

        for (size_t i = 0; i < len; i++)
            buf[i] = (char)('0' + st->digits[letter_index(p, p->words[w][i])]);
        buf[len] = '\0';

        if (len > 1 && buf[0] == '0')
            return 0;

        long long value = strtoll(buf, NULL, 10);
        if (w < p->word_count - 1)
            sum += value;
        else
            result = value;
    }
    return sum == result;
}

static int search(SourceState* st, int idx)
{
    if (idx == st->p->letter_count)
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

int solve_source(const char* puzzle, char* out, size_t out_size, long long* checked)
{
    Puzzle p;
    SourceState st;
    int digit_of[26];

    if (checked)
        *checked = 0;
    if (parse_puzzle(puzzle, &p) != 0)
        return 0;

    st.p = &p;
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