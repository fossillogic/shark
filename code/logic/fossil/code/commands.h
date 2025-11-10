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

#include <fossil/cryptic/framework.h>
#include <fossil/math/framework.h>
#include <fossil/sys/framework.h>
#include <fossil/ai/framework.h>
#include <fossil/io/framework.h>

#ifdef __cplusplus
extern "C" {
#endif

// ========================================================
// File and Directory Commands
// ========================================================

/**
 * Display files and directories with various formatting options
 * @param path The file or directory path to display
 * @param show_all Show hidden files and directories
 * @param long_format Use detailed long format listing
 * @param human_readable Show file sizes in human readable format
 * @param recursive Recursively list subdirectories
 * @param format Output format specification
 * @param show_time Display timestamps
 * @param depth Current depth in recursive listing
 * @return 0 on success, non-zero on error
 */
int fossil_shark_show(const char *path, bool show_all, bool long_format,
                      bool human_readable, bool recursive,
                      const char *format, bool show_time, int depth);

/**
 * Move or rename files and directories
 * @param src Source path of the file or directory
 * @param dest Destination path
 * @param force Force move without confirmation
 * @param interactive Prompt before overwriting
 * @param backup Create backup of existing destination
 * @return 0 on success, non-zero on error
 */
int fossil_shark_move(const char *src, const char *dest,
                      bool force, bool interactive, bool backup);

/**
 * Copy files or directories with various options
 * @param src Source path to copy from
 * @param dest Destination path to copy to
 * @param recursive Copy directories recursively
 * @param update Copy only when source is newer
 * @param preserve Preserve file attributes and permissions
 * @return 0 on success, non-zero on error
 */
int fossil_shark_copy(const char *src, const char *dest,
                      bool recursive, bool update, bool preserve);

/**
 * Remove or delete files and directories safely
 * @param path Path of file or directory to remove
 * @param recursive Remove directories and their contents recursively
 * @param force Force removal without prompts
 * @param interactive Prompt before each removal
 * @param use_trash Move to trash instead of permanent deletion
 * @return 0 on success, non-zero on error
 */
int fossil_shark_remove(const char *path, bool recursive, bool force,
                        bool interactive, bool use_trash);

/**
 * Rename files or directories with safety checks
 * @param old_name Current name of the file or directory
 * @param new_name New name to assign
 * @param force Force rename without confirmation
 * @param interactive Prompt before overwriting existing files
 * @return 0 on success, non-zero on error
 */
int fossil_shark_rename(const char *old_name, const char *new_name,
                        bool force, bool interactive);

/**
 * Create new directories or files with specified options
 * @param path Path where to create the new item
 * @param create_parents Create parent directories if they don't exist
 * @param type Type of item to create ("file" or "dir")
 * @return 0 on success, non-zero on error
 */
int fossil_shark_create(const char *path, bool create_parents,
                        const char *type);  // "file" or "dir"

/**
 * Search for files by name patterns or content matching
 * @param path Root path to start searching from
 * @param recursive Search subdirectories recursively
 * @param name_pattern Pattern to match against file names
 * @param content_pattern Pattern to search within file contents
 * @param ignore_case Perform case-insensitive matching
 * @return 0 on success, non-zero on error
 */
int fossil_shark_search(const char *path, bool recursive,
                        const char *name_pattern, const char *content_pattern,
                        bool ignore_case);

/**
 * Perform archive operations (create, extract, list)
 * @param path Path to archive file or directory to archive
 * @param create Create new archive
 * @param extract Extract existing archive
 * @param list List contents of archive
 * @param format Archive format specification
 * @param password Password for encrypted archives
 * @return 0 on success, non-zero on error
 */
int fossil_shark_archive(const char *path, bool create, bool extract,
                         bool list, const char *format, const char *password);

/**
 * View and display file contents with various formatting options
 * @param path Path to file to view
 * @param number_lines Show line numbers for all lines
 * @param number_non_blank Show line numbers only for non-blank lines
 * @param squeeze_blank Reduce multiple blank lines to single blank line
 * @param head_lines Number of lines to show from beginning (0 for all)
 * @param tail_lines Number of lines to show from end (0 for all)
 * @param show_time Display file timestamps
 * @return 0 on success, non-zero on error
 */
int fossil_shark_view(const char *path, bool number_lines,
                      bool number_non_blank, bool squeeze_blank,
                      int head_lines, int tail_lines, bool show_time);

/**
 * Compare two files or directories and show differences
 * @param path1 First file or directory to compare
 * @param path2 Second file or directory to compare
 * @param text_diff Perform text-based difference comparison
 * @param binary_diff Perform binary difference comparison
 * @param context_lines Number of context lines to show around differences
 * @param ignore_case Ignore case differences in text comparison
 * @return 0 on success, non-zero on error
 */
int fossil_shark_compare(const char *path1, const char *path2,
                         bool text_diff, bool binary_diff,
                         int context_lines, bool ignore_case);

/**
 * Display help information and usage examples
 * @param show_examples Include usage examples in help output
 * @param full_manual Show complete manual instead of brief help
 * @return 0 on success, non-zero on error
 */
int fossil_shark_help(bool show_examples, bool full_manual);


// ========================================================
// AI Commands
// ========================================================

/**
 * Start an interactive AI chat session with context management
 * @param file_path Path to file for AI to analyze (optional)
 * @param model_name Name of AI model to use for chat
 * @param system_role System role/persona for the AI assistant
 * @param save_path Path to save chat conversation history
 * @param keep_context Maintain conversation context across interactions
 * @return 0 on success, non-zero on error
 */
int fossil_shark_chat(const char *file_path, const char *model_name,
                      const char *system_role, const char *save_path,
                      bool keep_context);

/**
 * Ask AI a single question about a file or general topic
 * @param file_path Path to file for AI to analyze
 * @param explain Request detailed explanation from AI
 * @param analyze Request thorough analysis from AI
 * @param quiet Suppress verbose output, show only essential information
 * @return 0 on success, non-zero on error
 */
int fossil_shark_ask(const char *file_path, bool explain,
                     bool analyze, bool quiet);

/**
 * Generate a concise AI-powered summary of file contents
 * @param file_path Path to file to summarize
 * @param depth Level of detail in summary (1-10 scale)
 * @param quiet Suppress additional output, show only summary
 * @param color Use colored output for better readability
 * @param show_time Display timestamp information
 * @return 0 on success, non-zero on error
 */
int fossil_shark_summery(const char *file_path, int depth,
                         bool quiet, bool color, bool show_time);

#ifdef __cplusplus
}
#endif

#endif /* FOSSIL_APP_CODE_H */
