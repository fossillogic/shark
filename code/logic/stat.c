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
    #include <io.h>
    #include <sys/types.h>
    #include <sys/stat.h>
    #include <direct.h>
    #include <time.h>
    #include <errno.h>
    #include <string.h>
#else
    #include <sys/stat.h>
    #include <sys/vfs.h>  // For filesystem info
    #include <pwd.h>
    #include <grp.h>
    #include <time.h>
    #include <unistd.h>
    #include <errno.h>
    #include <string.h>
#endif

/**
 * Display file statistics (size, blocks, permissions)
 * @param path Path to file or directory
 * @param format Custom format string
 * @param filesystem Show filesystem info if true
 * @return 0 on success, non-zero on error
 */
int fossil_shark_stat(const char *path, const char *format, bool filesystem)
{
    #if defined(_WIN32) || defined(_WIN64)
        struct _stat st;
        if (_stat(path, &st) != 0) {
            cstring msg = fossil_io_cstring_format("{red,bold}Failed to stat path: %s{normal}\n", path);
            fossil_fstream_write(FOSSIL_STDERR, msg, fossil_io_cstring_length(msg), 1);
            fossil_io_cstring_free(msg);
            return errno;
        }
    #else
        struct stat st;
        if (lstat(path, &st) != 0) {
            cstring msg = fossil_io_cstring_format("{red,bold}Failed to stat path: %s{normal}\n", path);
            fossil_fstream_write(FOSSIL_STDERR, msg, fossil_io_cstring_length(msg), 1);
            fossil_io_cstring_free(msg);
            return errno;
        }
    #endif

    char perms[11] = "----------";
    #if defined(_WIN32) || defined(_WIN64)
        perms[0] = (st.st_mode & _S_IFDIR) ? 'd' : '-';
        perms[1] = (st.st_mode & _S_IREAD) ? 'r' : '-';
        perms[2] = (st.st_mode & _S_IWRITE) ? 'w' : '-';
        perms[3] = (st.st_mode & _S_IEXEC) ? 'x' : '-';
    #else
        if (S_ISREG(st.st_mode)) perms[0] = '-';
        else if (S_ISDIR(st.st_mode)) perms[0] = 'd';
        else if (S_ISLNK(st.st_mode)) perms[0] = 'l';
        perms[1] = (st.st_mode & S_IRUSR) ? 'r' : '-';
        perms[2] = (st.st_mode & S_IWUSR) ? 'w' : '-';
        perms[3] = (st.st_mode & S_IXUSR) ? 'x' : '-';
        perms[4] = (st.st_mode & S_IRGRP) ? 'r' : '-';
        perms[5] = (st.st_mode & S_IWGRP) ? 'w' : '-';
        perms[6] = (st.st_mode & S_IXGRP) ? 'x' : '-';
        perms[7] = (st.st_mode & S_IROTH) ? 'r' : '-';
        perms[8] = (st.st_mode & S_IWOTH) ? 'w' : '-';
        perms[9] = (st.st_mode & S_IXOTH) ? 'x' : '-';
    #endif

    #if defined(_WIN32) || defined(_WIN64)
        cstring owner = fossil_io_cstring_create("N/A");
        cstring group = fossil_io_cstring_create("N/A");
    #else
        struct passwd *pw = getpwuid(st.st_uid);
        struct group *gr = getgrgid(st.st_gid);
        cstring owner = fossil_io_cstring_create(pw ? pw->pw_name : "Unknown");
        cstring group = fossil_io_cstring_create(gr ? gr->gr_name : "Unknown");
    #endif

    char accessed[64], modified[64], changed[64];
    struct tm *tm_info;
    #if defined(_WIN32) || defined(_WIN64)
        tm_info = localtime(&st.st_atime);
        strftime(accessed, sizeof(accessed), "%Y-%m-%d %H:%M:%S", tm_info);
        tm_info = localtime(&st.st_mtime);
        strftime(modified, sizeof(modified), "%Y-%m-%d %H:%M:%S", tm_info);
        tm_info = localtime(&st.st_ctime);
        strftime(changed, sizeof(changed), "%Y-%m-%d %H:%M:%S", tm_info);
    #else
        tm_info = localtime(&st.st_atime);
        strftime(accessed, sizeof(accessed), "%Y-%m-%d %H:%M:%S", tm_info);
        tm_info = localtime(&st.st_mtime);
        strftime(modified, sizeof(modified), "%Y-%m-%d %H:%M:%S", tm_info);
        tm_info = localtime(&st.st_ctime);
        strftime(changed, sizeof(changed), "%Y-%m-%d %H:%M:%S", tm_info);
    #endif

    fossil_io_cstring_stream *stream = fossil_io_cstring_stream_create(1024);

    if (!format || fossil_io_cstring_length(format) == 0) {
        fossil_io_cstring_stream_write_format(stream,
            "{cyan,bold}File:{normal} %s\n"
            "{yellow}Permissions:{normal} %s\n",
            path, perms);

        #if defined(_WIN32) || defined(_WIN64)
            fossil_io_cstring_stream_write_format(stream,
                "{green}Size:{normal} %lld bytes\n"
                "{magenta}Blocks:{normal} N/A\n"
                "{blue}Owner:{normal} %s\n"
                "{blue}Group:{normal} %s\n"
                "{bright_black}Accessed:{normal} %s\n"
                "{bright_black}Modified:{normal} %s\n"
                "{bright_black}Changed:{normal} %s\n",
                (long long)st.st_size, owner, group, accessed, modified, changed);
        #else
            fossil_io_cstring_stream_write_format(stream,
                "{green}Size:{normal} %lld bytes\n"
                "{magenta}Blocks:{normal} %lld\n"
                "{blue}Owner:{normal} %s (UID %d)\n"
                "{blue}Group:{normal} %s (GID %d)\n"
                "{bright_black}Accessed:{normal} %s\n"
                "{bright_black}Modified:{normal} %s\n"
                "{bright_black}Changed:{normal} %s\n",
                (long long)st.st_size, (long long)st.st_blocks,
                owner, st.st_uid,
                group, st.st_gid,
                accessed, modified, changed);
        #endif
        ccstring out = fossil_io_cstring_stream_read(stream);
        fossil_fstream_write(FOSSIL_STDOUT, out, fossil_io_cstring_stream_length(stream), 1);
    } else {
        for (size_t i = 0; format[i] != '\0'; ++i) {
            if (format[i] == '%' && format[i+1] != '\0') {
                ++i;
                switch (format[i]) {
                    case 'n':
                        fossil_io_cstring_stream_write(stream, path);
                        break;
                    case 's': {
                        cstring sz = fossil_io_cstring_format("%lld", (long long)st.st_size);
                        fossil_io_cstring_stream_write(stream, sz);
                        fossil_io_cstring_free(sz);
                        break;
                    }
                    #if defined(_WIN32) || defined(_WIN64)
                        case 'b':
                            fossil_io_cstring_stream_write(stream, "N/A");
                            break;
                        case 'u':
                            fossil_io_cstring_stream_write(stream, owner);
                            break;
                        case 'g':
                            fossil_io_cstring_stream_write(stream, group);
                            break;
                        case 'U':
                        case 'G':
                            fossil_io_cstring_stream_write(stream, "N/A");
                            break;
                    #else
                        case 'b': {
                            cstring blocks = fossil_io_cstring_format("%lld", (long long)st.st_blocks);
                            fossil_io_cstring_stream_write(stream, blocks);
                            fossil_io_cstring_free(blocks);
                            break;
                        }
                        case 'u':
                            fossil_io_cstring_stream_write(stream, owner);
                            break;
                        case 'g':
                            fossil_io_cstring_stream_write(stream, group);
                            break;
                        case 'U': {
                            cstring uid = fossil_io_cstring_format("%d", st.st_uid);
                            fossil_io_cstring_stream_write(stream, uid);
                            fossil_io_cstring_free(uid);
                            break;
                        }
                        case 'G': {
                            cstring gid = fossil_io_cstring_format("%d", st.st_gid);
                            fossil_io_cstring_stream_write(stream, gid);
                            fossil_io_cstring_free(gid);
                            break;
                        }
                    #endif
                    case 'a':
                        fossil_io_cstring_stream_write(stream, perms);
                        break;
                    case 'A':
                        fossil_io_cstring_stream_write(stream, accessed);
                        break;
                    case 'M':
                        fossil_io_cstring_stream_write(stream, modified);
                        break;
                    case 'C':
                        fossil_io_cstring_stream_write(stream, changed);
                        break;
                    case '%':
                        fossil_io_cstring_stream_write(stream, "%");
                        break;
                    default:
                        fossil_io_cstring_stream_write(stream, "%");
                        fossil_io_cstring_stream_write_safe(stream, &format[i], 1);
                        break;
                }
            } else {
                fossil_io_cstring_stream_write_safe(stream, &format[i], 1);
            }
        }
        fossil_io_cstring_stream_write(stream, "\n");
        ccstring out = fossil_io_cstring_stream_read(stream);
        fossil_fstream_write(FOSSIL_STDOUT, out, fossil_io_cstring_stream_length(stream), 1);
    }

    fossil_io_cstring_free(owner);
    fossil_io_cstring_free(group);
    fossil_io_cstring_stream_free(stream);

    if (filesystem) {
        #if defined(_WIN32) || defined(_WIN64)
            cstring msg = fossil_io_cstring_create("{red}Filesystem info: Not supported on Windows{normal}\n");
            fossil_fstream_write(FOSSIL_STDOUT, msg, fossil_io_cstring_length(msg), 1);
            fossil_io_cstring_free(msg);
        #else
            struct statfs fs;
            if (statfs(path, &fs) != 0) {
                cstring msg = fossil_io_cstring_format("{red,bold}Failed to get filesystem info: %s{normal}\n", path);
                fossil_fstream_write(FOSSIL_STDERR, msg, fossil_io_cstring_length(msg), 1);
                fossil_io_cstring_free(msg);
                return errno;
            }
            cstring fsinfo = fossil_io_cstring_format(
                "{bright_blue}Filesystem type:{normal} %lx\n"
                "{bright_blue}Block size:{normal} %lu\n"
                "{bright_blue}Total blocks:{normal} %lu\n"
                "{bright_blue}Free blocks:{normal} %lu\n",
                (unsigned long)fs.f_type,
                (unsigned long)fs.f_bsize,
                (unsigned long)fs.f_blocks,
                (unsigned long)fs.f_bfree);
            fossil_fstream_write(FOSSIL_STDOUT, fsinfo, fossil_io_cstring_length(fsinfo), 1);
            fossil_io_cstring_free(fsinfo);
        #endif
    }

    return 0;
}
