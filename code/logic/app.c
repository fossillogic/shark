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
#define DP(i, j) dp[(i) * (len2 + 1) + (j)]

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

    fossil_io_printf("{cyan}  info             {reset}Show detailed metadata about a file or directory\n");
    fossil_io_printf("{yellow}                   -p, --permissions {reset}{bright_black}Show permissions{reset}\n");
    fossil_io_printf("{yellow}                   -o, --owner       {reset}{bright_black}Show owner/group{reset}\n");
    fossil_io_printf("{yellow}                   -s, --size        {reset}{bright_black}Show size{reset}\n");
    fossil_io_printf("{yellow}                   -t, --timestamps  {reset}{bright_black}Show times{reset}\n");

    fossil_io_printf("{cyan}  sync             {reset}Synchronize files/directories\n");
    fossil_io_printf("{yellow}                   -r, --recursive   {reset}{bright_black}Include subdirs{reset}\n");
    fossil_io_printf("{yellow}                   -u, --update      {reset}{bright_black}Copy only newer{reset}\n");
    fossil_io_printf("{yellow}                   --delete          {reset}{bright_black}Remove extraneous files from target{reset}\n");

    fossil_io_printf("{cyan}  watch            {reset}Continuously monitor files or directories\n");
    fossil_io_printf("{yellow}                   -r, --recursive   {reset}{bright_black}Include subdirs{reset}\n");
    fossil_io_printf("{yellow}                   -e, --events <list> {reset}{bright_black}Filter events: create/modify/delete{reset}\n");
    fossil_io_printf("{yellow}                   -t, --interval <n> {reset}{bright_black}Poll interval in seconds{reset}\n");

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

    fossil_io_printf("{blue}🤖 AI Commands (Jellyfish Integration):{reset}\n");
    fossil_io_printf("{cyan}  ask              {reset}Run a one-shot prompt against a module or chain\n");
    fossil_io_printf("{yellow}                   -m, --model <id>   {reset}{bright_black}Model to use{reset}\n");
    fossil_io_printf("{yellow}                   -f, --file <path>  {reset}{bright_black}Provide file context{reset}\n");
    fossil_io_printf("{yellow}                   --explain          {reset}{bright_black}Request explanation{reset}\n");

    fossil_io_printf("{cyan}  chat             {reset}Interactive conversation session with a local module\n");
    fossil_io_printf("{yellow}                   --context          {reset}{bright_black}Keep conversation history{reset}\n");
    fossil_io_printf("{yellow}                   --save <file>      {reset}{bright_black}Save chat transcript{reset}\n");
    fossil_io_printf("{yellow}                   -m, --model <id>   {reset}{bright_black}Model to use{reset}\n");

    fossil_io_printf("{cyan}  summary          {reset}Summarize datasets, chains, logs, or model states\n");
    fossil_io_printf("{yellow}                   -f, --file <path>  {reset}{bright_black}File to summarize{reset}\n");
    fossil_io_printf("{yellow}                   --depth <n>        {reset}{bright_black}Summary depth{reset}\n");
    fossil_io_printf("{yellow}                   --time             {reset}{bright_black}Show timestamps{reset}\n\n");

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

/*
 * ==================================================================
 * TI Reasoning Metadata (advanced struct for audit/debug)
 * ==================================================================
 */
typedef struct {
    const char *input;
    const char *suggested;
    int edit_distance;
    float confidence_score; // 0.0 to 1.0
    int jaccard_index;      // 0-100, string similarity by token overlap
    int prefix_match;       // 1 if input is prefix of suggested, else 0
    int suffix_match;       // 1 if input is suffix of suggested, else 0
    int case_insensitive;   // 1 if match is case-insensitive, else 0
    const char *reason;
} fossil_ti_reason_t;

/*
 * ==================================================================
 * Jaccard Index (token overlap for advanced similarity)
 * ==================================================================
 */
static int shark_jaccard_index(const char *s1, const char *s2) {
    if (!s1 || !s2) return 0;
    int match = 0, total = 0;
    size_t len1 = strlen(s1), len2 = strlen(s2);
    int used[256] = {0};
    for (size_t i = 0; i < len1; ++i) {
        used[(unsigned char)s1[i]] = 1;
    }
    for (size_t i = 0; i < len2; ++i) {
        if (used[(unsigned char)s2[i]]) match++;
        used[(unsigned char)s2[i]] = 2;
    }
    for (int i = 0; i < 256; ++i) {
        if (used[i]) total++;
    }
    return total ? (100 * match / total) : 0;
}

/*
 * ==================================================================
 * Levenshtein Distance (Unchanged Core)
 * ==================================================================
 */
int shark_levenshtein_distance(const char *s1, const char *s2) {
    if (!s1 || !s2) return INT_MAX;
    int len1 = (int)strlen(s1), len2 = (int)strlen(s2);
    int i, j;
    int *dp = (int *)calloc((size_t)(len1 + 1) * (size_t)(len2 + 1), sizeof(int));
    if (!dp) return INT_MAX;
    for (i = 0; i <= len1; i++) DP(i, 0) = i;
    for (j = 0; j <= len2; j++) DP(0, j) = j;
    for (i = 1; i <= len1; i++) {
        for (j = 1; j <= len2; j++) {
            int cost = (s1[i - 1] == s2[j - 1]) ? 0 : 1;
            int del = DP(i - 1, j) + 1;
            int ins = DP(i, j - 1) + 1;
            int sub = DP(i - 1, j - 1) + cost;
            int min = del < ins ? del : ins;
            min = min < sub ? min : sub;
            DP(i, j) = min;
        }
    }
    int result = DP(len1, len2);
    free(dp);
    return result;
}

/*
 * ==================================================================
 * TI-Aware Command Suggestion (advanced reasoning)
 * ==================================================================
 */
const char* shark_suggest_command_ti(const char *input, const char **commands, int num_commands, fossil_ti_reason_t *out_reason) {
    if (!input || !commands || num_commands <= 0) return NULL;

    const char *best_match = NULL;
    int min_distance = INT_MAX;
    int best_length = 1;
    int best_jaccard = 0;
    int best_prefix = 0;
    int best_suffix = 0;
    int best_case_insensitive = 0;

    for (int i = 0; i < num_commands; ++i) {
        if (commands[i]) {
            int distance = shark_levenshtein_distance(input, commands[i]);
            int jaccard = shark_jaccard_index(input, commands[i]);
            int prefix = (strncmp(input, commands[i], strlen(input)) == 0) ? 1 : 0;
            int suffix = 0;
            size_t inlen = strlen(input), cmdlen = strlen(commands[i]);
            if (inlen <= cmdlen && strcmp(input, commands[i] + cmdlen - inlen) == 0) suffix = 1;
            int case_insensitive = fossil_io_cstring_iequals(input, commands[i]);

            // Prefer prefix, then lowest distance, then jaccard
            if (prefix ||
                (distance < min_distance) ||
                (distance == min_distance && jaccard > best_jaccard)) {
                min_distance = distance;
                best_match = commands[i];
                best_length = (int)strlen(commands[i]);
                best_jaccard = jaccard;
                best_prefix = prefix;
                best_suffix = suffix;
                best_case_insensitive = case_insensitive;
            }
        }
    }

    if (!best_match) return NULL;

    float confidence = 1.0f - ((float)min_distance / (float)best_length);
    confidence += (float)best_jaccard / 200.0f; // boost by jaccard
    if (best_prefix) confidence += 0.15f;
    if (best_suffix) confidence += 0.10f;
    if (best_case_insensitive) confidence += 0.05f;
    confidence = (confidence < 0.0f) ? 0.0f : (confidence > 1.0f) ? 1.0f : confidence;

    if (out_reason) {
        out_reason->input = input;
        out_reason->suggested = best_match;
        out_reason->edit_distance = min_distance;
        out_reason->confidence_score = confidence;
        out_reason->jaccard_index = best_jaccard;
        out_reason->prefix_match = best_prefix;
        out_reason->suffix_match = best_suffix;
        out_reason->case_insensitive = best_case_insensitive;
        if (confidence >= 0.85f)
            out_reason->reason = "Strong semantic and token match";
        else if (confidence >= 0.7f)
            out_reason->reason = "Close semantic match";
        else if (best_prefix)
            out_reason->reason = "Prefix match";
        else if (best_case_insensitive)
            out_reason->reason = "Case-insensitive match";
        else
            out_reason->reason = "Low confidence match";
    }

    return (confidence >= 0.7f) ? best_match : NULL;
}

bool app_entry(int argc, char** argv) {
    // List of supported commands for suggestion
    static const char *supported_commands[] = {
        "show", "move", "copy", "remove", "delete", "rename", "create", "search",
        "archive", "view", "compare", "help", "ask", "chat", "summary", "sync",
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
            const char *suggested = shark_suggest_command_ti(argv[i], supported_commands, num_supported, &ti_reason);
            if (suggested) {
                fossil_io_printf(
                    "{yellow}Did you mean: {cyan}%s{yellow}?{reset}\n"
                    "  {bright_black}TI Reasoning:{reset}\n"
                    "    Edit Distance: %d\n"
                    "    Confidence: %.2f\n"
                    "    Jaccard Index: %d\n"
                    "    Prefix Match: %d\n"
                    "    Suffix Match: %d\n"
                    "    Case-Insensitive: %d\n"
                    "    Reason: %s\n",
                    suggested,
                    ti_reason.edit_distance,
                    ti_reason.confidence_score,
                    ti_reason.jaccard_index,
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
            // Parse show command arguments and call fossil_shark_show
            const char *path = "."; // default current directory
            bool show_all = false, long_format = false, human_readable = false;
            bool recursive = false, show_time = false;
            const char *format = "list";
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
            // Parse move command arguments and call fossil_shark_move
            const char *src = NULL, *dest = NULL;
            bool force = false, interactive = false, backup = false;
            
            for (int j = i + 1; j < argc; j++) {
            if (fossil_io_cstring_compare(argv[j], "-f") == 0 || fossil_io_cstring_compare(argv[j], "--force") == 0) {
                force = true;
            } else if (fossil_io_cstring_compare(argv[j], "-i") == 0 || fossil_io_cstring_compare(argv[j], "--interactive") == 0) {
                interactive = true;
            } else if (fossil_io_cstring_compare(argv[j], "-b") == 0 || fossil_io_cstring_compare(argv[j], "--backup") == 0) {
                backup = true;
            } else if (!src) {
                src = argv[j];
            } else if (!dest) {
                dest = argv[j];
            }
            i = j;
            }
            if (src && dest) fossil_shark_move(src, dest, force, interactive, backup);
            
        } else if (fossil_io_cstring_compare(argv[i], "copy") == 0) {
            // Parse copy command arguments and call fossil_shark_copy
            const char *src = NULL, *dest = NULL;
            bool recursive = false, update = false, preserve = false;
            
            for (int j = i + 1; j < argc; j++) {
            if (fossil_io_cstring_compare(argv[j], "-r") == 0 || fossil_io_cstring_compare(argv[j], "--recursive") == 0) {
                recursive = true;
            } else if (fossil_io_cstring_compare(argv[j], "-u") == 0 || fossil_io_cstring_compare(argv[j], "--update") == 0) {
                update = true;
            } else if (fossil_io_cstring_compare(argv[j], "-p") == 0 || fossil_io_cstring_compare(argv[j], "--preserve") == 0) {
                preserve = true;
            } else if (!src) {
                src = argv[j];
            } else if (!dest) {
                dest = argv[j];
            }
            i = j;
            }
            if (src && dest) fossil_shark_copy(src, dest, recursive, update, preserve);
            
        } else if (fossil_io_cstring_compare(argv[i], "remove") == 0 || 
               fossil_io_cstring_compare(argv[i], "delete") == 0) {
            // Parse remove command arguments and call fossil_shark_remove
            const char *path = NULL;
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
            } else if (!path) {
                path = argv[j];
            }
            i = j;
            }
            if (path) fossil_shark_remove(path, recursive, force, interactive, use_trash);
            
        } else if (fossil_io_cstring_compare(argv[i], "rename") == 0) {
            // Parse rename command arguments and call fossil_shark_rename
            const char *old_name = NULL, *new_name = NULL;
            bool force = false, interactive = false;
            
            for (int j = i + 1; j < argc; j++) {
            if (fossil_io_cstring_compare(argv[j], "-f") == 0 || fossil_io_cstring_compare(argv[j], "--force") == 0) {
                force = true;
            } else if (fossil_io_cstring_compare(argv[j], "-i") == 0 || fossil_io_cstring_compare(argv[j], "--interactive") == 0) {
                interactive = true;
            } else if (!old_name) {
                old_name = argv[j];
            } else if (!new_name) {
                new_name = argv[j];
            }
            i = j;
            }
            if (old_name && new_name) fossil_shark_rename(old_name, new_name, force, interactive);
            
        } else if (fossil_io_cstring_compare(argv[i], "create") == 0) {
            // Parse create command arguments and call fossil_shark_create
            const char *path = NULL, *type = "dir";
            bool create_parents = false;
            
            for (int j = i + 1; j < argc; j++) {
            if (fossil_io_cstring_compare(argv[j], "-p") == 0 || fossil_io_cstring_compare(argv[j], "--parents") == 0) {
                create_parents = true;
            } else if (fossil_io_cstring_compare(argv[j], "-t") == 0 || fossil_io_cstring_compare(argv[j], "--type") == 0) {
                if (j + 1 < argc) type = argv[++j];
            } else if (!path) {
                path = argv[j];
            }
            i = j;
            }
            if (path) fossil_shark_create(path, create_parents, type);
            
        } else if (fossil_io_cstring_compare(argv[i], "search") == 0) {
            // Parse search command arguments and call fossil_shark_search
            const char *path = ".", *name_pattern = NULL, *content_pattern = NULL;
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
            // Parse archive command arguments and call fossil_shark_archive
            const char *path = NULL, *format = "zip", *password = NULL;
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
            } else if (!path) {
                path = argv[j];
            }
            i = j;
            }
            if (path) fossil_shark_archive(path, create, extract, list, format, password);
            
        } else if (fossil_io_cstring_compare(argv[i], "view") == 0) {
            // Parse view command arguments and call fossil_shark_view
            const char *path = NULL;
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
            } else if (!path) {
                path = argv[j];
            }
            i = j;
            }
            if (path) fossil_shark_view(path, number_lines, number_non_blank, squeeze_blank, head_lines, tail_lines, show_time);
            
        } else if (fossil_io_cstring_compare(argv[i], "compare") == 0) {
            // Parse compare command arguments and call fossil_shark_compare
            const char *path1 = NULL, *path2 = NULL;
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
            } else if (!path1) {
                path1 = argv[j];
            } else if (!path2) {
                path2 = argv[j];
            }
            i = j;
            }
            if (path1 && path2) fossil_shark_compare(path1, path2, text_diff, binary_diff, context_lines, ignore_case);
            
        } else if (fossil_io_cstring_compare(argv[i], "help") == 0) {
            // Parse help command arguments and call fossil_shark_help
            const char *command = NULL;
            bool show_examples = false, full_manual = false;
            
            for (int j = i + 1; j < argc; j++) {
            if (fossil_io_cstring_compare(argv[j], "--examples") == 0) {
                show_examples = true;
            } else if (fossil_io_cstring_compare(argv[j], "--man") == 0) {
                full_manual = true;
            } else if (!command && argv[j][0] != '-') {
                command = argv[j];
            }
            i = j;
            }
            fossil_shark_help(command, show_examples, full_manual);
        }
        // AI Commands
        else if (fossil_io_cstring_compare(argv[i], "ask") == 0) {
            // Parse ask command arguments and call fossil_shark_ask
            const char *model_id = NULL, *file_path = NULL;
            bool explain = false;
            for (int j = i + 1; j < argc; j++) {
            if (fossil_io_cstring_compare(argv[j], "-m") == 0 || fossil_io_cstring_compare(argv[j], "--model") == 0) {
                if (j + 1 < argc) model_id = argv[++j];
            } else if (fossil_io_cstring_compare(argv[j], "-f") == 0 || fossil_io_cstring_compare(argv[j], "--file") == 0) {
                if (j + 1 < argc) file_path = argv[++j];
            } else if (fossil_io_cstring_compare(argv[j], "--explain") == 0) {
                explain = true;
            }
            i = j;
            }
            fossil_shark_ask(model_id, file_path, explain);

        } else if (fossil_io_cstring_compare(argv[i], "chat") == 0) {
            // Parse chat command arguments and call fossil_shark_chat
            const char *model_id = NULL, *save_file = NULL;
            bool keep_context = false;
            for (int j = i + 1; j < argc; j++) {
            if (fossil_io_cstring_compare(argv[j], "-m") == 0 || fossil_io_cstring_compare(argv[j], "--model") == 0) {
                if (j + 1 < argc) model_id = argv[++j];
            } else if (fossil_io_cstring_compare(argv[j], "--context") == 0) {
                keep_context = true;
            } else if (fossil_io_cstring_compare(argv[j], "--save") == 0 && j + 1 < argc) {
                save_file = argv[++j];
            }
            i = j;
            }
            fossil_shark_chat(model_id, keep_context, save_file);

        } else if (fossil_io_cstring_compare(argv[i], "summary") == 0) {
            // Parse summary command arguments and call fossil_shark_summary
            const char *file_path = NULL;
            int depth = 0;
            bool show_time = false;
            for (int j = i + 1; j < argc; j++) {
            if (fossil_io_cstring_compare(argv[j], "-f") == 0 || fossil_io_cstring_compare(argv[j], "--file") == 0) {
                if (j + 1 < argc) file_path = argv[++j];
            } else if (fossil_io_cstring_compare(argv[j], "--depth") == 0 && j + 1 < argc) {
                depth = atoi(argv[++j]);
            } else if (fossil_io_cstring_compare(argv[j], "--time") == 0) {
                show_time = true;
            }
            i = j;
            }
            fossil_shark_summary(file_path, depth, show_time);
            
        } else if (fossil_io_cstring_compare(argv[i], "sync") == 0) {
            // Parse sync command arguments and call fossil_shark_sync
            const char *src = NULL, *dest = NULL;
            bool recursive = false, update = false, delete_flag = false;
            for (int j = i + 1; j < argc; j++) {
            if (fossil_io_cstring_compare(argv[j], "-r") == 0 || fossil_io_cstring_compare(argv[j], "--recursive") == 0) {
                recursive = true;
            } else if (fossil_io_cstring_compare(argv[j], "-u") == 0 || fossil_io_cstring_compare(argv[j], "--update") == 0) {
                update = true;
            } else if (fossil_io_cstring_compare(argv[j], "--delete") == 0) {
                delete_flag = true;
            } else if (!src) {
                src = argv[j];
            } else if (!dest) {
                dest = argv[j];
            }
            i = j;
            }
            if (src && dest) fossil_shark_sync(src, dest, recursive, update, delete_flag);

        } else if (fossil_io_cstring_compare(argv[i], "watch") == 0) {
            // Parse watch command arguments and call fossil_shark_watch
            const char *path = NULL, *events = NULL;
            bool recursive = false;
            int interval = 1;
            for (int j = i + 1; j < argc; j++) {
            if (fossil_io_cstring_compare(argv[j], "-r") == 0 || fossil_io_cstring_compare(argv[j], "--recursive") == 0) {
                recursive = true;
            } else if (fossil_io_cstring_compare(argv[j], "-e") == 0 || fossil_io_cstring_compare(argv[j], "--events") == 0) {
                if (j + 1 < argc) events = argv[++j];
            } else if (fossil_io_cstring_compare(argv[j], "-t") == 0 || fossil_io_cstring_compare(argv[j], "--interval") == 0) {
                if (j + 1 < argc) interval = atoi(argv[++j]);
            } else if (!path) {
                path = argv[j];
            }
            i = j;
            }
            if (path) fossil_shark_watch(path, recursive, events, interval);

        } else if (fossil_io_cstring_compare(argv[i], "rewrite") == 0) {
            // Parse rewrite command arguments and call fossil_shark_rewrite
            const char *path = NULL;
            const char *content = NULL;
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
                } else if (!path) {
                    path = argv[j];
                } else if (!content) {
                    content = argv[j];
                }
                i = j;
            }

            if (path) {
                int rc = fossil_shark_rewrite(path, in_place, append, content, size, update_access, update_mod);
                if (rc != 0) {
                    fossil_io_printf("{red}Rewrite failed: %s{reset}\n", path);
                }
            }

        } else if (fossil_io_cstring_compare(argv[i], "introspect") == 0) {
            // Parse introspect command arguments and call fossil_shark_introspect
            const char *path = NULL;
            int head_lines = 0, tail_lines = 0;
            bool count_lwb = false; // count lines/words/bytes
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
                } else if (!path) {
                    path = argv[j];
                }
                i = j;
            }

            if (path) {
                int rc = fossil_shark_introspect(path, head_lines, tail_lines, count_lwb, show_type, output_fson);
                if (rc != 0) {
                    fossil_io_printf("{red}Introspect failed: %s{reset}\n", path);
                }
            }

        } else if (fossil_io_cstring_compare(argv[i], "grammar") == 0) {
            // Parse grammar command arguments and call fossil_shark_grammar
            const char *file_path = NULL;
            bool check = false, fix = false, sanitize = false, suggest = false, tone = false;
            const char *detect_type = NULL;

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
                } else if (!file_path) {
                    file_path = argv[j];
                }
                i = j;
            }

            if (file_path) {
                int rc = fossil_shark_grammar(file_path, check, fix, sanitize, suggest, tone, detect_type);
                if (rc != 0) {
                    fossil_io_printf("{red}Grammar analysis failed: %s{reset}\n", file_path);
                }
            }
        }
    }
    return 0;
}
