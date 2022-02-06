#ifndef HANOIFS_HANOILIB
#define HANOIFS_HANOILIB


struct bitstack_t;

struct hanoi_t {
    unsigned int pegs_n;
    unsigned int discs_n;
    struct bitstack_t *pegs;
    unsigned int peg_full_mask;
};


#ifdef __cplusplus
extern "C" {
#endif


int hanoi_create(struct hanoi_t *h, unsigned int pegs_n, unsigned int discs_n);

void hanoi_destroy(struct hanoi_t *h);

/*
 * Return 1 if the last peg is full (meaning that the user
 * completed the Hanoi challenge.
 *
 * Return 0 otherwise.
 * */
int hanoi_is_challenge_completed(struct hanoi_t *h);

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
int hanoi_try_move_disc(struct hanoi_t *h, unsigned int disc, unsigned int from_peg, unsigned int to_peg);

#ifdef __cplusplus
}
#endif

#endif
