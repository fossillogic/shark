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
#ifndef FOSSIL_APP_COMMAND_SPLIT_H
#define FOSSIL_APP_COMMAND_SPLIT_H

#include "common.h"

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * Split files into smaller segments
 * @param file_path Path to the file to split
 * @param split_by_lines Split by line count (lines per segment)
 * @param split_by_bytes Split by byte size (bytes per segment)
 * @param num_segments Number of segments to create
 * @param output_prefix Output file prefix for split segments
 * @param suffix_digits Number of digits for suffix (0 for none)
 * @param numeric_suffix Use numeric suffix instead of alphabetic
 * @param delimiter Custom delimiter character (NULL for default)
 * @param dry_run Preview split without executing (--dry-run)
 * @return 0 on success, non-zero on error
 */
int fossil_spino_split(ccstring file_path, size_t split_by_lines,
                        size_t split_by_bytes, size_t num_segments,
                        ccstring output_prefix, int suffix_digits,
                        bool numeric_suffix, ccstring delimiter,
                        bool dry_run);

#ifdef __cplusplus
}
#endif

#endif /* FOSSIL_APP_CODE_H */
