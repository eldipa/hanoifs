
# Hanoi FS

The classic Tower of Hanoi game/challenge/puzzle implemented
as a file system using FUSE.

To mount it call:

```shell
$ ./hanoifs mnt/
```

Within the mounted file system the folders represent the
*towers* of Hanoi

```shell
$ ls -lah mnt/
total 4.0K
drwxr-xr-x 5 root root    0 Jan  1  1970 .
drwxr-xr-x <...> ..
drwxr-xr-x 2 root root    0 Jan  1  1970 A
drwxr-xr-x 2 root root    0 Jan  1  1970 B
drwxr-xr-x 2 root root    0 Jan  1  1970 C
```

Inside each folder there are the files which represent the *discs*
of the game.

Initially all the files (discs) are in the first folder (tower).

```shell
$ ls -lah mnt/*         # byexample: +rm=~
mnt/A:
total 0
drwxr-xr-x 2 root root 0 Jan  1  1970 .
drwxr-xr-x 5 root root 0 Jan  1  1970 ..
-r--r--r-- 1 root root 1 Jan  1  1970 0
-r--r--r-- 1 root root 2 Jan  1  1970 1
-r--r--r-- 1 root root 4 Jan  1  1970 2
~
mnt/B:
total 0
drwxr-xr-x 2 root root 0 Jan  1  1970 .
drwxr-xr-x 5 root root 0 Jan  1  1970 ..
~
mnt/C:
total 0
drwxr-xr-x 2 root root 0 Jan  1  1970 .
drwxr-xr-x 5 root root 0 Jan  1  1970 ..
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

Just call `make`. You will need `libfuse-dev` installed and `gcc`
to compile it.

If you have Python 3, install
[byexample](https://byexamples.github.io/) and run the tests
doing `make tests`. You will need to pull a docker image to run C/C++
tests, just call `make pull_cling` before `make test`.
