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
#ifndef FOSSIL_APP_COMMAND_GRAMMAR_H
#define FOSSIL_APP_COMMAND_GRAMMAR_H

#include "common.h"

#ifdef __cplusplus
extern "C"
{
#endif

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

#ifdef __cplusplus
}
#endif

#endif /* FOSSIL_APP_CODE_H */
