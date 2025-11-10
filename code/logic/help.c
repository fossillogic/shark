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

/**
 * Display help information and usage examples
 */
int fossil_shark_help(ccstring command, bool show_examples, bool full_manual) {
    if (!command || fossil_io_cstring_equals(command, "all")) {
        fossil_io_printf("{cyan,bold}Fossil Shark - Command Overview:{normal}\n");
        fossil_io_printf("{cyan}------------------------------------------------------------{normal}\n");
        fossil_io_printf("{blue,bold}Core File Commands:{normal}\n");
        fossil_io_printf("  {cyan}show{normal}      - Display files and directories\n");
        fossil_io_printf("  {cyan}move{normal}      - Move or rename files/directories\n");
        fossil_io_printf("  {cyan}copy{normal}      - Copy files or directories\n");
        fossil_io_printf("  {cyan}remove{normal}    - Delete files or directories\n");
        fossil_io_printf("  {cyan}rename{normal}    - Rename files or directories\n");
        fossil_io_printf("  {cyan}create{normal}    - Create files or directories\n");
        fossil_io_printf("  {cyan}search{normal}    - Find files by name or content\n");
        fossil_io_printf("  {cyan}archive{normal}   - Create, extract, or list archives\n");
        fossil_io_printf("  {cyan}view{normal}      - Output file contents\n");
        fossil_io_printf("  {cyan}compare{normal}   - Compare files or directories\n");
        fossil_io_printf("  {cyan}help{normal}      - Show help and usage\n");
        fossil_io_printf("\n{blue,bold}AI Commands:{normal}\n");
        fossil_io_printf("  {cyan}chat{normal}      - Interactive AI chat session\n");
        fossil_io_printf("  {cyan}ask{normal}       - Ask AI a single question\n");
        fossil_io_printf("  {cyan}summery{normal}   - Generate AI summary of file or directory\n");
        fossil_io_printf("{cyan}------------------------------------------------------------{normal}\n");

        if (cunlikely(!command)) return 0; // general overview only
    }

    // Detailed help for specific command
    if (clikely(cnotnull(command))) {
        if (fossil_io_cstring_equals(command, "show")) {
            fossil_io_printf("{blue,bold}Usage:{normal} show [options] <path>\n");
            fossil_io_printf("{blue,bold}Options:{normal}\n");
            fossil_io_printf("  {cyan}-a, --all{normal}        Show hidden files\n");
            fossil_io_printf("  {cyan}-l, --long{normal}       Long listing format\n");
            fossil_io_printf("  {cyan}-h, --human{normal}      Human-readable file sizes\n");
            fossil_io_printf("  {cyan}-r, --recursive{normal}  Include subdirectories\n");
            fossil_io_printf("  {cyan}-d, --depth <n>{normal}  Limit recursion depth\n");
            fossil_io_printf("  {cyan}--as <format>{normal}    Output format: list, tree, graph\n");
            fossil_io_printf("  {cyan}--time{normal}           Show timestamps\n");
        } else if (fossil_io_cstring_equals(command, "move")) {
            fossil_io_printf("{blue,bold}Usage:{normal} move [options] <src> <dest>\n");
            fossil_io_printf("{blue,bold}Options:{normal}\n");
            fossil_io_printf("  {cyan}-f, --force{normal}      Overwrite without prompt\n");
            fossil_io_printf("  {cyan}-i, --interactive{normal} Prompt before overwrite\n");
            fossil_io_printf("  {cyan}-b, --backup{normal}     Backup existing destination\n");
        } else if (fossil_io_cstring_equals(command, "copy")) {
            fossil_io_printf("{blue,bold}Usage:{normal} copy [options] <src> <dest>\n");
            fossil_io_printf("{blue,bold}Options:{normal}\n");
            fossil_io_printf("  {cyan}-r, --recursive{normal}  Copy directories recursively\n");
            fossil_io_printf("  {cyan}-u, --update{normal}     Only copy newer files\n");
            fossil_io_printf("  {cyan}-p, --preserve{normal}   Preserve attributes\n");
        } else if (fossil_io_cstring_equals(command, "remove")) {
            fossil_io_printf("{blue,bold}Usage:{normal} remove [options] <path>\n");
            fossil_io_printf("{blue,bold}Options:{normal}\n");
            fossil_io_printf("  {cyan}-r, --recursive{normal}  Delete directories recursively\n");
            fossil_io_printf("  {cyan}-f, --force{normal}      No confirmation\n");
            fossil_io_printf("  {cyan}-i, --interactive{normal} Confirm per file\n");
            fossil_io_printf("  {cyan}--trash{normal}          Move to system trash\n");
        } else if (fossil_io_cstring_equals(command, "rename")) {
            fossil_io_printf("{blue,bold}Usage:{normal} rename [options] <old_name> <new_name>\n");
            fossil_io_printf("{blue,bold}Options:{normal}\n");
            fossil_io_printf("  {cyan}-f, --force{normal}      Overwrite target without prompt\n");
            fossil_io_printf("  {cyan}-i, --interactive{normal} Confirm before overwrite\n");
        } else if (fossil_io_cstring_equals(command, "create")) {
            fossil_io_printf("{blue,bold}Usage:{normal} create [options] <path>\n");
            fossil_io_printf("{blue,bold}Options:{normal}\n");
            fossil_io_printf("  {cyan}-p, --parents{normal}    Create parent directories\n");
            fossil_io_printf("  {cyan}-t, --type{normal}       Type: file or dir\n");
        } else if (fossil_io_cstring_equals(command, "search")) {
            fossil_io_printf("{blue,bold}Usage:{normal} search [options] <path>\n");
            fossil_io_printf("{blue,bold}Options:{normal}\n");
            fossil_io_printf("  {cyan}-r, --recursive{normal}  Include subdirectories\n");
            fossil_io_printf("  {cyan}-n, --name{normal}       Match filename\n");
            fossil_io_printf("  {cyan}-c, --content{normal}    Search file contents\n");
            fossil_io_printf("  {cyan}-i, --ignore-case{normal} Case-insensitive\n");
        } else if (fossil_io_cstring_equals(command, "archive")) {
            fossil_io_printf("{blue,bold}Usage:{normal} archive [options] <path>\n");
            fossil_io_printf("{blue,bold}Options:{normal}\n");
            fossil_io_printf("  {cyan}-c, --create{normal}     Create archive\n");
            fossil_io_printf("  {cyan}-x, --extract{normal}    Extract archive\n");
            fossil_io_printf("  {cyan}-l, --list{normal}       List contents\n");
            fossil_io_printf("  {cyan}-f <format>{normal}      Format: zip, tar, gz\n");
            fossil_io_printf("  {cyan}-p, --password{normal}   Encrypt/decrypt archive\n");
        } else if (fossil_io_cstring_equals(command, "view")) {
            fossil_io_printf("{blue,bold}Usage:{normal} view [options] <file>\n");
            fossil_io_printf("{blue,bold}Options:{normal}\n");
            fossil_io_printf("  {cyan}-n, --number{normal}      Number all lines\n");
            fossil_io_printf("  {cyan}-b, --non-blank{normal}   Number non-empty lines\n");
            fossil_io_printf("  {cyan}-s, --squeeze{normal}     Remove extra blank lines\n");
            fossil_io_printf("  {cyan}-h, --head <n>{normal}    First n lines\n");
            fossil_io_printf("  {cyan}-t, --tail <n>{normal}    Last n lines\n");
            fossil_io_printf("  {cyan}--time{normal}            Show timestamps\n");
        } else if (fossil_io_cstring_equals(command, "compare")) {
            fossil_io_printf("{blue,bold}Usage:{normal} compare [options] <path1> <path2>\n");
            fossil_io_printf("{blue,bold}Options:{normal}\n");
            fossil_io_printf("  {cyan}-t, --text{normal}       Text diff\n");
            fossil_io_printf("  {cyan}-b, --binary{normal}     Binary diff\n");
            fossil_io_printf("  {cyan}--context <n>{normal}    Show context lines\n");
            fossil_io_printf("  {cyan}--ignore-case{normal}    Ignore case differences\n");
        } else if (fossil_io_cstring_equals(command, "chat")) {
            fossil_io_printf("{blue,bold}Usage:{normal} chat [options]\n");
            fossil_io_printf("{blue,bold}Options:{normal}\n");
            fossil_io_printf("  {cyan}-f, --file <path>{normal}  File content context\n");
            fossil_io_printf("  {cyan}-m, --model <name>{normal} Select AI model\n");
            fossil_io_printf("  {cyan}-s, --system <role>{normal} System persona\n");
            fossil_io_printf("  {cyan}--save <path>{normal}      Save conversation\n");
            fossil_io_printf("  {cyan}--context{normal}          Keep session history\n");
        } else if (fossil_io_cstring_equals(command, "ask")) {
            fossil_io_printf("{blue,bold}Usage:{normal} ask [options]\n");
            fossil_io_printf("{blue,bold}Options:{normal}\n");
            fossil_io_printf("  {cyan}-f, --file <path>{normal}  File context\n");
            fossil_io_printf("  {cyan}--explain{normal}          Request explanation\n");
            fossil_io_printf("  {cyan}--analyze{normal}          Request analysis\n");
            fossil_io_printf("  {cyan}-q, --quiet{normal}        Minimal output\n");
        } else if (fossil_io_cstring_equals(command, "summery")) {
            fossil_io_printf("{blue,bold}Usage:{normal} summery [options] <file>\n");
            fossil_io_printf("{blue,bold}Options:{normal}\n");
            fossil_io_printf("  {cyan}-f, --file <path>{normal}  File to summarize\n");
            fossil_io_printf("  {cyan}--depth <level>{normal}    Detail level 1-10\n");
            fossil_io_printf("  {cyan}-q, --quiet{normal}        Minimal output\n");
            fossil_io_printf("  {cyan}--color{normal}            Colored output\n");
            fossil_io_printf("  {cyan}--time{normal}             Include timestamps\n");
        } else if (fossil_io_cstring_equals(command, "help")) {
            fossil_io_printf("{blue,bold}Usage:{normal} help [command]\n");
            fossil_io_printf("{blue,bold}Options:{normal}\n");
            fossil_io_printf("  {cyan}--examples{normal}  Show usage examples\n");
            fossil_io_printf("  {cyan}--man{normal}       Show full manual\n");
        } else {
            fossil_io_fprintf(stderr, "{red,bold}Unknown command: %s{normal}\n", command);
            return 1;
        }

        if (clikely(show_examples)) {
            fossil_io_printf("\n{blue,bold}Example usage:{normal}\n");
            if (fossil_io_cstring_equals(command, "show")) fossil_io_printf("  {cyan}show -a -l /home/user{normal}\n");
            else if (fossil_io_cstring_equals(command, "move")) fossil_io_printf("  {cyan}move -f file1.txt file2.txt{normal}\n");
            else if (fossil_io_cstring_equals(command, "copy")) fossil_io_printf("  {cyan}copy -r src_dir dest_dir{normal}\n");
            else if (fossil_io_cstring_equals(command, "remove")) fossil_io_printf("  {cyan}remove -r --trash old_dir{normal}\n");
            else if (fossil_io_cstring_equals(command, "rename")) fossil_io_printf("  {cyan}rename -i old.txt new.txt{normal}\n");
            else if (fossil_io_cstring_equals(command, "create")) fossil_io_printf("  {cyan}create -p -t dir new_folder{normal}\n");
            else if (fossil_io_cstring_equals(command, "search")) fossil_io_printf("  {cyan}search -r -n report -i ./docs{normal}\n");
            else if (fossil_io_cstring_equals(command, "archive")) fossil_io_printf("  {cyan}archive -c -f zip backup.zip{normal}\n");
            else if (fossil_io_cstring_equals(command, "view")) fossil_io_printf("  {cyan}view -n -s file.txt{normal}\n");
            else if (fossil_io_cstring_equals(command, "compare")) fossil_io_printf("  {cyan}compare -t file1.txt file2.txt{normal}\n");
            else if (fossil_io_cstring_equals(command, "chat")) fossil_io_printf("  {cyan}chat -m jellyfish -s assistant{normal}\n");
            else if (fossil_io_cstring_equals(command, "ask")) fossil_io_printf("  {cyan}ask -f file.txt --explain{normal}\n");
            else if (fossil_io_cstring_equals(command, "summery")) fossil_io_printf("  {cyan}summery -f report.txt --color --depth 3{normal}\n");
            else if (fossil_io_cstring_equals(command, "help")) fossil_io_printf("  {cyan}help show --examples{normal}\n");
        }

        if (cunlikely(full_manual)) {
            fossil_io_printf("\n{blue,bold}Full manual for '%s' not implemented in this demo.{normal}\n", command);
        }
    }

    return 0;
}
