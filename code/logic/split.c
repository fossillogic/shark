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


int fossil_shark_split(ccstring file_path, size_t split_by_lines,
                       size_t split_by_bytes, size_t num_segments,
                       ccstring output_prefix, int suffix_digits,
                       bool numeric_suffix, ccstring delimiter,
                       bool dry_run) {
    if (cunlikely(!cnotnull(file_path) || !cnotnull(output_prefix))) {
        fossil_io_printf("{red}Error: File path and output prefix cannot be null{normal}\n");
        return 1;
    }

    struct stat st;
    if (stat(file_path, &st) != 0) {
        fossil_io_printf("{red}Error: Cannot access file '%s': %s{normal}\n", file_path, strerror(errno));
        return errno;
    }

    if (!S_ISREG(st.st_mode)) {
        fossil_io_printf("{red}Error: '%s' is not a regular file{normal}\n", file_path);
        return 1;
    }

    fossil_io_file_t src_stream;
    if (fossil_io_file_open(&src_stream, file_path, "rb") != 0) {
        fossil_io_printf("{red}Error: Cannot open file '%s': %s{normal}\n", file_path, strerror(errno));
        return errno;
    }

    size_t segment_size = 0;
    if (split_by_bytes > 0) {
        segment_size = split_by_bytes;
    } else if (split_by_lines > 0) {
        segment_size = split_by_lines;
    } else if (num_segments > 0) {
        segment_size = (st.st_size + num_segments - 1) / num_segments;
    } else {
        fossil_io_printf("{red}Error: Must specify split_by_lines, split_by_bytes, or num_segments{normal}\n");
        fossil_io_file_close(&src_stream);
        return 1;
    }

    fossil_io_printf("{cyan}Splitting file '%s' into segments of %zu %s{normal}\n",
                     file_path, segment_size, split_by_bytes > 0 ? "bytes" : "lines");

    char buffer[8192];
    char output_file[1024];
    int segment_num = 0;
    size_t current_count = 0;
    fossil_io_file_t dest_stream = {0};
    bool file_open = false;

    size_t n;
    while ((n = fossil_io_file_read(&src_stream, buffer, 1, sizeof(buffer))) > 0) {
        if (!file_open) {
            if (suffix_digits > 0) {
                snprintf(output_file, sizeof(output_file), "%s%0*d",
                         output_prefix, suffix_digits, segment_num);
            } else if (numeric_suffix) {
                snprintf(output_file, sizeof(output_file), "%s%d", output_prefix, segment_num);
            } else {
                snprintf(output_file, sizeof(output_file), "%s%c", output_prefix, 'a' + segment_num);
            }

            if (dry_run) {
                fossil_io_printf("{cyan}[DRY RUN] Would create segment: %s{normal}\n", output_file);
            } else {
                if (fossil_io_file_open(&dest_stream, output_file, "wb") != 0) {
                    fossil_io_printf("{red}Error: Cannot create output file '%s': %s{normal}\n", output_file, strerror(errno));
                    fossil_io_file_close(&src_stream);
                    return errno;
                }
                fossil_io_printf("{cyan}Creating segment: %s{normal}\n", output_file);
            }
            file_open = true;
            current_count = 0;
            segment_num++;
        }

        if (!dry_run) {
            if (cunlikely(fossil_io_file_write(&dest_stream, buffer, 1, n) != n)) {
                fossil_io_printf("{red}Error: Write failed for '%s': %s{normal}\n", output_file, strerror(errno));
                fossil_io_file_close(&src_stream);
                fossil_io_file_close(&dest_stream);
                return errno;
            }
        }

        current_count += (split_by_bytes > 0) ? n : 1;

        if (current_count >= segment_size) {
            if (file_open && !dry_run) {
                fossil_io_file_close(&dest_stream);
            }
            file_open = false;
        }
    }

    if (file_open && !dry_run) {
        fossil_io_file_close(&dest_stream);
    }
    fossil_io_file_close(&src_stream);

    fossil_io_printf("{cyan}Split complete: %d segments created{normal}\n", segment_num);
    return 0;
}
