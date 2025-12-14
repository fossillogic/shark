/**
 * -----------------------------------------------------------------------------
 * Project: Fossil Logic
 *
 * This file is part of the Fossil Logic project, which aims to develop
 * high-performance, cross-platform applications and libraries. The code
 * contained herein is licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License. You may obtain
 * a copy of the License at:
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
 * License for the specific language governing permissions and limitations
 * under the License.
 *
 * Author: Michael Gene Brockus (Dreamer)
 * Date: 04/05/2014
 *
 * Copyright (C) 2014-2025 Fossil Logic. All rights reserved.
 * -----------------------------------------------------------------------------
 */
#include "fossil/code/commands.h"


static int copy_file(ccstring src, ccstring dest, bool update, bool preserve) {
    if (cunlikely(!cnotnull(src) || !cnotnull(dest))) {
        fossil_io_printf("{red}Error: Source and destination paths cannot be null{normal}\n");
        return 1;
    }

    struct stat st_src, st_dest;
    if (stat(src, &st_src) != 0) {
        fossil_io_printf("{red}Error: Cannot stat source file '%s': %s{normal}\n", src, strerror(errno));
        return errno;
    }

    bool dest_exists = (stat(dest, &st_dest) == 0);

    if (update && dest_exists) {
        // Compute hash of source and destination files to check for content changes
        char src_hash[128], dest_hash[128];
        int src_hash_res = 0, dest_hash_res = 0;
        {
            fossil_io_file_t src_stream;
            if (fossil_io_file_open(&src_stream, src, "rb") == 0) {
                size_t total = 0;
                // Read file into buffer (for demonstration, read up to 1MB)
                char* file_data = malloc(st_src.st_size);
                if (file_data) {
                    size_t n;
                    while ((n = fossil_io_file_read(&src_stream, file_data + total, 1, (size_t)st_src.st_size - total)) > 0) {
                        total += n;
                        if (total >= (size_t)st_src.st_size) break;
                    }
                    src_hash_res = fossil_cryptic_hash_compute(
                        "xxhash64", "auto", "hex",
                        src_hash, sizeof(src_hash),
                        file_data, total
                    );
                    free(file_data);
                } else {
                    src_hash_res = 1;
                }
                fossil_io_file_close(&src_stream);
            } else {
                src_hash_res = 1;
            }
        }
        {
            fossil_io_file_t dest_stream;
            if (fossil_io_file_open(&dest_stream, dest, "rb") == 0) {
                char* file_data = malloc(st_dest.st_size);
                size_t total = 0;
                if (file_data) {
                    size_t n;
                    while ((n = fossil_io_file_read(&dest_stream, file_data + total, 1, (size_t)st_dest.st_size - total)) > 0) {
                        total += n;
                        if (total >= (size_t)st_dest.st_size) break;
                    }
                    dest_hash_res = fossil_cryptic_hash_compute(
                        "xxhash64", "auto", "hex",
                        dest_hash, sizeof(dest_hash),
                        file_data, total
                    );
                    free(file_data);
                } else {
                    dest_hash_res = 1;
                }
                fossil_io_file_close(&dest_stream);
            } else {
                dest_hash_res = 1;
            }
        }
        if (src_hash_res == 0 && dest_hash_res == 0 && strcmp(src_hash, dest_hash) == 0) {
            fossil_io_printf("{cyan}Skipping '%s' - destination is up to date (hash match){normal}\n", src);
            return 0;
        }
    }

    fossil_io_file_t src_stream, dest_stream;
    
    if (fossil_io_file_open(&src_stream, src, "rb") != 0) {
        fossil_io_printf("{red}Error: Cannot open source file '%s': %s{normal}\n", src, strerror(errno));
        return errno;
    }

    if (fossil_io_file_open(&dest_stream, dest, "wb") != 0) {
        fossil_io_printf("{red}Error: Cannot create destination file '%s': %s{normal}\n", dest, strerror(errno));
        fossil_io_file_close(&src_stream);
        return errno;
    }

    fossil_io_printf("{cyan}Copying file: %s -> %s{normal}\n", src, dest);

    char buffer[8192];
    size_t n;
    while ((n = fossil_io_file_read(&src_stream, buffer, 1, sizeof(buffer))) > 0) {
        if (cunlikely(fossil_io_file_write(&dest_stream, buffer, 1, n) != n)) {
            fossil_io_printf("{red}Error: Write failed for '%s': %s{normal}\n", dest, strerror(errno));
            fossil_io_file_close(&src_stream);
            fossil_io_file_close(&dest_stream);
            return errno;
        }
    }

    fossil_io_file_close(&src_stream);
    fossil_io_file_close(&dest_stream);

    // Preserve permissions and timestamps
    if (preserve) {
#ifdef _WIN32
        // Windows: Set file attributes and times
        HANDLE hFile = CreateFileA(dest, GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
        if (hFile != INVALID_HANDLE_VALUE) {
            FILETIME ft;
            LONGLONG ll = Int32x32To64(st_src.st_mtime, 10000000) + 116444736000000000;
            ft.dwLowDateTime = (DWORD)ll;
            ft.dwHighDateTime = (DWORD)(ll >> 32);
            SetFileTime(hFile, NULL, NULL, &ft);
            CloseHandle(hFile);
        }
#else
        chmod(dest, st_src.st_mode);
        struct utimbuf times = {st_src.st_atime, st_src.st_mtime};
        utime(dest, &times);
#endif
        fossil_io_printf("{cyan}Preserved permissions and timestamps for '%s'{normal}\n", dest);
    }

    return 0;
}

static int copy_directory(ccstring src, ccstring dest,
                          bool recursive, bool update, bool preserve) {
    if (cunlikely(!cnotnull(src) || !cnotnull(dest))) {
        fossil_io_printf("{red}Error: Source and destination paths cannot be null{normal}\n");
        return 1;
    }

    struct stat st;
    if (stat(src, &st) != 0) { 
        fossil_io_printf("{red}Error: Cannot stat source directory '%s': %s{normal}\n", src, strerror(errno));
        return errno; 
    }

    // Create destination directory using fossil_io_dir_create
    fossil_io_printf("{cyan}Creating directory: %s{normal}\n", dest);
    int32_t dir_create_result = fossil_io_dir_create(dest);
    if (dir_create_result < 0 && dir_create_result != -EEXIST) {
        fossil_io_printf("{red}Error: Cannot create directory '%s'{normal}\n", dest);
        return 1;
    }

    // Use directory iterator API for traversal
    fossil_io_dir_iter_t it;
    if (fossil_io_dir_iter_open(&it, src) < 0) {
        fossil_io_printf("{red}Error: Cannot open directory '%s'{normal}\n", src);
        return 1;
    }

    while (fossil_io_dir_iter_next(&it) > 0) {
        fossil_io_dir_entry_t *entry = &it.current;
        // Skip "." and ".."
        if (strcmp(entry->name, ".") == 0 || strcmp(entry->name, "..") == 0)
            continue;

        char dest_path[1024];
        if (fossil_io_dir_join(dest, entry->name, dest_path, sizeof(dest_path)) < 0) {
            fossil_io_printf("{red}Error: Path join failed for '%s'{normal}\n", entry->name);
            fossil_io_dir_iter_close(&it);
            return 1;
        }

        if (entry->type == 1) { // Directory
            if (recursive) {
                if (copy_directory(entry->path, dest_path, recursive, update, preserve) != 0) {
                    fossil_io_dir_iter_close(&it);
                    return 1;
                }
            }
        } else if (entry->type == 0) { // File
            if (copy_file(entry->path, dest_path, update, preserve) != 0) {
                fossil_io_dir_iter_close(&it);
                return 1;
            }
        }
        // Symlinks and other types can be handled here if needed
    }
    fossil_io_dir_iter_close(&it);

    // Preserve directory timestamps
    if (preserve) {
#ifdef _WIN32
        HANDLE hDir = CreateFileA(dest, GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, 
                                  OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, NULL);
        if (hDir != INVALID_HANDLE_VALUE) {
            FILETIME ft;
            LONGLONG ll = Int32x32To64(st.st_mtime, 10000000) + 116444736000000000;
            ft.dwLowDateTime = (DWORD)ll;
            ft.dwHighDateTime = (DWORD)(ll >> 32);
            SetFileTime(hDir, NULL, NULL, &ft);
            CloseHandle(hDir);
        }
#else
        struct utimbuf times = {st.st_atime, st.st_mtime};
        utime(dest, &times);
#endif
    }

    return 0;
}

int fossil_shark_copy(ccstring src, ccstring dest,
                      bool recursive, bool update, bool preserve) {
    if (cunlikely(!cnotnull(src) || !cnotnull(dest))) {
        fossil_io_printf("{red}Error: Source and destination must be specified{normal}\n");
        return 1;
    }

    struct stat st;
    if (stat(src, &st) != 0) {
        fossil_io_printf("{red}Error: Cannot access source '%s': %s{normal}\n", src, strerror(errno));
        return errno;
    }

    if (S_ISDIR(st.st_mode)) {
        if (!recursive) {
            fossil_io_printf("{red}Error: Source is a directory. Use recursive flag to copy directories{normal}\n");
            return 1;
        }
        fossil_io_printf("{cyan}Starting recursive copy of directory: %s -> %s{normal}\n", src, dest);
        return copy_directory(src, dest, recursive, update, preserve);
    } else if (S_ISREG(st.st_mode)) {
        return copy_file(src, dest, update, preserve);
    } else {
        fossil_io_printf("{red}Error: Unsupported file type for '%s'{normal}\n", src);
        return 1;
    }
    return 0;
}
