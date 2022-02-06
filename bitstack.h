#ifndef HANOIFS_BITSTACK
#define HANOIFS_BITSTACK

struct bitstack_t {
    unsigned int bv;
};



#ifdef __cplusplus
extern "C" {
#endif

int bitstack_create(struct bitstack_t *st);

void bitstack_destroy(struct bitstack_t *st);

int bitstack_copy(struct bitstack_t *st, struct bitstack_t * const src);

/*
 * Return the top of the stack value (a number between 0 and 31)
 * If the stack is empty, return -1.
 *
 * */
int bitstack_top(struct bitstack_t *st);


/*
 * Pop a value from the stack and return it.
 * If the stack was empty return -1.
 * */
int bitstack_pop(struct bitstack_t *st);

int bitstack_pop_if(struct bitstack_t *st, unsigned int expected_top);

/*
 * Push a value onto the stack and return it.
 * If the value exceeds the stack limit or the top of
 * the stack is greater than the value return -1.
 * */
int bitstack_push(struct bitstack_t *st, unsigned int val);

/*
 * Add the value at its correct position. This violates
 * the "push-pop / stack" semantic (LIFO) because bitstack_add
 * can add an element in the middle of the stack.
 *
 * If the value exceeds the stack limit return -1
 **/
int bitstack_add(struct bitstack_t *st, unsigned int val);


#ifdef __cplusplus
}
#endif

#endif
