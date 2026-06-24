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
#ifndef FOSSIL_APP_COMMAND_PROCESS_H
#define FOSSIL_APP_COMMAND_PROCESS_H

#include "common.h"

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * Manage and monitor system processes
 * @param pid Process ID
 * @param name Get process name if true
 * @param info Get process info if true
 * @param list List all processes if true
 * @param terminate Kill process if true
 * @param force Force kill if true
 * @param suspend Pause process if true
 * @param resume Resume process if true
 * @param priority Set/get priority level
 * @param exe_path Get executable path if true
 * @param ppid Get parent PID if true
 * @param exists Check if process exists if true
 * @param env Get environment variables if true
 * @param spawn_path Start new process from path
 * @param signal Send signal number
 * @param wait_timeout Wait for process exit with timeout
 * @param exit_code Retrieve exit code if true
 * @return 0 on success, non-zero on error
 */
int fossil_shark_process(int pid, bool name, bool info, bool list,
                         bool terminate, bool force, bool suspend,
                         bool resume, int priority, bool exe_path,
                         bool ppid, bool exists, bool env,
                         ccstring spawn_path, int signal,
                         int wait_timeout, bool exit_code);

#ifdef __cplusplus
}
#endif

#endif /* FOSSIL_APP_CODE_H */
