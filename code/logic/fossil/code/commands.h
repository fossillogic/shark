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
#ifndef FOSSIL_APP_COMMANDS_H
#define FOSSIL_APP_COMMANDS_H

#include "common.h"

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
int fossil_shark_help(ccstring command, bool show_examples, bool full_manual);

/**
 * Synchronize files or directories between source and destination
 * @param src Source path
 * @param dest Destination path
 * @param recursive Include subdirectories
 * @param update Copy only newer files
 * @param delete Remove extraneous files from target
 * @return 0 on success, non-zero on error
 */
int fossil_shark_sync(const char *src, const char *dest,
                      bool recursive, bool update, bool delete);

/**
 * Continuously monitor files or directories for changes
 * @param path Path to monitor
 * @param recursive Monitor subdirectories
 * @param events List of events to filter ("create", "modify", "delete")
 * @param interval Poll interval in seconds
 * @return 0 on success, non-zero on error
 */
int fossil_shark_watch(const char *path, bool recursive,
                       const char *events, int interval);

/**
 * Modify or update file contents, timestamps, or size
 * @param path Path to the file to modify
 * @param in_place Perform in-place modification (for content changes)
 * @param append Append content instead of overwriting
 * @param new_content Content to write to the file (NULL if only updating timestamps/size)
 * @param size Set exact file size (truncate or extend)
 * @param update_access_time Update file access time
 * @param update_mod_time Update file modification time
 * @return 0 on success, non-zero on error
 */
int fossil_shark_rewrite(const char *path, bool in_place, bool append,
                         const char *new_content, size_t size,
                         bool update_access_time, bool update_mod_time);

/**
 * Examine file contents, type, or metadata
 * @param path Path to the file to introspect
 * @param show_head_lines Number of lines from the start to display (0 for none)
 * @param show_tail_lines Number of lines from the end to display (0 for none)
 * @param count_lines_words_bytes Count lines, words, and bytes if true
 * @param show_file_type Determine file type (e.g., MIME or description)
 * @param output_json Output metadata in JSON format
 * @return 0 on success, non-zero on error
 */
int fossil_shark_introspect(const char *path, int show_head_lines,
                             int show_tail_lines, bool count_lines_words_bytes,
                             bool show_file_type, bool output_json);

/**
 * Perform grammar analysis, correction, sanitization, tone detection, and content-risk scanning.
 * Uses SOAP API for advanced grammar and content analysis.
 * @param file_path Path to file or text to analyze
 * @param check Run grammar check
 * @param fix Auto-correct grammar issues
 * @param sanitize Remove meme/rot-brain language
 * @param suggest Suggest alternative phrasing
 * @param tone Detect tone of content
 * @param detect_type Type of detector to run (e.g., ragebait, clickbait, spam, woke, bot, sarcasm, formal, snowflake, offensive, neutral, hype, quality, political, conspiracy, marketing, technobabble)
 * @return 0 on success, non-zero on error
 */
int fossil_shark_grammar(const char *file_path, bool check, bool fix,
                         bool sanitize, bool suggest, bool tone,
                         const char *detect_type);

/**
 * @brief Generate structured summaries of text/code/log/document files.
 *
 * @param paths Array of file paths.
 * @param count Number of paths.
 * @param max_lines Limit number of lines to analyze (0 = no limit).
 * @param auto_detect Enable automatic file-type inference.
 * @param keywords Extract keyword list.
 * @param topics Perform simple topic clustering.
 * @param stats Include file statistics.
 * @param fson Output structured FSON format.
 * @return int Status code.
 */
int fossil_shark_summary(const char **paths, int count,
                         int max_lines, bool auto_detect,
                         bool keywords, bool topics,
                         bool stats, bool fson);

#ifdef __cplusplus
}
#endif

#endif /* FOSSIL_APP_CODE_H */
