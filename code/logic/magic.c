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
#include <unistd.h>
#include <ctype.h>
#include <stdint.h>
#include <math.h>


/* Remove enum redefining PATH_MAX, rely on limits.h definition */

#define DP(i, j) dp[(i) * (len2 + 1) + (j)]

/* ==========================================================================
 * Static Helpers (internal)
 * ========================================================================== */

static int fossil_it_magic_is_code_file(ccstring path) {
    ccstring ext = strrchr(path, '.');
    if (!cnotnull(ext)) return 0;

    static ccstring code_exts[] = {
        ".c", ".h", ".cpp", ".hpp", ".cc", ".cxx", ".hxx", ".hh",
        ".py", ".pyw", ".ipynb", ".pyc", ".pyo", ".pyd",
        ".java", ".class", ".jar", ".jad", ".jmod",
        ".cs", ".vb", ".fs",
        ".go", ".mod", ".sum",
        ".rs", ".rlib", ".toml",
        ".js", ".jsx", ".mjs", ".cjs",
        ".ts", ".tsx",
        ".php", ".phtml", ".php3", ".php4", ".php5", ".phps",
        ".rb", ".erb", ".rake", ".gemspec",
        ".pl", ".pm", ".pod", ".t",
        ".swift",
        ".kt", ".kts",
        ".scala", ".sc",
        ".sh", ".bash", ".zsh", ".csh", ".tcsh", ".ksh",
        ".bat", ".cmd", ".ps1", ".psm1",
        ".lua",
        ".sql", ".sqlite", ".db",
        ".html", ".htm", ".xhtml",
        ".css", ".scss", ".less",
        ".xml", ".xsd", ".xslt",
        ".json", ".yaml", ".yml",
        ".dart",
        ".groovy", ".gradle",
        ".r", ".R", ".Rmd",
        ".m", ".mm",
        ".asm", ".s", ".S",
        ".v", ".vh", ".sv", ".vhd", ".vhdl",
        ".coffee",
        ".clj", ".cljs", ".cljc", ".edn",
        ".hs", ".lhs", ".ghc",
        ".ml", ".mli", ".ocaml",
        ".ada", ".adb", ".ads",
        ".for", ".f90", ".f95", ".f03", ".f08", ".f", ".f77",
        ".pro", ".pl", ".tcl",
        ".tex", ".sty", ".cls",
        ".nim",
        ".cr",
        ".ex", ".exs",
        ".elm",
        ".erl", ".hrl",
        ".lisp", ".el", ".scm", ".cl", ".lsp",
        ".pas", ".pp", ".p",
        ".d",
        ".vala",
        ".vbs",
        ".awk",
        ".ps",
        ".raku", ".pl6", ".pm6",
        ".sol",
        ".cmake",
        ".build", ".options",
        ".dockerfile",
        ".ini", ".conf", ".cfg",
        ".toml",
        ".tsx",
        ".sln", ".vcxproj", ".csproj",
        ".xcodeproj", ".xcworkspace",
        ".bazel", ".bzl", "BUILD", "WORKSPACE",
        ".ninja",
        ".gitignore", ".gitattributes", ".editorconfig", ".env"
    };
    static const int code_exts_count = sizeof(code_exts) / sizeof(code_exts[0]);

    for (int i = 0; i < code_exts_count; i++) {
        if (fossil_io_cstring_case_compare(ext, code_exts[i]) == 0)
            return 1;
    }

    static ccstring special_names[] = {
        "Makefile", "CMakeLists.txt", "Dockerfile", "BUILD", "WORKSPACE", "SConstruct", "Rakefile", "Gemfile"
    };
    ccstring base = strrchr(path, '/');
    base = cnotnull(base) ? base + 1 : path;
    for (int i = 0; i < (int)(sizeof(special_names)/sizeof(special_names[0])); i++) {
        if (fossil_io_cstring_compare(base, special_names[i]) == 0)
            return 1;
    }
    return 0;
}

static int fossil_it_magic_contains_git(ccstring path) {
    char buf[PATH_MAX];
    snprintf(buf, sizeof(buf), "%s/.git", path);
    struct stat st;
    if (stat(buf, &st) == 0 && S_ISDIR(st.st_mode))
        return 1;
    snprintf(buf, sizeof(buf), "%s/.gitignore", path);
    if (stat(buf, &st) == 0)
        return 1;
    return 0;
}

static int fossil_it_magic_contains_secret(ccstring path) {
    char buf[PATH_MAX];
    static ccstring secret_files[] = {
        ".env", "secret.key", "id_rsa", "private.pem", "credentials.json", "config.yml", "secrets.yml"
    };
    static const int secret_count = sizeof(secret_files) / sizeof(secret_files[0]);
    for (int i = 0; i < secret_count; i++) {
        snprintf(buf, sizeof(buf), "%s/%s", path, secret_files[i]);
        struct stat st;
        if (stat(buf, &st) == 0)
            return 1;
    }
    DIR *d = opendir(path);
    if (!cnotnull(d)) return 0;
    struct dirent *ent;
    while ((ent = readdir(d)) != cnull) {
        if (fossil_io_cstring_icontains(ent->d_name, "password") || fossil_io_cstring_icontains(ent->d_name, "secret")) {
            closedir(d);
            return 1;
        }
    }
    closedir(d);
    return 0;
}

static long fossil_it_magic_directory_size(ccstring path) {
    long total = 0;
    DIR *d = opendir(path);
    if (!cnotnull(d)) return 0;

    struct dirent *ent;
    struct stat st;
    char full[PATH_MAX];

    while ((ent = readdir(d)) != cnull) {
        if (!strcmp(ent->d_name, ".") || !strcmp(ent->d_name, ".."))
            continue;

        snprintf(full, sizeof(full), "%s/%s", path, ent->d_name);
        if (stat(full, &st) == 0) {
            if (S_ISDIR(st.st_mode)) {
                total += fossil_it_magic_directory_size(full);
            } else {
                total += st.st_size;
            }
        }
    }
    closedir(d);
    return total;
}

/* ==========================================================================
 * Similarity Utilities
 * ========================================================================== */

// Advanced Jaccard index: token-based, case-insensitive, ignores punctuation
int fossil_it_magic_jaccard_index(ccstring s1, ccstring s2) {
    if (!cnotnull(s1) || !cnotnull(s2)) return 0;

    char tokens1[32][32], tokens2[32][32];
    int count1 = 0, count2 = 0;

    ccstring p = s1;
    while (*p && count1 < 32) {
        while (*p && !isalnum((unsigned char)*p)) p++;
        int i = 0;
        while (*p && isalnum((unsigned char)*p) && i < 31)
            tokens1[count1][i++] = tolower((unsigned char)*p++);
        if (i) {
            tokens1[count1][i] = 0;
            count1++;
        }
    }
    p = s2;
    while (*p && count2 < 32) {
        while (*p && !isalnum((unsigned char)*p)) p++;
        int i = 0;
        while (*p && isalnum((unsigned char)*p) && i < 31)
            tokens2[count2][i++] = tolower((unsigned char)*p++);
        if (i) {
            tokens2[count2][i] = 0;
            count2++;
        }
    }

    int match = 0;
    int used[32] = {0};
    for (int i = 0; i < count1; i++) {
        for (int j = 0; j < count2; j++) {
            if (!used[j] && fossil_io_cstring_compare(tokens1[i], tokens2[j]) == 0) {
                match++;
                used[j] = 1;
                break;
            }
        }
    }
    int total = count1 + count2 - match;
    return total ? (100 * match / total) : 0;
}

// Advanced Levenshtein: case-insensitive, transpositions (Damerau-Levenshtein)
int fossil_it_magic_levenshtein_distance(ccstring s1, ccstring s2) {
    if (!cnotnull(s1) || !cnotnull(s2)) return INT_MAX;
    int len1 = (int)strlen(s1), len2 = (int)strlen(s2);

    int *dp = (int *)calloc((size_t)(len1 + 2) * (size_t)(len2 + 2), sizeof(int));
    if (!cnotnull(dp)) return INT_MAX;

    /* Use only one DP macro, do not redefine */
    for (int j = 0; j <= len2; j++) dp[(0)*(len2+2)+(j)] = j;

    for (int i = 1; i <= len1; i++) {
        for (int j = 1; j <= len2; j++) {
            int cost = (tolower((unsigned char)s1[i - 1]) == tolower((unsigned char)s2[j - 1])) ? 0 : 1;
            int del = DP(i - 1, j) + 1;
            int ins = DP(i, j - 1) + 1;
            int sub = DP(i - 1, j - 1) + cost;
            int min = del < ins ? del : ins;
            min = (min < sub) ? min : sub;
            DP(i, j) = min;

            if (i > 1 && j > 1 &&
                tolower((unsigned char)s1[i - 1]) == tolower((unsigned char)s2[j - 2]) &&
                tolower((unsigned char)s1[i - 2]) == tolower((unsigned char)s2[j - 1])) {
                DP(i, j) = DP(i, j) < (DP(i - 2, j - 2) + cost) ? DP(i, j) : (DP(i - 2, j - 2) + cost);
            }
        }
    }

    int result = DP(len1, len2);
    free(dp);
    #undef DP
    return result;
}

float fossil_it_magic_similarity(ccstring a, ccstring b) {
    if (!cnotnull(a) || !cnotnull(b)) return 0.0f;
    int len_a = (int)strlen(a), len_b = (int)strlen(b);
    if (len_a == 0 && len_b == 0) return 1.0f;

    int dist = fossil_it_magic_levenshtein_distance(a, b);
    int max_len = len_a > len_b ? len_a : len_b;
    float sim = 1.0f - ((float)dist / (float)max_len);

    int jaccard = fossil_it_magic_jaccard_index(a, b);
    sim += jaccard / 200.0f;

    if (fossil_io_cstring_case_starts_with(a, b))
        sim += 0.10f;
    if (len_a <= len_b &&
        fossil_io_cstring_case_ends_with(b, a))
        sim += 0.07f;

    if (sim > 1.0f) sim = 1.0f;
    if (sim < 0.0f) sim = 0.0f;
    return sim;
}

/* ==========================================================================
 * Command Suggestion
 * ========================================================================== */

ccstring fossil_it_magic_suggest_command(
    ccstring input,
    ccstring *commands,
    int num_commands,
    fossil_ti_reason_t *out_reason
) {
    if (!cnotnull(input) || !cnotnull(commands) || num_commands <= 0) return cnull;

    ccstring best_match = cnull;
    float best_score = 0.0f;
    int best_distance = INT_MAX;
    int best_jaccard = 0;
    int best_prefix = 0;
    int best_suffix = 0;
    int best_case_insensitive = 0;
    // Removed unused variable: int best_length = 1;

    for (int i = 0; i < num_commands; i++) {
        if (!cnotnull(commands[i])) continue;

        int distance = fossil_it_magic_levenshtein_distance(input, commands[i]);
        int jaccard = fossil_it_magic_jaccard_index(input, commands[i]);
        int prefix = fossil_io_cstring_starts_with(input, commands[i]);
        int suffix = (strlen(input) <= strlen(commands[i]) &&
                     fossil_io_cstring_case_ends_with(commands[i], input)) ? 1 : 0;
        int case_insensitive = fossil_io_cstring_iequals(input, commands[i]);
        int exact = fossil_io_cstring_equals(input, commands[i]);

        // int len = (int)strlen(commands[i]); // Removed unused variable
        float sim = fossil_it_magic_similarity(input, commands[i]);
        float score = sim;

        if (prefix) score += 0.15f;
        if (suffix) score += 0.10f;
        if (case_insensitive) score += 0.05f;
        if (exact) score += 0.20f;
        score += jaccard / 200.0f;
        if (score > 1.0f) score = 1.0f;
        if (score < 0.0f) score = 0.0f;

        if (exact ||
            score > best_score ||
            (score == best_score && distance < best_distance) ||
            (score == best_score && distance == best_distance && prefix > best_prefix)) {
            best_match = commands[i];
            best_score = score;
            best_distance = distance;
            best_jaccard = jaccard;
            best_prefix = prefix;
            best_suffix = suffix;
            best_case_insensitive = case_insensitive;
            // Removed assignment to best_length
        }
    }

    if (!cnotnull(best_match)) return cnull;

    if (cnotnull(out_reason)) {
        out_reason->input = input;
        out_reason->suggested = best_match;
        out_reason->edit_distance = best_distance;
        out_reason->confidence_score = best_score;
        out_reason->jaccard_index = best_jaccard;
        out_reason->prefix_match = best_prefix;
        out_reason->suffix_match = best_suffix;
        out_reason->case_insensitive = best_case_insensitive;
        out_reason->reason = (best_score >= 0.95f) ? "Exact or strong semantic match" :
                             (best_score >= 0.85f) ? "Strong semantic and token match" :
                             (best_score >= 0.7f) ? "Close semantic match" :
                             best_prefix ? "Prefix match" :
                             best_case_insensitive ? "Case-insensitive match" :
                             "Low confidence match";
    }

    return (best_score >= 0.7f) ? best_match : cnull;
}

/* ==========================================================================
 * Path Auto-Correction
 * ========================================================================== */

void fossil_it_magic_path_suggest(
    ccstring bad_path,
    ccstring base_dir,
    fossil_ti_path_suggestion_set_t *out
) {
    out->count = 0;
    DIR *d = opendir(base_dir);
    if (!cnotnull(d)) return;

    struct dirent *ent;
    struct {
        char name[PATH_MAX];
        float score;
        int exists;
    } candidates[32];

    float best_score = 0.0f;
    // Removed unused variable: int best_idx = -1;
    int idx = 0;

    while ((ent = readdir(d)) && idx < 32) {
        if (!strcmp(ent->d_name, ".") || !strcmp(ent->d_name, ".."))
            continue;
        float score = fossil_it_magic_similarity(bad_path, ent->d_name);

        if (fossil_io_cstring_case_starts_with(bad_path, ent->d_name))
            score += 0.10f;
        if (strlen(bad_path) <= strlen(ent->d_name) &&
            fossil_io_cstring_case_ends_with(ent->d_name, bad_path))
            score += 0.07f;

        if (score < 0.18f) continue;

        snprintf(candidates[idx].name, sizeof(candidates[idx].name), "%s/%s", base_dir, ent->d_name);
        struct stat st;
        candidates[idx].exists = (stat(candidates[idx].name, &st) == 0);
        candidates[idx].score = score;
        if (score > best_score) {
            best_score = score;
            // Removed assignment to best_idx
        }
        idx++;
    }
    closedir(d);

    for (int i = 0; i < idx - 1; i++) {
        for (int j = i + 1; j < idx; j++) {
            if (candidates[j].score > candidates[i].score) {
                /* Portable swap instead of typeof */
                struct { char name[PATH_MAX]; float score; int exists; } tmp;
                memcpy(&tmp, &candidates[i], sizeof(tmp));
                memcpy(&candidates[i], &candidates[j], sizeof(tmp));
                memcpy(&candidates[j], &tmp, sizeof(tmp));
            }
        }
    }
    for (int i = 0; i < idx; i++) {
        strncpy(out->list[out->count].candidate_path, candidates[i].name, sizeof(out->list[out->count].candidate_path)-1);
        out->list[out->count].candidate_path[sizeof(out->list[out->count].candidate_path)-1] = cterm;
        out->list[out->count].similarity_score = candidates[i].score;
        out->list[out->count].exists = candidates[i].exists;
        out->count++;
    }
}

void fossil_it_magic_autorecovery_token(
    ccstring token,
    ccstring candidates[],
    int candidate_count,
    fossil_ti_autorecovery_t *out
) {
    float best_score = 0.0f;
    int best_idx = -1;
    float second_best_score = 0.0f;
    int second_best_idx = -1;

    for (int i = 0; i < candidate_count; i++) {
        float score = fossil_it_magic_similarity(token, candidates[i]);
        if (fossil_io_cstring_case_starts_with(token, candidates[i]))
            score += 0.10f;
        if (strlen(token) <= strlen(candidates[i]) &&
            fossil_io_cstring_case_ends_with(candidates[i], token))
            score += 0.07f;

        if (score > best_score) {
            second_best_score = best_score;
            second_best_idx = best_idx;
            best_score = score;
            best_idx = i;
        } else if (score > second_best_score) {
            second_best_score = score;
            second_best_idx = i;
        }
    }

    strncpy(out->original_token, token, sizeof(out->original_token)-1);
    out->original_token[sizeof(out->original_token)-1] = cterm;

    // Fill first_best_token and first_best_confidence
    if (best_idx >= 0) {
        strncpy(out->first_best_token, candidates[best_idx], sizeof(out->first_best_token)-1);
        out->first_best_token[sizeof(out->first_best_token)-1] = cterm;
        out->first_best_confidence = best_score;

        // Fill recovered_token and confidence for compatibility
        strncpy(out->recovered_token, candidates[best_idx], sizeof(out->recovered_token)-1);
        out->recovered_token[sizeof(out->recovered_token)-1] = cterm;
        out->confidence = best_score;
        out->applied = (best_score > 0.80f);

        // Fill second_best_token and second_best_confidence
        if (second_best_idx >= 0) {
            strncpy(out->second_best_token, candidates[second_best_idx], sizeof(out->second_best_token)-1);
            out->second_best_token[sizeof(out->second_best_token)-1] = cterm;
            out->second_best_confidence = second_best_score;
        } else {
            out->second_best_token[0] = cterm;
            out->second_best_confidence = 0.0f;
        }
    } else {
        out->first_best_token[0] = cterm;
        out->first_best_confidence = 0.0f;
        out->recovered_token[0] = cterm;
        out->confidence = 0.0f;
        out->applied = 0;
        out->second_best_token[0] = cterm;
        out->second_best_confidence = 0.0f;
    }
}

/* ==========================================================================
 * Danger Detection
 * ========================================================================== */

void fossil_it_magic_danger_analyze(
    ccstring path,
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
    out->writable =
    #ifdef _WIN32
        (_access(path, 2) == 0);
    #else
        (st.st_mode & S_IWUSR) != 0;
    #endif

    out->contains_code = out->is_directory ? fossil_it_magic_contains_git(path) : fossil_it_magic_is_code_file(path);
    out->contains_secrets = out->is_directory ? fossil_it_magic_contains_secret(path) : 0;

    long sz = out->is_directory ? fossil_it_magic_directory_size(path) : (long)st.st_size;
    out->large_size = (sz > 10 * 1024 * 1024);

    out->world_writable =
    #ifdef _WIN32
        0; // Windows does not have S_IWOTH
    #else
        (st.st_mode & S_IWOTH) != 0;
    #endif

    #if defined(S_ISLNK)
        out->is_symlink = S_ISLNK(st.st_mode);
    #elif defined(_WIN32)
        out->is_symlink = 0; // Symlink detection not portable on Windows
    #else
        out->is_symlink = 0;
    #endif

    static ccstring danger_exts[] = { ".exe", ".dll", ".bin", ".sh", ".bat", ".cmd", ".scr", ".pif", ".com", ".js", ".vbs" };
    out->suspicious_extension = 0;
    if (!out->is_directory) {
        ccstring ext = strrchr(path, '.');
        if (cnotnull(ext)) {
            for (int i = 0; i < (int)(sizeof(danger_exts)/sizeof(danger_exts[0])); i++) {
                if (fossil_io_cstring_case_compare(ext, danger_exts[i]) == 0) {
                    out->suspicious_extension = 1;
                    break;
                }
            }
        }
    }

    out->recently_modified = 0;
    #ifdef _WIN32
        out->recently_modified = 0; // st_mtime may not be reliable on Windows
    #else
        time_t now = time(cnull);
        if (now != (time_t)-1 && (now - st.st_mtime) < 24 * 3600)
            out->recently_modified = 1;
    #endif

    out->contains_suspicious_files = 0;
    if (out->is_directory) {
        DIR *d = opendir(path);
        if (cnotnull(d)) {
            struct dirent *ent;
            while ((ent = readdir(d)) != cnull) {
                if (!strcmp(ent->d_name, ".") || !strcmp(ent->d_name, ".."))
                    continue;
                ccstring ext = strrchr(ent->d_name, '.');
                if (cnotnull(ext)) {
                    for (int i = 0; i < (int)(sizeof(danger_exts)/sizeof(danger_exts[0])); i++) {
                        if (fossil_io_cstring_case_compare(ext, danger_exts[i]) == 0) {
                            out->contains_suspicious_files = 1;
                            break;
                        }
                    }
                }
                if (out->contains_suspicious_files)
                    break;
            }
            closedir(d);
        }
    }

    int score = 0;
    if (out->contains_code) score += 3;
    if (out->contains_secrets) score += 5;
    if (out->large_size) score += 2;
    if (out->world_writable) score += 2;
    if (out->is_symlink) score += 1;
    if (out->suspicious_extension) score += 2;
    if (out->recently_modified) score += 1;
    if (out->contains_suspicious_files) score += 2;

    if (score >= 8)
        out->level = FOSSIL_TI_DANGER_CRITICAL;
    else if (score >= 5)
        out->level = FOSSIL_TI_DANGER_HIGH;
    else if (score >= 3)
        out->level = FOSSIL_TI_DANGER_MEDIUM;
    else if (score >= 1)
        out->level = FOSSIL_TI_DANGER_LOW;
    else
        out->level = FOSSIL_TI_DANGER_NONE;
}

void fossil_it_magic_danger_report(
    ccstring paths[],
    int path_count,
    fossil_ti_danger_report_t *report
) {
    memset(report, 0, sizeof(*report));
    fossil_ti_danger_level_t maxLevel = FOSSIL_TI_DANGER_NONE;
    int total_score = 0;

    for (int i = 0; i < path_count && i < 8; i++) {
        fossil_it_magic_danger_analyze(paths[i], &report->items[i]);
        report->item_count++;
        if (report->items[i].level > maxLevel)
            maxLevel = report->items[i].level;
        switch (report->items[i].level) {
            case FOSSIL_TI_DANGER_CRITICAL: total_score += 8; break;
            case FOSSIL_TI_DANGER_HIGH: total_score += 5; break;
            case FOSSIL_TI_DANGER_MEDIUM: total_score += 3; break;
            case FOSSIL_TI_DANGER_LOW: total_score += 1; break;
            default: break;
        }
    }

    report->overall_level = maxLevel;
    report->warning_required = (maxLevel >= FOSSIL_TI_DANGER_MEDIUM || total_score >= 10);
    report->block_recommended = (maxLevel >= FOSSIL_TI_DANGER_CRITICAL || total_score >= 16);

    // If you want to provide a summary, add a summary_reason field to the struct.
}
