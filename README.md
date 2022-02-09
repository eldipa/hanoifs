
# Hanoi FS

The classic Tower of Hanoi game/challenge/puzzle implemented
as a file system using FUSE.

To mount it call:

```shell
$ mkdir -p mnt/
$ ./hanoifs mnt/
```

Within the mounted file system the folders represent the
*towers* of Hanoi

```shell
$ ls -a mnt/            # byexample: +norm-ws
.  ..  A  B  C
```

Inside each folder there are the files which represent the *discs*
of the game.

Initially all the files (discs) are in the first folder (tower).

```shell
$ ls -a mnt/*         # byexample: +rm=~ +norm-ws
mnt/A:
.  ..  0  1  2
~
mnt/B:
.  ..
~
mnt/C:
.  ..
```

To move a disc from one tower to another do a plain `mv`

```shell
$ mv mnt/A/0 mnt/C
```

The movements are restricted of course.

You cannot move a disc that is not in the top of its tower (it is not
the smallest file); you cannot move a disc to a tower on top
of a disc smaller either.

```shell
$ mv mnt/A/2 mnt/B
mv: cannot move 'mnt/A/2' to 'mnt/B/2': Permission denied

$ mv mnt/A/1 mnt/C
mv: cannot move 'mnt/A/1' to 'mnt/C/1': Permission denied
```

Trying to move nonexistent files/folders or rename one disc
to another is not allowed either.

```shell
$ mv mnt/A/999 mnt/B
mv: cannot stat 'mnt/A/999': No such file or directory

$ mv mnt/Z/1 mnt/C
mv: cannot stat 'mnt/Z/1': No such file or directory

$ mv mnt/C/0 mnt/C/2
mv: cannot move 'mnt/C/0' to 'mnt/C/2': Permission denied
```

It is the same to move one file into another file or into a folder.
Both notation are supported:

```shell
$ mv mnt/A/1 mnt/B/1

$ mv mnt/C/0 mnt/B/
```

The goal of the Tower of Hanoi is to move all the discs to the latest
tower (the latest folder, `C` in our case).

Once completed, a special file will appear at the root:

```shell
$ mv mnt/A/2 mnt/C
$ mv mnt/B/0 mnt/A
$ mv mnt/B/1 mnt/C
$ mv mnt/A/0 mnt/C

$ ls mnt/           # byexample: +norm-ws
 A   B   C  'you win'
```

Like any other FUSE file system, you can unmount it calling
`fusermount`.

```shell
$ fusermount -u mnt     # byexample: -skip +pass
```

## Help

The file system supports a few extra options like the
name of the file that signals that the game is over, the count
of pegs (towers) and discs.

```shell
$ ./hanoifs --help      # byexample: -capture
<...>
File-system specific options:
    --flagname=<s>      Name of the "flag" file that indicates victory
                        (default: "you win")
    --pegs=<n>          Number of pegs (folders)
                        (default 3)
    --discs=<n>         Number of discs (files)
                        (default 3)
<...>
FUSE options:
    -h   --help            print help
    -V   --version         print version
    -d   -o debug          enable debug output (implies -f)
    -f                     foreground operation
    -s                     disable multi-threaded operation
<...>
```

## Compile

You will need:
 - `libfuse3-dev`
 - `fuse3`
 - `libfuse3-3`
 - `gcc`

You may just run `make install_deps` as a shortcut.

For the tests you will need to install `docker`, `python3` and
`byexample`.

If you have `pip`, install
[byexample](https://byexamples.github.io/) running:

```shell
$ pip install byexample     # byexample: -skip
```

Then pull a docker image with `cling` installed inside running:

```shell
$ make pull_cling       # byexample: -skip
```

You may just run `make install_test_deps` as a shortcut.

Finally, compile with `make` and run the tests with `make test`
