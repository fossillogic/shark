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
#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>
#include <stdio.h>
#else
#include <sys/stat.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#endif

int fossil_shark_info(const char *path, bool show_permissions,
                      bool show_owner, bool show_size, bool show_timestamps)
{
    fossil_fstream_t *err_stream = FOSSIL_STDERR;
    fossil_fstream_t *out_stream = FOSSIL_STDOUT;

#if defined(_WIN32) || defined(_WIN64)
    // Windows implementation
    WIN32_FILE_ATTRIBUTE_DATA fad;
    if (!GetFileAttributesExA(path, GetFileExInfoStandard, &fad)) {
        cstring msg = fossil_io_cstring_format("{red,bold}Failed to stat path: %lu{normal}\n", GetLastError());
        fossil_fstream_write(err_stream, msg, 1, fossil_io_cstring_length(msg));
        fossil_io_cstring_free(msg);
        return (int)GetLastError();
    }

    // Permissions (simplified for Windows)
    if (show_permissions) {
        cstring msg = fossil_io_cstring_format("{green,bold}Permissions:{normal} (Windows attributes: 0x%08lx)\n", fad.dwFileAttributes);
        fossil_fstream_write(out_stream, msg, 1, fossil_io_cstring_length(msg));
        fossil_io_cstring_free(msg);
    }

    // Owner and group (not easily available in standard C on Windows)
    if (show_owner) {
        cstring owner_msg = fossil_io_cstring_copy("{yellow,bold}Owner:{normal} (Not available on Windows)\n");
        cstring group_msg = fossil_io_cstring_copy("{yellow,bold}Group:{normal} (Not available on Windows)\n");
        fossil_fstream_write(out_stream, owner_msg, 1, fossil_io_cstring_length(owner_msg));
        fossil_fstream_write(out_stream, group_msg, 1, fossil_io_cstring_length(group_msg));
        fossil_io_cstring_free(owner_msg);
        fossil_io_cstring_free(group_msg);
    }

    // Size
    if (show_size) {
        ULONGLONG size = ((ULONGLONG)fad.nFileSizeHigh << 32) | fad.nFileSizeLow;
        cstring msg = fossil_io_cstring_format("{cyan,bold}Size:{normal} %llu bytes\n", size);
        fossil_fstream_write(out_stream, msg, 1, fossil_io_cstring_length(msg));
        fossil_io_cstring_free(msg);
    }

    // Timestamps
    if (show_timestamps) {
        FILETIME ft;
        SYSTEMTIME st;
        char buf[64];

        // Accessed
        ft = fad.ftLastAccessTime;
        FileTimeToSystemTime(&ft, &st);
        snprintf(buf, sizeof(buf), "%04d-%02d-%02d %02d:%02d:%02d",
                 st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);
        cstring msg = fossil_io_cstring_format("{magenta,bold}Accessed:{normal} %s\n", buf);
        fossil_fstream_write(out_stream, msg, 1, fossil_io_cstring_length(msg));
        fossil_io_cstring_free(msg);

        // Modified
        ft = fad.ftLastWriteTime;
        FileTimeToSystemTime(&ft, &st);
        snprintf(buf, sizeof(buf), "%04d-%02d-%02d %02d:%02d:%02d",
                 st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);
        msg = fossil_io_cstring_format("{magenta,bold}Modified:{normal} %s\n", buf);
        fossil_fstream_write(out_stream, msg, 1, fossil_io_cstring_length(msg));
        fossil_io_cstring_free(msg);

        // Created
        ft = fad.ftCreationTime;
        FileTimeToSystemTime(&ft, &st);
        snprintf(buf, sizeof(buf), "%04d-%02d-%02d %02d:%02d:%02d",
                 st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);
        msg = fossil_io_cstring_format("{magenta,bold}Created:{normal} %s\n", buf);
        fossil_fstream_write(out_stream, msg, 1, fossil_io_cstring_length(msg));
        fossil_io_cstring_free(msg);
    }
#else
    // POSIX implementation
    struct stat st;
    if (lstat(path, &st) != 0) {
        cstring msg = fossil_io_cstring_format("{red,bold}Failed to stat path: %s{normal}\n", strerror(errno));
        fossil_fstream_write(err_stream, msg, 1, fossil_io_cstring_length(msg));
        fossil_io_cstring_free(msg);
        return errno;
    }

    // Permissions
    if (show_permissions) {
        char perms[11] = "----------";

        if (S_ISREG(st.st_mode)) perms[0] = '-';
        else if (S_ISDIR(st.st_mode)) perms[0] = 'd';
        else if (S_ISLNK(st.st_mode)) perms[0] = 'l';
        else if (S_ISCHR(st.st_mode)) perms[0] = 'c';
        else if (S_ISBLK(st.st_mode)) perms[0] = 'b';
        else if (S_ISFIFO(st.st_mode)) perms[0] = 'p';
        else if (S_ISSOCK(st.st_mode)) perms[0] = 's';

        perms[1] = (st.st_mode & S_IRUSR) ? 'r' : '-';
        perms[2] = (st.st_mode & S_IWUSR) ? 'w' : '-';
        perms[3] = (st.st_mode & S_IXUSR) ? 'x' : '-';
        perms[4] = (st.st_mode & S_IRGRP) ? 'r' : '-';
        perms[5] = (st.st_mode & S_IWGRP) ? 'w' : '-';
        perms[6] = (st.st_mode & S_IXGRP) ? 'x' : '-';
        perms[7] = (st.st_mode & S_IROTH) ? 'r' : '-';
        perms[8] = (st.st_mode & S_IWOTH) ? 'w' : '-';
        perms[9] = (st.st_mode & S_IXOTH) ? 'x' : '-';

        cstring msg = fossil_io_cstring_format("{green,bold}Permissions:{normal} %s\n", perms);
        fossil_fstream_write(out_stream, msg, 1, fossil_io_cstring_length(msg));
        fossil_io_cstring_free(msg);
    }

    // Owner and group
    if (show_owner) {
        struct passwd *pw = getpwuid(st.st_uid);
        struct group  *gr = getgrgid(st.st_gid);

        cstring owner_msg = fossil_io_cstring_format("{yellow,bold}Owner:{normal} %s (UID %d)\n", pw ? pw->pw_name : "Unknown", st.st_uid);
        cstring group_msg = fossil_io_cstring_format("{yellow,bold}Group:{normal} %s (GID %d)\n", gr ? gr->gr_name : "Unknown", st.st_gid);

        fossil_fstream_write(out_stream, owner_msg, 1, fossil_io_cstring_length(owner_msg));
        fossil_fstream_write(out_stream, group_msg, 1, fossil_io_cstring_length(group_msg));
        fossil_io_cstring_free(owner_msg);
        fossil_io_cstring_free(group_msg);
    }

    // Size
    if (show_size) {
        cstring msg = fossil_io_cstring_format("{cyan,bold}Size:{normal} %lld bytes\n", (long long)st.st_size);
        fossil_fstream_write(out_stream, msg, 1, fossil_io_cstring_length(msg));
        fossil_io_cstring_free(msg);
    }

    // Timestamps
    if (show_timestamps) {
        char buf[64];
        struct tm *tm_info;

        tm_info = localtime(&st.st_atime);
        strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", tm_info);
        cstring msg = fossil_io_cstring_format("{magenta,bold}Accessed:{normal} %s\n", buf);
        fossil_fstream_write(out_stream, msg, 1, fossil_io_cstring_length(msg));
        fossil_io_cstring_free(msg);

        tm_info = localtime(&st.st_mtime);
        strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", tm_info);
        msg = fossil_io_cstring_format("{magenta,bold}Modified:{normal} %s\n", buf);
        fossil_fstream_write(out_stream, msg, 1, fossil_io_cstring_length(msg));
        fossil_io_cstring_free(msg);

        tm_info = localtime(&st.st_ctime);
        strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", tm_info);
        msg = fossil_io_cstring_format("{magenta,bold}Changed:{normal} %s\n", buf);
        fossil_fstream_write(out_stream, msg, 1, fossil_io_cstring_length(msg));
        fossil_io_cstring_free(msg);
    }
#endif
    return 0;
}
