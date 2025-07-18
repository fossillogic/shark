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
#else
    #include <unistd.h>   // symlink, link, stat, utime
    #include <sys/stat.h> // chmod
    #include <utime.h>    // utime
#endif

#include <string.h>     // strcmp
#include <stdbool.h>    // bool
#include <errno.h>      // errno
#include <stdio.h>      // perror

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
    if (file_link && strcmp(file_link, "sym") == 0) {
        if (symlink(source, destination) != 0) {
            perror("symlink failed");
        }
        return;
    } else if (file_link && strcmp(file_link, "hard") == 0) {
        if (link(source, destination) != 0) {
            perror("link failed");
        }
        return;
    }

    // Default: copy contents
    fossil_fstream_copy(source, destination);

    if (preserve) {
        struct stat src_stat;
        if (stat(source, &src_stat) == 0) {
            // Preserve permissions
            chmod(destination, src_stat.st_mode);

            // Preserve timestamps
            struct utimbuf times;
            times.actime = src_stat.st_atime;
            times.modtime = src_stat.st_mtime;
            utime(destination, &times);
        } else {
            perror("stat failed on source for preserve");
        }
    }
}

/**
 * @brief Display files or directories in a list or tree format with sorting options.
 * @param path The path of the directory to display.
 * @param as The format to display (list/tree).
 */
void shark_list(const char *path, const char *as) {
    fossil_fstream_t stream;
    if (fossil_fstream_open(&stream, path, "r") == 0) {
        char buffer[1024];
        while (fossil_fstream_read(&stream, buffer, 1, sizeof(buffer)) > 0) {
            // Display in list or tree format
            if (strcmp(as, "list") == 0) {
                printf("%s\n", buffer);
            } else if (strcmp(as, "tree") == 0) {
                printf("|-- %s\n", buffer);
            }
        }
        fossil_fstream_close(&stream);
    } else {
        printf("Error: Unable to open path %s\n", path);
    }
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
    fossil_fstream_t stream;
    if (fossil_fstream_open(&stream, path, "r") == 0) {
        char buffer[1024];
        while (fossil_fstream_read(&stream, buffer, 1, sizeof(buffer)) > 0) {
            if (strstr(buffer, name) != NULL) {
                if (type == NULL || strcmp(type, "file") == 0 || strcmp(type, "dir") == 0) {
                    printf("Found: %s\n", buffer);
                }
            }
        }
        fossil_fstream_close(&stream);
    } else {
        printf("Error: Unable to open path %s\n", path);
    }
}

/**
 * @brief Show the size of files or directories with optional human-readable formatting.
 * @param path The path of the file or directory to check.
 * @param human_readable If true, format sizes in a human-readable way.
 */
void shark_size(const char *path, bool human_readable) {
    fossil_fstream_t stream;
    if (fossil_fstream_open(&stream, path, "r") == 0) {
        int32_t size = fossil_fstream_get_size(&stream);
        if (human_readable) {
            printf("Size: %.2f KB\n", size / 1024.0);
        } else {
            printf("Size: %d bytes\n", size);
        }
        fossil_fstream_close(&stream);
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
                printf("Files are identical (binary comparison).\n");
            } else {
                printf("Files differ (binary comparison).\n");
            }

            fossil_fstream_close(&stream1);
            fossil_fstream_close(&stream2);
        } else {
            printf("Error opening files for binary comparison.\n");
        }
    }

    if (diff) {
        // Placeholder for diff comparison logic
        printf("Diff comparison is not implemented yet.\n");
    }

    if (hash) {
        // Placeholder for hash comparison logic
        printf("Hash comparison is not implemented yet.\n");
    }
}

/**
 * @brief Verify the existence or type of files or directories.
 * @param exists Path to check for existence.
 * @param not_exist Path to check for non-existence.
 * @param type The type of entity to verify (file/dir).
 */
void shark_ask(const char *exists, const char *not_exist, const char *type) {
    if (fossil_fstream_file_exists(exists)) {
        printf("%s exists.\n", exists);
    } else if (!fossil_fstream_file_exists(not_exist)) {
        printf("%s does not exist.\n", not_exist);
    } else {
        printf("Neither %s nor %s exist.\n", exists, not_exist);
    }

    if (strcmp(type, "file") == 0) {
        printf("%s is a file.\n", exists);
    } else if (strcmp(type, "dir") == 0) {
        printf("%s is a directory.\n", exists);
    } else {
        printf("Unknown type for %s.\n", exists);
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
        // Implement ownership change logic, possibly using chown or similar system calls
    }
}
