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
#include "fossil/code/process.h"

int fossil_shark_process(int pid, bool name, bool info, bool list,
                         bool terminate, bool force, bool suspend,
                         bool resume, int priority, bool exe_path,
                         bool ppid, bool exists, bool env,
                         ccstring spawn_path, int signal,
                         int wait_timeout, bool exit_code)
{
    int rc = 0;
    uint32_t target_pid = (pid > 0 ? (uint32_t)pid : fossil_sys_process_get_pid());

    if (spawn_path) {
        char *const argv[] = { (char *)spawn_path, NULL };
        uint32_t spawned_pid = 0;

        rc = fossil_sys_process_spawn(spawn_path, argv, NULL, &spawned_pid);
        if (rc != 0) {
            return rc;
        }

        target_pid = spawned_pid;
    }

    if (list) {
        fossil_sys_process_list_t proc_list;
        rc = fossil_sys_process_list(&proc_list);
        if (rc != 0) {
            return rc;
        }
    }

    if (exists) {
        rc = fossil_sys_process_exists(target_pid);
        if (rc != 1) {
            return (rc < 0 ? rc : -1);
        }
    }

    if (name) {
        char proc_name[256];
        rc = fossil_sys_process_get_name(target_pid, proc_name, sizeof(proc_name));
        if (rc != 0) {
            return rc;
        }
    }

    if (info) {
        fossil_sys_process_info_t proc_info;
        rc = fossil_sys_process_get_info(target_pid, &proc_info);
        if (rc != 0) {
            return rc;
        }
    }

    if (exe_path) {
        char proc_exe_path[4096];
        rc = fossil_sys_process_get_exe_path(target_pid, proc_exe_path, sizeof(proc_exe_path));
        if (rc != 0) {
            return rc;
        }
    }

    if (ppid) {
        rc = fossil_sys_process_get_ppid(target_pid);
        if (rc < 0) {
            return rc;
        }
    }

    if (env) {
        char proc_env[8192];
        rc = fossil_sys_process_get_environment(target_pid, proc_env, sizeof(proc_env));
        if (rc < 0) {
            return rc;
        }
    }

    if (signal != 0) {
        rc = fossil_sys_process_send_signal(target_pid, signal);
        if (rc != 0) {
            return rc;
        }
    }

    if (terminate) {
        rc = fossil_sys_process_terminate(target_pid, force ? 1 : 0);
        if (rc != 0) {
            return rc;
        }
    }

    if (suspend) {
        rc = fossil_sys_process_suspend(target_pid);
        if (rc != 0) {
            return rc;
        }
    }

    if (resume) {
        rc = fossil_sys_process_resume(target_pid);
        if (rc != 0) {
            return rc;
        }
    }

    if (priority != 0) {
        rc = fossil_sys_process_set_priority(target_pid, priority);
        if (rc != 0) {
            return rc;
        }
    } else {
        int current_priority = 0;
        rc = fossil_sys_process_get_priority(target_pid, &current_priority);
        if (rc != 0) {
            return rc;
        }
    }

    if (wait_timeout >= 0 || exit_code) {
        int process_exit_code = 0;
        int timeout_ms = (wait_timeout >= 0 ? wait_timeout : -1);

        rc = fossil_sys_process_wait(target_pid, exit_code ? &process_exit_code : NULL, timeout_ms);
        if (rc != 0) {
            return rc;
        }
    }

    return 0;
}
