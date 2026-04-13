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
#ifndef FOSSIL_APP_COMMAND_SEARCH_H
#define FOSSIL_APP_COMMAND_SEARCH_H

#include "common.h"

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * Search for files by name patterns or content matching
 * @param path Root path to start searching from
 * @param recursive Search subdirectories recursively
 * @param name_pattern Pattern to match against file names (supports regex)
 * @param content_pattern Pattern to search within file contents (supports regex)
 * @param ignore_case Perform case-insensitive matching
 * @return 0 on success, non-zero on error
 */
int fossil_shark_search(ccstring path, bool recursive,
                        ccstring name_pattern, ccstring content_pattern,
                        bool ignore_case);

#ifdef __cplusplus
}
#endif

#endif /* FOSSIL_APP_CODE_H */
