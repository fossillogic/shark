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
#ifndef FOSSIL_APP_COMMAND_SHOW_H
#define FOSSIL_APP_COMMAND_SHOW_H

#include "common.h"

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * Display files and directories with various formatting options
 * @param path The file or directory path to display
 * @param show_all Show hidden files and directories
 * @param long_format Use detailed long format listing
 * @param human_readable Show file sizes in human readable format
 * @param recursive Recursively list subdirectories
 * @param format Output format specification
 * @param show_time Display timestamps
 * @param depth Current depth in recursive listing
 * @param sort_key Sort by: "desc" or "asc"
 * @param match_pattern Filter by name pattern
 * @param size_filter Filter by size (e.g. ">1MB")
 * @param type_filter Filter by type: "file", "dir", "link"
 * @return 0 on success, non-zero on error
 */
int fossil_shark_show(ccstring path, bool show_all, bool long_format,
                        bool human_readable, bool recursive,
                        ccstring format, bool show_time, int depth,
                        ccstring sort_key, ccstring match_pattern,
                        ccstring size_filter, ccstring type_filter);

#ifdef __cplusplus
}
#endif

#endif /* FOSSIL_APP_CODE_H */
