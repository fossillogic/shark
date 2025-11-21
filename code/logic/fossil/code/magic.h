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
#ifndef FOSSIL_APP_MAGIC_H
#define FOSSIL_APP_MAGIC_H

#include "common.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ==========================================================================
 * Path Suggestion Types
 * ========================================================================== */

/**
 * @brief A scored path suggestion result.
 */
typedef struct fossil_ti_path_suggestion_s {
    char  candidate_path[512];   /**< Valid filesystem path */
    float similarity_score;      /**< 0.0 - 1.0 ("edit distance" ↔ "semantic similarity") */
    int   exists;                /**< Non-zero if path exists on disk */
} fossil_ti_path_suggestion_t;

/**
 * @brief Ranked list of possible auto-corrections for a single incorrect path.
 */
typedef struct fossil_ti_path_suggestion_set_s {
    fossil_ti_path_suggestion_t list[16]; /**< Up to 16 ranked matches */
    int count;                             /**< Number of valid entries in list */
} fossil_ti_path_suggestion_set_t;

/**
 * @brief High-level wrapper containing path suggestions for all arguments.
 */
typedef struct fossil_ti_path_ai_report_s {
    fossil_ti_path_suggestion_set_t sets[8]; /**< Up to 8 tokens needing help */
    int set_count;                            /**< Number of valid sets */
} fossil_ti_path_ai_report_t;

/* ==========================================================================
 * Auto-Recovery Types
 * ========================================================================== */

/**
 * @brief Represents an automatically recovered or suggested token.
 */
typedef struct fossil_ti_autorecovery_s {
    char  original_token[256];        /**< Input token */
    char  recovered_token[256];       /**< Suggested correction */
    float confidence;                 /**< 0.0 - 1.0 confidence score */
    int   applied;                    /**< 1 = auto-applied, 0 = manual review */
    char  first_best_token[256];        /**< Best suggestion */
    float first_best_confidence;      /**< Confidence score for best suggestion */
    char  second_best_token[256];     /**< Second-best suggestion */
    float second_best_confidence;     /**< Confidence score for second-best */
} fossil_ti_autorecovery_t;

/* ==========================================================================
 * Danger Detection Types
 * ========================================================================== */

/**
 * @brief Enumerated danger levels for filesystem operations.
 */
typedef enum {
    FOSSIL_TI_DANGER_NONE = 0,     /**< Safe */
    FOSSIL_TI_DANGER_LOW,          /**< Mild (overwrites small file) */
    FOSSIL_TI_DANGER_MEDIUM,       /**< Questionable (move large tree) */
    FOSSIL_TI_DANGER_HIGH,         /**< Risky but reversible */
    FOSSIL_TI_DANGER_CRITICAL      /**< Destructive (rm -r, wiping codebase) */
} fossil_ti_danger_level_t;

/**
 * @brief Structured danger analysis results for a single path or target.
 */
typedef struct fossil_ti_danger_item_s {
    char target_path[512];              /**< Path being analyzed */
    fossil_ti_danger_level_t level;     /**< Danger level */

    int is_directory;                   /**< Non-zero if directory */
    int contains_code;                  /**< Non-zero if contains code files (.c, .h, .cpp, .py, etc.) */
    int contains_vcs;                   /**< Non-zero if VCS detected (.git, .svn) */
    int contains_secrets;               /**< Non-zero if secret files detected (.env, .key, .pem) */
    int large_size;                     /**< Non-zero if large (> threshold) */
    int writable;                       /**< Non-zero if writable */
    int world_writable;                 /**< Non-zero if world-writable */
    int is_symlink;                     /**< Non-zero if symlink */
    int suspicious_extension;           /**< Non-zero if file has suspicious extension (.exe, .dll, etc.) */
    int recently_modified;              /**< Non-zero if modified in last 24 hours */
    int contains_suspicious_files;      /**< Non-zero if directory contains suspicious files */
} fossil_ti_danger_item_t;

/**
 * @brief Combined safety analysis for multi-target command operations.
 */
typedef struct fossil_ti_danger_report_s {
    fossil_ti_danger_item_t items[8];   /**< Individual path analyses */
    int item_count;                      /**< Number of valid items */

    fossil_ti_danger_level_t overall_level; /**< Max level across all items */

    int block_recommended;               /**< Non-zero = halt unless --force present */
    int warning_required;                /**< Non-zero = display multi-line warning */
} fossil_ti_danger_report_t;

/* ==========================================================================
 * TI Reasoning / AI Metadata
 * ========================================================================== */

/**
 * @brief Advanced metadata for reasoning, audit, and debug.
 */
typedef struct fossil_ti_reason_s {
    const char *input;                 /**< Original input */
    const char *suggested;             /**< Suggested correction */
    int         edit_distance;         /**< Levenshtein distance */
    float       confidence_score;      /**< 0.0 - 1.0 confidence */
    int         jaccard_index;         /**< 0-100 token overlap similarity */
    int         prefix_match;          /**< 1 if input is prefix of suggested */
    int         suffix_match;          /**< 1 if input is suffix of suggested */
    int         case_insensitive;      /**< 1 if match is case-insensitive */
    const char *reason;                /**< Human-readable explanation */
} fossil_ti_reason_t;

/* ==========================================================================
 * Similarity Utilities
 * ========================================================================== */

/**
 * @brief Compute Jaccard Index (token overlap) between two strings.
 *
 * This function splits both input strings into tokens (words), ignoring punctuation
 * and case. It then counts the number of matching tokens between the two sets,
 * and computes the Jaccard index as (matches / union of tokens), scaled to 0-100.
 * Used to measure token-level similarity, robust to word order and punctuation.
 */
int fossil_it_magic_jaccard_index(const char *s1, const char *s2);

/**
 * @brief Compute Levenshtein distance between two strings.
 *
 * Calculates the minimum number of single-character edits (insertions, deletions,
 * substitutions, and transpositions) required to change one string into another.
 * This implementation is case-insensitive and supports transpositions (Damerau-Levenshtein).
 * Used for fuzzy matching and typo correction.
 */
int fossil_it_magic_levenshtein_distance(const char *s1, const char *s2);

/**
 * @brief Compute a normalized similarity score (0.0 - 1.0) between two strings.
 *
 * Combines Levenshtein distance, Jaccard index, and prefix/suffix matches to produce
 * a floating-point similarity score. The score is higher for strings that are
 * semantically and structurally similar, and is capped between 0.0 and 1.0.
 * Used for ranking candidates in suggestions and corrections.
 */
float fossil_it_magic_similarity(const char *a, const char *b);

/* ==========================================================================
 * Command Suggestion
 * ========================================================================== */

/**
 * @brief Suggest the closest matching command from a list of candidates.
 *
 * Iterates through the candidate command list, scoring each using fuzzy matching
 * (Levenshtein, Jaccard, prefix/suffix, case-insensitive, exact match).
 * Returns the best match if its score exceeds a threshold (0.7), otherwise NULL.
 * Optionally fills out_reason with detailed scoring and explanation.
 *
 * @param input Input string to match
 * @param commands Array of candidate strings
 * @param num_commands Number of candidates
 * @param out_reason Optional pointer to fossil_ti_reason_t for detailed scoring
 * @return Pointer to best matching command, or NULL if none meets threshold
 */
const char *fossil_it_magic_suggest_command(
    const char *input,
    const char **commands,
    int num_commands,
    fossil_ti_reason_t *out_reason
);

/* ==========================================================================
 * Path Auto-Correction
 * ========================================================================== */

/**
 * @brief Suggest paths based on similarity to a “bad” path.
 *
 * Scans the base directory, comparing each entry to the bad_path using fuzzy
 * similarity metrics. Collects up to 16 top-ranked suggestions, storing their
 * path, similarity score, and existence status in the output set.
 * Used for typo correction and auto-completion of filesystem paths.
 *
 * @param bad_path The incorrect or misspelled path
 * @param base_dir Directory to search for candidates
 * @param out Output suggestion set (filled by function)
 */
void fossil_it_magic_path_suggest(
    const char *bad_path,
    const char *base_dir,
    fossil_ti_path_suggestion_set_t *out
);

/**
 * @brief Recover a token from a list of candidates.
 *
 * Compares the input token to each candidate using fuzzy similarity. Selects
 * the best match (and optionally the second-best), storing the recovered token,
 * confidence score, and whether it should be auto-applied. Used for typo
 * correction and auto-recovery of command-line arguments.
 *
 * @param token The input token to recover
 * @param candidates Array of candidate strings
 * @param candidate_count Number of candidates
 * @param out Output structure with recovery result
 */
void fossil_it_magic_autorecovery_token(
    const char *token,
    const char *candidates[],
    int candidate_count,
    fossil_ti_autorecovery_t *out
);

/* ==========================================================================
 * Danger Detection
 * ========================================================================== */

/**
 * @brief Analyze a single path for potential danger.
 *
 * Inspects the given path for risk factors: code files, secrets, large size,
 * world-writable permissions, symlinks, suspicious extensions, recent modification,
 * and presence of suspicious files. Assigns a danger level based on cumulative score.
 * Used to warn or block risky filesystem operations.
 *
 * @param path Path to analyze
 * @param out Output structure with analysis result
 */
void fossil_it_magic_danger_analyze(
    const char *path,
    fossil_ti_danger_item_t *out
);

/**
 * @brief Analyze multiple paths for potential danger and summarize.
 *
 * Runs danger analysis on each path, aggregates the highest danger level and
 * total score. Sets flags to recommend blocking or warning, and provides a
 * summary reason string. Used for batch safety checks before executing commands.
 *
 * @param paths Array of paths to analyze
 * @param path_count Number of paths
 * @param report Output structure with combined analysis
 */
void fossil_it_magic_danger_report(
    const char *paths[],
    int path_count,
    fossil_ti_danger_report_t *report
);

#ifdef __cplusplus
}
#endif

#endif /* FOSSIL_APP_CODE_H */
