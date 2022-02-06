/*
  FUSE: Filesystem in Userspace
  Copyright (C) 2001-2007  Miklos Szeredi <miklos@szeredi.hu>

  This program can be distributed under the terms of the GNU GPLv2.
  See the file COPYING.
*/

/** @file
 *
 * minimal example filesystem using high-level API
 *
 * Compile with:
 *
 *     gcc -Wall hello.c `pkg-config fuse3 --cflags --libs` -o hello
 *
 * ## Source code ##
 * \include hello.c
 */


#define FUSE_USE_VERSION 31

#include <fuse.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <stddef.h>
#include <assert.h>
#include <stdbool.h>
#include <sys/types.h>

#include "hanoi.h"
#include "bitstack.h"

// These are the options coming from the command line.
// It is a single static object as it is (apparently) the only
// way to share the options with the functions/hooks.
//
// We cannot initialize it here but we must do it in the main().
// This is because the field msg is a char* and FUSE will assume
// that if it is not NULL it is a malloc'd string and it will call free()
//
// So we need to set the default using malloc in the main()
static
struct options_t {
    unsigned pegs_n;
    unsigned discs_n;

    char *msg;

    int show_help;
} options;

// This macro initialize a single struct fuse_opt object.
// The fields are:
//  - char* template : the string to match with an optional param format
//    to capture a value
//
//  - unsigned long offset : the offset of where in the data struct we
//    should store the value. This data struct is the struct options_t in
//    out case.
//    The offset then can be calculated using the compiler's
//      offsetof(struct, field)
//    Another valid value of offset is -1. TODO for what?
//
//  - int value : the value to set in the data struct at the given offset
//    iff the template does not have a formatting argument, otherwise

static
const struct fuse_opt option_spec[] = {
    { "--pegs=%u", offsetof(struct options_t, pegs_n), 1 },
    { "--discs=%u", offsetof(struct options_t, discs_n), 1 },
    { "--msg=%s", offsetof(struct options_t, msg), 1 },

    { "-h", offsetof(struct options_t, show_help), 1 },
    { "--help", offsetof(struct options_t, show_help), 1 },

    // Signal the end of the options.
    FUSE_OPT_END
};


static
void *hanoifs_init(
        struct fuse_conn_info *conn,
        struct fuse_config *cfg) {
    (void) conn;
    cfg->kernel_cache = 1;

    printf("%u %u\n", options.pegs_n, options.discs_n);

    // Initialize the Hanoi Tower game
    struct hanoi_t *h = malloc(sizeof(*h));
    int ret = hanoi_create(h, options.pegs_n, options.discs_n);

    // If something didn't work, abort the program.
    // Unfortunatelly we don't have a better way to do this
    // except with a violent exti() call
    if (ret == -1)
        exit(-1);


    // The return value will be saved into the private_data attribute
    // of struct fuse_context and it'll be passed to
    // the hanoifs_destroy for clean up.
    return h;
}

static
void hanoifs_destroy(void *private_data) {
    struct hanoi_t *h = private_data;
    hanoi_destroy(h);
    free(h);
}

enum PathKind {
    PKind_Root,
    PKind_Peg,
    PKind_Disc,
    PKind_Flag,
    PKind_Invalid
};

static
enum PathKind hanoifs_parse_path(
        const char *path,
        struct hanoi_t *h,
        unsigned *peg,
        unsigned *disc) {
    if (strcmp(path, "/") == 0) {
        return PKind_Root;
    }

    char folder;
    unsigned file;
    int read;

    // Does it look like a file /A/4 ?
    if (sscanf(path, "/%c/%u%n", &folder, &file, &read) == 2) {
        if (strlen(path) != read) {
            // The path looks like a file but it seems that the path
            // is longer than expected. Something like /A/4foo.
            return PKind_Invalid;
        }

        // Check out of range folders (pegs) and files (discs)
        // and discard any invalid entry.
        char dir = folder - 'A';
        if (dir < 0 || dir >= h->pegs_n)
            return PKind_Invalid;

        if (file < 0 || file >= h->discs_n)
            return PKind_Invalid;

        // All good, it is a regular valid file
        *peg = dir;
        *disc = file;
        return PKind_Disc;
    }

    // Does it look like a folder /A ?
    if (sscanf(path, "/%c%n", &folder, &read) == 1) {
        if (strlen(path) != read) {
            // The path looks like a folder but it seems that the path
            // is longer than expected. Something like /Afoo.
            return PKind_Invalid;
        }

        char dir = folder - 'A';
        if (dir < 0 || dir >= h->pegs_n)
            return PKind_Invalid;

        *peg = dir;
        return PKind_Peg;
    }

    return PKind_Invalid;
}

/* Everytime that the user wants to know what a given path is we
 * respond with:
 *  - it is a directory
 *  - it is a regular file
 *  - it does not exist such path (entry)
 *
 * In hanoifs we want to show:
 *  - the root folder /
 *  - a folder per peg named with capital letters /A, /B, /C
 *  - a file per disc, file within one of the folders/pegs /A/0 /A/1 /B/6
 *  */
static
int hanoifs_getattr(
        const char *path,
        struct stat *stbuf,
        struct fuse_file_info *fi) {
	(void) fi;
	int res = 0;

        // Set to zero the output struct. Mandatory.
	memset(stbuf, 0, sizeof(struct stat));

        struct hanoi_t *h = fuse_get_context()->private_data;

        unsigned peg;
        unsigned disc;
        enum PathKind kind = hanoifs_parse_path(path, h, &peg, &disc);

        switch (kind) {
            case PKind_Root:
                // st_mode is a mask that contains.
                //  - the file type (S_IFSOCK (socket), S_IFLNK (symlink), ...)
                //  - the file mode (S_ISVTX (sticky bit), S_ISUID (set-user-ID), ...)
                //  - the file permissions (S_IRUSR (owner can read), S_IWGRP (group can write), ...)
                //
                // See inode(7)
                //
                // The following sets a mask for a directory with 0755
                // permissions (rwx for user, rx for group and others)
                //
		stbuf->st_mode = S_IFDIR | 0755;

                // Count the hard links to the folder.
                //
                // The exact number requires to know how many other folders
                // are pointing to this directory.
                //
                // For '/' we have:
                //  - the parent of / which points to /  (+1)
                //  - the '.' entry which points to itself  (+1)
                //  - each subfolder /A, /B, /C points to its parent /  (+3)
                //
                // TODO: it is not clear for / if we should count the hard link
                // of its parent because it is outside of the file system.
                // The FUSE's example hello.c counts it but the example
                // invalidate_path.c doesn't.
                // */
		stbuf->st_nlink = 5;

                return res;

            case PKind_Peg:
                // All good, it is a valid folder
                stbuf->st_mode = S_IFDIR | 0755;

                // For /A, /B, /C we have:
                //  - the parent of them which points to they (the /)  (+1)
                //  - the '.' entry which points to itself  (+1)
                //
                stbuf->st_nlink = 2;
                return res;
            case PKind_Disc:
                // All good, it is a regular valid file
                stbuf->st_mode = S_IFREG | 0444;
                stbuf->st_nlink = 1;

                // Just for fun, make the files smaller or larger based on
                // their names to "emulate" the discs' sizes.
                //
                // NOTE: we assume that disc is less than 32 otherwise the
                // shift operation is undefined.
                // This should be okay given that the current hanoi_t supports
                // only up to 32 discs (numered from 0 to 31)
                assert(disc < 32);
                stbuf->st_size = (1 << disc);
                return res;

            default:
                return -ENOENT;
        }

        assert(0);
}

static int hanoifs_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
			 off_t offset, struct fuse_file_info *fi,
			 enum fuse_readdir_flags flags)
{
	(void) offset;
	(void) fi;
	(void) flags;

        struct hanoi_t *h = fuse_get_context()->private_data;

        unsigned peg;
        unsigned disc;
        enum PathKind kind = hanoifs_parse_path(path, h, &peg, &disc);

        switch (kind) {
            case PKind_Root:
                // filler() adds entries to the directory. Its arguments are:
                //  - buf: the buffer received by readdir
                //  - name: the directory/file name (the entry name)
                //  - stat: a pointer to struct stat with the stats of the file (optional)
                //  - offset: see below
                //
                // TODO it is unclear the use of offset and the size of the buffer.
                //
                filler(buf, ".", NULL, 0, 0);
                filler(buf, "..", NULL, 0, 0);

                for (peg = 0; peg < h->pegs_n; ++peg) {
                    char folder[] = {'A' + peg, '\0'};
                    filler(buf, folder, NULL, 0, 0);
                }
                return 0;

            case PKind_Peg:
                filler(buf, ".", NULL, 0, 0);
                filler(buf, "..", NULL, 0, 0);

                // Get a copy of the selected peg and iterate
                // of its discs (poppin one by one and creating
                // a directory entry for each)
                struct bitstack_t bt;
                if (hanoi_get_peg_copy(h, peg, &bt) == -1)
                    return -ENOENT;

                int top = bitstack_pop(&bt);
                while (top != -1) {
                    // We assume a small value so it can fit in a small
                    // file[] buffer
                    assert(top >= 0 && top < 32);
                    char file[4] = {0};
                    snprintf(file, sizeof(file), "%u", top);

                    filler(buf, file, NULL, 0, 0);

                    top = bitstack_pop(&bt);
                }

                return 0;

            case PKind_Disc:
                // This should never happen because a disc is regular
                // file, not a folder
                return -ENOENT;

            default:
                return -ENOENT;
        }

        assert(0);
}


static
const struct fuse_operations hanoifs_ops = {
    // These two are kind of optional but they are good
    // place to init some filesyste-specific data and configuring
    // the FUSE system
    .init           = hanoifs_init,
    .destroy        = hanoifs_destroy,

    // These two are virtually mandatory. Without them you
    // cannot stat a file or list a directory.
    // Basically without this the output of "ls" makes no sense.
    .getattr	= hanoifs_getattr,
    .readdir	= hanoifs_readdir
};

static void show_help(const char *progname)
{
	printf("usage: %s [options] <mountpoint>\n\n", progname);
        /*
	printf("File-system specific options:\n"
	       "    --name=<s>          Name of the \"hello\" file\n"
	       "                        (default: \"hello\")\n"
	       "    --contents=<s>      Contents \"hello\" file\n"
	       "                        (default \"Hello, World!\\n\")\n"
	       "\n");
               */
}

int main(int argc, char *argv[])
{
	int ret;
	struct fuse_args args = FUSE_ARGS_INIT(argc, argv);

        // Set the default values in case the user does not specify
        // them
        options.pegs_n = 3;
        options.discs_n = 3;

        // Strings must be malloc'd because FUSE assumes that and it will
        // call free() on them.
        options.msg = strdup("well done");


	// Parse options
	if (fuse_opt_parse(&args, &options, option_spec, NULL) == -1)
		return 1;



	/* When --help is specified, first print our own file-system
	   specific help text, then signal fuse_main to show
	   additional help (by adding `--help` to the options again)
	   without usage: line (by setting argv[0] to the empty
	   string) */
	if (options.show_help) {
		show_help(argv[0]);
		assert(fuse_opt_add_arg(&args, "--help") == 0);
		args.argv[0][0] = '\0';
	}

	ret = fuse_main(args.argc, args.argv, &hanoifs_ops, NULL);
	fuse_opt_free_args(&args);
	return ret;
}

