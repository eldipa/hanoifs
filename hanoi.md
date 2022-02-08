# Tower of Hanoi

The `hanoi` lib implements the classic Tower of Hanoi
game/puzzle/challenge.

But before explaining it, let's load some helper functions
for documentation purposes

```cpp
?: #include <bitset>
?: #include <iostream>
?: #include "hanoi.h"
?: #include "bitstack.h"

?: void p(struct hanoi_t *h) {
::    for (auto i = 0; i < h->pegs_n; ++i) {
::        std::cout << i << " - " << std::bitset<32>(h->pegs[i].bv) << '\n';
::    }
:: }
```

Now, let's load the required libraries:

```cpp
?: #pragma cling load("libbitstack.so")
?: #pragma cling load("libhanoi.so")
```

The classic Tower of Hanoi consists in 3 towers (pegs) and 3 discs
of sizes different sizes.

`hanoi_create` allows to create a challenge of up to `HANOI_MAX_PEGS`
pegs and up to `HANOI_MAX_DISCS` discs.

For simplicity we are going to use 3 pegs and 3 discs:

```cpp
?: struct hanoi_t h;
?: hanoi_create(&h, 3, 3);

?: p(&h);
0 - 00000000000000000000000000000111
1 - 00000000000000000000000000000000
2 - 00000000000000000000000000000000
```

> How `hanoi` represents the pegs and the discs is an implementation
> detail however because I want to show you how it works I will tell
> you how the they are represented.
>
> Each peg is a bit vector (`bitstack_t`) where each bit set represents
> the disc of that size in the peg.
>
> Discs are numbered from 0 to 31 being the number a representation
> of the disc's size. So the disc 3 is smaller than the disc 5.
>
> The helper method `p()` shows each peg and the discs set in each one
> Notice how the discs 0, 1 and 2 are set in the peg number 0.


In the Tower of Hanoi we can move the discs from one peg to another.

Here I'm moving the disc 0 from the peg 0 to the peg 2:

```cpp
?: hanoi_try_move_disc(&h, 0, 0, 2)
(int) 0

?: p(&h);
0 - 00000000000000000000000000000110
1 - 00000000000000000000000000000000
2 - 00000000000000000000000000000001
```

However no every movement is allowed.

The disc that to move must be the smallest of the peg
that you are taking it from and the smallest of the peg
that you are putting it on.

So the disc 2 cannot be taken from the peg 0 because it is not the
smallest (the disc 1 is there). The disc 1 can be pulled from
the peg 0 but it cannot be put in the peg 2 because there is a disc
there (the 1) smaller than it.

```cpp
?: hanoi_try_move_disc(&h, 2, 0, 1)
(int) -1
?: p(&h);
0 - 00000000000000000000000000000110
1 - 00000000000000000000000000000000
2 - 00000000000000000000000000000001

?: hanoi_try_move_disc(&h, 1, 0, 2)
(int) -1
?: p(&h);
0 - 00000000000000000000000000000110
1 - 00000000000000000000000000000000
2 - 00000000000000000000000000000001
```

Invalid peg or disc numbers are forbidden too:

```cpp
?: hanoi_try_move_disc(&h, 3, 0, 1) // disc out of range
(int) -1

?: hanoi_try_move_disc(&h, 1, 0, 3) // peg out of range
(int) -1

?: hanoi_try_move_disc(&h, 1, 1, 1) // no such disc in that peg
(int) -1
```

The following *are* valid movements: the disc 1 is the smallest
of pegs 0 and 1; the disc 0 is the smallest of peg 2 and 1.

```cpp
?: hanoi_try_move_disc(&h, 1, 0, 1)
(int) 0
?: p(&h);
0 - 00000000000000000000000000000100
1 - 00000000000000000000000000000010
2 - 00000000000000000000000000000001

?: hanoi_try_move_disc(&h, 0, 2, 1)
(int) 0
?: p(&h);
0 - 00000000000000000000000000000100
1 - 00000000000000000000000000000011
2 - 00000000000000000000000000000000
```

The goal of the challenge is to move all the discs to the latest
peg, the number 2 in our examples.

```cpp
?: hanoi_is_challenge_completed(&h)
(int) 0
```

With the following movements should be able to complete the puzzle.

```cpp
?: hanoi_try_move_disc(&h, 2, 0, 2);
?: hanoi_try_move_disc(&h, 0, 1, 0);
?: hanoi_try_move_disc(&h, 1, 1, 2);
?: hanoi_try_move_disc(&h, 0, 0, 2);

?: p(&h);
0 - 00000000000000000000000000000000
1 - 00000000000000000000000000000000
2 - 00000000000000000000000000000111

?: hanoi_is_challenge_completed(&h)
(int) 1
```
