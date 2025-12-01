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
#include <limits.h>

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
    static const i32 code_exts_count = (i32)(sizeof(code_exts) / sizeof(code_exts[0]));

    for (i32 i = 0; i < code_exts_count; i++) {
        if (fossil_io_cstring_case_compare(ext, code_exts[i]) == 0)
            return 1;
    }

    static ccstring special_names[] = {
        "Makefile", "CMakeLists.txt", "Dockerfile", "BUILD", "WORKSPACE", "SConstruct", "Rakefile", "Gemfile", "meson.build"
    };
    ccstring base = strrchr(path, '/');
    base = cnotnull(base) ? base + 1 : path;
    for (i32 i = 0; i < (i32)(sizeof(special_names)/sizeof(special_names[0])); i++) {
        if (fossil_io_cstring_compare(base, special_names[i]) == 0)
            return 1;
    }
    return 0;
}

static int fossil_it_magic_contains_git(ccstring path) {
    fossil_io_dir_iter_t it;
    if (fossil_io_dir_iter_open(&it, path) != 0)
        return 0;

    i32 found = 0;
    do {
        ccstring name = it.current.name;
        if (fossil_io_cstring_equals(name, ".git") && it.current.type == 1) { // 1=dir
            found = 1;
            break;
        }
        if (fossil_io_cstring_equals(name, ".gitignore") && (it.current.type == 0 || it.current.type == 2)) { // 0=file, 2=symlink
            found = 1;
            break;
        }
    } while (fossil_io_dir_iter_next(&it) > 0);

    fossil_io_dir_iter_close(&it);
    return found;
}

static int fossil_it_magic_contains_secret(ccstring path) {
    static ccstring secret_files[] = {
        ".env", "secret.key", "id_rsa", "id_dsa", "id_ed25519", "id_ecdsa",
        "private.pem", "private.key", "server.key", "client.key", "jwt.key",
        "credentials.json", "credentials.yml", "config.yml", "config.yaml",
        "secrets.yml", "secrets.yaml", "passwords.txt", "password.txt",
        "passwd", "shadow", "auth.json", "auth.yaml", "auth.yml",
        "api_key.txt", "api_keys.txt", "apikey.txt", "apikeys.txt",
        "token.txt", "tokens.txt", "access_token.txt", "refresh_token.txt",
        "vault.json", "vault.yml", "vault.yaml", "db_password.txt",
        "db_secrets.txt", "db_credentials.txt", "ssh_config", "pgpass",
        ".docker_secret", ".aws/credentials", ".npmrc", ".netrc",
        ".gcp_secret.json", ".azure_secret.json", ".git-credentials",
        ".pypirc", ".gem/credentials", "firebase.json", "firebase.key",
        "service-account.json", "service_account.json", "client_secret.json",
        "client_secrets.json", "google_api_key.txt", "azure_api_key.txt",
        "aws_secret_access_key.txt", "aws_access_key_id.txt"
    };
    static const i32 secret_count = (i32)(sizeof(secret_files) / sizeof(secret_files[0]));
    char candidate[1024];

    for (i32 i = 0; i < secret_count; i++) {
        if (fossil_io_dir_join(path, secret_files[i], candidate, sizeof(candidate)) == 0) {
            if (fossil_io_dir_is_file(candidate) > 0)
                return 1;
        }
    }

    fossil_io_dir_iter_t it;
    if (fossil_io_dir_iter_open(&it, path) != 0)
        return 0;

    do {
        ccstring name = it.current.name;
        if (fossil_io_cstring_icontains(name, "password") || fossil_io_cstring_icontains(name, "secret")) {
            fossil_io_dir_iter_close(&it);
            return 1;
        }
    } while (fossil_io_dir_iter_next(&it) > 0);

    fossil_io_dir_iter_close(&it);
    return 0;
}

static i64 fossil_it_magic_directory_size(ccstring path) {
    u64 total = 0;
    if (fossil_io_dir_size(path, &total) == 0)
        return (i64)total;
    return 0;
}

/* ==========================================================================
 * Similarity Utilities
 * ========================================================================== */

// Advanced Jaccard index: token-based, case-insensitive, ignores punctuation
int fossil_it_magic_jaccard_index(ccstring s1, ccstring s2) {
    if (!cnotnull(s1) || !cnotnull(s2)) return 0;

    char tokens1[32][32], tokens2[32][32];
    i32 count1 = 0, count2 = 0;

    ccstring p = s1;
    while (*p && count1 < 32) {
        while (*p && !isalnum((unsigned char)*p)) p++;
        i32 i = 0;
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
        i32 i = 0;
        while (*p && isalnum((unsigned char)*p) && i < 31)
            tokens2[count2][i++] = tolower((unsigned char)*p++);
        if (i) {
            tokens2[count2][i] = 0;
            count2++;
        }
    }

    i32 match = 0;
    i32 used[32] = {0};
    for (i32 i = 0; i < count1; i++) {
        for (i32 j = 0; j < count2; j++) {
            if (!used[j] && fossil_io_cstring_compare(tokens1[i], tokens2[j]) == 0) {
                match++;
                used[j] = 1;
                break;
            }
        }
    }
    i32 total = count1 + count2 - match;
    return total ? (100 * match / total) : 0;
}

// Advanced Levenshtein: case-insensitive, transpositions (Damerau-Levenshtein)
int fossil_it_magic_levenshtein_distance(ccstring s1, ccstring s2) {
    if (!cnotnull(s1) || !cnotnull(s2)) return INT_MAX;
    i32 len1 = (i32)strlen(s1), len2 = (i32)strlen(s2);

    i32 *dp = (i32 *)fossil_sys_memory_calloc((size_t)(len1 + 2) * (size_t)(len2 + 2), sizeof(i32));
    if (!cnotnull(dp)) return INT_MAX;

    for (i32 j = 0; j <= len2; j++) dp[(0)*(len2+2)+(j)] = j;

    for (i32 i = 1; i <= len1; i++) {
        for (i32 j = 1; j <= len2; j++) {
            i32 cost = (tolower((unsigned char)s1[i - 1]) == tolower((unsigned char)s2[j - 1])) ? 0 : 1;
            i32 del = DP(i - 1, j) + 1;
            i32 ins = DP(i, j - 1) + 1;
            i32 sub = DP(i - 1, j - 1) + cost;
            i32 min = del < ins ? del : ins;
            min = (min < sub) ? min : sub;
            DP(i, j) = min;

            if (i > 1 && j > 1 &&
                tolower((unsigned char)s1[i - 1]) == tolower((unsigned char)s2[j - 2]) &&
                tolower((unsigned char)s1[i - 2]) == tolower((unsigned char)s2[j - 1])) {
                DP(i, j) = DP(i, j) < (DP(i - 2, j - 2) + cost) ? DP(i, j) : (DP(i - 2, j - 2) + cost);
            }
        }
    }

    i32 result = DP(len1, len2);
    fossil_sys_memory_free(dp);
    #undef DP
    return result;
}

f32 fossil_it_magic_similarity(ccstring a, ccstring b) {
    if (!cnotnull(a) || !cnotnull(b)) return 0.0f;
    i32 len_a = (i32)strlen(a), len_b = (i32)strlen(b);
    if (len_a == 0 && len_b == 0) return 1.0f;

    i32 dist = fossil_it_magic_levenshtein_distance(a, b);
    i32 max_len = len_a > len_b ? len_a : len_b;
    f32 sim = 1.0f - ((f32)dist / (f32)max_len);

    i32 jaccard = fossil_it_magic_jaccard_index(a, b);
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
    i32 num_commands,
    fossil_ti_reason_t *out_reason
) {
    if (!cnotnull(input) || !cnotnull(commands) || num_commands <= 0) return cnull;

    ccstring best_match = cnull;
    f32 best_score = 0.0f;
    i32 best_distance = INT_MAX;
    i32 best_jaccard = 0;
    i32 best_prefix = 0;
    i32 best_suffix = 0;
    i32 best_case_insensitive = 0;

    for (i32 i = 0; i < num_commands; i++) {
        if (!cnotnull(commands[i])) continue;

        i32 distance = fossil_it_magic_levenshtein_distance(input, commands[i]);
        i32 jaccard = fossil_it_magic_jaccard_index(input, commands[i]);
        i32 prefix = fossil_io_cstring_starts_with(input, commands[i]);
        i32 suffix = (strlen(input) <= strlen(commands[i]) &&
                     fossil_io_cstring_case_ends_with(commands[i], input)) ? 1 : 0;
        i32 case_insensitive = fossil_io_cstring_iequals(input, commands[i]);
        i32 exact = fossil_io_cstring_equals(input, commands[i]);

        f32 sim = fossil_it_magic_similarity(input, commands[i]);
        f32 score = sim;

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
        out_reason->reason = (best_score >= 0.99f) ? "Exact match" :
                     (best_score >= 0.95f) ? "Strong semantic match" :
                     (best_score >= 0.90f) ? "Strong semantic and token match" :
                     (best_score >= 0.85f) ? "High semantic and token similarity" :
                     (best_score >= 0.80f) ? "Good semantic similarity" :
                     (best_score >= 0.75f) ? "Moderate semantic similarity" :
                     (best_score >= 0.70f) ? "Close semantic match" :
                     best_prefix && best_suffix ? "Prefix and suffix match" :
                     best_prefix ? "Prefix match" :
                     best_suffix ? "Suffix match" :
                     best_case_insensitive ? "Case-insensitive match" :
                     (best_jaccard >= 50) ? "Token overlap match" :
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
    fossil_io_dir_iter_t it;
    if (fossil_io_dir_iter_open(&it, base_dir) != 0)
        return;

    struct {
        char name[PATH_MAX];
        f32 score;
        int exists;
    } candidates[32];

    f32 best_score = 0.0f;
    i32 idx = 0;

    while (fossil_io_dir_iter_next(&it) > 0 && idx < 32) {
        ccstring entry_name = it.current.name;
        if (!strcmp(entry_name, ".") || !strcmp(entry_name, ".."))
            continue;

        f32 score = fossil_it_magic_similarity(bad_path, entry_name);

        if (fossil_io_cstring_case_starts_with(bad_path, entry_name))
            score += 0.10f;
        if (strlen(bad_path) <= strlen(entry_name) &&
            fossil_io_cstring_case_ends_with(entry_name, bad_path))
            score += 0.07f;

        if (score < 0.18f) continue;

        if (fossil_io_dir_join(base_dir, entry_name, candidates[idx].name, sizeof(candidates[idx].name)) != 0)
            continue;

        candidates[idx].exists = (fossil_io_dir_exists(candidates[idx].name) > 0 || fossil_io_dir_is_file(candidates[idx].name) > 0);
        candidates[idx].score = score;
        if (score > best_score) {
            best_score = score;
        }
        idx++;
    }
    fossil_io_dir_iter_close(&it);

    for (i32 i = 0; i < idx - 1; i++) {
        for (i32 j = i + 1; j < idx; j++) {
            if (candidates[j].score > candidates[i].score) {
                struct { char name[PATH_MAX]; f32 score; int exists; } tmp;
                memcpy(&tmp, &candidates[i], sizeof(tmp));
                memcpy(&candidates[i], &candidates[j], sizeof(tmp));
                memcpy(&candidates[j], &tmp, sizeof(tmp));
            }
        }
    }
    for (i32 i = 0; i < idx; i++) {
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
    i32 candidate_count,
    fossil_ti_autorecovery_t *out
) {
    f32 best_score = 0.0f;
    i32 best_idx = -1;
    f32 second_best_score = 0.0f;
    i32 second_best_idx = -1;

    for (i32 i = 0; i < candidate_count; i++) {
        f32 score = fossil_it_magic_similarity(token, candidates[i]);
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

    // Use safe string copy for all output fields
    cstring orig = fossil_io_cstring_copy_safe(token, sizeof(out->original_token)-1);
    strncpy(out->original_token, orig ? orig : "", sizeof(out->original_token)-1);
    out->original_token[sizeof(out->original_token)-1] = cterm;
    if (orig) fossil_io_cstring_free_safe(&orig);

    if (best_idx >= 0) {
        cstring best = fossil_io_cstring_copy_safe(candidates[best_idx], sizeof(out->first_best_token)-1);
        strncpy(out->first_best_token, best ? best : "", sizeof(out->first_best_token)-1);
        out->first_best_token[sizeof(out->first_best_token)-1] = cterm;
        if (best) fossil_io_cstring_free_safe(&best);

        out->first_best_confidence = best_score;

        cstring rec = fossil_io_cstring_copy_safe(candidates[best_idx], sizeof(out->recovered_token)-1);
        strncpy(out->recovered_token, rec ? rec : "", sizeof(out->recovered_token)-1);
        out->recovered_token[sizeof(out->recovered_token)-1] = cterm;
        if (rec) fossil_io_cstring_free_safe(&rec);

        out->confidence = best_score;
        out->applied = (best_score > 0.80f);

        if (second_best_idx >= 0) {
            cstring second = fossil_io_cstring_copy_safe(candidates[second_best_idx], sizeof(out->second_best_token)-1);
            strncpy(out->second_best_token, second ? second : "", sizeof(out->second_best_token)-1);
            out->second_best_token[sizeof(out->second_best_token)-1] = cterm;
            if (second) fossil_io_cstring_free_safe(&second);

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
    fossil_sys_memory_zero(out, sizeof(*out));
    // Use safe copy for target_path
    cstring safe_path = fossil_io_cstring_copy_safe(path, sizeof(out->target_path)-1);
    strncpy(out->target_path, safe_path ? safe_path : "", sizeof(out->target_path)-1);
    out->target_path[sizeof(out->target_path)-1] = cterm;
    if (safe_path) fossil_io_cstring_free_safe(&safe_path);

    i32 is_dir = fossil_io_dir_is_directory(path);
    i32 is_file = fossil_io_dir_is_file(path);
    i32 is_symlink = fossil_io_dir_is_symlink(path);

    out->is_directory = (is_dir > 0);
    out->is_symlink = (is_symlink > 0);

    // Permissions
    #ifdef _WIN32
        out->writable = (_access(path, 2) == 0);
        out->world_writable = 0;
    #else
        struct stat st;
        if (stat(path, &st) == 0) {
            out->writable = (st.st_mode & S_IWUSR) != 0;
            out->world_writable = (st.st_mode & S_IWOTH) != 0;
        } else {
            out->writable = 0;
            out->world_writable = 0;
        }
    #endif

    // Code/secrets detection
    out->contains_code = out->is_directory ? fossil_it_magic_contains_git(path) : fossil_it_magic_is_code_file(path);
    out->contains_secrets = out->is_directory ? fossil_it_magic_contains_secret(path) : 0;

    // Size
    u64 sz = 0;
    #ifndef _WIN32
    struct stat st;
    #endif
    if (out->is_directory) {
        fossil_io_dir_size(path, &sz);
    } else if (is_file > 0) {
        #ifdef _WIN32
            struct _stat st;
            if (_stat(path, &st) == 0)
                sz = (u64)st.st_size;
        #else
            if (stat(path, &st) == 0)
                sz = (u64)st.st_size;
        #endif
    }
    out->large_size = (sz > 10 * 1024 * 1024);

    // Suspicious extension
    static ccstring danger_exts[] = { ".exe", ".dll", ".bin", ".sh", ".bat", ".cmd", ".scr", ".pif", ".com", ".js", ".vbs", ".elf" };
    out->suspicious_extension = 0;
    if (!out->is_directory && is_file > 0) {
        ccstring ext = strrchr(path, '.');
        if (cnotnull(ext)) {
            for (i32 i = 0; i < (i32)(sizeof(danger_exts)/sizeof(danger_exts[0])); i++) {
                if (fossil_io_cstring_case_compare(ext, danger_exts[i]) == 0) {
                    out->suspicious_extension = 1;
                    break;
                }
            }
        }
    // Recently modified
    out->recently_modified = 0;
    u64 mod_time = 0;
    #ifdef _WIN32
        struct _stat st;
        if (_stat(path, &st) == 0)
            mod_time = (u64)st.st_mtime;
    #else
        if (stat(path, &st) == 0)
            mod_time = (u64)st.st_mtime;
    #endif
    u64 now = (u64)time(cnull);
    if (mod_time && now && (now > mod_time) && ((now - mod_time) < 24 * 3600))
        out->recently_modified = 1;
    if (mod_time && now && (now > mod_time) && ((now - mod_time) < 24 * 3600))
        out->recently_modified = 1;

    // Contains suspicious files (use directory iterator)
    out->contains_suspicious_files = 0;
    if (out->is_directory) {
        fossil_io_dir_iter_t it;
        if (fossil_io_dir_iter_open(&it, path) == 0) {
            while (fossil_io_dir_iter_next(&it) > 0) {
                ccstring ext = strrchr(it.current.name, '.');
                if (cnotnull(ext)) {
                    for (i32 i = 0; i < (i32)(sizeof(danger_exts)/sizeof(danger_exts[0])); i++) {
                        if (fossil_io_cstring_case_compare(ext, danger_exts[i]) == 0) {
                            out->contains_suspicious_files = 1;
                            break;
                        }
                    }
                }
                if (out->contains_suspicious_files)
                    break;
            }
            fossil_io_dir_iter_close(&it);
        }
    }

    i32 score = 0;
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
    i32 path_count,
    fossil_ti_danger_report_t *report
) {
    fossil_sys_memory_zero(report, sizeof(*report));
    fossil_ti_danger_level_t maxLevel = FOSSIL_TI_DANGER_NONE;
    i32 total_score = 0;

    for (i32 i = 0; i < path_count && i < 8; i++) {
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
}
