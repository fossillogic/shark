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
#include "fossil/code/app.h"
#include <unistd.h>

int FOSSIL_IO_VERBOSE = false; // Verbose output flag

void show_commands(char* app_name) {
    fossil_io_printf("{blue}Usage: {cyan}%s{blue} <options>{reset}\n", app_name);
    fossil_io_printf("{blue}Options:{reset}\n");
    fossil_io_printf("{cyan}  --help           Display help information for the Shark tool.{reset}\n");
    fossil_io_printf("{cyan}  --version        Display the current version of the Shark tool.{reset}\n");
    fossil_io_printf("{cyan}  --name           Display app name.{reset}\n");
    fossil_io_printf("{cyan}  --verbose        Enable verbose output for all commands.{reset}\n");
    fossil_io_printf("{cyan}  --color          Set color output mode: {yellow}enable{cyan}, {yellow}disable{cyan}, or {yellow}auto{cyan}.{reset}\n");
    fossil_io_printf("{cyan}  --clear          Clear terminal or console output.{reset}\n");
    exit(FOSSIL_IO_SUCCESS);
}

void show_version(void) {
    fossil_io_printf("{blue}%s version %s{reset}\n", FOSSIL_APP_NAME, FOSSIL_APP_VERSION);
    exit(FOSSIL_IO_SUCCESS);
}

void show_name(void) {
    fossil_io_printf("{blue}App Name: {cyan}%s{reset}\n", FOSSIL_APP_NAME);
    exit(FOSSIL_IO_SUCCESS);
}

bool app_entry(int argc, char** argv) {
    for (i32 i = 1; i < argc; ++i) {
        if (argv[i] == cnullptr) continue;

        if (fossil_io_cstring_compare(argv[i], "--help") == 0) {
            show_commands(argv[0]);
        } else if (fossil_io_cstring_compare(argv[i], "--version") == 0) {
            show_version();
        } else if (fossil_io_cstring_compare(argv[i], "--name") == 0) {
            show_name();
        } else if (fossil_io_cstring_compare(argv[i], "--verbose") == 0) {
            FOSSIL_IO_VERBOSE = true;
            fossil_io_printf("{blue}Verbose mode enabled{reset}\n");
        } else if (fossil_io_cstring_compare(argv[i], "--color") == 0) {
            if (i + 1 < argc && argv[i + 1] != cnullptr) {
                if (fossil_io_cstring_compare(argv[i + 1], "enable") == 0) {
                    FOSSIL_IO_COLOR_ENABLE = true;
                } else if (fossil_io_cstring_compare(argv[i + 1], "disable") == 0) {
                    FOSSIL_IO_COLOR_ENABLE = false;
                } else if (fossil_io_cstring_compare(argv[i + 1], "auto") == 0) {
                    FOSSIL_IO_COLOR_ENABLE = isatty(STDOUT_FILENO);
                }
                ++i; // Skip next argument
            }
        } else if (fossil_io_cstring_compare(argv[i], "--clear") == 0) {
            fossil_io_printf("\033[H\033[J"); // ANSI escape sequence to clear screen
        } else {
            fossil_io_printf("{red}Unknown command: %s{reset}\n", argv[i]);
            return false;
        }
    }
    return 0;
}
