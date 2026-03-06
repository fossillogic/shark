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
int fossil_shark_show(ccstring path, bool show_all, bool long_format,
                      bool human_readable, bool recursive,
                      ccstring format, bool show_time, int depth);

/**
 * Move or rename files and directories
 * @param src Source path of the file or directory
 * @param dest Destination path
 * @param force Force move without confirmation (--force)
 * @param interactive Prompt before overwriting (--interactive)
 * @param backup Create backup before move (--backup)
 * @param atomic Perform atomic move operation (--atomic)
 * @param progress Show progress during move (--progress)
 * @param dry_run Simulate the move without executing (--dry-run)
 * @param exclude_pattern Pattern for files to exclude (--exclude)
 * @param include_pattern Pattern for files to include (--include)
 * @return 0 on success, non-zero on error
 */
int fossil_shark_move(ccstring src, ccstring dest,
                      bool force, bool interactive, bool backup,
                      bool atomic, bool progress, bool dry_run,
                      ccstring exclude_pattern, ccstring include_pattern);

/**
 * Copy files or directories with various options
 * @param src Source path to copy from
 * @param dest Destination path to copy to
 * @param recursive Copy directories recursively (--recursive)
 * @param update Copy only when source is newer (--update)
 * @param preserve Preserve file attributes and permissions (--preserve)
 * @param checksum Verify integrity after copy (--checksum)
 * @param sparse Preserve sparse files (--sparse)
 * @param link Create hardlinks instead of copies (--link)
 * @param reflink Use copy-on-write if available (--reflink)
 * @param progress Show progress during copy (--progress)
 * @param dry_run Simulate the copy without executing (--dry-run)
 * @param exclude_pattern Pattern for files to exclude (--exclude)
 * @param include_pattern Pattern for files to include (--include)
 * @return 0 on success, non-zero on error
 */
int fossil_shark_copy(ccstring src, ccstring dest,
                      bool recursive, bool update, bool preserve,
                      bool checksum, bool sparse, bool link, bool reflink,
                      bool progress, bool dry_run,
                      ccstring exclude_pattern, ccstring include_pattern);

/**
 * Remove or delete files and directories safely
 * @param path Path of file or directory to remove
 * @param recursive Remove directories and their contents recursively
 * @param force Force removal without prompts
 * @param interactive Prompt before each removal
 * @param use_trash Move to trash instead of permanent deletion
 * @param wipe Securely overwrite files before deletion
 * @param shred_passes Number of passes for multi-pass secure deletion (0 for none)
 * @param older_than Delete only files older than specified time (NULL for no filter)
 * @param larger_than Delete only files larger than specified size (0 for no filter)
 * @param empty_only Delete only empty directories
 * @param log_file Write deletion log to specified file (NULL for no logging)
 * @return 0 on success, non-zero on error
 */
int fossil_shark_remove(ccstring path, bool recursive, bool force,
                        bool interactive, bool use_trash, bool wipe,
                        int shred_passes, ccstring older_than,
                        size_t larger_than, bool empty_only,
                        ccstring log_file);

/**
 * Rename files or directories with safety checks
 * @param old_name Current name of the file or directory
 * @param new_name New name to assign
 * @param force Force rename without confirmation
 * @param interactive Prompt before overwriting existing files
 * @return 0 on success, non-zero on error
 */
int fossil_shark_rename(ccstring old_name, ccstring new_name,
                        bool force, bool interactive);

/**
 * Create new directories or files with specified options
 * @param path Path where to create the new item
 * @param create_parents Create parent directories if they don't exist
 * @param type Type of item to create ("file" or "dir")
 * @return 0 on success, non-zero on error
 */
int fossil_shark_create(ccstring path, bool create_parents,
                        ccstring type);  // "file" or "dir"

/**
 * Search for files by name patterns or content matching
 * @param path Root path to start searching from
 * @param recursive Search subdirectories recursively
 * @param name_pattern Pattern to match against file names
 * @param content_pattern Pattern to search within file contents
 * @param ignore_case Perform case-insensitive matching
 * @return 0 on success, non-zero on error
 */
int fossil_shark_search(ccstring path, bool recursive,
                        ccstring name_pattern, ccstring content_pattern,
                        bool ignore_case);

/**
 * Perform archive operations (create, extract, list)
 * @param path Path to archive file or directory to archive
 * @param create Create new archive
 * @param extract Extract existing archive
 * @param list List contents of archive
 * @param format Archive format specification (zip/tar/gz)
 * @param password Password for encrypted archives
 * @param compress_level Compression level (0-9, 0 for no compression)
 * @param stdout_output Output to stdout instead of file
 * @param exclude_pattern Pattern for files to exclude
 * @return 0 on success, non-zero on error
 */
int fossil_shark_archive(ccstring path, bool create, bool extract,
                         bool list, ccstring format, ccstring password,
                         int compress_level, bool stdout_output,
                         ccstring exclude_pattern);

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
int fossil_shark_compare(ccstring path1, ccstring path2,
                         bool text_diff, bool binary_diff,
                         int context_lines, bool ignore_case);

/**
 * Display help information and usage examples
 * @param command Command name to show help for
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
int fossil_shark_sync(ccstring src, ccstring dest,
                      bool recursive, bool update, bool delete);

/**
 * Continuously monitor files or directories for changes
 * @param path Path to monitor
 * @param recursive Monitor subdirectories
 * @param events List of events to filter ("create", "modify", "delete")
 * @param interval Poll interval in seconds
 * @return 0 on success, non-zero on error
 */
int fossil_shark_watch(ccstring path, bool recursive,
                       ccstring events, int interval);

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
int fossil_shark_rewrite(ccstring path, bool in_place, bool append,
                         ccstring new_content, size_t size,
                         bool update_access_time, bool update_mod_time);

/**
 * Examine file contents, type, or metadata
 * @param path Path to the file to introspect
 * @param show_head_lines Number of lines from the start to display (0 for none)
 * @param show_tail_lines Number of lines from the end to display (0 for none)
 * @param count_lines_words_bytes Count lines, words, and bytes if true
 * @param count_lines_only Count total lines only if true
 * @param show_size Display file size in bytes and human-readable format
 * @param show_time Display file timestamps (modified, created, accessed)
 * @param show_file_type Determine file type (e.g., MIME or description)
 * @param find_pattern Search for string or pattern in file
 * @param output_fson Output metadata in FSON structured format
 * @param output_json Output metadata in JSON structured format
 * @return 0 on success, non-zero on error
 */
int fossil_shark_introspect(ccstring path, int show_head_lines,
                             int show_tail_lines, bool count_lines_words_bytes,
                             bool count_lines_only, bool show_size,
                             bool show_time, bool show_file_type,
                             ccstring find_pattern, bool output_fson, bool output_json);

/**
 * Analyze, correct, and assess grammar, style, and readability of text files.
 * Supports grammar/style analysis, correction, tone/style detection, sanitization,
 * suggestions, summarization, scoring, trait detection, reflow, capitalization,
 * formatting, decluttering, and punctuation normalization.
 * @param file_path Path to file or text to analyze
 * @param check Analyze grammar and style (--check)
 * @param correct Output corrected text (--correct)
 * @param tone Detect tone and style (--tone)
 * @param sanitize Remove unsafe or low-quality language (--sanitize)
 * @param suggest Suggest improvements (--suggest)
 * @param summarize Summarize content (--summarize)
 * @param score Show readability/clarity/quality scores (--score)
 * @param detect_type Detect specific traits (e.g., spam, clickbait, passive) (--detect <type>)
 * @param reflow_width Reflow text to width n (--reflow <n>), 0 for no reflow
 * @param capitalize_mode Capitalize sentences or titles (--capitalize <mode>), NULL for none
 * @param format Pretty-print text (--format)
 * @param declutter Repair word boundaries and whitespace (--declutter)
 * @param punctuate Normalize punctuation (--punctuate)
 * @return 0 on success, non-zero on error
 */
int fossil_shark_grammar(ccstring file_path,
                         bool check,
                         bool correct,
                         bool tone,
                         bool sanitize,
                         bool suggest,
                         bool summarize,
                         bool score,
                         ccstring detect_type,
                         int reflow_width,
                         ccstring capitalize_mode,
                         bool format,
                         bool declutter,
                         bool punctuate);

/**
 * Encode or decode text using various ciphers
 * @param text Text to encode or decode
 * @param encode Encode the text
 * @param decode Decode the text
 * @param cipher Cipher type to use ("caesar", "vigenere", "base64", "base32", "binary", "morse", "baconian", "railfence", "haxor", "leet", "rot13", "atbash")
 * @return 0 on success, non-zero on error
 */
int fossil_shark_cryptic(ccstring text, bool encode, bool decode,
                         ccstring cipher);

#ifdef __cplusplus
}
#endif

#endif /* FOSSIL_APP_CODE_H */
