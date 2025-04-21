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


bool app_entry(int argc, char** argv) {
    if (argc < 2) {
        fossil_io_printf("{blue,bold}Usage: shark <command> [options]{reset}\n");
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
    }

    // Parse the command
    if (fossil_io_cstring_compare(command, "move") == 0) {
        if (argc < 4) {
            fossil_io_printf("Usage: shark move <source> <destination>\n");
            return EXIT_FAILURE;
        }
        handle_move(argv[2], argv[3]);
    } else if (fossil_io_cstring_compare(command, "copy") == 0) {
        if (argc < 4) {
            fossil_io_printf("Usage: shark copy <source> <destination>\n");
            return EXIT_FAILURE;
        }
        handle_copy(argv[2], argv[3]);
    } else if (fossil_io_cstring_compare(command, "delete") == 0) {
        if (argc < 3) {
            fossil_io_printf("Usage: shark delete <target>\n");
            return EXIT_FAILURE;
        }
        handle_delete(argv[2]);
    } else if (fossil_io_cstring_compare(command, "list") == 0) {
        if (argc < 3) {
            fossil_io_printf("Usage: shark list <directory>\n");
            return EXIT_FAILURE;
        }
        handle_list(argv[2]);
    } else if (fossil_io_cstring_compare(command, "show") == 0) {
        if (argc < 3) {
            fossil_io_printf("Usage: shark show <file>\n");
            return EXIT_FAILURE;
        }
        handle_show(argv[2]);
    } else if (fossil_io_cstring_compare(command, "find") == 0) {
        if (argc < 4) {
            fossil_io_printf("Usage: shark find <directory> <pattern>\n");
            return EXIT_FAILURE;
        }
        handle_find(argv[2], argv[3]);
    } else if (fossil_io_cstring_compare(command, "search") == 0) {
        if (argc < 4) {
            fossil_io_printf("Usage: shark search <file> <pattern>\n");
            return EXIT_FAILURE;
        }
        handle_search(argv[2], argv[3]);
    } else if (fossil_io_cstring_compare(command, "size") == 0) {
        if (argc < 3) {
            fossil_io_printf("Usage: shark size <target>\n");
            return EXIT_FAILURE;
        }
        handle_size(argv[2]);
    } else if (fossil_io_cstring_compare(command, "disk") == 0) {
        if (argc < 3) {
            fossil_io_printf("Usage: shark disk <path>\n");
            return EXIT_FAILURE;
        }
        handle_disk(argv[2]);
    } else if (fossil_io_cstring_compare(command, "tree") == 0) {
        if (argc < 3) {
            fossil_io_printf("Usage: shark tree <directory>\n");
            return EXIT_FAILURE;
        }
        handle_tree(argv[2]);
    } else if (fossil_io_cstring_compare(command, "compare") == 0) {
        if (argc < 4) {
            fossil_io_printf("Usage: shark compare <path1> <path2>\n");
            return EXIT_FAILURE;
        }
        handle_compare(argv[2], argv[3]);
    } else if (fossil_io_cstring_compare(command, "create") == 0) {
        if (argc < 3) {
            fossil_io_printf("Usage: shark create <target>\n");
            return EXIT_FAILURE;
        }
        handle_create(argv[2]);
    } else {
        handle_help();
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
