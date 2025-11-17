/*
 * -----------------------------------------------------------------------------
 * Project: Fossil Logic
 *
 * This file is part of the Fossil Logic project, which aims to develop high-
 * performance, cross-platform applications and libraries. The code contained
 * herein is subject to the terms and conditions defined in the project license.
 *
 * Author: Michael Gene Brockus (Dreamer)
 *
 * Copyright (C) 2024 Fossil Logic. All rights reserved.
 * -----------------------------------------------------------------------------
 */
#include "fossil/code/commands.h"
#ifdef _WIN32
#include <windows.h>
#include <io.h>
#include <errno.h>
#include <sys/stat.h>
#include <stdbool.h>
#else
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdbool.h>
#endif
#include <stdio.h>

/**
 * Create hard or symbolic links between files or directories (cross-platform)
 * @param target Path to the target file or directory
 * @param link_name Path for the new link
 * @param symbolic Create a symbolic link if true, otherwise hard link
 * @param force Overwrite existing link if true
 * @param no_dereference Treat destination as file, do not follow symlinks
 * @return 0 on success, non-zero on error
 */
int fossil_shark_link(ccstring target, ccstring link_name,
                      bool symbolic, bool force, bool no_dereference)
{
    struct stat st;

    // Check if target exists
#ifdef _WIN32
    if (_stat(target, &st) != 0) {
#else
    if (lstat(target, &st) != 0) {
#endif
        cstring msg = fossil_io_cstring_format("{red,bold}Target does not exist: %s{normal}\n", target);
        fossil_fstream_write(FOSSIL_STDERR, msg, 1, fossil_io_cstring_length(msg));
        fossil_io_cstring_free(msg);
        return errno;
    }

    // Handle existing destination
#ifdef _WIN32
    if (_stat(link_name, &st) == 0) {
#else
    if (lstat(link_name, &st) == 0) {
#endif
        if (!force) {
            cstring msg = fossil_io_cstring_format("{yellow,bold}Destination '%s' already exists{normal}\n", link_name);
            fossil_fstream_write(FOSSIL_STDOUT, msg, 1, fossil_io_cstring_length(msg));
            fossil_io_cstring_free(msg);
            return EEXIST;
        }

        // Remove existing file/link
#ifdef _WIN32
        if (remove(link_name) != 0) {
#else
        if (unlink(link_name) != 0) {
#endif
            cstring msg = fossil_io_cstring_format("{red,bold}Failed to remove existing destination: %s{normal}\n", link_name);
            fossil_fstream_write(FOSSIL_STDERR, msg, 1, fossil_io_cstring_length(msg));
            fossil_io_cstring_free(msg);
            return errno;
        }
    }

    int rc;

    if (symbolic) {
#ifdef _WIN32
        // On Windows, use CreateSymbolicLink (requires admin for files, or developer mode)
        DWORD flags = 0;
        struct _stat target_st;
        if (_stat(target, &target_st) == 0 && (target_st.st_mode & _S_IFDIR)) {
            flags = 0x1; // SYMBOLIC_LINK_FLAG_DIRECTORY
        }
        rc = CreateSymbolicLinkA(link_name, target, flags);
        if (rc == 0) {
            cstring msg = fossil_io_cstring_format("{red,bold}symlink failed: %lu{normal}\n", GetLastError());
            fossil_fstream_write(FOSSIL_STDERR, msg, 1, fossil_io_cstring_length(msg));
            fossil_io_cstring_free(msg);
            return GetLastError();
        }
#else
        rc = symlink(target, link_name);
        if (rc != 0) {
            cstring msg = fossil_io_cstring_format("{red,bold}symlink failed: %s{normal}\n", strerror(errno));
            fossil_fstream_write(FOSSIL_STDERR, msg, 1, fossil_io_cstring_length(msg));
            fossil_io_cstring_free(msg);
            return errno;
        }
#endif
    } else {
        // Hard link: only works for files, not directories
#ifdef _WIN32
        struct _stat target_st;
        if (_stat(target, &target_st) != 0) {
            cstring msg = fossil_io_cstring_format("{red,bold}Failed to stat target: %lu{normal}\n", GetLastError());
            fossil_fstream_write(FOSSIL_STDERR, msg, 1, fossil_io_cstring_length(msg));
            fossil_io_cstring_free(msg);
            return GetLastError();
        }
        if (target_st.st_mode & _S_IFDIR) {
            cstring msg = fossil_io_cstring_create("{red,bold}Cannot create hard link to a directory{normal}\n");
            fossil_fstream_write(FOSSIL_STDERR, msg, 1, fossil_io_cstring_length(msg));
            fossil_io_cstring_free(msg);
            return EPERM;
        }
        rc = CreateHardLinkA(link_name, target, NULL);
        if (rc == 0) {
            cstring msg = fossil_io_cstring_format("{red,bold}hard link failed: %lu{normal}\n", GetLastError());
            fossil_fstream_write(FOSSIL_STDERR, msg, 1, fossil_io_cstring_length(msg));
            fossil_io_cstring_free(msg);
            return GetLastError();
        }
#else
        if (stat(target, &st) != 0) {
            cstring msg = fossil_io_cstring_format("{red,bold}Failed to stat target: %s{normal}\n", strerror(errno));
            fossil_fstream_write(FOSSIL_STDERR, msg, 1, fossil_io_cstring_length(msg));
            fossil_io_cstring_free(msg);
            return errno;
        }

        if (S_ISDIR(st.st_mode)) {
            cstring msg = fossil_io_cstring_create("{red,bold}Cannot create hard link to a directory{normal}\n");
            fossil_fstream_write(FOSSIL_STDERR, msg, 1, fossil_io_cstring_length(msg));
            fossil_io_cstring_free(msg);
            return EPERM;
        }

        rc = link(target, link_name);
        if (rc != 0) {
            cstring msg = fossil_io_cstring_format("{red,bold}hard link failed: %s{normal}\n", strerror(errno));
            fossil_fstream_write(FOSSIL_STDERR, msg, 1, fossil_io_cstring_length(msg));
            fossil_io_cstring_free(msg);
            return errno;
        }
#endif
    }

    // No-dereference behavior: for hard links, already safe; for symlinks, lstat ignores target
    if (no_dereference && symbolic) {
#ifndef _WIN32
        struct stat dst_st;
        if (lstat(link_name, &dst_st) == 0 && S_ISLNK(dst_st.st_mode)) {
            // Already handled above: we remove the symlink if force is set
        }
#endif
    }

    return 0;
}
