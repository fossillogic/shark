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
#include "fossil/code/magic.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <limits.h>

/* ==========================================================================
 * Static Helpers (internal)
 * ========================================================================== */

static int fossil_it_magic_is_code_file(const char *path) {
    const char *ext = strrchr(path, '.');
    if (!ext) return 0;

    const char *code_exts[] = {
        // C/C++
        ".c", ".h", ".cpp", ".hpp", ".cc", ".cxx", ".hxx", ".hh",
        // Python
        ".py", ".pyw", ".ipynb", ".pyc", ".pyo", ".pyd",
        // Java
        ".java", ".class", ".jar", ".jad", ".jmod",
        // C#
        ".cs", ".vb", ".fs",
        // Go
        ".go", ".mod", ".sum",
        // Rust
        ".rs", ".rlib", ".toml",
        // JavaScript/TypeScript
        ".js", ".jsx", ".mjs", ".cjs",
        ".ts", ".tsx",
        // PHP
        ".php", ".phtml", ".php3", ".php4", ".php5", ".phps",
        // Ruby
        ".rb", ".erb", ".rake", ".gemspec",
        // Perl
        ".pl", ".pm", ".pod", ".t",
        // Swift
        ".swift",
        // Kotlin
        ".kt", ".kts",
        // Scala
        ".scala", ".sc",
        // Shell
        ".sh", ".bash", ".zsh", ".csh", ".tcsh", ".ksh",
        // Batch/PowerShell
        ".bat", ".cmd", ".ps1", ".psm1",
        // Lua
        ".lua",
        // SQL
        ".sql", ".sqlite", ".db",
        // Web
        ".html", ".htm", ".xhtml",
        ".css", ".scss", ".less",
        ".xml", ".xsd", ".xslt",
        ".json", ".yaml", ".yml",
        // Dart
        ".dart",
        // Groovy
        ".groovy", ".gradle",
        // R
        ".r", ".R", ".Rmd",
        // Objective-C
        ".m", ".mm",
        // Assembly
        ".asm", ".s", ".S",
        // Verilog/VHDL
        ".v", ".vh", ".sv", ".vhd", ".vhdl",
        // CoffeeScript
        ".coffee",
        // Clojure/EDN
        ".clj", ".cljs", ".cljc", ".edn",
        // Haskell
        ".hs", ".lhs", ".ghc",
        // OCaml
        ".ml", ".mli", ".ocaml",
        // Ada
        ".ada", ".adb", ".ads",
        // Fortran
        ".for", ".f90", ".f95", ".f03", ".f08", ".f", ".f77",
        // Prolog/Tcl
        ".pro", ".pl", ".tcl",
        // TeX
        ".tex", ".sty", ".cls",
        // Nim
        ".nim",
        // Crystal
        ".cr",
        // Elixir
        ".ex", ".exs",
        // Elm
        ".elm",
        // Erlang
        ".erl", ".hrl",
        // Lisp/Scheme
        ".lisp", ".el", ".scm", ".cl", ".lsp",
        // Pascal
        ".pas", ".pp", ".p",
        // D
        ".d",
        // Vala
        ".vala",
        // VBScript
        ".vbs",
        // AWK
        ".awk",
        // PostScript
        ".ps",
        // Raku/Perl 6
        ".raku", ".pl6", ".pm6",
        // Solidity
        ".sol",
        // CMake
        ".cmake",
        // Meson build files
        ".build", ".options",
        // Dockerfile
        ".dockerfile",
        // Config/INI
        ".ini", ".conf", ".cfg",
        // TOML
        ".toml",
        // TypeScript JSX (duplicate, but kept for completeness)
        ".tsx",
        // Visual Studio
        ".sln", ".vcxproj", ".csproj",
        // Xcode
        ".xcodeproj", ".xcworkspace",
        // Bazel
        ".bazel", ".bzl", "BUILD", "WORKSPACE",
        // Ninja
        ".ninja",
        // Others
        ".gitignore", ".gitattributes", ".editorconfig", ".env"
    };

    for (int i = 0; i < 11; i++)
        if (strcmp(ext, code_exts[i]) == 0)
            return 1;
    return 0;
}

static int fossil_it_magic_contains_git(const char *path) {
    char buf[600];
    snprintf(buf, sizeof(buf), "%s/.git", path);
    struct stat st;
    return (stat(buf, &st) == 0);
}

static int fossil_it_magic_contains_secret(const char *path) {
    char buf[600];
    const char *secret_files[] = {
        ".env", "secret.key", "id_rsa", "private.pem"
    };

    for (int i = 0; i < 4; i++) {
        snprintf(buf, sizeof(buf), "%s/%s", path, secret_files[i]);
        struct stat st;
        if (stat(buf, &st) == 0)
            return 1;
    }
    return 0;
}

static long fossil_it_magic_directory_size(const char *path) {
    long total = 0;
    DIR *d = opendir(path);
    if (!d) return 0;

    struct dirent *ent;
    struct stat st;
    char full[600];

    while ((ent = readdir(d)) != NULL) {
        if (!strcmp(ent->d_name, ".") || !strcmp(ent->d_name, ".."))
            continue;

        snprintf(full, sizeof(full), "%s/%s", path, ent->d_name);
        if (stat(full, &st) == 0)
            total += st.st_size;
    }
    closedir(d);
    return total;
}

/* ==========================================================================
 * Similarity Utilities
 * ========================================================================== */

int fossil_it_magic_jaccard_index(const char *s1, const char *s2) {
    if (!s1 || !s2) return 0;

    int match = 0, total = 0;
    int used[256] = {0};
    size_t len1 = strlen(s1), len2 = strlen(s2);

    for (size_t i = 0; i < len1; ++i)
        used[(unsigned char)s1[i]] = 1;
    for (size_t i = 0; i < len2; ++i) {
        if (used[(unsigned char)s2[i]]) match++;
        used[(unsigned char)s2[i]] = 2;
    }

    for (int i = 0; i < 256; ++i)
        if (used[i]) total++;

    return total ? (100 * match / total) : 0;
}

int fossil_it_magic_levenshtein_distance(const char *s1, const char *s2) {
    if (!s1 || !s2) return INT_MAX;
    int len1 = (int)strlen(s1), len2 = (int)strlen(s2);

    int *dp = (int *)calloc((size_t)(len1 + 1) * (size_t)(len2 + 1), sizeof(int));
    if (!dp) return INT_MAX;

    #define DP(i,j) dp[(i)*(len2+1)+(j)]

    for (int i = 0; i <= len1; i++) DP(i, 0) = i;
    for (int j = 0; j <= len2; j++) DP(0, j) = j;

    for (int i = 1; i <= len1; i++) {
        for (int j = 1; j <= len2; j++) {
            int cost = (s1[i - 1] == s2[j - 1]) ? 0 : 1;
            int del = DP(i - 1, j) + 1;
            int ins = DP(i, j - 1) + 1;
            int sub = DP(i - 1, j - 1) + cost;
            int min = del < ins ? del : ins;
            DP(i, j) = (min < sub) ? min : sub;
        }
    }

    int result = DP(len1, len2);
    free(dp);
    #undef DP
    return result;
}

float fossil_it_magic_similarity(const char *a, const char *b) {
    if (!a || !b) return 0.0f;
    int len_a = (int)strlen(a), len_b = (int)strlen(b);
    if (len_a == 0 && len_b == 0) return 1.0f;

    int dist = fossil_it_magic_levenshtein_distance(a, b);
    int max_len = len_a > len_b ? len_a : len_b;
    return 1.0f - ((float)dist / (float)max_len);
}

/* ==========================================================================
 * Command Suggestion
 * ========================================================================== */

const char *fossil_it_magic_suggest_command(
    const char *input,
    const char **commands,
    int num_commands,
    fossil_ti_reason_t *out_reason
) {
    if (!input || !commands || num_commands <= 0) return NULL;

    const char *best_match = NULL;
    int min_distance = INT_MAX;
    int best_length = 1;
    int best_jaccard = 0;
    int best_prefix = 0;
    int best_suffix = 0;
    int best_case_insensitive = 0;

    for (int i = 0; i < num_commands; ++i) {
        if (!commands[i]) continue;

        int distance = fossil_it_magic_levenshtein_distance(input, commands[i]);
        int jaccard = fossil_it_magic_jaccard_index(input, commands[i]);
        int prefix = (strncmp(input, commands[i], strlen(input)) == 0) ? 1 : 0;
        int suffix = (strlen(input) <= strlen(commands[i]) &&
                     strcmp(input, commands[i] + strlen(commands[i]) - strlen(input)) == 0) ? 1 : 0;
        int case_insensitive = fossil_io_cstring_iequals(input, commands[i]);

        if (prefix || (distance < min_distance) || (distance == min_distance && jaccard > best_jaccard)) {
            best_match = commands[i];
            min_distance = distance;
            best_length = (int)strlen(commands[i]);
            best_jaccard = jaccard;
            best_prefix = prefix;
            best_suffix = suffix;
            best_case_insensitive = case_insensitive;
        }
    }

    if (!best_match) return NULL;

    float confidence = 1.0f - ((float)min_distance / best_length);
    confidence += best_jaccard / 200.0f;
    if (best_prefix) confidence += 0.15f;
    if (best_suffix) confidence += 0.10f;
    if (best_case_insensitive) confidence += 0.05f;
    if (confidence > 1.0f) confidence = 1.0f;
    if (confidence < 0.0f) confidence = 0.0f;

    if (out_reason) {
        out_reason->input = input;
        out_reason->suggested = best_match;
        out_reason->edit_distance = min_distance;
        out_reason->confidence_score = confidence;
        out_reason->jaccard_index = best_jaccard;
        out_reason->prefix_match = best_prefix;
        out_reason->suffix_match = best_suffix;
        out_reason->case_insensitive = best_case_insensitive;
        out_reason->reason = (confidence >= 0.85f) ? "Strong semantic and token match" :
                             (confidence >= 0.7f) ? "Close semantic match" :
                             best_prefix ? "Prefix match" :
                             best_case_insensitive ? "Case-insensitive match" :
                             "Low confidence match";
    }

    return (confidence >= 0.7f) ? best_match : NULL;
}

/* ==========================================================================
 * Path Auto-Correction
 * ========================================================================== */

void fossil_it_magic_path_suggest(
    const char *bad_path,
    const char *base_dir,
    fossil_ti_path_suggestion_set_t *out
) {
    out->count = 0;
    DIR *d = opendir(base_dir);
    if (!d) return;

    struct dirent *ent;
    while ((ent = readdir(d)) && out->count < 16) {
        float score = fossil_it_magic_similarity(bad_path, ent->d_name);
        if (score < 0.25f) continue;

        snprintf(out->list[out->count].candidate_path,
                 sizeof(out->list[out->count].candidate_path),
                 "%s/%s", base_dir, ent->d_name);
        out->list[out->count].similarity_score = score;
        struct stat st;
        out->list[out->count].exists = (stat(out->list[out->count].candidate_path, &st) == 0);
        out->count++;
    }
    closedir(d);
}

void fossil_it_magic_autorecovery_token(
    const char *token,
    const char *candidates[],
    int candidate_count,
    fossil_ti_autorecovery_t *out
) {
    float best_score = 0.0f;
    const char *best = NULL;

    for (int i = 0; i < candidate_count; i++) {
        float score = fossil_it_magic_similarity(token, candidates[i]);
        if (score > best_score) {
            best_score = score;
            best = candidates[i];
        }
    }

    strncpy(out->original_token, token, sizeof(out->original_token)-1);
    out->original_token[sizeof(out->original_token)-1] = 0;

    if (best) {
        strncpy(out->recovered_token, best, sizeof(out->recovered_token)-1);
        out->recovered_token[sizeof(out->recovered_token)-1] = 0;
        out->confidence = best_score;
        out->applied = (best_score > 0.83f);
    } else {
        out->recovered_token[0] = 0;
        out->confidence = 0.0f;
        out->applied = 0;
    }
}

/* ==========================================================================
 * Danger Detection
 * ========================================================================== */

void fossil_it_magic_danger_analyze(
    const char *path,
    fossil_ti_danger_item_t *out
) {
    struct stat st;
    memset(out, 0, sizeof(*out));
    strncpy(out->target_path, path, sizeof(out->target_path)-1);

    if (stat(path, &st) != 0) {
        out->level = FOSSIL_TI_DANGER_NONE;
        return;
    }

    out->is_directory = S_ISDIR(st.st_mode);
    out->writable = (st.st_mode & S_IWUSR) != 0;
    out->contains_code = out->is_directory ? fossil_it_magic_contains_git(path) : fossil_it_magic_is_code_file(path);
    out->contains_secrets = out->is_directory ? fossil_it_magic_contains_secret(path) : 0;

    long sz = out->is_directory ? fossil_it_magic_directory_size(path) : st.st_size;
    out->large_size = (sz > 10 * 1024 * 1024);

    out->level = FOSSIL_TI_DANGER_NONE;
    if (out->contains_code) out->level = FOSSIL_TI_DANGER_HIGH;
    if (out->contains_secrets) out->level = FOSSIL_TI_DANGER_CRITICAL;
    if (out->large_size && out->level < FOSSIL_TI_DANGER_MEDIUM) out->level = FOSSIL_TI_DANGER_MEDIUM;
}

void fossil_it_magic_danger_report(
    const char *paths[],
    int path_count,
    fossil_ti_danger_report_t *report
) {
    memset(report, 0, sizeof(*report));
    fossil_ti_danger_level_t maxLevel = FOSSIL_TI_DANGER_NONE;

    for (int i = 0; i < path_count && i < 8; i++) {
        fossil_it_magic_danger_analyze(paths[i], &report->items[i]);
        report->item_count++;
        if (report->items[i].level > maxLevel)
            maxLevel = report->items[i].level;
    }

    report->overall_level = maxLevel;
    report->warning_required = (maxLevel >= FOSSIL_TI_DANGER_MEDIUM);
    report->block_recommended = (maxLevel >= FOSSIL_TI_DANGER_CRITICAL);
}
