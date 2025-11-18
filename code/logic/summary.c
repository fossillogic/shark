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
#include "fossil/code/commands.h"
#include <sys/stat.h>
#include <time.h>

int fossil_shark_summary(const char *file_path, int depth, bool show_time) {
    fossil_ai_jellyfish_chain_t chain;
    int rc = fossil_ai_jellyfish_load(&chain, file_path);
    if (rc != 0) {
        fossil_io_printf("{red}Failed to load chain from file: {cyan}%s{reset}\n", file_path);
        return rc;
    }

    fossil_io_printf("{blue}Jellyfish Chain Summary for: {cyan}%s{reset}\n", file_path);
    fossil_io_printf("{blue}Commits: {cyan}%zu{reset}\n", chain.count);
    fossil_io_printf("{blue}Branches: {cyan}%zu{reset}\n", chain.branch_count);
    fossil_io_printf("{blue}Summary depth: {cyan}%d{reset}\n", depth);
    fossil_io_printf("{blue}Show timestamps: {cyan}%s{reset}\n", show_time ? "Yes" : "No");

    size_t shown = 0;
    for (size_t i = 0; i < chain.count && shown < (size_t)depth; ++i) {
        const fossil_ai_jellyfish_block_t *block = &chain.commits[i];
        char type_str[32];
        snprintf(type_str, sizeof(type_str), "%s", fossil_ai_jellyfish_commit_type_name(block->block_type));
        fossil_io_printf("  {yellow}[%zu]{reset} {green}%s{reset}: {white}%s{reset}\n",
            i, type_str, block->identity.commit_message);

        if (show_time) {
            fossil_io_printf("    {magenta}Timestamp:{reset} %llu\n", (unsigned long long)block->time.timestamp);
        }
        shown++;
    }

    return FOSSIL_IO_SUCCESS;
}
