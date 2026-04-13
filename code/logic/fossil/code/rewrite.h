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
#ifndef FOSSIL_APP_COMMAND_REWRITE_H
#define FOSSIL_APP_COMMAND_REWRITE_H

#include "common.h"

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * Modify or update file contents, timestamps, or size
 * @param path Path to the file to modify
 * @param in_place Perform in-place modification (for content changes)
 * @param append Append content instead of overwriting
 * @param new_content Content to write to the file (NULL if only updating timestamps/size)
 * @param size Set exact file size (truncate or extend)
 * @param update_access_time Update file access time
 * @param update_mod_time Update file modification time
 * @return 0 on success, non-zero on error
 */
int fossil_shark_rewrite(ccstring path, bool in_place, bool append,
                            ccstring new_content, size_t size,
                            bool update_access_time, bool update_mod_time);

#ifdef __cplusplus
}
#endif

#endif /* FOSSIL_APP_CODE_H */
