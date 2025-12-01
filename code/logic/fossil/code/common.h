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
#ifndef FOSSIL_APP_COMMON_H
#define FOSSIL_APP_COMMON_H

#include <fossil/sys/framework.h>
#include <fossil/io/framework.h>
#include <fossil/math/framework.h>
#include <fossil/cryptic/framework.h>

#ifdef _WIN32
#include <windows.h>
#include <fileapi.h>
#include <winbase.h>
#include <handleapi.h>
#include <direct.h>
#include <io.h>
#define _CRT_INTERNAL_NONSTDC_NAMES 1
#include <sys/types.h>
#include <sys/stat.h>
#define mkdir(path, mode) _mkdir(path)
#else
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#endif

#if defined(_WIN32)
#include <windows.h>
static inline __attribute__((unused))
int portable_link(const char *src, const char *dest) {
    return CreateHardLinkA(dest, src, NULL) ? 0 : -1;
}
#ifndef link
#define link(src, dest) portable_link(src, dest)
#endif

#else
#include <unistd.h>
#endif

#include <errno.h>
#include <utime.h>
#include <libgen.h>
#include <math.h>

#endif /* FOSSIL_APP_CODE_H */
