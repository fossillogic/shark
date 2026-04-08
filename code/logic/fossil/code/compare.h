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
#ifndef FOSSIL_APP_COMMAND_COMPARE_H
#define FOSSIL_APP_COMMAND_COMPARE_H

#include "common.h"

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * Compare two files or directories and show differences
 * @param path1 First file or directory to compare
 * @param path2 Second file or directory to compare
 * @param text_diff Perform text-based difference comparison
 * @param binary_diff Perform binary difference comparison
 * @param context_lines Number of context lines to show around differences
 * @param ignore_case Ignore case differences in text comparison
 * @return 0 on success, non-zero on error
 */
int fossil_spino_compare(ccstring path1, ccstring path2,
                            bool text_diff, bool binary_diff,
                            int context_lines, bool ignore_case);

#ifdef __cplusplus
}
#endif

#endif /* FOSSIL_APP_CODE_H */
