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
    #include <io.h>       // _chmod, _utime
    #include <sys/stat.h> // _stat
    #include <utime.h>    // _utime
    #include <direct.h>   // _mkdir
    #include <fileapi.h>
#else
    #ifndef _POSIX_C_SOURCE
        #define _POSIX_C_SOURCE 200809L
    #endif
    #include <unistd.h>   // symlink, link, stat, utime
    #include <sys/stat.h> // chmod
    #include <utime.h>    // utime
    #include <dirent.h> // For POSIX directory traversal
#endif

#include <string.h>     // strcmp
#include <stdbool.h>    // bool
#include <errno.h>      // errno
#include <stdio.h>      // perror

#ifndef _WIN32
extern int symlink(const char *target, const char *linkpath);
#endif


/**
 * @brief Create new files or directories with specified type and name.
 * @param type The type of the entity to create (file/dir).
 * @param file The path where the file or directory should be created.
 */
void shark_create(const char *file, const char *type) {
    if (strcmp(type, "file") == 0) {
        fossil_fstream_t stream;
        if (fossil_fstream_open(&stream, file, "w") == 0) {
            fossil_fstream_close(&stream);
        }
    } else if (strcmp(type, "dir") == 0) {
#ifdef _WIN32
        _mkdir(file);  // Windows version: no mode
#else
        mkdir(file, 0755);  // POSIX version: includes mode
#endif
    }
}

/**
 * @brief Remove files or directories. Optionally move them to trash instead of permanent deletion.
 * @param force If true, force deletion without confirmation.
 * @param file The path of the file or directory to delete.
 */
void shark_delete(const char *file, bool force) {
    if (force) {
        fossil_fstream_remove(file);
    } else {
        // Implement confirmation logic if needed
        fossil_fstream_remove(file);
    }
}

/**
 * @brief Move or rename files and directories with options to overwrite or create backups.
 * @param source The source path of the file or directory to move.
 * @param destination The destination path where the file or directory should be moved.
 * @param force If true, overwrite existing files.
 * @param backup If true, create backups of overwritten files.
 */
void shark_move(const char *source, const char *destination, bool force, bool backup) {
    if (backup) {
        char backup_name[512];
        snprintf(backup_name, sizeof(backup_name), "%s.bak", destination);
        fossil_fstream_copy(destination, backup_name);
    }
    if (force || !fossil_fstream_file_exists(destination)) {
        fossil_fstream_rename(source, destination);
    }
}

/**
 * @brief Rename files or directories with conflict handling options.
 * @param old_name The current name of the file or directory.
 * @param new_name The new name for the file or directory.
 * @param force If true, overwrite existing files.
 * @param backup If true, create backups of overwritten files.
 */
void shark_rename(const char *old_name, const char *new_name, bool force, bool backup) {
    shark_move(old_name, new_name, force, backup);
}

/**
 * @brief Duplicate files or directories while preserving attributes or creating links.
 * @param source The source path of the file or directory to copy.
 * @param destination The destination path where the copy should be created.
 * @param preserve If true, preserve file attributes.
 * @param file_link The type of link to create (hard/sym).
 */
void shark_copy(const char *source, const char *destination, bool preserve, const char *file_link) {
    // Handle symbolic or hard links
    if (file_link && strcmp(file_link, "sym") == 0) {
#ifdef _WIN32
        // Windows symlink requires privileges + needs to know if it's a file or dir
        DWORD attrs = GetFileAttributesA(source);
        if (attrs == INVALID_FILE_ATTRIBUTES) {
            perror("GetFileAttributes failed");
            return;
        }

        int is_dir = (attrs & FILE_ATTRIBUTE_DIRECTORY) != 0;
        if (CreateSymbolicLinkA(destination, source, is_dir ? SYMBOLIC_LINK_FLAG_DIRECTORY : 0) == 0) {
            fossil_io_fprintf(FOSSIL_STDERR, "{red}CreateSymbolicLink failed: %lu{reset}\n", GetLastError());
        }
#else
        if (symlink(source, destination) != 0) {
            perror("symlink failed");
        }
#endif
        return;
    } else if (file_link && strcmp(file_link, "hard") == 0) {
#ifdef _WIN32
        if (!CreateHardLinkA(destination, source, NULL)) {
            fossil_io_fprintf(FOSSIL_STDERR, "{red}CreateHardLink failed: %lu{reset}\n", GetLastError());
        }
#else
        if (link(source, destination) != 0) {
            perror("link failed");
        }
#endif
        return;
    }

    // Default copy
    fossil_fstream_copy(source, destination);

    if (preserve) {
#ifdef _WIN32
        struct _stat src_stat;
        if (_stat(source, &src_stat) == 0) {
            _chmod(destination, src_stat.st_mode);

            struct _utimbuf times;
            times.actime = src_stat.st_atime;
            times.modtime = src_stat.st_mtime;
            _utime(destination, &times);
        } else {
            perror("_stat failed on source for preserve");
        }
#else
        struct stat src_stat;
        if (stat(source, &src_stat) == 0) {
            chmod(destination, src_stat.st_mode);

            struct utimbuf times;
            times.actime = src_stat.st_atime;
            times.modtime = src_stat.st_mtime;
            utime(destination, &times);
        } else {
            perror("stat failed on source for preserve");
        }
#endif
    }
}

/**
 * @brief Display files or directories in a list or tree format with sorting options.
 * @param path The path of the directory to display.
 * @param as The format to display (list/tree).
 */
void shark_list(const char *path, const char *as) {
#ifdef _WIN32
    WIN32_FIND_DATA find_data;
    char mutable_path[MAX_PATH];
    snprintf(mutable_path, sizeof(mutable_path), "%s\\*", path);
    HANDLE hFind = FindFirstFile(mutable_path, &find_data);
    if (hFind == INVALID_HANDLE_VALUE) {
        fossil_io_printf("{red}Error: Unable to open path %s{reset}\n", path);
        return;
    }
    do {
        if (strcmp(as, "list") == 0) {
            printf("%s\n", find_data.cFileName);
        } else if (strcmp(as, "tree") == 0) {
            printf("|-- %s\n", find_data.cFileName);
        }
    } while (FindNextFile(hFind, &find_data) != 0);
    FindClose(hFind);
#else
    DIR *dir = opendir(path);
    if (!dir) {
        perror("opendir failed");
        return;
    }
    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(as, "list") == 0) {
            printf("%s\n", entry->d_name);
        } else if (strcmp(as, "tree") == 0) {
            printf("|-- %s\n", entry->d_name);
        }
    }
    closedir(dir);
#endif
}

/**
 * @brief View file contents with support for pagination and line offsets.
 * @param file The path of the file to view.
 * @param lines The number of lines to display.
 * @param offset The line offset to start displaying from.
 */
void shark_show(const char *file, int lines, int offset) {
    fossil_fstream_t stream;
    if (fossil_fstream_open(&stream, file, "r") == 0) {
        char buffer[1024];
        int line_count = 0;
        while (fossil_fstream_read(&stream, buffer, 1, sizeof(buffer)) > 0) {
            if (line_count >= offset && line_count < offset + lines) {
                printf("%s", buffer);
            }
            line_count++;
        }
        fossil_fstream_close(&stream);
    }
}

/**
 * @brief Search for files or directories by name, type, or other criteria.
 * @param path The path to search within.
 * @param name The name pattern to search for.
 * @param type The type of entity to search for (file/dir).
 */
void shark_find(const char *path, const char *name, const char *type) {
#ifdef _WIN32
    WIN32_FIND_DATA find_data;
    char search_path[MAX_PATH];
    snprintf(search_path, sizeof(search_path), "%s\\*", path);
    HANDLE hFind = FindFirstFile(search_path, &find_data);
    if (hFind == INVALID_HANDLE_VALUE) {
        fossil_io_printf("{red}Error: Unable to open path %s{reset}\n", path);
        return;
    }
    do {
        if (strstr(find_data.cFileName, name) != NULL) {
            if ((type == NULL) ||
                (strcmp(type, "file") == 0 && !(find_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) ||
                (strcmp(type, "dir") == 0 && (find_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))) {
                printf("{cyan}Found: %s{reset}\n", find_data.cFileName);
            }
        }
    } while (FindNextFile(hFind, &find_data) != 0);
    FindClose(hFind);
#else
    DIR *dir = opendir(path);
    if (!dir) {
        perror("opendir failed");
        return;
    }
    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        if (strstr(entry->d_name, name) != NULL) {
            if ((type == NULL) ||
                (strcmp(type, "file") == 0 && entry->d_type != DT_DIR) ||
                (strcmp(type, "dir") == 0 && entry->d_type == DT_DIR)) {
                printf("{cyan}Found: %s{reset}\n", entry->d_name);
            }
        }
    }
    closedir(dir);
#endif
}

/**
 * @brief Show the size of files or directories with optional human-readable formatting.
 * @param path The path of the file or directory to check.
 * @param human_readable If true, format sizes in a human-readable way.
 */
void shark_size(const char *path, bool human_readable) {
    struct stat file_stat;
    if (stat(path, &file_stat) == 0) {
        off_t size = file_stat.st_size;
        if (human_readable) {
            printf("{cyan}Size: %.2f KB{reset}\n", size / 1024.0);
        } else {
            printf("{cyan}Size: %lld bytes{reset}\n", (long long)size);
        }
    } else {
        perror("stat failed");
    }
}

/**
 * @brief Compare files or directories using binary, diff, or hash methods.
 * @param path1 The path of the first file or directory to compare.
 * @param path2 The path of the second file or directory to compare.
 * @param binary If true, perform a binary comparison.
 * @param diff If true, perform a diff comparison.
 * @param hash If true, perform a hash comparison.
 */
void shark_compare(const char *path1, const char *path2, bool binary, bool diff, bool hash) {
    if (binary) {
        fossil_fstream_t stream1, stream2;
        if (fossil_fstream_open(&stream1, path1, "rb") == 0 && fossil_fstream_open(&stream2, path2, "rb") == 0) {
            char buffer1[1024], buffer2[1024];
            size_t read1, read2;
            bool are_equal = true;

            while ((read1 = fossil_fstream_read(&stream1, buffer1, 1, sizeof(buffer1))) > 0 &&
                   (read2 = fossil_fstream_read(&stream2, buffer2, 1, sizeof(buffer2))) > 0) {
                if (read1 != read2 || memcmp(buffer1, buffer2, read1) != 0) {
                    are_equal = false;
                    break;
                }
            }

            if (are_equal && fossil_fstream_read(&stream1, buffer1, 1, sizeof(buffer1)) == 0 &&
                fossil_fstream_read(&stream2, buffer2, 1, sizeof(buffer2)) == 0) {
                    fossil_io_printf("{cyan}Files are identical (binary comparison).{reset}\n");
            } else {
                fossil_io_printf("{cyan}Files differ (binary comparison).{reset}\n");
            }

            fossil_fstream_close(&stream1);
            fossil_fstream_close(&stream2);
        } else {
            fossil_io_printf("{red}Error opening files for binary comparison.{reset}\n");
        }
    }

    if (diff) {
        fossil_fstream_t stream1, stream2;
        if (fossil_fstream_open(&stream1, path1, "r") != 0 || fossil_fstream_open(&stream2, path2, "r") != 0) {
            fossil_io_printf("{red}Error opening files for diff comparison.{reset}\n");
            if (fossil_fstream_is_open(&stream1)) fossil_fstream_close(&stream1);
            if (fossil_fstream_is_open(&stream2)) fossil_fstream_close(&stream2);
            return;
        }

        char line1[1024], line2[1024];
        int line_number = 1;
        bool differences_found = false;

        while (fossil_fstream_read(&stream1, line1, 1, sizeof(line1)) > 0 &&
               fossil_fstream_read(&stream2, line2, 1, sizeof(line2)) > 0) {
            if (strcmp(line1, line2) != 0) {
                fossil_io_printf("{cyan}Difference at line %d:{reset}\n", line_number);
                fossil_io_printf("{cyan}File1: %s{reset}\n", line1);
                fossil_io_printf("{cyan}File2: %s{reset}\n", line2);
                differences_found = true;
            }
            line_number++;
        }

        // Check for remaining lines in either file
        while (fossil_fstream_read(&stream1, line1, 1, sizeof(line1)) > 0) {
            fossil_io_printf("{cyan}Extra line in File1 at line %d: %s{reset}\n", line_number, line1);
            differences_found = true;
            line_number++;
        }

        while (fossil_fstream_read(&stream2, line2, 1, sizeof(line2)) > 0) {
            fossil_io_printf("{cyan}Extra line in File2 at line %d: %s{reset}\n", line_number, line2);
            differences_found = true;
            line_number++;
        }

        if (!differences_found) {
            fossil_io_printf("{cyan}Files are identical (diff comparison).{reset}\n");
        }

        fossil_fstream_close(&stream1);
        fossil_fstream_close(&stream2);
    }

    if (hash) {
        fossil_fstream_t stream1, stream2;
        if (fossil_fstream_open(&stream1, path1, "rb") != 0 || fossil_fstream_open(&stream2, path2, "rb") != 0) {
            fossil_io_printf("{red}Error opening files for hash comparison.{reset}\n");
            if (fossil_fstream_is_open(&stream1)) fossil_fstream_close(&stream1);
            if (fossil_fstream_is_open(&stream2)) fossil_fstream_close(&stream2);
            return;
        }

        unsigned long hash1 = 0, hash2 = 0;
        char buffer[1024];
        size_t bytes_read;

        while ((bytes_read = fossil_fstream_read(&stream1, buffer, 1, sizeof(buffer))) > 0) {
            for (size_t i = 0; i < bytes_read; i++) {
                hash1 = (hash1 * 31) + buffer[i];
            }
        }

        while ((bytes_read = fossil_fstream_read(&stream2, buffer, 1, sizeof(buffer))) > 0) {
            for (size_t i = 0; i < bytes_read; i++) {
                hash2 = (hash2 * 31) + buffer[i];
            }
        }

        fossil_fstream_close(&stream1);
        fossil_fstream_close(&stream2);

        if (hash1 == hash2) {
            fossil_io_printf("{cyan}Files are identical (hash comparison).{reset}\n");
        } else {
            fossil_io_printf("{cyan}Files differ (hash comparison).{reset}\n");
        }
    }
}

/**
 * @brief Verify the existence or type of files or directories.
 * @param exists Path to check for existence.
 * @param not_exist Path to check for non-existence.
 * @param type The type of entity to verify (file/dir).
 */
void shark_ask(const char *exists, const char *not_exist, const char *type) {
    struct stat exists_stat, not_exist_stat;
    bool exists_found = (stat(exists, &exists_stat) == 0);
    bool not_exist_found = (stat(not_exist, &not_exist_stat) == 0);

    if (exists_found) {
        printf("{cyan}%s exists.{reset}\n", exists);
    } else if (!not_exist_found) {
        printf("{red}%s does not exist.{reset}\n", not_exist);
    } else {
        printf("{red}Neither %s nor %s exist.{reset}\n", exists, not_exist);
    }

    if (exists_found) {
        if (strcmp(type, "file") == 0 && S_ISREG(exists_stat.st_mode)) {
            printf("{cyan}%s is a file.{reset}\n", exists);
        } else if (strcmp(type, "dir") == 0 && S_ISDIR(exists_stat.st_mode)) {
            printf("{cyan}%s is a directory.{reset}\n", exists);
        } else {
            printf("{red}Unknown type for %s.{reset}\n", exists);
        }
    }
}

/**
 * @brief Update file properties such as permissions, ownership, or mode.
 * @param target The path of the file or directory to modify.
 * @param value The new value to set.
 * @param owner If true, update ownership.
 * @param mode If true, update permissions/mode.
 */
void shark_change(const char *target, const char *value, bool owner, bool mode) {
    if (mode) {
        int32_t permissions = strtol(value, NULL, 8);
        fossil_fstream_set_permissions(target, permissions);
    }
    if (owner) {
        #ifdef _WIN32
            // Windows does not have a direct API for changing ownership
            fossil_io_printf("{red}Changing ownership is not supported on Windows.{reset}\n");
        #else
            if (chown(target, -1, atoi(value)) != 0) {
                perror("chown failed");
            }
        #endif
    }
}
