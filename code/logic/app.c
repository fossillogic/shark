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
    fossil_io_printf("{white}Usage: {cyan}%s{white} <command> [options]{reset}\n\n", app_name);

    fossil_io_printf("{blue}Commands:{reset}\n");

    fossil_io_printf("{cyan}  show             {reset}Display files and directories\n");
    fossil_io_printf("{bright_black}    -a, --all           Show hidden files\n");
    fossil_io_printf("{bright_black}    -l, --long          Detailed info\n");
    fossil_io_printf("{bright_black}    -h, --human         Human-readable sizes\n");
    fossil_io_printf("{bright_black}    -r, --recursive     Include subdirs\n");
    fossil_io_printf("{bright_black}    -d, --depth <n>     Limit recursion\n");
    fossil_io_printf("{bright_black}    --as <mode>         Format: list/tree/graph\n");
    fossil_io_printf("{bright_black}    --time              Show timestamps\n");

    fossil_io_printf("{cyan}  move             {reset}Move or rename files/directories\n");
    fossil_io_printf("{bright_black}    -f, --force         Overwrite\n");
    fossil_io_printf("{bright_black}    -i, --interactive   Confirm overwrite\n");
    fossil_io_printf("{bright_black}    -b, --backup        Backup before move\n");
    fossil_io_printf("{bright_black}    --atomic            Atomic move\n");
    fossil_io_printf("{bright_black}    --progress          Show progress\n");
    fossil_io_printf("{bright_black}    --dry-run           Simulate\n");
    fossil_io_printf("{bright_black}    --exclude <pat>     Exclude files\n");
    fossil_io_printf("{bright_black}    --include <pat>     Include files\n");

    fossil_io_printf("{cyan}  copy             {reset}Copy files or directories\n");
    fossil_io_printf("{bright_black}    -r, --recursive     Copy subdirs\n");
    fossil_io_printf("{bright_black}    -u, --update        Only newer\n");
    fossil_io_printf("{bright_black}    -p, --preserve      Keep permissions/timestamps\n");
    fossil_io_printf("{bright_black}    --checksum          Verify after copy\n");
    fossil_io_printf("{bright_black}    --sparse            Preserve sparse files\n");
    fossil_io_printf("{bright_black}    --link              Hardlink instead\n");
    fossil_io_printf("{bright_black}    --reflink           Copy-on-write\n");
    fossil_io_printf("{bright_black}    --progress          Show progress\n");
    fossil_io_printf("{bright_black}    --dry-run           Simulate\n");
    fossil_io_printf("{bright_black}    --exclude <pat>     Exclude files\n");
    fossil_io_printf("{bright_black}    --include <pat>     Include files\n");

    fossil_io_printf("{cyan}  remove, delete   {reset}Delete files or directories\n");
    fossil_io_printf("{bright_black}    -r, --recursive     Delete contents\n");
    fossil_io_printf("{bright_black}    -f, --force         No confirmation\n");
    fossil_io_printf("{bright_black}    -i, --interactive   Confirm per file\n");
    fossil_io_printf("{bright_black}    --trash             Move to trash\n");
    fossil_io_printf("{bright_black}    --wipe              Securely overwrite\n");
    fossil_io_printf("{bright_black}    --shred <n>         Multi-pass secure delete\n");
    fossil_io_printf("{bright_black}    --older-than <t>    Delete files older than time\n");
    fossil_io_printf("{bright_black}    --larger-than <s>   Delete files larger than size\n");
    fossil_io_printf("{bright_black}    --empty-only        Delete empty dirs only\n");
    fossil_io_printf("{bright_black}    --log-file <path>   Write deletion log\n");

    fossil_io_printf("{cyan}  rename           {reset}Rename files or directories\n");
    fossil_io_printf("{bright_black}    -f, --force         Overwrite target\n");
    fossil_io_printf("{bright_black}    -i, --interactive   Confirm overwrite\n");

    fossil_io_printf("{cyan}  create           {reset}Create new directories or files\n");
    fossil_io_printf("{bright_black}    -p, --parents       Create parent dirs\n");
    fossil_io_printf("{bright_black}    -t, --type <type>   File or dir\n");

    fossil_io_printf("{cyan}  search           {reset}Find files by name or content\n");
    fossil_io_printf("{bright_black}    -r, --recursive     Include subdirs\n");
    fossil_io_printf("{bright_black}    -n, --name <pat>    Filename match\n");
    fossil_io_printf("{bright_black}    -c, --content <pat> Search contents\n");
    fossil_io_printf("{bright_black}    -i, --ignore-case   Case-insensitive\n");

    fossil_io_printf("{cyan}  archive          {reset}Create, extract, or list archives\n");
    fossil_io_printf("{bright_black}    -c, --create        Create new archive\n");
    fossil_io_printf("{bright_black}    -x, --extract       Extract archive\n");
    fossil_io_printf("{bright_black}    -l, --list          List archive contents\n");
    fossil_io_printf("{bright_black}    -f <format>         Format: zip/tar/gz\n");
    fossil_io_printf("{bright_black}    -p, --password <pw> Encrypt with password\n");
    fossil_io_printf("{bright_black}    --stdout            Output to stdout\n");
    fossil_io_printf("{bright_black}    --compress <n>      Compression level (0-9)\n");
    fossil_io_printf("{bright_black}    --exclude <pat>     Exclude files\n");

    fossil_io_printf("{cyan}  view             {reset}Output file contents to terminal\n");
    fossil_io_printf("{bright_black}    -n, --number        Number lines\n");
    fossil_io_printf("{bright_black}    -b, --non-blank     Number non-blank\n");
    fossil_io_printf("{bright_black}    -s, --squeeze       Remove blank lines\n");
    fossil_io_printf("{bright_black}    -h, --head <n>      First n lines\n");
    fossil_io_printf("{bright_black}    -t, --tail <n>      Last n lines\n");
    fossil_io_printf("{bright_black}    --time              Show timestamps\n");
    fossil_io_printf("{bright_black}    --format            Pretty format\n");

    fossil_io_printf("{cyan}  compare          {reset}Compare two files/directories\n");
    fossil_io_printf("{bright_black}    -t, --text          Line diff\n");
    fossil_io_printf("{bright_black}    -b, --binary        Binary diff\n");
    fossil_io_printf("{bright_black}    --context <n>       Context lines\n");
    fossil_io_printf("{bright_black}    --ignore-case       Ignore case\n");

    fossil_io_printf("{cyan}  help             {reset}Display help for commands\n");
    fossil_io_printf("{bright_black}    --examples          Usage examples\n");
    fossil_io_printf("{bright_black}    --man               Full manual\n");

    fossil_io_printf("{cyan}  sync             {reset}Synchronize files/directories\n");
    fossil_io_printf("{bright_black}    -r, --recursive     Include subdirs\n");
    fossil_io_printf("{bright_black}    -u, --update        Only newer\n");
    fossil_io_printf("{bright_black}    --delete            Remove extraneous files\n");

    fossil_io_printf("{cyan}  watch            {reset}Monitor files or directories\n");
    fossil_io_printf("{bright_black}    -r, --recursive     Include subdirs\n");
    fossil_io_printf("{bright_black}    -e, --events <list> Event filter\n");
    fossil_io_printf("{bright_black}    -t, --interval <n>  Poll interval\n");

    fossil_io_printf("{cyan}  rewrite          {reset}Modify file contents or metadata\n");
    fossil_io_printf("{bright_black}    -a, --append        Append\n");
    fossil_io_printf("{bright_black}    --in-place          Edit in place\n");
    fossil_io_printf("{bright_black}    --access-time       Update atime\n");
    fossil_io_printf("{bright_black}    --mod-time          Update mtime\n");
    fossil_io_printf("{bright_black}    --size <n>          Set file size\n");

    fossil_io_printf("{cyan}  introspect       {reset}Examine file contents/type/meta\n");
    fossil_io_printf("{bright_black}    --head <n>          First n lines\n");
    fossil_io_printf("{bright_black}    --tail <n>          Last n lines\n");
    fossil_io_printf("{bright_black}    --count             Count lines/words/bytes\n");
    fossil_io_printf("{bright_black}    --type              Show type\n");
    fossil_io_printf("{bright_black}    --fson              FSON output\n");

    fossil_io_printf("{cyan}  grammar          {reset}Analyze/correct grammar/style\n");
    fossil_io_printf("{bright_black}    --check             Analyze\n");
    fossil_io_printf("{bright_black}    --fix, --correct    Correct\n");
    fossil_io_printf("{bright_black}    --sanitize          Sanitize text\n");
    fossil_io_printf("{bright_black}    --suggest           Suggest improvements\n");
    fossil_io_printf("{bright_black}    --tone              Detect tone\n");
    fossil_io_printf("{bright_black}    --summarize         Summarize\n");
    fossil_io_printf("{bright_black}    --score             Show scores\n");
    fossil_io_printf("{bright_black}    --detect <type>     Detect traits\n");
    fossil_io_printf("{bright_black}    --reflow-width <n>  Reflow text\n");
    fossil_io_printf("{bright_black}    --capitalize <mode> Capitalize\n");
    fossil_io_printf("{bright_black}    --format            Pretty-print\n");
    fossil_io_printf("{bright_black}    --declutter         Repair whitespace\n");
    fossil_io_printf("{bright_black}    --punctuate         Normalize punctuation\n");

    fossil_io_printf("{cyan}  cryptic          {reset}Encode or decode text using various ciphers\n");
    fossil_io_printf("{bright_black}    -e, --encode        Encode text\n");
    fossil_io_printf("{bright_black}    -d, --decode        Decode text\n");
    fossil_io_printf("{bright_black}    -c, --cipher <type> Cipher type: caesar, vigenere, base64, base32,\n");
    fossil_io_printf("{bright_black}                        binary, morse, baconian, railfence, haxor,\n");
    fossil_io_printf("{bright_black}                        leet, rot13, atbash\n");

    fossil_io_printf("\n{blue}Global Flags:{reset}\n");
    fossil_io_printf("{bright_black}  --help                Show command help\n");
    fossil_io_printf("{bright_black}  --version             Display Shark Tool version\n");
    fossil_io_printf("{bright_black}  --name                Display application name\n");
    fossil_io_printf("{bright_black}  --verbose             Enable detailed output\n");
    fossil_io_printf("{bright_black}  --color [enable|disable|auto]  Colorize output\n");
    fossil_io_printf("{bright_black}  --clear               Clear the terminal screen\n");

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
        "archive", "view", "compare", "help", "sync", "cryptic",
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
                    "  {bright_cyan}TI Reason:{reset} {magenta}%s{reset} "
                    "({blue}edit:{reset} {yellow}%d{reset}, "
                    "{blue}sim:{reset} {yellow}%.2f{reset}, "
                    "{blue}jaccard:{reset} {yellow}%d{reset}, "
                    "{blue}prefix:{reset} {yellow}%d{reset}, "
                    "{blue}suffix:{reset} {yellow}%d{reset}, "
                    "{blue}ci:{reset} {yellow}%d{reset})\n",
                    suggested,
                    ti_reason.reason,
                    edit_dist,
                    similarity,
                    jaccard,
                    ti_reason.prefix_match,
                    ti_reason.suffix_match,
                    ti_reason.case_insensitive
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
            fossil_io_cstring_compare(argv[i], "grammar") == 0 ||
            fossil_io_cstring_compare(argv[i], "cryptic") == 0
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
            fossil_io_clear_screen(); // ANSI escape sequence to clear screen
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
            bool atomic = false, progress = false, dry_run = false;
            ccstring exclude_pattern = cnull, include_pattern = cnull;
            
            for (int j = i + 1; j < argc; j++) {
                if (fossil_io_cstring_compare(argv[j], "-f") == 0 || fossil_io_cstring_compare(argv[j], "--force") == 0) {
                    force = true;
                } else if (fossil_io_cstring_compare(argv[j], "-i") == 0 || fossil_io_cstring_compare(argv[j], "--interactive") == 0) {
                    interactive = true;
                } else if (fossil_io_cstring_compare(argv[j], "-b") == 0 || fossil_io_cstring_compare(argv[j], "--backup") == 0) {
                    backup = true;
                } else if (fossil_io_cstring_compare(argv[j], "--atomic") == 0) {
                    atomic = true;
                } else if (fossil_io_cstring_compare(argv[j], "--progress") == 0) {
                    progress = true;
                } else if (fossil_io_cstring_compare(argv[j], "--dry-run") == 0) {
                    dry_run = true;
                } else if (fossil_io_cstring_compare(argv[j], "--exclude") == 0 && j + 1 < argc) {
                    exclude_pattern = argv[++j];
                } else if (fossil_io_cstring_compare(argv[j], "--include") == 0 && j + 1 < argc) {
                    include_pattern = argv[++j];
                } else if (!cnotnull(src)) {
                    src = argv[j];
                } else if (!cnotnull(dest)) {
                    dest = argv[j];
                }
                i = j;
            }
            if (cnotnull(src) && cnotnull(dest)) fossil_shark_move(src, dest, force, interactive, backup, atomic, progress, dry_run, exclude_pattern, include_pattern);
            
        } else if (fossil_io_cstring_compare(argv[i], "copy") == 0) {
            ccstring src = cnull, dest = cnull;
            bool recursive = false, update = false, preserve = false;
            bool checksum = false, sparse = false, link = false, reflink = false;
            bool progress = false, dry_run = false;
            ccstring exclude_pattern = cnull, include_pattern = cnull;
            
            for (int j = i + 1; j < argc; j++) {
                if (fossil_io_cstring_compare(argv[j], "-r") == 0 || fossil_io_cstring_compare(argv[j], "--recursive") == 0) {
                    recursive = true;
                } else if (fossil_io_cstring_compare(argv[j], "-u") == 0 || fossil_io_cstring_compare(argv[j], "--update") == 0) {
                    update = true;
                } else if (fossil_io_cstring_compare(argv[j], "-p") == 0 || fossil_io_cstring_compare(argv[j], "--preserve") == 0) {
                    preserve = true;
                } else if (fossil_io_cstring_compare(argv[j], "--checksum") == 0) {
                    checksum = true;
                } else if (fossil_io_cstring_compare(argv[j], "--sparse") == 0) {
                    sparse = true;
                } else if (fossil_io_cstring_compare(argv[j], "--link") == 0) {
                    link = true;
                } else if (fossil_io_cstring_compare(argv[j], "--reflink") == 0) {
                    reflink = true;
                } else if (fossil_io_cstring_compare(argv[j], "--progress") == 0) {
                    progress = true;
                } else if (fossil_io_cstring_compare(argv[j], "--dry-run") == 0) {
                    dry_run = true;
                } else if (fossil_io_cstring_compare(argv[j], "--exclude") == 0 && j + 1 < argc) {
                    exclude_pattern = argv[++j];
                } else if (fossil_io_cstring_compare(argv[j], "--include") == 0 && j + 1 < argc) {
                    include_pattern = argv[++j];
                } else if (!cnotnull(src)) {
                    src = argv[j];
                } else if (!cnotnull(dest)) {
                    dest = argv[j];
                }
                i = j;
            }
            if (cnotnull(src) && cnotnull(dest)) fossil_shark_copy(src, dest, recursive, update, preserve, checksum, sparse, link, reflink, progress, dry_run, exclude_pattern, include_pattern);
            
        } else if (fossil_io_cstring_compare(argv[i], "remove") == 0 || 
               fossil_io_cstring_compare(argv[i], "delete") == 0) {
            ccstring path = cnull;
            bool recursive = false, force = false, interactive = false, use_trash = false;
            bool wipe = false, empty_only = false;
            int shred_passes = 0;
            ccstring older_than = cnull, log_file = cnull;
            size_t larger_than = 0;
            
            for (int j = i + 1; j < argc; j++) {
                if (fossil_io_cstring_compare(argv[j], "-r") == 0 || fossil_io_cstring_compare(argv[j], "--recursive") == 0) {
                    recursive = true;
                } else if (fossil_io_cstring_compare(argv[j], "-f") == 0 || fossil_io_cstring_compare(argv[j], "--force") == 0) {
                    force = true;
                } else if (fossil_io_cstring_compare(argv[j], "-i") == 0 || fossil_io_cstring_compare(argv[j], "--interactive") == 0) {
                    interactive = true;
                } else if (fossil_io_cstring_compare(argv[j], "--trash") == 0) {
                    use_trash = true;
                } else if (fossil_io_cstring_compare(argv[j], "--wipe") == 0) {
                    wipe = true;
                } else if (fossil_io_cstring_compare(argv[j], "--shred") == 0 && j + 1 < argc) {
                    shred_passes = atoi(argv[++j]);
                } else if (fossil_io_cstring_compare(argv[j], "--older-than") == 0 && j + 1 < argc) {
                    older_than = argv[++j];
                } else if (fossil_io_cstring_compare(argv[j], "--larger-than") == 0 && j + 1 < argc) {
                    larger_than = (size_t)atoi(argv[++j]);
                } else if (fossil_io_cstring_compare(argv[j], "--empty-only") == 0) {
                    empty_only = true;
                } else if (fossil_io_cstring_compare(argv[j], "--log-file") == 0 && j + 1 < argc) {
                    log_file = argv[++j];
                } else if (!cnotnull(path)) {
                    path = argv[j];
                }
                i = j;
            }
            if (cnotnull(path)) fossil_shark_remove(path, recursive, force, interactive, use_trash, wipe, shred_passes, older_than, larger_than, empty_only, log_file);
            
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
            ccstring path = cnull, format = "zip", password = cnull, exclude_pattern = cnull;
            bool create = false, extract = false, list = false;
            bool stdout_output = false;
            int compress_level = 6;
            
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
                } else if (fossil_io_cstring_compare(argv[j], "--stdout") == 0) {
                    stdout_output = true;
                } else if (fossil_io_cstring_compare(argv[j], "--compress") == 0 && j + 1 < argc) {
                    compress_level = atoi(argv[++j]);
                } else if (fossil_io_cstring_compare(argv[j], "--exclude") == 0 && j + 1 < argc) {
                    exclude_pattern = argv[++j];
                } else if (!cnotnull(path)) {
                    path = argv[j];
                }
                i = j;
            }
            if (cnotnull(path)) fossil_shark_archive(path, create, extract, list, format, password, compress_level, stdout_output, exclude_pattern);
            
        } else if (fossil_io_cstring_compare(argv[i], "view") == 0) {
            ccstring path = cnull;
            bool number_lines = false, number_non_blank = false, squeeze_blank = false, show_time = false;
            bool format = false;
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
                } else if (fossil_io_cstring_compare(argv[j], "--format") == 0) {
                    format = true;
                } else if (fossil_io_cstring_compare(argv[j], "-h") == 0 || fossil_io_cstring_compare(argv[j], "--head") == 0) {
                    if (j + 1 < argc) head_lines = atoi(argv[++j]);
                } else if (fossil_io_cstring_compare(argv[j], "-t") == 0 || fossil_io_cstring_compare(argv[j], "--tail") == 0) {
                    if (j + 1 < argc) tail_lines = atoi(argv[++j]);
                } else if (!cnotnull(path)) {
                    path = argv[j];
                }
                i = j;
            }
            if (cnotnull(path)) fossil_shark_view(path, number_lines, number_non_blank, squeeze_blank, format, head_lines, tail_lines, show_time);
            
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
            bool check = false, correct = false, sanitize = false, suggest = false, tone = false;
            bool summarize = false, score = false, format = false, declutter = false, punctuate = false;
            int reflow_width = 0;
            ccstring detect_type = cnull;
            ccstring capitalize_mode = cnull;

            for (int j = i + 1; j < argc; j++) {
                if (fossil_io_cstring_compare(argv[j], "--check") == 0) {
                    check = true;
                } else if (fossil_io_cstring_compare(argv[j], "--fix") == 0 || fossil_io_cstring_compare(argv[j], "--correct") == 0) {
                    correct = true;
                } else if (fossil_io_cstring_compare(argv[j], "--sanitize") == 0) {
                    sanitize = true;
                } else if (fossil_io_cstring_compare(argv[j], "--suggest") == 0) {
                    suggest = true;
                } else if (fossil_io_cstring_compare(argv[j], "--tone") == 0) {
                    tone = true;
                } else if (fossil_io_cstring_compare(argv[j], "--summarize") == 0) {
                    summarize = true;
                } else if (fossil_io_cstring_compare(argv[j], "--score") == 0) {
                    score = true;
                } else if (fossil_io_cstring_compare(argv[j], "--format") == 0) {
                    format = true;
                } else if (fossil_io_cstring_compare(argv[j], "--declutter") == 0) {
                    declutter = true;
                } else if (fossil_io_cstring_compare(argv[j], "--punctuate") == 0) {
                    punctuate = true;
                } else if (fossil_io_cstring_compare(argv[j], "--reflow-width") == 0 && j + 1 < argc) {
                    reflow_width = atoi(argv[++j]);
                } else if (fossil_io_cstring_compare(argv[j], "--capitalize") == 0 && j + 1 < argc) {
                    capitalize_mode = argv[++j];
                } else if (fossil_io_cstring_compare(argv[j], "--detect") == 0 && j + 1 < argc) {
                    detect_type = argv[++j];
                } else if (!cnotnull(file_path)) {
                    file_path = argv[j];
                }
                i = j;
            }

            if (cnotnull(file_path)) {
                int rc = fossil_shark_grammar(
                    file_path,
                    check,
                    correct,
                    tone,
                    sanitize,
                    suggest,
                    summarize,
                    score,
                    detect_type,
                    reflow_width,
                    capitalize_mode,
                    format,
                    declutter,
                    punctuate
                );
                if (rc != 0) {
                    fossil_io_printf("{red}Grammar analysis failed: %s{reset}\n", file_path);
                }
            }
        } else if (fossil_io_cstring_compare(argv[i], "cryptic") == 0) {
            ccstring text = cnull, cipher = "caesar";
            bool encode = false, decode = false;

            for (int j = i + 1; j < argc; j++) {
                if (fossil_io_cstring_compare(argv[j], "-e") == 0 || fossil_io_cstring_compare(argv[j], "--encode") == 0) {
                    encode = true;
                } else if (fossil_io_cstring_compare(argv[j], "-d") == 0 || fossil_io_cstring_compare(argv[j], "--decode") == 0) {
                    decode = true;
                } else if (fossil_io_cstring_compare(argv[j], "-c") == 0 || fossil_io_cstring_compare(argv[j], "--cipher") == 0) {
                    if (j + 1 < argc) cipher = argv[++j];
                } else if (!cnotnull(text)) {
                    text = argv[j];
                }
                i = j;
            }

            if (cnotnull(text)) {
            int rc = fossil_shark_cryptic(text, encode, decode, cipher);
            if (rc != 0) {
                fossil_io_printf("{red}Cryptic operation failed{reset}\n");
            }
            }
        } //
    }
    return 0;
}
