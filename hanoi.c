#include "hanoi.h"
#include "bitstack.h"

#include <stdlib.h>

#define HANOI_MIN_PEGS 3
#define HANOI_MAX_PEGS 25

// The maximum number of discs is limited by the size
// of the bitstack which it is currently 32
#define HANOI_MIN_DISCS 3
#define HANOI_MAX_DISCS 32


int hanoi_create(struct hanoi_t *h, unsigned int pegs_n, unsigned int discs_n) {
    if (pegs_n < HANOI_MIN_PEGS || pegs_n > HANOI_MAX_PEGS)
        return -1;

    if (discs_n < HANOI_MIN_DISCS || discs_n > HANOI_MAX_DISCS)
        return -1;

    h->peg_full_mask = ~(0xffffffff << discs_n);

    h->pegs_n = pegs_n;
    h->discs_n = discs_n;

    h->pegs = malloc(sizeof(struct bitstack_t) * pegs_n);
    for (unsigned i = 0; i < pegs_n; ++i)
        bitstack_create(&h->pegs[i]);

    h->pegs[0].bv = h->peg_full_mask;

    return 0;
}

void hanoi_destroy(struct hanoi_t *h) {
    for (unsigned i = 0; i < h->pegs_n; ++i)
        bitstack_destroy(&h->pegs[i]);

    free(h->pegs);
}

/*
 * Return 1 if the last peg is full (meaning that the user
 * completed the Hanoi challenge.
 *
 * Return 0 otherwise.
 * */
int hanoi_is_challenge_completed(struct hanoi_t *h) {
    return h->pegs[h->pegs_n - 1].bv == h->peg_full_mask;
}

int hanoi_get_peg_copy(struct hanoi_t *h, unsigned peg, struct bitstack_t *bt) {
    if (peg >= h->pegs_n)
        return -1;

    return bitstack_copy(bt, &h->pegs[peg]);
}

/*
 * Move one disc from one peg to another.
 *
 * If the movement is valid, return 0, otherwise return -1.
 *
 * A movement is valid iff:
 *  - the disc is in <from_peg> and it is on the top
 *    which means that you can move the disc only if it is
 *    on the top of the peg stack.
 *  - the <to_peg>'s top is less than disc which means that
 *    you cannot put a larger disc on top of a smaller one.
 * */
int hanoi_try_move_disc(struct hanoi_t *h, unsigned int disc, unsigned int from_peg, unsigned int to_peg) {
    if (from_peg >= h->pegs_n || to_peg >= h->pegs_n)
        return -1;

    if (disc >= h->discs_n)
        return -1;

    // 1: disc must be smallest of to_peg
    // note that if the peg is empty, bitstack_top will return -1
    // so the disc can be put in the peg anyways
    int top = bitstack_top(&h->pegs[to_peg]);
    if (top < disc && top != -1)
        return -1;

    // 2: disc must be in from_peg
    // 3: disc must be smallest of from_peg (aka on top)
    int ret = bitstack_pop_if(&h->pegs[from_peg], disc);
    if (ret == -1)
        return -1;

    bitstack_add(&h->pegs[to_peg], disc);
    return 0;
}
