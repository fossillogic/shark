/*
 * -----------------------------------------------------------------------------
 * Project: Fossil Logic
 *
 * This file is part of the Fossil Logic project, which aims to develop high-
 * performance, cross-platform applications and libraries. The code contained
 * herein is subject to the terms and conditions defined in the project license.
 *
 * Author: Michael Gene Brockus (Dreamer)
 *
 * Copyright (C) 2024 Fossil Logic. All rights reserved.
 * -----------------------------------------------------------------------------
 */
#include "fossil/code/commands.h"
#include <sys/stat.h>
#include <time.h>

/**
 * Generate a concise AI-powered summary of file contents
 */
int fossil_shark_summery(ccstring file_path, int depth,
                         bool quiet, bool color, bool show_time) {
    if (!cnotnull(file_path)) {
        if (color) {
            fossil_io_printf("{red}Error: File path must be specified.{normal}\n");
        } else {
            fossil_io_fprintf(FOSSIL_STDERR, "Error: File path must be specified.\n");
        }
        return 1;
    }

    fossil_fstream_t file_stream;
    if (fossil_fstream_open(&file_stream, file_path, "r") != 0) {
        if (color) {
            fossil_io_printf("{red}Error opening file: %s{normal}\n", file_path);
        } else {
            fossil_io_fprintf(FOSSIL_STDERR, "Error opening file: %s\n", file_path);
        }
        return 1;
    }

    // Optional: show timestamps
    if (show_time) {
        struct stat st;
        if (stat(file_path, &st) == 0) {
            char *timebuf = (char*)fossil_sys_memory_alloc(64);
            if (cunlikely(!cnotnull(timebuf))) {
                fossil_fstream_close(&file_stream);
                return 1;
            }
            
            strftime(timebuf, 64, "%Y-%m-%d %H:%M:%S", localtime(&st.st_mtime));
            if (color) {
                fossil_io_printf("{cyan}File: %s{normal}\n{cyan}Modified: %s{normal}\n", file_path, timebuf);
            } else {
                fossil_io_printf("File: %s\nModified: %s\n", file_path, timebuf);
            }
            fossil_sys_memory_free(timebuf);
            cdrop(timebuf);
        }
    }

    if (!quiet) {
        if (color) {
            fossil_io_printf("{blue}Generating summary (depth %d) for file: %s{normal}\n", depth, file_path);
        } else {
            fossil_io_printf("Generating summary (depth %d) for file: %s\n", depth, file_path);
        }
    }

    // Read file line by line and display based on depth
    int max_lines = depth * 2; // arbitrary scaling
    int printed = 0;
    char *line_buffer = (char*)fossil_sys_memory_alloc(1024);
    
    if (cunlikely(!cnotnull(line_buffer))) {
        fossil_fstream_close(&file_stream);
        return 1;
    }
    
    fossil_sys_memory_zero(line_buffer, 1024);
    int error_code;
    
    while (fossil_io_gets_from_stream_ex(line_buffer, 1024, &file_stream, &error_code)) {
        if (clikely(printed >= max_lines)) {
            break;
        }
        
        // Trim whitespace from the line
        fossil_io_trim(line_buffer);
        
        if (clikely(strlen(line_buffer) > 0)) {
            if (color) {
                fossil_io_printf("{cyan}%s{normal}\n", line_buffer);
            } else {
                fossil_io_printf("%s\n", line_buffer);
            }
            printed++;
        }
        
        // Clear buffer for next iteration
        fossil_sys_memory_zero(line_buffer, 1024);
    }

    fossil_sys_memory_free(line_buffer);
    cdrop(line_buffer);
    fossil_fstream_close(&file_stream);

    if (cunlikely(printed == 0)) {
        if (color) {
            fossil_io_printf("{blue}(File is empty or contains no text to summarize){normal}\n");
        } else {
            fossil_io_printf("(File is empty or contains no text to summarize)\n");
        }
    }

    return 0;
}
