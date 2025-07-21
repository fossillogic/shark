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
#include <unistd.h> // For STDOUT_FILENO


bool app_entry(int argc, char** argv) {
    if (argc < 2) {
        fossil_io_printf("{blue,bold}Usage:{cyan} shark <command> [options]{reset}\n");
        return EXIT_FAILURE;
    }

    // The first argument is the command or flag
    const char *command = argv[1];

    // Handle flags
    if (fossil_io_cstring_compare(command, "--help") == 0) {
        handle_help();
        return EXIT_SUCCESS;
    } else if (fossil_io_cstring_compare(command, "--version") == 0) {
        handle_version();
        return EXIT_SUCCESS;
    } else if (fossil_io_cstring_compare(command, "--name") == 0) {
        handle_name();
        return EXIT_SUCCESS;
    } else if (fossil_io_cstring_compare(command, "--verbose") == 0) {
        ENABLE_VERBOSE = true;
        fossil_io_printf("{cyan}Verbose mode enabled.{reset}\n");
        return EXIT_SUCCESS;
    } else if (fossil_io_cstring_compare(command, "--clear") == 0) {
        fossil_io_printf("\033[H\033[J"); // ANSI escape sequence to clear screen
    }

    // Parse the command
    if (fossil_io_cstring_compare(command, "move") == 0) {
        if (argc < 4) {
            fossil_io_printf("{blue}Usage:{cyan} shark move <source> <destination> [--force] [--interactive]{reset}\n");
            return EXIT_FAILURE;
        }
        int force = 0, interactive = 0;
        for (int i = 4; i < argc; ++i) {
            if (fossil_io_cstring_compare(argv[i], "--force") == 0) force = 1;
            if (fossil_io_cstring_compare(argv[i], "--interactive") == 0) interactive = 1;
        }
        handle_move(argv[2], argv[3], force, interactive);
    } else if (fossil_io_cstring_compare(command, "copy") == 0) {
        if (argc < 4) {
            fossil_io_printf("{blue}Usage:{cyan} shark copy <source> <destination> [--force] [--recursive]{reset}\n");
            return EXIT_FAILURE;
        }
        int force = 0, recursive = 0;
        for (int i = 4; i < argc; ++i) {
            if (fossil_io_cstring_compare(argv[i], "--force") == 0) force = 1;
            if (fossil_io_cstring_compare(argv[i], "--recursive") == 0) recursive = 1;
        }
        handle_copy(argv[2], argv[3], force, recursive);
    } else if (fossil_io_cstring_compare(command, "delete") == 0) {
        if (argc < 3) {
            fossil_io_printf("{blue}Usage:{cyan} shark delete <target> [--force] [--recursive] [--interactive]{reset}\n");
            return EXIT_FAILURE;
        }
        int force = 0, recursive = 0, interactive = 0;
        for (int i = 3; i < argc; ++i) {
            if (fossil_io_cstring_compare(argv[i], "--force") == 0) force = 1;
            if (fossil_io_cstring_compare(argv[i], "--recursive") == 0) recursive = 1;
            if (fossil_io_cstring_compare(argv[i], "--interactive") == 0) interactive = 1;
        }
        handle_delete(argv[2], force, recursive, interactive);
    } else if (fossil_io_cstring_compare(command, "list") == 0) {
        if (argc < 3) {
            fossil_io_printf("{blue}Usage:{cyan} shark list <directory> [--sort=name|size|time] [--format=columns|lines] [--all] [--long]{reset}\n");
            return EXIT_FAILURE;
        }
        const char *sort = "name";
        const char *format = "columns";
        int all = 0, long_format = 0;
        for (int i = 3; i < argc; ++i) {
            if (fossil_io_cstring_compare(argv[i], "--all") == 0) all = 1;
            if (fossil_io_cstring_compare(argv[i], "--long") == 0) long_format = 1;
            if (strncmp(argv[i], "--sort=", 7) == 0) sort = argv[i] + 7;
            if (strncmp(argv[i], "--format=", 9) == 0) format = argv[i] + 9;
        }
        handle_list(argv[2], sort, format, all, long_format);
    } else if (fossil_io_cstring_compare(command, "show") == 0) {
        if (argc < 3) {
            fossil_io_printf("{blue}Usage:{cyan} shark show <file> [--head=N] [--tail=N] [--lines=RANGE]{reset}\n");
            return EXIT_FAILURE;
        }
        int head = 0, tail = 0;
        const char *lines = cnull;
        for (int i = 3; i < argc; ++i) {
            if (strncmp(argv[i], "--head=", 7) == 0) head = atoi(argv[i] + 7);
            if (strncmp(argv[i], "--tail=", 7) == 0) tail = atoi(argv[i] + 7);
            if (strncmp(argv[i], "--lines=", 8) == 0) lines = argv[i] + 8;
        }
        handle_show(argv[2], head, tail, lines);
    } else if (fossil_io_cstring_compare(command, "find") == 0) {
        if (argc < 4) {
            fossil_io_printf("{blue}Usage:{cyan} shark find <directory> <pattern> [--size=SIZE] [--type=TYPE] [--hidden]{reset}\n");
            return EXIT_FAILURE;
        }
        const char *size = cnull, *type = cnull;
        int hidden = 0;
        for (int i = 4; i < argc; ++i) {
            if (strncmp(argv[i], "--size=", 7) == 0) size = argv[i] + 7;
            if (strncmp(argv[i], "--type=", 7) == 0) type = argv[i] + 7;
            if (fossil_io_cstring_compare(argv[i], "--hidden") == 0) hidden = 1;
        }
        handle_find(argv[2], argv[3], size, type, hidden);
    } else if (fossil_io_cstring_compare(command, "search") == 0) {
        if (argc < 4) {
            fossil_io_printf("{blue}Usage:{cyan} shark search <file> <pattern> [--ignore-case] [--count]{reset}\n");
            return EXIT_FAILURE;
        }
        int ignore_case = 0, count = 0;
        for (int i = 4; i < argc; ++i) {
            if (fossil_io_cstring_compare(argv[i], "--ignore-case") == 0) ignore_case = 1;
            if (fossil_io_cstring_compare(argv[i], "--count") == 0) count = 1;
        }
        handle_search(argv[2], argv[3], ignore_case, count);
    } else if (fossil_io_cstring_compare(command, "size") == 0) {
        if (argc < 3) {
            fossil_io_printf("{blue}Usage:{cyan} shark size <target> [--human-readable] [--total]{reset}\n");
            return EXIT_FAILURE;
        }
        int human_readable = 0, total = 0;
        for (int i = 3; i < argc; ++i) {
            if (fossil_io_cstring_compare(argv[i], "--human-readable") == 0) human_readable = 1;
            if (fossil_io_cstring_compare(argv[i], "--total") == 0) total = 1;
        }
        handle_size(argv[2], human_readable, total);
    } else if (fossil_io_cstring_compare(command, "disk") == 0) {
        if (argc < 3) {
            fossil_io_printf("{blue}Usage:{cyan} shark disk <path> [--all] [--human-readable]{reset}\n");
            return EXIT_FAILURE;
        }
        int all = 0, human_readable = 0;
        for (int i = 3; i < argc; ++i) {
            if (fossil_io_cstring_compare(argv[i], "--all") == 0) all = 1;
            if (fossil_io_cstring_compare(argv[i], "--human-readable") == 0) human_readable = 1;
        }
        handle_disk(argv[2], all, human_readable);
    } else if (fossil_io_cstring_compare(command, "tree") == 0) {
        if (argc < 3) {
            fossil_io_printf("{blue}Usage:{cyan} shark tree <directory> [--depth=N] [--all] [--color]{reset}\n");
            return EXIT_FAILURE;
        }
        int depth = 0, all = 0, color = 0;
        for (int i = 3; i < argc; ++i) {
            if (strncmp(argv[i], "--depth=", 8) == 0) depth = atoi(argv[i] + 8);
            if (fossil_io_cstring_compare(argv[i], "--all") == 0) all = 1;
            if (fossil_io_cstring_compare(argv[i], "--color") == 0) color = 1;
        }
        handle_tree(argv[2], depth, all, color);
    } else if (fossil_io_cstring_compare(command, "compare") == 0) {
        if (argc < 4) {
            fossil_io_printf("{blue}Usage:{cyan} shark compare <path1> <path2> [--recursive] [--brief] [--ignore-case]{reset}\n");
            return EXIT_FAILURE;
        }
        int recursive = 0, brief = 0, ignore_case = 0;
        for (int i = 4; i < argc; ++i) {
            if (fossil_io_cstring_compare(argv[i], "--recursive") == 0) recursive = 1;
            if (fossil_io_cstring_compare(argv[i], "--brief") == 0) brief = 1;
            if (fossil_io_cstring_compare(argv[i], "--ignore-case") == 0) ignore_case = 1;
        }
        handle_compare(argv[2], argv[3], recursive, brief, ignore_case);
    } else if (fossil_io_cstring_compare(command, "create") == 0) {
        if (argc < 3) {
            fossil_io_printf("{blue}Usage:{cyan} shark create <target> [--directory] [--parents]{reset}\n");
            return EXIT_FAILURE;
        }
        int directory = 0, parents = 0;
        for (int i = 3; i < argc; ++i) {
            if (fossil_io_cstring_compare(argv[i], "--directory") == 0) directory = 1;
            if (fossil_io_cstring_compare(argv[i], "--parents") == 0) parents = 1;
        }
        handle_create(argv[2], directory, parents);
    } else if (fossil_io_cstring_compare(command, "clear") == 0) {
        fossil_io_clear_screen();
    } else if (fossil_io_cstring_compare(command, "ask") == 0) {
        if (argc < 3) {
            fossil_io_printf("{blue}Usage:{cyan} shark ask <prompt> [--default=RESPONSE] [--timeout=SECONDS]{reset}\n");
            return EXIT_FAILURE;
        }
        const char *default_response = cnull;
        int timeout = 0;
        for (int i = 3; i < argc; ++i) {
            if (strncmp(argv[i], "--default=", 10) == 0) default_response = argv[i] + 10;
            if (strncmp(argv[i], "--timeout=", 10) == 0) timeout = atoi(argv[i] + 10);
        }
        const char *response = handle_ask(argv[2], default_response, timeout);
        if (response != cnull) {
            fossil_io_printf("{cyan}You responded: %s{reset}\n", response);
        } else {
            fossil_io_printf("{red}No response received.{reset}\n");
        }
    } else if (fossil_io_cstring_compare(command, "edit") == 0) {
        if (argc < 3) {
            fossil_io_printf("{blue}Usage:{cyan} shark edit <file> [--editor=EDITOR] [--backup]{reset}\n");
            return EXIT_FAILURE;
        }
        const char *editor = cnull;
        int backup = 0;
        for (int i = 3; i < argc; ++i) {
            if (strncmp(argv[i], "--editor=", 9) == 0) editor = argv[i] + 9;
            if (fossil_io_cstring_compare(argv[i], "--backup") == 0) backup = 1;
        }
        handle_edit(argv[2], editor, backup);
    } else if (fossil_io_cstring_compare(command, "rename") == 0) {
        if (argc < 4) {
            fossil_io_printf("{blue}Usage:{cyan} shark rename <old_name> <new_name> [--force]{reset}\n");
            return EXIT_FAILURE;
        }
        int force = 0;
        for (int i = 4; i < argc; ++i) {
            if (fossil_io_cstring_compare(argv[i], "--force") == 0) force = 1;
        }
        handle_rename(argv[2], argv[3], force);
    } else if (fossil_io_cstring_compare(command, "where") == 0) {
        const char *name = cnull, *type = cnull;
        for (int i = 2; i < argc; ++i) {
            if (strncmp(argv[i], "--name=", 7) == 0) name = argv[i] + 7;
            if (strncmp(argv[i], "--type=", 7) == 0) type = argv[i] + 7;
        }
        handle_where(name, type);
    } else if (fossil_io_cstring_compare(command, "version") == 0) {
        handle_version();
    } else if (fossil_io_cstring_compare(command, "name") == 0) {
        handle_name();
    } else if (fossil_io_cstring_compare(command, "color=") == 0) {
        if (argc < 3) {
            fossil_io_printf("{blue}Usage:{cyan} shark color=<enable|disable|auto>{reset}\n");
            return EXIT_FAILURE;
        }

        const char *color_option = argv[2];
        if (fossil_io_cstring_compare(color_option, "enable") == 0) {
            FOSSIL_IO_COLOR_ENABLE = true;
            fossil_io_printf("{green}Color output enabled.{reset}\n");
        } else if (fossil_io_cstring_compare(color_option, "disable") == 0) {
            FOSSIL_IO_COLOR_ENABLE = false;
            fossil_io_printf("{yellow}Color output disabled.{reset}\n");
        } else if (fossil_io_cstring_compare(color_option, "auto") == 0) {
            FOSSIL_IO_COLOR_ENABLE = isatty(STDOUT_FILENO);
            fossil_io_printf("{cyan}Color output set to auto-detect.{reset}\n");
        } else {
            fossil_io_printf("{red}Invalid color option. Use enable, disable, or auto.{reset}\n");
            return EXIT_FAILURE;
        }
        return EXIT_SUCCESS;
    } else {
        handle_help();
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
