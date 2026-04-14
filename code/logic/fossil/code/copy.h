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
#ifndef FOSSIL_APP_COMMAND_COPY_H
#define FOSSIL_APP_COMMAND_COPY_H

#include "common.h"

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * Copy files or directories with various options
 * @param src Source path to copy from
 * @param dest Destination path to copy to
 * @param recursive Copy directories recursively (--recursive)
 * @param update Copy only when source is newer (--update)
 * @param preserve Preserve file attributes and permissions (--preserve)
 * @param checksum Verify integrity after copy (--checksum)
 * @param sparse Preserve sparse files (--sparse)
 * @param link Create hardlinks instead of copies (--link)
 * @param reflink Use copy-on-write if available (--reflink)
 * @param progress Show progress during copy (--progress)
 * @param dry_run Simulate the copy without executing (--dry-run)
 * @param exclude_pattern Pattern for files to exclude (--exclude)
 * @param include_pattern Pattern for files to include (--include)
 * @return 0 on success, non-zero on error
 */
int fossil_shark_copy(ccstring src, ccstring dest,
                        bool recursive, bool update, bool preserve,
                        bool checksum, bool sparse, bool link, bool reflink,
                        bool progress, bool dry_run,
                        ccstring exclude_pattern, ccstring include_pattern);

#ifdef __cplusplus
}
#endif

#endif /* FOSSIL_APP_CODE_H */
