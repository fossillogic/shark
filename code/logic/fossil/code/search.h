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
 * Date: 04/05/2013
 *
 * Copyright (C) 2013-Current Fossil Logic. All rights reserved.
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
 * Search command for the Shark tool.
 *
 * This function scans a directory tree starting at the specified path and
 * prints matching files to standard output. It supports name-based filtering
 * using a filename pattern and optional content-based filtering by searching
 * inside text files. Hidden files may be excluded in the implementation, and
 * binary files are skipped for content searches.
 *
 * @param path Root path to start searching from. If NULL, the current working
 *             directory is used.
 * @param recursive If true, subdirectories are traversed recursively.
 * @param name_pattern Pattern used to match file names. If NULL, all file names
 *                     are accepted.
 * @param content_pattern Pattern used to match file contents. If NULL, file
 *                        contents are not searched.
 * @param ignore_case If true, matching is performed without case sensitivity.
 *
 * @return 0 on success, non-zero on error.
 */
int fossil_shark_search(ccstring path,
                        bool recursive,
                        ccstring name_pattern,
                        ccstring content_pattern,
                        bool ignore_case);

#ifdef __cplusplus
}
#endif

#endif /* FOSSIL_APP_CODE_H */
