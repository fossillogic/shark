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

    // The first argument is the command
    const char *command = argv[1];

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
    } else if (fossil_io_cstring_compare(command, "meta") == 0) {
        if (argc < 3) {
            fossil_io_printf("Usage: shark meta <target>\n");
            return EXIT_FAILURE;
        }
        handle_meta(argv[2]);
    } else if (fossil_io_cstring_compare(command, "compress") == 0) {
        if (argc < 4) {
            fossil_io_printf("Usage: shark compress <source> <archive>\n");
            return EXIT_FAILURE;
        }
        handle_compress(argv[2], argv[3]);
    } else if (fossil_io_cstring_compare(command, "extract") == 0) {
        if (argc < 4) {
            fossil_io_printf("Usage: shark extract <archive> <destination>\n");
            return EXIT_FAILURE;
        }
        handle_extract(argv[2], argv[3]);
    } else if (fossil_io_cstring_compare(command, "chmod") == 0) {
        if (argc < 4) {
            fossil_io_printf("Usage: shark chmod <target> <permissions>\n");
            return EXIT_FAILURE;
        }
        handle_chmod(argv[2], argv[3]);
    } else if (fossil_io_cstring_compare(command, "ownership") == 0) {
        if (argc < 4) {
            fossil_io_printf("Usage: shark ownership <target> <owner>\n");
            return EXIT_FAILURE;
        }
        handle_ownership(argv[2], argv[3]);
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
    } else if (fossil_io_cstring_compare(command, "sync") == 0) {
        if (argc < 4) {
            fossil_io_printf("Usage: shark sync <source> <destination>\n");
            return EXIT_FAILURE;
        }
        handle_sync(argv[2], argv[3]);
    } else if (fossil_io_cstring_compare(command, "update") == 0) {
        if (argc < 3) {
            fossil_io_printf("Usage: shark update <target>\n");
            return EXIT_FAILURE;
        }
        handle_update(argv[2]);
    } else if (fossil_io_cstring_compare(command, "open") == 0) {
        if (argc < 3) {
            fossil_io_printf("Usage: shark open <file>\n");
            return EXIT_FAILURE;
        }
        handle_open(argv[2]);
    } else if (fossil_io_cstring_compare(command, "edit") == 0) {
        if (argc < 3) {
            fossil_io_printf("Usage: shark edit <file>\n");
            return EXIT_FAILURE;
        }
        handle_edit(argv[2]);
    } else if (fossil_io_cstring_compare(command, "push") == 0) {
        if (argc < 4) {
            fossil_io_printf("Usage: shark push <source> <remote>\n");
            return EXIT_FAILURE;
        }
        handle_push(argv[2], argv[3]);
    } else if (fossil_io_cstring_compare(command, "pull") == 0) {
        if (argc < 4) {
            fossil_io_printf("Usage: shark pull <remote> <destination>\n");
            return EXIT_FAILURE;
        }
        handle_pull(argv[2], argv[3]);
    } else if (fossil_io_cstring_compare(command, "--help") == 0) {
        handle_help();
    } else if (fossil_io_cstring_compare(command, "--version") == 0) {
        handle_version();
    } else if (fossil_io_cstring_compare(command, "--name") == 0) {
        handle_name();
    } else {
        handle_help();
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
