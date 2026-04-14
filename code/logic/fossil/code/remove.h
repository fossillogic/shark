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
#ifndef FOSSIL_APP_COMMAND_REMOVE_H
#define FOSSIL_APP_COMMAND_REMOVE_H

#include "common.h"

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * Remove or delete files and directories safely
 * @param path Path of file or directory to remove
 * @param recursive Remove directories and their contents recursively
 * @param force Force removal without prompts
 * @param interactive Prompt before each removal
 * @param use_trash Move to trash instead of permanent deletion
 * @param wipe Securely overwrite files before deletion
 * @param shred_passes Number of passes for multi-pass secure deletion (0 for none)
 * @param older_than Delete only files older than specified time (NULL for no filter)
 * @param larger_than Delete only files larger than specified size (0 for no filter)
 * @param empty_only Delete only empty directories
 * @param log_file Write deletion log to specified file (NULL for no logging)
 * @return 0 on success, non-zero on error
 */
int fossil_shark_remove(ccstring path, bool recursive, bool force,
                        bool interactive, bool use_trash, bool wipe,
                        int shred_passes, ccstring older_than,
                        size_t larger_than, bool empty_only,
                        ccstring log_file);

#ifdef __cplusplus
}
#endif

#endif /* FOSSIL_APP_CODE_H */
