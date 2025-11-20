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
#ifndef FOSSIL_APP_CODE_H
#define FOSSIL_APP_CODE_H

#include "commands.h"
#include "magic.h"
#include <errno.h>
#include <utime.h>

#define FOSSIL_APP_NAME "Shark Tool"
#define FOSSIL_APP_VERSION "0.1.1"

#ifdef __cplusplus
extern "C" {
#endif

extern int FOSSIL_IO_VERBOSE; // Verbose output flag

/**
 * @brief Custom application entry point.
 * This function is the main entry point for the custom application.
 * 
 * @param argc The number of command-line arguments.
 * @param argv The array of command-line arguments.
 * @return 0 if the application runs successfully, non-zero otherwise.
 */
bool app_entry(int argc, char** argv);

#ifdef __cplusplus
}
#endif

#endif /* FOSSIL_APP_CODE_H */
