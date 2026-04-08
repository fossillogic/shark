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
#ifndef FOSSIL_APP_COMMAND_SNAPSHOT_H
#define FOSSIL_APP_COMMAND_SNAPSHOT_H

#include "common.h"

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * Capture the current state of a file or directory
 * @param file_path Target file path (NULL if using dir_path)
 * @param dir_path Target directory path (NULL if using file_path)
 * @param label Snapshot label (NULL for default)
 * @param output_json Export snapshot in JSON if true
 * @param compare_with Previous snapshot to diff against (NULL for none)
 * @param compress Store snapshot compressed if true
 * @return 0 on success, non-zero on error
 */
int fossil_spino_snapshot(ccstring file_path, ccstring dir_path,
                          ccstring label, bool output_json,
                          ccstring compare_with, bool compress);

#ifdef __cplusplus
}
#endif

#endif /* FOSSIL_APP_CODE_H */
