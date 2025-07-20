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
#ifndef FOSSIL_APP_COMMANDS_H
#define FOSSIL_APP_COMMANDS_H

#include <fossil/sys/framework.h>
#include <fossil/io/framework.h>

#define FOSSIL_APP_NAME "Shark Tool"
#define FOSSIL_APP_VERSION "0.1.0"

#ifdef __cplusplus
extern "C" {
#endif

extern int ENABLE_VERBOSE; // Global variable to control verbose output

/**
 * Displays the general help message for available commands.
 */
void handle_help(void);

/**
 * Prints the application version.
 */
void handle_version(void);

/**
 * Prints the application name.
 */
void handle_name(void);

/**
 * Moves a file or directory from source to destination.
 *
 * @param source The source path.
 * @param destination The destination path.
 */
void handle_move(const char *source, const char *destination);

/**
 * Copies a file or directory from source to destination.
 *
 * @param source The source path.
 * @param destination The destination path.
 */
void handle_copy(const char *source, const char *destination);

/**
 * Deletes a file or directory.
 *
 * @param target The target path to delete.
 */
void handle_delete(const char *target);

/**
 * Lists the contents of a directory.
 *
 * @param directory The directory path to list.
 */
void handle_list(const char *directory);

/**
 * Displays the contents of a file.
 *
 * @param file The file path to display.
 */
void handle_show(const char *file);

/**
 * Searches a directory for entries matching a pattern.
 *
 * @param directory The directory to search.
 * @param pattern The pattern to match.
 */
void handle_find(const char *directory, const char *pattern);

/**
 * Searches a file for lines matching a pattern.
 *
 * @param file The file to search.
 * @param pattern The pattern to search for.
 */
void handle_search(const char *file, const char *pattern);

/**
 * Displays the size of a file or directory.
 *
 * @param target The target path.
 */
void handle_size(const char *target);

/**
 * Displays disk usage statistics for a given path.
 *
 * @param path The path to analyze.
 */
void handle_disk(const char *path);

/**
 * Displays the directory tree structure of a given directory.
 *
 * @param directory The directory to walk recursively.
 */
void handle_tree(const char *directory);

/**
 * Compares two files or directories.
 *
 * @param path1 The first path.
 * @param path2 The second path.
 */
void handle_compare(const char *path1, const char *path2);

/**
 * Creates a new file or directory.
 *
 * @param target The target path to create.
 */
void handle_create(const char *target);

/**
 * Prompts the user for input or confirmation.
 *
 * @param prompt The prompt message to display.
 * @return The user's response.
 */
const char *handle_ask(const char *prompt);

/**
 * Renames a file or directory.
 *
 * @param old_name The current name.
 * @param new_name The new name.
 */
void handle_rename(const char *old_name, const char *new_name);

/**
 * Displays the current working directory.
 */
void handle_where(void);

/**
 * Opens a file for editing.
 *
 * @param file The file path to edit.
 */
void handle_edit(const char *file);

#ifdef __cplusplus
}
#endif

#endif /* FOSSIL_APP_LIFECYCLE_H */
