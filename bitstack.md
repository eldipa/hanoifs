
`bitstack` is a bit vector with an API to see it as a stack
with a strict ordering semantic.

First, let's load some helper tools for this documentation

```cpp
?: #include <bitset>
?: #include <iostream>
?: #include "bitstack.h"

?: void p(struct bitstack_t *st) {
::    std::cout << std::bitset<32>(st->bv) << '\n';
:: }
```

Then, let's load the `bitstack` library and create one instance

```cpp
?: #pragma cling load("libbitstack.so")

?: struct bitstack_t st;
?: bitstack_create(&st);
```

The stack is initialized empty so asking for the top value
or trying to pop it returns an error code.

```cpp
?: bitstack_top(&st)
(int) -1

?: bitstack_pop(&st)
(int) -1
```

The only thing that we can do is to push values.

```cpp
?: bitstack_push(&st, 30)
(int) 30

?: bitstack_push(&st, 28)
(int) 28

?: p(&st);
01010000000000000000000000000000
```

Notice that `bitstack_t` enforces a strict order of the values
inside the stack.

It is *not* allowed to push a value which it is larger than
the current top:

```cpp
?: bitstack_top(&st)
(int) 28

?: bitstack_push(&st, 29)
(int) -1

?: p(&st);
01010000000000000000000000000000
```

It is possible however to bypass this restriction and set
and arbitrary value

```cpp
?: bitstack_add(&st, 29)
(int) 29

?: p(&st);
01110000000000000000000000000000
```

But of course, without going beyond the supported range.

```cpp
?: bitstack_add(&st, 32)
(int) -1
```

Like push we have pop

```cpp
?: bitstack_push(&st, 0)
(int) 0

?: p(&st);
01110000000000000000000000000001

?: bitstack_pop(&st)
(int) 0

?: p(&st);
01110000000000000000000000000000
```

The pop can be conditional: if the current top is the one that it is
expected (passed as parameter), the pop successes but if not returns
an error

```cpp
?: bitstack_top(&st)
(int) 28

?: bitstack_pop_if(&st, 10)
(int) -1

?: p(&st);
01110000000000000000000000000000

?: bitstack_pop_if(&st, 28)
(int) 28

?: p(&st);
01100000000000000000000000000000
```

`bitstack` can be copied:

```cpp
?: struct bitstack_t st2;
?: bitstack_copy_from(&st2, &st);

?: bitstack_destroy(&st); // this will not modify st2

?: p(&st2);
01100000000000000000000000000000
```
