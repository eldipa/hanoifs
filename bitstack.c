#include "bitstack.h"


int bitstack_create(struct bitstack_t *st) {
    st->bv = 0;
    return 0;
}

void bitstack_destroy(struct bitstack_t *st) {
    st->bv = 0;
}

int bitstack_copy_from(struct bitstack_t *st, struct bitstack_t * const src) {
    st->bv = src->bv;
    return 0;
}

/*
 * Return the top of the stack value (a number between 0 and 31)
 * If the stack is empty, return -1.
 *
 * */
int bitstack_top(struct bitstack_t *st) {
    if (!st->bv)
        return -1;

    // Note: __builtin_ctz counts the trailing zeros from
    // the LSB.
    return __builtin_ctz(st->bv);
}


/*
 * Pop a value from the stack and return it.
 * If the stack was empty return -1.
 * */
int bitstack_pop(struct bitstack_t *st) {
    int top = bitstack_top(st);
    if (top == -1)
        return -1;

    st->bv ^= (0x1 << top);
    return top;
}

int bitstack_pop_if(struct bitstack_t *st, unsigned int expected_top) {
    int top = bitstack_top(st);
    if (top == -1)
        return -1;

    if (top != expected_top)
        return -1;

    st->bv ^= (0x1 << top);
    return top;
}

int bitstack_push(struct bitstack_t *st, unsigned int val) {
    int top = bitstack_top(st);
    if (top < val)
        return -1;

    return bitstack_add(st, val);
}

int bitstack_add(struct bitstack_t *st, unsigned int val) {
    if (val >= 32)
        return -1;

    st->bv |= (0x1 << val);
    return val;
}

