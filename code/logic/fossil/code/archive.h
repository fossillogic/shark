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
#ifndef FOSSIL_APP_COMMAND_ARCHIVE_H
#define FOSSIL_APP_COMMAND_ARCHIVE_H

#include "common.h"

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * Perform archive operations (create, extract, list)
 * @param path Path to archive file or directory to archive
 * @param create Create new archive
 * @param extract Extract existing archive
 * @param list List contents of archive
 * @param format Archive format specification (zip/tar/gz)
 * @param password Password for encrypted archives
 * @param compress_level Compression level (0-9, 0 for no compression)
 * @param stdout_output Output to stdout instead of file
 * @param exclude_pattern Pattern for files to exclude
 * @return 0 on success, non-zero on error
 */
int fossil_shark_archive(ccstring path, bool create, bool extract,
                            bool list, ccstring format, ccstring password,
                            int compress_level, bool stdout_output,
                            ccstring exclude_pattern);

#ifdef __cplusplus
}
#endif

#endif /* FOSSIL_APP_CODE_H */
