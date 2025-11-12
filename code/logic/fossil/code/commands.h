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

/**
 * @brief Command Palette - Comprehensive Command-Line Interface for Shark Tool
 * 
 * This header defines the complete command palette for the Shark Tool, providing
 * a comprehensive set of file operations, AI-powered commands, and utility functions.
 * The command palette is organized into three main categories:
 * 
 * ## 🗂 Core File Operations
 * 
 * ### show - Display files and directories
 * Display files and directories with various formatting options.
 * Common flags:
 * - `-a, --all` Show hidden files
 * - `-l, --long` Detailed info
 * - `-h, --human` Human-readable sizes
 * - `-r, --recursive` Include subdirs
 * - `-d, --depth <n>` Limit recursion depth
 * - `--as format` options "list", tree, graph
 * - `--time` Show timestamps
 * 
 * ### move - Move or rename files/directories
 * Move or rename files and directories with safety options.
 * Common flags:
 * - `-f, --force` Overwrite without prompt
 * - `-i, --interactive` Ask before overwrite
 * - `-b, --backup` Backup before move
 * 
 * ### copy - Copy files or directories
 * Copy files or directories with preservation options.
 * Common flags:
 * - `-r, --recursive` Copy subdirectories
 * - `-u, --update` Only copy newer
 * - `-p, --preserve` Keep permissions/timestamps
 * 
 * ### remove/delete - Delete files or directories
 * Delete files or directories with safety measures.
 * Common flags:
 * - `-r, --recursive` Delete contents
 * - `-f, --force` No confirmation
 * - `-i, --interactive` Confirm per file
 * - `--trash` Move to system trash
 * 
 * ### rename - Rename files or directories
 * Rename files or directories with collision handling.
 * Common flags:
 * - `-f, --force` Overwrite target
 * - `-i, --interactive` Confirm before overwrite
 * 
 * ### create - Create new directories
 * Create new directories with parent creation support.
 * Common flags:
 * - `-p, --parents` Create parent dirs
 * - `-t, --type` Creates either file or dir
 * 
 * ### search - Find files by name or content
 * Find files by name or content with pattern matching.
 * Common flags:
 * - `-r, --recursive` Include subdirs
 * - `-n, --name` Match filename
 * - `-c, --content` Search in file contents
 * - `-i, --ignore-case` Case-insensitive
 * 
 * ### archive - Create, extract, or list archives
 * Archive management with multiple format support.
 * Common flags:
 * - `-c, --create` New archive
 * - `-x, --extract` Extract contents
 * - `-l, --list` List archive
 * - `-f <format>` Format: zip/tar/gz
 * - `-p, --password` Encrypt archive
 * 
 * ### view - Output file contents to terminal
 * Display file contents with formatting options.
 * Common flags:
 * - `-n, --number` Number all lines
 * - `-b, --non-blank` Number non-empty lines
 * - `-s, --squeeze` Remove blank lines
 * - `-h, --head <n>` First n lines
 * - `-t, --tail <n>` Last n lines
 * - `--time` Show timestamps
 * 
 * ### compare - Compare two files/directories
 * Compare files or directories (text or binary).
 * Common flags:
 * - `-t, --text` Line diff
 * - `-b, --binary` Binary diff
 * - `--context <n>` Show context lines
 * - `--ignore-case` Ignore case differences
 * 
 * ### help - Display help for supported commands
 * Display comprehensive help documentation.
 * Common flags:
 * - `--examples` Show usage examples
 * - `--man` Full manual
 * 
 * ## 🤖 AI Commands (Jellyfish Integration)
 * 
 * ### chat - Start an interactive AI chat session
 * Start an interactive AI chat session with context support.
 * Common flags:
 * - `-f, --file <path>` Use file content
 * - `-m, --model <name>` Select model
 * - `-s, --system <role>` AI persona
 * - `--save <path>` Save chat transcript
 * - `--context` Keep session history
 * 
 * ### ask - Ask Jellyfish AI a one-shot question
 * Ask Jellyfish AI a single question with file context.
 * Common flags:
 * - `-f, --file <path>` Provide file context
 * - `--explain` Force explanation
 * - `--analyze` Deep analysis
 * - `-q, --quiet` Minimal output
 * 
 * ### summery - Generate a concise AI summary
 * Generate AI summaries of files or directories.
 * Common flags:
 * - `-f, --file <path>` Use file content
 * - `--depth <level>` Summary depth
 * - `-q, --quiet` Minimal output
 * - `--color` Highlight key items
 * - `--time` Include timestamps
 * 
 * ## 🌍 Global Flags (Available to All Commands)
 * 
 * These flags are available across all commands for consistent behavior:
 * - `--help` Show command help
 * - `--version` Display Shark Tool version
 * - `-v, --verbose` Enable detailed output
 * - `-q, --quiet` Suppress standard output
 * - `--dry-run` Simulate actions without changes
 * - `--color` Colorize output where applicable
 * - `--time` Display timestamps in output
 * 
 * @note This command palette provides a unified interface for file operations,
 *       AI-powered assistance, and system utilities, making the Shark Tool
 *       a comprehensive command-line solution.
 * 
 * @see For implementation details, refer to the corresponding source files
 *      in the commands/ directory.
 */
//

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
