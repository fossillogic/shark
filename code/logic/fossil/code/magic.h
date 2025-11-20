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

#include "commands.h"

#ifdef __cplusplus
extern "C" {
#endif

#pragma once
#include <stdint.h>

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
    char  original_token[256];   /**< Input token */
    char  recovered_token[256];  /**< Suggested correction */
    float confidence;            /**< 0.0 - 1.0 confidence score */
    int   applied;               /**< 1 = auto-applied, 0 = manual review */
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

// AI magic helpers here

#ifdef __cplusplus
}
#endif

#endif /* FOSSIL_APP_CODE_H */