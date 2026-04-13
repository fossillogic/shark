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
#ifndef FOSSIL_APP_COMMAND_INTROSPECT_H
#define FOSSIL_APP_COMMAND_INTROSPECT_H

#include "common.h"

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * Examine file contents, type, or metadata
 * @param path Path to the file to introspect
 * @param show_head_lines Number of lines from the start to display (0 for none)
 * @param show_tail_lines Number of lines from the end to display (0 for none)
 * @param count_lines_words_bytes Count lines, words, and bytes if true
 * @param count_lines_only Count total lines only if true
 * @param show_size Display file size in bytes and human-readable format
 * @param show_time Display file timestamps (modified, created, accessed)
 * @param show_file_type Determine file type (e.g., MIME or description)
 * @param find_pattern Search for string or pattern in file
 * @param media Output structured media format
 * @return 0 on success, non-zero on error
 */
int fossil_spino_introspect(
    ccstring path,
    int show_head_lines,
    int show_tail_lines,
    bool count_lines_words_bytes,
    bool count_lines_only,
    bool show_size,
    bool show_time,
    bool show_file_type,
    ccstring find_pattern,
    const char* media /* "text", "json", "fson" */
);

#ifdef __cplusplus
}
#endif

#endif /* FOSSIL_APP_CODE_H */
