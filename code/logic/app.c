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
#include "fossil/code/app.h"
#include <unistd.h>
#include <limits.h>
#include <dirent.h>
#include <sys/stat.h>

#ifdef _WIN32
#include <io.h>
#define isatty _isatty
#ifndef STDOUT_FILENO
#define STDOUT_FILENO _fileno(stdout)
#endif
#else
#include <unistd.h>
#endif

int FOSSIL_IO_VERBOSE = false; // Verbose output flag

void show_commands(char* app_name) {
    fossil_io_printf("{blue}Usage: {cyan}%s{blue} <command> [options]{reset}\n\n", app_name);

    fossil_io_printf("{blue}🗂 Core File Operations:{reset}\n");
    fossil_io_printf("{cyan}  show             {reset}Display files and directories\n");
    fossil_io_printf("{yellow}                   -a, --all         {reset}{bright_black}Show hidden files{reset}\n");
    fossil_io_printf("{yellow}                   -l, --long        {reset}{bright_black}Detailed info{reset}\n");
    fossil_io_printf("{yellow}                   -h, --human       {reset}{bright_black}Human-readable sizes{reset}\n");
    fossil_io_printf("{yellow}                   -r, --recursive   {reset}{bright_black}Include subdirs{reset}\n");
    fossil_io_printf("{yellow}                   -d, --depth <n>   {reset}{bright_black}Limit recursion depth{reset}\n");
    fossil_io_printf("{yellow}                   --as <format>     {reset}{bright_black}Format: list, tree, graph{reset}\n");
    fossil_io_printf("{yellow}                   --time            {reset}{bright_black}Show timestamps{reset}\n");

    fossil_io_printf("{cyan}  move             {reset}Move or rename files/directories\n");
    fossil_io_printf("{yellow}                   -f, --force       {reset}{bright_black}Overwrite without prompt{reset}\n");
    fossil_io_printf("{yellow}                   -i, --interactive {reset}{bright_black}Ask before overwrite{reset}\n");
    fossil_io_printf("{yellow}                   -b, --backup      {reset}{bright_black}Backup before move{reset}\n");

    fossil_io_printf("{cyan}  copy             {reset}Copy files or directories\n");
    fossil_io_printf("{yellow}                   -r, --recursive   {reset}{bright_black}Copy subdirectories{reset}\n");
    fossil_io_printf("{yellow}                   -u, --update      {reset}{bright_black}Only copy newer{reset}\n");
    fossil_io_printf("{yellow}                   -p, --preserve    {reset}{bright_black}Keep permissions/timestamps{reset}\n");

    fossil_io_printf("{cyan}  remove/delete    {reset}Delete files or directories\n");
    fossil_io_printf("{yellow}                   -r, --recursive   {reset}{bright_black}Delete contents{reset}\n");
    fossil_io_printf("{yellow}                   -f, --force       {reset}{bright_black}No confirmation{reset}\n");
    fossil_io_printf("{yellow}                   -i, --interactive {reset}{bright_black}Confirm per file{reset}\n");
    fossil_io_printf("{yellow}                   --trash           {reset}{bright_black}Move to system trash{reset}\n");

    fossil_io_printf("{cyan}  rename           {reset}Rename files or directories\n");
    fossil_io_printf("{yellow}                   -f, --force       {reset}{bright_black}Overwrite target{reset}\n");
    fossil_io_printf("{yellow}                   -i, --interactive {reset}{bright_black}Confirm before overwrite{reset}\n");

    fossil_io_printf("{cyan}  create           {reset}Create new directories or files\n");
    fossil_io_printf("{yellow}                   -p, --parents     {reset}{bright_black}Create parent dirs{reset}\n");
    fossil_io_printf("{yellow}                   -t, --type        {reset}{bright_black}Creates either file or dir{reset}\n");

    fossil_io_printf("{cyan}  search           {reset}Find files by name or content\n");
    fossil_io_printf("{yellow}                   -r, --recursive   {reset}{bright_black}Include subdirs{reset}\n");
    fossil_io_printf("{yellow}                   -n, --name        {reset}{bright_black}Match filename{reset}\n");
    fossil_io_printf("{yellow}                   -c, --content     {reset}{bright_black}Search in file contents{reset}\n");
    fossil_io_printf("{yellow}                   -i, --ignore-case {reset}{bright_black}Case-insensitive{reset}\n");

    fossil_io_printf("{cyan}  archive          {reset}Create, extract, or list archives\n");
    fossil_io_printf("{yellow}                   -c, --create      {reset}{bright_black}New archive{reset}\n");
    fossil_io_printf("{yellow}                   -x, --extract     {reset}{bright_black}Extract contents{reset}\n");
    fossil_io_printf("{yellow}                   -l, --list        {reset}{bright_black}List archive{reset}\n");
    fossil_io_printf("{yellow}                   -f <format>       {reset}{bright_black}Format: zip/tar/gz{reset}\n");
    fossil_io_printf("{yellow}                   -p, --password    {reset}{bright_black}Encrypt archive{reset}\n");

    fossil_io_printf("{cyan}  view             {reset}Output file contents to terminal\n");
    fossil_io_printf("{yellow}                   -n, --number      {reset}{bright_black}Number all lines{reset}\n");
    fossil_io_printf("{yellow}                   -b, --non-blank   {reset}{bright_black}Number non-empty lines{reset}\n");
    fossil_io_printf("{yellow}                   -s, --squeeze     {reset}{bright_black}Remove blank lines{reset}\n");
    fossil_io_printf("{yellow}                   -h, --head <n>    {reset}{bright_black}First n lines{reset}\n");
    fossil_io_printf("{yellow}                   -t, --tail <n>    {reset}{bright_black}Last n lines{reset}\n");
    fossil_io_printf("{yellow}                   --time            {reset}{bright_black}Show timestamps{reset}\n");

    fossil_io_printf("{cyan}  compare          {reset}Compare two files/directories\n");
    fossil_io_printf("{yellow}                   -t, --text        {reset}{bright_black}Line diff{reset}\n");
    fossil_io_printf("{yellow}                   -b, --binary      {reset}{bright_black}Binary diff{reset}\n");
    fossil_io_printf("{yellow}                   --context <n>     {reset}{bright_black}Show context lines{reset}\n");
    fossil_io_printf("{yellow}                   --ignore-case     {reset}{bright_black}Ignore case differences{reset}\n");

    fossil_io_printf("{cyan}  help             {reset}Display help for supported commands\n");
    fossil_io_printf("{yellow}                   --examples        {reset}{bright_black}Show usage examples{reset}\n");
    fossil_io_printf("{yellow}                   --man             {reset}{bright_black}Full manual{reset}\n\n");

    fossil_io_printf("{cyan}  rewrite          {reset}Modify or update file contents, timestamps, or size\n");
    fossil_io_printf("{yellow}                   --in-place        {reset}{bright_black}Modify file directly (default){reset}\n");
    fossil_io_printf("{yellow}                   -a, --append      {reset}{bright_black}Append content instead of overwriting{reset}\n");
    fossil_io_printf("{yellow}                   --size <n>        {reset}{bright_black}Truncate or extend file to exact size{reset}\n");
    fossil_io_printf("{yellow}                   --access-time     {reset}{bright_black}Update file access time{reset}\n");
    fossil_io_printf("{yellow}                   --mod-time        {reset}{bright_black}Update file modification time{reset}\n");
    fossil_io_printf("{yellow}                   <content>         {reset}{bright_black}Optional content to write{reset}\n");

    fossil_io_printf("{cyan}  introspect       {reset}Examine file contents, type, or metadata\n");
    fossil_io_printf("{yellow}                   --head <n>        {reset}{bright_black}Show first n lines{reset}\n");
    fossil_io_printf("{yellow}                   --tail <n>        {reset}{bright_black}Show last n lines{reset}\n");
    fossil_io_printf("{yellow}                   --count           {reset}{bright_black}Count lines, words, bytes{reset}\n");
    fossil_io_printf("{yellow}                   --type            {reset}{bright_black}Show file type or MIME{reset}\n");
    fossil_io_printf("{yellow}                   --fson            {reset}{bright_black}Output metadata in FSON format{reset}\n");

    fossil_io_printf("{cyan}  grammar          {reset}Perform grammar analysis and correction via SOAP API\n");
    fossil_io_printf("{yellow}                   --check           {reset}{bright_black}Run grammar check{reset}\n");
    fossil_io_printf("{yellow}                   --fix             {reset}{bright_black}Auto-correct grammar{reset}\n");
    fossil_io_printf("{yellow}                   --sanitize        {reset}{bright_black}Remove rot-brain/meme language{reset}\n");
    fossil_io_printf("{yellow}                   --suggest         {reset}{bright_black}Suggest alternatives{reset}\n");
    fossil_io_printf("{yellow}                   --tone            {reset}{bright_black}Detect tone{reset}\n");
    fossil_io_printf("{yellow}                   --detect <type>   {reset}{bright_black}Run detectors: ragebait, clickbait, spam, woke, bot, sarcasm, formal, snowflake, offensive, neutral, hype, quality, political, conspiracy, marketing, technobabble{reset}\n");

    fossil_io_printf("{cyan}  summary          {reset}Summarize files, logs, code, or documents\n");
    fossil_io_printf("{yellow}                   -f, --file <path>  {reset}{bright_black}File to summarize{reset}\n");
    fossil_io_printf("{yellow}                   -l, --lines <n>    {reset}{bright_black}Limit number of lines analyzed{reset}\n");
    fossil_io_printf("{yellow}                   --auto             {reset}{bright_black}Auto-detect file type{reset}\n");
    fossil_io_printf("{yellow}                   --keywords         {reset}{bright_black}Extract keywords from file{reset}\n");
    fossil_io_printf("{yellow}                   --topics           {reset}{bright_black}Perform simple topic clustering{reset}\n");
    fossil_io_printf("{yellow}                   --stats            {reset}{bright_black}Show file statistics (lines, chars, entropy){reset}\n");
    fossil_io_printf("{yellow}                   --fson             {reset}{bright_black}Output structured FSON summary{reset}\n\n");
    
    fossil_io_printf("{cyan}  sync             {reset}Synchronize files/directories\n");
    fossil_io_printf("{yellow}                   -r, --recursive   {reset}{bright_black}Include subdirs{reset}\n");
    fossil_io_printf("{yellow}                   -u, --update      {reset}{bright_black}Copy only newer{reset}\n");
    fossil_io_printf("{yellow}                   --delete          {reset}{bright_black}Remove extraneous files from target{reset}\n");

    fossil_io_printf("{cyan}  watch            {reset}Continuously monitor files or directories\n");
    fossil_io_printf("{yellow}                   -r, --recursive   {reset}{bright_black}Include subdirs{reset}\n");
    fossil_io_printf("{yellow}                   -e, --events <list> {reset}{bright_black}Filter events: create/modify/delete{reset}\n");
    fossil_io_printf("{yellow}                   -t, --interval <n> {reset}{bright_black}Poll interval in seconds{reset}\n");

    fossil_io_printf("{blue}🌍 Global Options:{reset}\n");
    fossil_io_printf("{cyan}  --help           {reset}{bright_black}Display help information{reset}\n");
    fossil_io_printf("{cyan}  --version        {reset}{bright_black}Display current version{reset}\n");
    fossil_io_printf("{cyan}  --name           {reset}{bright_black}Display app name{reset}\n");
    fossil_io_printf("{cyan}  --verbose        {reset}{bright_black}Enable detailed output{reset}\n");
    fossil_io_printf("{cyan}  --color          {reset}{bright_black}Set color mode: enable, disable, auto{reset}\n");
    fossil_io_printf("{cyan}  --clear          {reset}{bright_black}Clear terminal screen{reset}\n");

    exit(FOSSIL_IO_SUCCESS);
}

void show_version(void) {
    fossil_io_printf("{blue}%s version %s{reset}\n", FOSSIL_APP_NAME, FOSSIL_APP_VERSION);
    exit(FOSSIL_IO_SUCCESS);
}

void show_name(void) {
    fossil_io_printf("{blue}App Name: {cyan}%s{reset}\n", FOSSIL_APP_NAME);
    exit(FOSSIL_IO_SUCCESS);
}


bool app_entry(int argc, char** argv) {
    // List of supported commands for suggestion
    static ccstring supported_commands[] = {
        "show", "move", "copy", "remove", "delete", "rename", "create", "search",
        "archive", "view", "compare", "help", "summary", "sync",
        "watch", "rewrite", "introspect", "grammar",
        "--help", "--version", "--name", "--verbose", "--color", "--clear"
    };
    const int num_supported = sizeof(supported_commands) / sizeof(supported_commands[0]);

    for (i32 i = 1; i < argc; ++i) {
        if (argv[i] == cnullptr) continue;

        // Try TI-aware suggestion for unknown commands
        bool handled = false;
        fossil_ti_reason_t ti_reason = {0};
        for (int k = 0; k < num_supported; ++k) {
            if (fossil_io_cstring_compare(argv[i], supported_commands[k]) == 0) {
                handled = true;
                break;
            }
        }

        if (!handled && argv[i][0] != '-') {
            ccstring suggested = fossil_it_magic_suggest_command(argv[i], supported_commands, num_supported, &ti_reason);
            if (cnotnull(suggested)) {
                int jaccard = fossil_it_magic_jaccard_index(argv[i], suggested);
                int edit_dist = fossil_it_magic_levenshtein_distance(argv[i], suggested);
                float similarity = fossil_it_magic_similarity(argv[i], suggested);
                fossil_io_printf(
                    "{yellow}Did you mean: {cyan}%s{yellow}?{reset}\n"
                    "  {bright_black}TI Reasoning:{reset}\n"
                    "    Edit Distance: %d\n"
                    "    Similarity Score: %.2f\n"
                    "    Jaccard Index: %d\n"
                    "    Prefix Match: %d\n"
                    "    Suffix Match: %d\n"
                    "    Case-Insensitive: %d\n"
                    "    Reason: %s\n",
                    suggested,
                    edit_dist,
                    similarity,
                    jaccard,
                    ti_reason.prefix_match,
                    ti_reason.suffix_match,
                    ti_reason.case_insensitive,
                    ti_reason.reason
                );
            } else {
                fossil_io_printf(
                    "{red}Unknown command: %s{reset}\n", argv[i]
                );
            }
            return false;
        }

        // Path suggestion for file/directory arguments
        // Only apply to commands that take a path argument
        if (
            fossil_io_cstring_compare(argv[i], "show") == 0 ||
            fossil_io_cstring_compare(argv[i], "move") == 0 ||
            fossil_io_cstring_compare(argv[i], "copy") == 0 ||
            fossil_io_cstring_compare(argv[i], "remove") == 0 ||
            fossil_io_cstring_compare(argv[i], "delete") == 0 ||
            fossil_io_cstring_compare(argv[i], "rename") == 0 ||
            fossil_io_cstring_compare(argv[i], "create") == 0 ||
            fossil_io_cstring_compare(argv[i], "search") == 0 ||
            fossil_io_cstring_compare(argv[i], "archive") == 0 ||
            fossil_io_cstring_compare(argv[i], "view") == 0 ||
            fossil_io_cstring_compare(argv[i], "compare") == 0 ||
            fossil_io_cstring_compare(argv[i], "sync") == 0 ||
            fossil_io_cstring_compare(argv[i], "watch") == 0 ||
            fossil_io_cstring_compare(argv[i], "rewrite") == 0 ||
            fossil_io_cstring_compare(argv[i], "introspect") == 0 ||
            fossil_io_cstring_compare(argv[i], "grammar") == 0
        ) {
            // Look ahead for path-like arguments and suggest corrections
            for (int j = i + 1; j < argc; ++j) {
                if (argv[j] && argv[j][0] != '-') {
                    fossil_ti_path_suggestion_set_t path_suggestions = {0};
                    fossil_it_magic_path_suggest(argv[j], ".", &path_suggestions);
                    if (path_suggestions.count > 0) {
                        float sim = fossil_it_magic_similarity(argv[j], path_suggestions.list[0].candidate_path);
                        if (sim > 0.7f && !path_suggestions.list[0].exists) {
                            int jaccard = fossil_it_magic_jaccard_index(argv[j], path_suggestions.list[0].candidate_path);
                            int edit_dist = fossil_it_magic_levenshtein_distance(argv[j], path_suggestions.list[0].candidate_path);
                            fossil_io_printf(
                                "{yellow}Path suggestion for '{red}%s{yellow}':{reset}\n"
                                "  {cyan}%s{reset} (score: %.2f, edit: %d, jaccard: %d)\n",
                                argv[j],
                                path_suggestions.list[0].candidate_path,
                                sim,
                                edit_dist,
                                jaccard
                            );
                        }
                    }
                }
                // Only check first non-flag argument after command
                break;
            }
        }

        if (fossil_io_cstring_compare(argv[i], "--help") == 0) {
            show_commands(argv[0]);
        } else if (fossil_io_cstring_compare(argv[i], "--version") == 0) {
            show_version();
        } else if (fossil_io_cstring_compare(argv[i], "--name") == 0) {
            show_name();
        } else if (fossil_io_cstring_compare(argv[i], "--verbose") == 0) {
            FOSSIL_IO_VERBOSE = true;
            fossil_io_printf("{blue}Verbose mode enabled{reset}\n");
        } else if (fossil_io_cstring_compare(argv[i], "--color") == 0) {
            if (i + 1 < argc && argv[i + 1] != cnullptr) {
                if (fossil_io_cstring_compare(argv[i + 1], "enable") == 0) {
                    FOSSIL_IO_COLOR_ENABLE = true;
                } else if (fossil_io_cstring_compare(argv[i + 1], "disable") == 0) {
                    FOSSIL_IO_COLOR_ENABLE = false;
                } else if (fossil_io_cstring_compare(argv[i + 1], "auto") == 0) {
                    FOSSIL_IO_COLOR_ENABLE = isatty(STDOUT_FILENO);
                }
                ++i; // Skip next argument
            }
        } else if (fossil_io_cstring_compare(argv[i], "--clear") == 0) {
            fossil_io_printf("\033[H\033[J"); // ANSI escape sequence to clear screen
        }
        // File Operations Commands
        else if (fossil_io_cstring_compare(argv[i], "show") == 0) {
            ccstring path = ".";
            bool show_all = false, long_format = false, human_readable = false;
            bool recursive = false, show_time = false;
            ccstring format = "list";
            int depth = -1;
            
            // Parse flags
            for (int j = i + 1; j < argc && argv[j][0] == '-'; j++) {
                if (fossil_io_cstring_compare(argv[j], "-a") == 0 || fossil_io_cstring_compare(argv[j], "--all") == 0) {
                    show_all = true;
                } else if (fossil_io_cstring_compare(argv[j], "-l") == 0 || fossil_io_cstring_compare(argv[j], "--long") == 0) {
                    long_format = true;
                } else if (fossil_io_cstring_compare(argv[j], "-h") == 0 || fossil_io_cstring_compare(argv[j], "--human") == 0) {
                    human_readable = true;
                } else if (fossil_io_cstring_compare(argv[j], "-r") == 0 || fossil_io_cstring_compare(argv[j], "--recursive") == 0) {
                    recursive = true;
                } else if (fossil_io_cstring_compare(argv[j], "--time") == 0) {
                    show_time = true;
                } else if (fossil_io_cstring_compare(argv[j], "--as") == 0 && j + 1 < argc) {
                    format = argv[++j];
                } else if (fossil_io_cstring_compare(argv[j], "-d") == 0 || fossil_io_cstring_compare(argv[j], "--depth") == 0) {
                    if (j + 1 < argc) depth = atoi(argv[++j]);
                } else {
                    path = argv[j];
                }
                i = j;
            }
            if (i + 1 < argc && argv[i + 1][0] != '-') path = argv[++i];
            fossil_shark_show(path, show_all, long_format, human_readable, recursive, format, show_time, depth);
            
        } else if (fossil_io_cstring_compare(argv[i], "move") == 0) {
            ccstring src = cnull, dest = cnull;
            bool force = false, interactive = false, backup = false;
            
            for (int j = i + 1; j < argc; j++) {
                if (fossil_io_cstring_compare(argv[j], "-f") == 0 || fossil_io_cstring_compare(argv[j], "--force") == 0) {
                    force = true;
                } else if (fossil_io_cstring_compare(argv[j], "-i") == 0 || fossil_io_cstring_compare(argv[j], "--interactive") == 0) {
                    interactive = true;
                } else if (fossil_io_cstring_compare(argv[j], "-b") == 0 || fossil_io_cstring_compare(argv[j], "--backup") == 0) {
                    backup = true;
                } else if (!cnotnull(src)) {
                    src = argv[j];
                } else if (!cnotnull(dest)) {
                    dest = argv[j];
                }
                i = j;
            }
            if (cnotnull(src) && cnotnull(dest)) fossil_shark_move(src, dest, force, interactive, backup);
            
        } else if (fossil_io_cstring_compare(argv[i], "copy") == 0) {
            ccstring src = cnull, dest = cnull;
            bool recursive = false, update = false, preserve = false;
            
            for (int j = i + 1; j < argc; j++) {
                if (fossil_io_cstring_compare(argv[j], "-r") == 0 || fossil_io_cstring_compare(argv[j], "--recursive") == 0) {
                    recursive = true;
                } else if (fossil_io_cstring_compare(argv[j], "-u") == 0 || fossil_io_cstring_compare(argv[j], "--update") == 0) {
                    update = true;
                } else if (fossil_io_cstring_compare(argv[j], "-p") == 0 || fossil_io_cstring_compare(argv[j], "--preserve") == 0) {
                    preserve = true;
                } else if (!cnotnull(src)) {
                    src = argv[j];
                } else if (!cnotnull(dest)) {
                    dest = argv[j];
                }
                i = j;
            }
            if (cnotnull(src) && cnotnull(dest)) fossil_shark_copy(src, dest, recursive, update, preserve);
            
        } else if (fossil_io_cstring_compare(argv[i], "remove") == 0 || 
               fossil_io_cstring_compare(argv[i], "delete") == 0) {
            ccstring path = cnull;
            bool recursive = false, force = false, interactive = false, use_trash = false;
            
            for (int j = i + 1; j < argc; j++) {
                if (fossil_io_cstring_compare(argv[j], "-r") == 0 || fossil_io_cstring_compare(argv[j], "--recursive") == 0) {
                    recursive = true;
                } else if (fossil_io_cstring_compare(argv[j], "-f") == 0 || fossil_io_cstring_compare(argv[j], "--force") == 0) {
                    force = true;
                } else if (fossil_io_cstring_compare(argv[j], "-i") == 0 || fossil_io_cstring_compare(argv[j], "--interactive") == 0) {
                    interactive = true;
                } else if (fossil_io_cstring_compare(argv[j], "--trash") == 0) {
                    use_trash = true;
                } else if (!cnotnull(path)) {
                    path = argv[j];
                }
                i = j;
            }
            if (cnotnull(path)) fossil_shark_remove(path, recursive, force, interactive, use_trash);
            
        } else if (fossil_io_cstring_compare(argv[i], "rename") == 0) {
            ccstring old_name = cnull, new_name = cnull;
            bool force = false, interactive = false;
            
            for (int j = i + 1; j < argc; j++) {
                if (fossil_io_cstring_compare(argv[j], "-f") == 0 || fossil_io_cstring_compare(argv[j], "--force") == 0) {
                    force = true;
                } else if (fossil_io_cstring_compare(argv[j], "-i") == 0 || fossil_io_cstring_compare(argv[j], "--interactive") == 0) {
                    interactive = true;
                } else if (!cnotnull(old_name)) {
                    old_name = argv[j];
                } else if (!cnotnull(new_name)) {
                    new_name = argv[j];
                }
                i = j;
            }
            if (cnotnull(old_name) && cnotnull(new_name)) fossil_shark_rename(old_name, new_name, force, interactive);
            
        } else if (fossil_io_cstring_compare(argv[i], "create") == 0) {
            ccstring path = cnull, type = "dir";
            bool create_parents = false;
            
            for (int j = i + 1; j < argc; j++) {
                if (fossil_io_cstring_compare(argv[j], "-p") == 0 || fossil_io_cstring_compare(argv[j], "--parents") == 0) {
                    create_parents = true;
                } else if (fossil_io_cstring_compare(argv[j], "-t") == 0 || fossil_io_cstring_compare(argv[j], "--type") == 0) {
                    if (j + 1 < argc) type = argv[++j];
                } else if (!cnotnull(path)) {
                    path = argv[j];
                }
                i = j;
            }
            if (cnotnull(path)) fossil_shark_create(path, create_parents, type);
            
        } else if (fossil_io_cstring_compare(argv[i], "search") == 0) {
            ccstring path = ".", name_pattern = cnull, content_pattern = cnull;
            bool recursive = false, ignore_case = false;
            
            for (int j = i + 1; j < argc; j++) {
                if (fossil_io_cstring_compare(argv[j], "-r") == 0 || fossil_io_cstring_compare(argv[j], "--recursive") == 0) {
                    recursive = true;
                } else if (fossil_io_cstring_compare(argv[j], "-i") == 0 || fossil_io_cstring_compare(argv[j], "--ignore-case") == 0) {
                    ignore_case = true;
                } else if (fossil_io_cstring_compare(argv[j], "-n") == 0 || fossil_io_cstring_compare(argv[j], "--name") == 0) {
                    if (j + 1 < argc) name_pattern = argv[++j];
                } else if (fossil_io_cstring_compare(argv[j], "-c") == 0 || fossil_io_cstring_compare(argv[j], "--content") == 0) {
                    if (j + 1 < argc) content_pattern = argv[++j];
                } else if (argv[j][0] != '-') {
                    path = argv[j];
                }
                i = j;
            }
            fossil_shark_search(path, recursive, name_pattern, content_pattern, ignore_case);
            
        } else if (fossil_io_cstring_compare(argv[i], "archive") == 0) {
            ccstring path = cnull, format = "zip", password = cnull;
            bool create = false, extract = false, list = false;
            
            for (int j = i + 1; j < argc; j++) {
                if (fossil_io_cstring_compare(argv[j], "-c") == 0 || fossil_io_cstring_compare(argv[j], "--create") == 0) {
                    create = true;
                } else if (fossil_io_cstring_compare(argv[j], "-x") == 0 || fossil_io_cstring_compare(argv[j], "--extract") == 0) {
                    extract = true;
                } else if (fossil_io_cstring_compare(argv[j], "-l") == 0 || fossil_io_cstring_compare(argv[j], "--list") == 0) {
                    list = true;
                } else if (fossil_io_cstring_compare(argv[j], "-f") == 0 && j + 1 < argc) {
                    format = argv[++j];
                } else if (fossil_io_cstring_compare(argv[j], "-p") == 0 || fossil_io_cstring_compare(argv[j], "--password") == 0) {
                    if (j + 1 < argc) password = argv[++j];
                } else if (!cnotnull(path)) {
                    path = argv[j];
                }
                i = j;
            }
            if (cnotnull(path)) fossil_shark_archive(path, create, extract, list, format, password);
            
        } else if (fossil_io_cstring_compare(argv[i], "view") == 0) {
            ccstring path = cnull;
            bool number_lines = false, number_non_blank = false, squeeze_blank = false, show_time = false;
            int head_lines = 0, tail_lines = 0;
            
            for (int j = i + 1; j < argc; j++) {
                if (fossil_io_cstring_compare(argv[j], "-n") == 0 || fossil_io_cstring_compare(argv[j], "--number") == 0) {
                    number_lines = true;
                } else if (fossil_io_cstring_compare(argv[j], "-b") == 0 || fossil_io_cstring_compare(argv[j], "--non-blank") == 0) {
                    number_non_blank = true;
                } else if (fossil_io_cstring_compare(argv[j], "-s") == 0 || fossil_io_cstring_compare(argv[j], "--squeeze") == 0) {
                    squeeze_blank = true;
                } else if (fossil_io_cstring_compare(argv[j], "--time") == 0) {
                    show_time = true;
                } else if (fossil_io_cstring_compare(argv[j], "-h") == 0 || fossil_io_cstring_compare(argv[j], "--head") == 0) {
                    if (j + 1 < argc) head_lines = atoi(argv[++j]);
                } else if (fossil_io_cstring_compare(argv[j], "-t") == 0 || fossil_io_cstring_compare(argv[j], "--tail") == 0) {
                    if (j + 1 < argc) tail_lines = atoi(argv[++j]);
                } else if (!cnotnull(path)) {
                    path = argv[j];
                }
                i = j;
            }
            if (cnotnull(path)) fossil_shark_view(path, number_lines, number_non_blank, squeeze_blank, head_lines, tail_lines, show_time);
            
        } else if (fossil_io_cstring_compare(argv[i], "compare") == 0) {
            ccstring path1 = cnull, path2 = cnull;
            bool text_diff = false, binary_diff = false, ignore_case = false;
            int context_lines = 3;
            
            for (int j = i + 1; j < argc; j++) {
                if (fossil_io_cstring_compare(argv[j], "-t") == 0 || fossil_io_cstring_compare(argv[j], "--text") == 0) {
                    text_diff = true;
                } else if (fossil_io_cstring_compare(argv[j], "-b") == 0 || fossil_io_cstring_compare(argv[j], "--binary") == 0) {
                    binary_diff = true;
                } else if (fossil_io_cstring_compare(argv[j], "--ignore-case") == 0) {
                    ignore_case = true;
                } else if (fossil_io_cstring_compare(argv[j], "--context") == 0 && j + 1 < argc) {
                    context_lines = atoi(argv[++j]);
                } else if (!cnotnull(path1)) {
                    path1 = argv[j];
                } else if (!cnotnull(path2)) {
                    path2 = argv[j];
                }
                i = j;
            }
            if (cnotnull(path1) && cnotnull(path2)) fossil_shark_compare(path1, path2, text_diff, binary_diff, context_lines, ignore_case);
            
        } else if (fossil_io_cstring_compare(argv[i], "help") == 0) {
            ccstring command = cnull;
            bool show_examples = false, full_manual = false;
            
            for (int j = i + 1; j < argc; j++) {
                if (fossil_io_cstring_compare(argv[j], "--examples") == 0) {
                    show_examples = true;
                } else if (fossil_io_cstring_compare(argv[j], "--man") == 0) {
                    full_manual = true;
                } else if (!cnotnull(command) && argv[j][0] != '-') {
                    command = argv[j];
                }
                i = j;
            }
            fossil_shark_help(command, show_examples, full_manual);
        } else if (fossil_io_cstring_compare(argv[i], "summary") == 0) {
            // Default values
            ccstring file_path = cnull;
            int max_lines = 0;
            bool auto_detect = false;
            bool do_keywords = false;
            bool do_topics = false;
            bool do_stats = false;
            bool fson = false;
        
            // Parse flags
            for (int j = i + 1; j < argc; j++) {
                if (fossil_io_cstring_compare(argv[j], "-f") == 0 ||
                    fossil_io_cstring_compare(argv[j], "--file") == 0) {
                    if (j + 1 < argc) file_path = argv[++j];
                } else if (fossil_io_cstring_compare(argv[j], "-l") == 0 ||
                           fossil_io_cstring_compare(argv[j], "--lines") == 0) {
                    if (j + 1 < argc) max_lines = atoi(argv[++j]);
                } else if (fossil_io_cstring_compare(argv[j], "--auto") == 0) {
                    auto_detect = true;
                } else if (fossil_io_cstring_compare(argv[j], "--keywords") == 0) {
                    do_keywords = true;
                } else if (fossil_io_cstring_compare(argv[j], "--topics") == 0) {
                    do_topics = true;
                } else if (fossil_io_cstring_compare(argv[j], "--stats") == 0) {
                    do_stats = true;
                } else if (fossil_io_cstring_compare(argv[j], "--fson") == 0) {
                    fson = true;
                } else {
                    break; // Stop parsing flags if unknown argument
                }
                i = j; // advance outer loop
            }
        
            if (!file_path) {
                fossil_io_fprintf(FOSSIL_STDERR, "No file specified for summary.\n");
                return 1;
            }
        
            // Call summary command with a single file path
            const char *paths[1] = { file_path };
            fossil_shark_summary(paths, 1, max_lines, auto_detect, do_keywords, do_topics, do_stats, fson);
        } else if (fossil_io_cstring_compare(argv[i], "sync") == 0) {
            ccstring src = cnull, dest = cnull;
            bool recursive = false, update = false, delete_flag = false;
            for (int j = i + 1; j < argc; j++) {
                if (fossil_io_cstring_compare(argv[j], "-r") == 0 || fossil_io_cstring_compare(argv[j], "--recursive") == 0) {
                    recursive = true;
                } else if (fossil_io_cstring_compare(argv[j], "-u") == 0 || fossil_io_cstring_compare(argv[j], "--update") == 0) {
                    update = true;
                } else if (fossil_io_cstring_compare(argv[j], "--delete") == 0) {
                    delete_flag = true;
                } else if (!cnotnull(src)) {
                    src = argv[j];
                } else if (!cnotnull(dest)) {
                    dest = argv[j];
                }
                i = j;
            }
            if (cnotnull(src) && cnotnull(dest)) fossil_shark_sync(src, dest, recursive, update, delete_flag);

        } else if (fossil_io_cstring_compare(argv[i], "watch") == 0) {
            ccstring path = cnull, events = cnull;
            bool recursive = false;
            int interval = 1;
            for (int j = i + 1; j < argc; j++) {
                if (fossil_io_cstring_compare(argv[j], "-r") == 0 || fossil_io_cstring_compare(argv[j], "--recursive") == 0) {
                    recursive = true;
                } else if (fossil_io_cstring_compare(argv[j], "-e") == 0 || fossil_io_cstring_compare(argv[j], "--events") == 0) {
                    if (j + 1 < argc) events = argv[++j];
                } else if (fossil_io_cstring_compare(argv[j], "-t") == 0 || fossil_io_cstring_compare(argv[j], "--interval") == 0) {
                    if (j + 1 < argc) interval = atoi(argv[++j]);
                } else if (!cnotnull(path)) {
                    path = argv[j];
                }
                i = j;
            }
            if (cnotnull(path)) fossil_shark_watch(path, recursive, events, interval);

        } else if (fossil_io_cstring_compare(argv[i], "rewrite") == 0) {
            ccstring path = cnull;
            ccstring content = cnull;
            size_t size = 0;
            bool append = false;
            bool in_place = true;  // default true
            bool update_access = false, update_mod = false;

            for (int j = i + 1; j < argc; j++) {
                if (fossil_io_cstring_compare(argv[j], "-a") == 0 || fossil_io_cstring_compare(argv[j], "--append") == 0) {
                    append = true;
                } else if (fossil_io_cstring_compare(argv[j], "--in-place") == 0) {
                    in_place = true;
                } else if (fossil_io_cstring_compare(argv[j], "--access-time") == 0) {
                    update_access = true;
                } else if (fossil_io_cstring_compare(argv[j], "--mod-time") == 0) {
                    update_mod = true;
                } else if (fossil_io_cstring_compare(argv[j], "--size") == 0 && j + 1 < argc) {
                    size = (size_t)atoi(argv[++j]);
                } else if (!cnotnull(path)) {
                    path = argv[j];
                } else if (!cnotnull(content)) {
                    content = argv[j];
                }
                i = j;
            }

            if (cnotnull(path)) {
                int rc = fossil_shark_rewrite(path, in_place, append, content, size, update_access, update_mod);
                if (rc != 0) {
                    fossil_io_printf("{red}Rewrite failed: %s{reset}\n", path);
                }
            }

        } else if (fossil_io_cstring_compare(argv[i], "introspect") == 0) {
            ccstring path = cnull;
            int head_lines = 0, tail_lines = 0;
            bool count_lwb = false;
            bool show_type = false; 
            bool output_fson = false;

            for (int j = i + 1; j < argc; j++) {
                if (fossil_io_cstring_compare(argv[j], "--head") == 0 && j + 1 < argc) {
                    head_lines = atoi(argv[++j]);
                } else if (fossil_io_cstring_compare(argv[j], "--tail") == 0 && j + 1 < argc) {
                    tail_lines = atoi(argv[++j]);
                } else if (fossil_io_cstring_compare(argv[j], "--count") == 0) {
                    count_lwb = true;
                } else if (fossil_io_cstring_compare(argv[j], "--type") == 0) {
                    show_type = true;
                } else if (fossil_io_cstring_compare(argv[j], "--fson") == 0) {
                    output_fson = true;
                } else if (!cnotnull(path)) {
                    path = argv[j];
                }
                i = j;
            }

            if (cnotnull(path)) {
                int rc = fossil_shark_introspect(path, head_lines, tail_lines, count_lwb, show_type, output_fson);
                if (rc != 0) {
                    fossil_io_printf("{red}Introspect failed: %s{reset}\n", path);
                }
            }

        } else if (fossil_io_cstring_compare(argv[i], "grammar") == 0) {
            ccstring file_path = cnull;
            bool check = false, fix = false, sanitize = false, suggest = false, tone = false;
            ccstring detect_type = cnull;

            for (int j = i + 1; j < argc; j++) {
                if (fossil_io_cstring_compare(argv[j], "--check") == 0) {
                    check = true;
                } else if (fossil_io_cstring_compare(argv[j], "--fix") == 0) {
                    fix = true;
                } else if (fossil_io_cstring_compare(argv[j], "--sanitize") == 0) {
                    sanitize = true;
                } else if (fossil_io_cstring_compare(argv[j], "--suggest") == 0) {
                    suggest = true;
                } else if (fossil_io_cstring_compare(argv[j], "--tone") == 0) {
                    tone = true;
                } else if (fossil_io_cstring_compare(argv[j], "--detect") == 0 && j + 1 < argc) {
                    detect_type = argv[++j];
                } else if (!cnotnull(file_path)) {
                    file_path = argv[j];
                }
                i = j;
            }

            if (cnotnull(file_path)) {
                int rc = fossil_shark_grammar(file_path, check, fix, sanitize, suggest, tone, detect_type);
                if (rc != 0) {
                    fossil_io_printf("{red}Grammar analysis failed: %s{reset}\n", file_path);
                }
            }
        } else if (fossil_io_cstring_compare(argv[i], "secure") == 0) {
            ccstring path = cnull, hash_alg = cnull;
            bool encrypt = false, decrypt = false, sign = false, verify = false;
            bool hash = false, scrub = false, snapshot = false;

            for (int j = i + 1; j < argc; j++) {
                if (fossil_io_cstring_compare(argv[j], "-e") == 0) {
                    encrypt = true;
                } else if (fossil_io_cstring_compare(argv[j], "-d") == 0) {
                    decrypt = true;
                } else if (fossil_io_cstring_compare(argv[j], "-s") == 0) {
                    sign = true;
                } else if (fossil_io_cstring_compare(argv[j], "-v") == 0) {
                    verify = true;
                } else if (fossil_io_cstring_compare(argv[j], "-h") == 0 && j + 1 < argc) {
                    hash = true;
                    hash_alg = argv[++j];
                } else if (fossil_io_cstring_compare(argv[j], "--scrub") == 0) {
                    scrub = true;
                } else if (fossil_io_cstring_compare(argv[j], "--snapshot") == 0) {
                    snapshot = true;
                } else if (!cnotnull(path)) {
                    path = argv[j];
                }
                i = j;
            }

            if (cnotnull(path)) {
                int rc = fossil_shark_secure(path, encrypt, decrypt, sign, verify, hash, hash_alg, scrub, snapshot);
                if (rc != 0) {
                    fossil_io_printf("{red}Secure operation failed: %s{reset}\n", path);
                }
            }
        } else if (fossil_io_cstring_compare(argv[i], "storage") == 0) {
            ccstring path = cnull;
            bool dedupe = false, catalog = false, index = false, snapshot = false, prune = false;

            for (int j = i + 1; j < argc; j++) {
                if (fossil_io_cstring_compare(argv[j], "--dedupe") == 0) {
                    dedupe = true;
                } else if (fossil_io_cstring_compare(argv[j], "--catalog") == 0) {
                    catalog = true;
                } else if (fossil_io_cstring_compare(argv[j], "--index") == 0) {
                    index = true;
                } else if (fossil_io_cstring_compare(argv[j], "--snapshot") == 0) {
                    snapshot = true;
                } else if (fossil_io_cstring_compare(argv[j], "--prune") == 0) {
                    prune = true;
                } else if (!cnotnull(path)) {
                    path = argv[j];
                }
                i = j;
            }

            if (cnotnull(path)) {
                int rc = fossil_shark_storage(path, dedupe, catalog, index, snapshot, prune);
                if (rc != 0) {
                    fossil_io_printf("{red}Storage operation failed: %s{reset}\n", path);
                }
            }
        }
    }
    return 0;
}
