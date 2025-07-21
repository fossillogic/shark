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
 * @param force If true, overwrite destination if it exists.
 * @param interactive If true, prompt before overwriting.
 */
void handle_move(const char *source, const char *destination, int force, int interactive);

/**
 * Copies a file or directory from source to destination.
 *
 * @param source The source path.
 * @param destination The destination path.
 * @param force If true, overwrite destination if it exists.
 * @param recursive If true, copy directories recursively.
 */
void handle_copy(const char *source, const char *destination, int force, int recursive);

/**
 * Deletes a file or directory.
 *
 * @param target The target path to delete.
 * @param force If true, ignore nonexistent files and arguments, never prompt.
 * @param recursive If true, remove directories and their contents recursively.
 * @param interactive If true, prompt before each removal.
 */
void handle_delete(const char *target, int force, int recursive, int interactive);

/**
 * Lists the contents of a directory.
 *
 * @param directory The directory path to list.
 * @param sort Sorting method (e.g., name, size, time).
 * @param format Output format (e.g., columns, lines).
 * @param all If true, include hidden files.
 * @param long_format If true, use long listing format.
 */
void handle_list(const char *directory, const char *sort, const char *format, int all, int long_format);

/**
 * Displays the contents of a file.
 *
 * @param file The file path to display.
 * @param head Number of lines from the start to show.
 * @param tail Number of lines from the end to show.
 * @param lines Specific lines to display.
 */
void handle_show(const char *file, int head, int tail, const char *lines);

/**
 * Searches a directory for entries matching a pattern.
 *
 * @param directory The directory to search.
 * @param name Name pattern to match.
 * @param size Size criteria to match.
 * @param type Type of entry (file, dir, etc.).
 * @param hidden If true, include hidden files.
 */
void handle_find(const char *directory, const char *name, const char *size, const char *type, int hidden);

/**
 * Searches a file for lines matching a pattern.
 *
 * @param file The file to search.
 * @param pattern The pattern to search for.
 * @param ignore_case If true, ignore case in matching.
 * @param count If true, only count matches.
 */
void handle_search(const char *file, const char *pattern, int ignore_case, int count);

/**
 * Displays the size of a file or directory.
 *
 * @param target The target path.
 * @param human_readable If true, use human-readable units.
 * @param total If true, show total size.
 */
void handle_size(const char *target, int human_readable, int total);

/**
 * Displays disk usage statistics for a given path.
 *
 * @param path The path to analyze.
 * @param all If true, show all mount points.
 * @param human_readable If true, use human-readable units.
 */
void handle_disk(const char *path, int all, int human_readable);

/**
 * Displays the directory tree structure of a given directory.
 *
 * @param directory The directory to walk recursively.
 * @param depth Maximum depth to display.
 * @param all If true, include hidden files.
 * @param color If true, use colored output.
 */
void handle_tree(const char *directory, int depth, int all, int color);

/**
 * Compares two files or directories.
 *
 * @param path1 The first path.
 * @param path2 The second path.
 * @param recursive If true, compare directories recursively.
 * @param brief If true, only report if files differ.
 * @param ignore_case If true, ignore case differences.
 */
void handle_compare(const char *path1, const char *path2, int recursive, int brief, int ignore_case);

/**
 * Creates a new file or directory.
 *
 * @param target The target path to create.
 * @param directory If true, create a directory.
 * @param parents If true, create parent directories as needed.
 */
void handle_create(const char *target, int directory, int parents);

/**
 * Prompts the user for input or confirmation.
 *
 * @param message The prompt message to display.
 * @param default_response The default response if user does not reply.
 * @param timeout Timeout in seconds for user response.
 * @return The user's response.
 */
const char *handle_ask(const char *message, const char *default_response, int timeout);

/**
 * Renames a file or directory.
 *
 * @param old_name The current name.
 * @param new_name The new name.
 * @param force If true, overwrite destination if it exists.
 */
void handle_rename(const char *old_name, const char *new_name, int force);

/**
 * Displays the current working directory or location of files/directories matching criteria.
 *
 * @param name Name pattern to match.
 * @param type Type of entry (file, dir, etc.).
 */
void handle_where(const char *name, const char *type);

/**
 * Opens a file for editing.
 *
 * @param file The file path to edit.
 * @param editor The editor to use.
 * @param backup If true, create a backup before editing.
 */
void handle_edit(const char *file, const char *editor, int backup);

#ifdef __cplusplus
}
#endif

#endif /* FOSSIL_APP_LIFECYCLE_H */
