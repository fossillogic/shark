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
#include "fossil/code/split.h"

int fossil_shark_split(ccstring file_path, size_t split_by_lines,
                       size_t split_by_bytes, size_t num_segments,
                       ccstring output_prefix, int suffix_digits,
                       bool numeric_suffix, ccstring delimiter,
                       bool dry_run)
{
    if (cunlikely(!cnotnull(file_path) || !cnotnull(output_prefix)))
    {
        fossil_io_printf("{red}Error: File path and output prefix cannot be null{normal}\n");
        return 1;
    }

    fossil_io_filesys_obj_t src_obj = {0};
    if (fossil_io_filesys_stat(file_path, &src_obj) != 0)
    {
        fossil_io_printf("{red}Error: Cannot access '%s'{normal}\n", file_path);
        return 1;
    }

    if (src_obj.type != FOSSIL_FILESYS_TYPE_FILE)
    {
        fossil_io_printf("{red}Error: '%s' is not a regular file{normal}\n", file_path);
        return 1;
    }

    fossil_io_filesys_file_t src_stream = {0};
    if (fossil_io_filesys_file_open(&src_stream, file_path, "rb") != 0)
    {
        fossil_io_printf("{red}Error: Cannot open '%s'{normal}\n", file_path);
        return 1;
    }

    /* Determine segment size */
    size_t segment_size = 0;
    bool mode_bytes = false;
    bool mode_lines = false;
    bool mode_delim = false;

    if (delimiter && *delimiter)
    {
        mode_delim = true;
        segment_size = split_by_lines ? split_by_lines : 1;
    }
    else if (split_by_bytes > 0)
    {
        mode_bytes = true;
        segment_size = split_by_bytes;
    }
    else if (split_by_lines > 0)
    {
        mode_lines = true;
        segment_size = split_by_lines;
    }
    else if (num_segments > 0)
    {
        mode_bytes = true;
        segment_size = (src_obj.size + num_segments - 1) / num_segments;
    }
    else
    {
        fossil_io_printf("{red}Error: Must specify split mode{normal}\n");
        fossil_io_filesys_file_close(&src_stream);
        return 1;
    }

    fossil_io_printf("{cyan}Splitting '%s'{normal}\n", file_path);

    char buffer[8192];
    char output_file[1024];

    size_t delimiter_len = delimiter ? strlen(delimiter) : 0;
    size_t delimiter_pos = 0;
    char delimiter_window[256] = {0};

    fossil_io_filesys_file_t dest_stream = {0};
    bool file_open = false;

    size_t current_count = 0;
    int segment_num = 0;

    size_t n;
    while ((n = fossil_io_filesys_file_read(&src_stream, buffer, 1, sizeof(buffer))) > 0)
    {
        for (size_t i = 0; i < n; i++)
        {
            if (!file_open)
            {
                if (suffix_digits > 0)
                {
                    snprintf(output_file, sizeof(output_file),
                             "%s%0*d", output_prefix, suffix_digits, segment_num);
                }
                else if (numeric_suffix)
                {
                    snprintf(output_file, sizeof(output_file),
                             "%s%d", output_prefix, segment_num);
                }
                else
                {
                    snprintf(output_file, sizeof(output_file),
                             "%s%c", output_prefix, 'a' + (segment_num % 26));
                }

                if (dry_run)
                {
                    fossil_io_printf("{yellow}[DRY RUN] Would create: %s{normal}\n", output_file);
                }
                else
                {
                    if (fossil_io_filesys_file_open(&dest_stream, output_file, "wb") != 0)
                    {
                        fossil_io_printf("{red}Error creating '%s'{normal}\n", output_file);
                        fossil_io_filesys_file_close(&src_stream);
                        return 1;
                    }
                    fossil_io_printf("{cyan}Creating: %s{normal}\n", output_file);
                }

                file_open = true;
                current_count = 0;
                segment_num++;
            }

            char c = buffer[i];

            if (!dry_run)
            {
                fossil_io_filesys_file_write(&dest_stream, &c, 1, 1);
            }

            /* Counting logic */
            if (mode_bytes)
            {
                current_count++;
            }
            else if (mode_lines)
            {
                if (c == '\n')
                    current_count++;
            }
            else if (mode_delim)
            {
                delimiter_window[delimiter_pos++] = c;

                if (delimiter_pos > delimiter_len)
                {
                    memmove(delimiter_window, delimiter_window + 1, --delimiter_pos);
                }

                if (delimiter_pos == delimiter_len &&
                    memcmp(delimiter_window, delimiter, delimiter_len) == 0)
                {
                    current_count++;
                }
            }

            if (current_count >= segment_size)
            {
                if (file_open && !dry_run)
                    fossil_io_filesys_file_close(&dest_stream);

                file_open = false;
                current_count = 0;
                delimiter_pos = 0;
            }
        }
    }

    if (file_open && !dry_run)
        fossil_io_filesys_file_close(&dest_stream);

    fossil_io_filesys_file_close(&src_stream);

    fossil_io_printf("{green}Split complete: %d segments created{normal}\n", segment_num);

    return 0;
}
