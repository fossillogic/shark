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
        fossil_io_printf("  {cyan}delete{normal}    - Delete files or directories (alias for remove)\n");
        fossil_io_printf("  {cyan}rename{normal}    - Rename files or directories\n");
        fossil_io_printf("  {cyan}create{normal}    - Create files or directories\n");
        fossil_io_printf("  {cyan}search{normal}    - Find files by name or content\n");
        fossil_io_printf("  {cyan}archive{normal}   - Create, extract, or list archives\n");
        fossil_io_printf("  {cyan}view{normal}      - Output file contents\n");
        fossil_io_printf("  {cyan}compare{normal}   - Compare files or directories\n");
        fossil_io_printf("  {cyan}sync{normal}      - Synchronize files/directories\n");
        fossil_io_printf("  {cyan}watch{normal}     - Monitor files/directories\n");
        fossil_io_printf("  {cyan}help{normal}      - Show help and usage\n");
        fossil_io_printf("  {cyan}grammar{normal}   - Grammar analysis and correction\n");
        fossil_io_printf("\n{blue,bold}AI Commands:{normal}\n");
        fossil_io_printf("  {cyan}chat{normal}      - Interactive AI chat session\n");
        fossil_io_printf("  {cyan}ask{normal}       - Ask AI a single question\n");
        fossil_io_printf("  {cyan}summery{normal}   - Generate AI summary of file or directory\n");
        fossil_io_printf("\n{blue,bold}Global Flags:{normal}\n");
        fossil_io_printf("  {cyan}--help{normal}    - Show command help\n");
        fossil_io_printf("  {cyan}--version{normal} - Display Shark Tool version\n");
        fossil_io_printf("  {cyan}-v, --verbose{normal} - Enable detailed output\n");
        fossil_io_printf("  {cyan}-q, --quiet{normal}   - Suppress standard output\n");
        fossil_io_printf("  {cyan}--dry-run{normal}     - Simulate actions without changes\n");
        fossil_io_printf("  {cyan}--color{normal}       - Colorize output where applicable\n");
        fossil_io_printf("  {cyan}--time{normal}        - Display timestamps in output\n");
        fossil_io_printf("{cyan}------------------------------------------------------------{normal}\n");

        if (cunlikely(!command)) return 0; // general overview only
    }

    // Detailed help for specific command
    if (clikely(cnotnull(command))) {
        if (fossil_io_cstring_equals(command, "show")) {
            fossil_io_printf("{blue,bold}Usage:{normal} show [options] <path>\n");
            fossil_io_printf("{blue,bold}Options:{normal}\n");
            fossil_io_printf("  {cyan}-a, --all{normal}        Show hidden files\n");
            fossil_io_printf("  {cyan}-l, --long{normal}       Detailed info\n");
            fossil_io_printf("  {cyan}-h, --human{normal}      Human-readable sizes\n");
            fossil_io_printf("  {cyan}-r, --recursive{normal}  Include subdirectories\n");
            fossil_io_printf("  {cyan}-d, --depth <n>{normal}  Limit recursion depth\n");
            fossil_io_printf("  {cyan}--as <format>{normal}    Output format: list, tree, graph\n");
            fossil_io_printf("  {cyan}--time{normal}           Show timestamps\n");
        } else if (fossil_io_cstring_equals(command, "move")) {
            fossil_io_printf("{blue,bold}Usage:{normal} move [options] <src> <dest>\n");
            fossil_io_printf("{blue,bold}Options:{normal}\n");
            fossil_io_printf("  {cyan}-f, --force{normal}      Overwrite without prompt\n");
            fossil_io_printf("  {cyan}-i, --interactive{normal} Ask before overwrite\n");
            fossil_io_printf("  {cyan}-b, --backup{normal}     Backup before move\n");
        } else if (fossil_io_cstring_equals(command, "copy")) {
            fossil_io_printf("{blue,bold}Usage:{normal} copy [options] <src> <dest>\n");
            fossil_io_printf("{blue,bold}Options:{normal}\n");
            fossil_io_printf("  {cyan}-r, --recursive{normal}  Copy subdirectories\n");
            fossil_io_printf("  {cyan}-u, --update{normal}     Only copy newer files\n");
            fossil_io_printf("  {cyan}-p, --preserve{normal}   Keep permissions/timestamps\n");
        } else if (fossil_io_cstring_equals(command, "remove") || fossil_io_cstring_equals(command, "delete")) {
            fossil_io_printf("{blue,bold}Usage:{normal} %s [options] <path>\n", command);
            fossil_io_printf("{blue,bold}Options:{normal}\n");
            fossil_io_printf("  {cyan}-r, --recursive{normal}  Delete contents\n");
            fossil_io_printf("  {cyan}-f, --force{normal}      No confirmation\n");
            fossil_io_printf("  {cyan}-i, --interactive{normal} Confirm per file\n");
            fossil_io_printf("  {cyan}--trash{normal}          Move to system trash\n");
        } else if (fossil_io_cstring_equals(command, "rename")) {
            fossil_io_printf("{blue,bold}Usage:{normal} rename [options] <old_name> <new_name>\n");
            fossil_io_printf("{blue,bold}Options:{normal}\n");
            fossil_io_printf("  {cyan}-f, --force{normal}      Overwrite target\n");
            fossil_io_printf("  {cyan}-i, --interactive{normal} Confirm before overwrite\n");
        } else if (fossil_io_cstring_equals(command, "create")) {
            fossil_io_printf("{blue,bold}Usage:{normal} create [options] <path>\n");
            fossil_io_printf("{blue,bold}Options:{normal}\n");
            fossil_io_printf("  {cyan}-p, --parents{normal}    Create parent directories\n");
            fossil_io_printf("  {cyan}-t, --type{normal}       Creates either file or dir\n");
        } else if (fossil_io_cstring_equals(command, "search")) {
            fossil_io_printf("{blue,bold}Usage:{normal} search [options] <path>\n");
            fossil_io_printf("{blue,bold}Options:{normal}\n");
            fossil_io_printf("  {cyan}-r, --recursive{normal}  Include subdirectories\n");
            fossil_io_printf("  {cyan}-n, --name{normal}       Match filename\n");
            fossil_io_printf("  {cyan}-c, --content{normal}    Search in file contents\n");
            fossil_io_printf("  {cyan}-i, --ignore-case{normal} Case-insensitive\n");
        } else if (fossil_io_cstring_equals(command, "archive")) {
            fossil_io_printf("{blue,bold}Usage:{normal} archive [options] <path>\n");
            fossil_io_printf("{blue,bold}Options:{normal}\n");
            fossil_io_printf("  {cyan}-c, --create{normal}     New archive\n");
            fossil_io_printf("  {cyan}-x, --extract{normal}    Extract contents\n");
            fossil_io_printf("  {cyan}-l, --list{normal}       List archive\n");
            fossil_io_printf("  {cyan}-f <format>{normal}      Format: zip/tar/gz\n");
            fossil_io_printf("  {cyan}-p, --password{normal}   Encrypt archive\n");
        } else if (fossil_io_cstring_equals(command, "view")) {
            fossil_io_printf("{blue,bold}Usage:{normal} view [options] <file>\n");
            fossil_io_printf("{blue,bold}Options:{normal}\n");
            fossil_io_printf("  {cyan}-n, --number{normal}      Number all lines\n");
            fossil_io_printf("  {cyan}-b, --non-blank{normal}   Number non-empty lines\n");
            fossil_io_printf("  {cyan}-s, --squeeze{normal}     Remove blank lines\n");
            fossil_io_printf("  {cyan}-h, --head <n>{normal}    First n lines\n");
            fossil_io_printf("  {cyan}-t, --tail <n>{normal}    Last n lines\n");
            fossil_io_printf("  {cyan}--time{normal}            Show timestamps\n");
        } else if (fossil_io_cstring_equals(command, "compare")) {
            fossil_io_printf("{blue,bold}Usage:{normal} compare [options] <path1> <path2>\n");
            fossil_io_printf("{blue,bold}Options:{normal}\n");
            fossil_io_printf("  {cyan}-t, --text{normal}       Line diff\n");
            fossil_io_printf("  {cyan}-b, --binary{normal}     Binary diff\n");
            fossil_io_printf("  {cyan}--context <n>{normal}    Show context lines\n");
            fossil_io_printf("  {cyan}--ignore-case{normal}    Ignore case differences\n");
        } else if (fossil_io_cstring_equals(command, "sync")) {
            fossil_io_printf("{blue,bold}Usage:{normal} sync [options] <src> <dest>\n");
            fossil_io_printf("{blue,bold}Options:{normal}\n");
            fossil_io_printf("  {cyan}-r, --recursive{normal}  Include subdirs\n");
            fossil_io_printf("  {cyan}-u, --update{normal}     Copy only newer\n");
            fossil_io_printf("  {cyan}--delete{normal}         Remove extraneous files from target\n");
        } else if (fossil_io_cstring_equals(command, "watch")) {
            fossil_io_printf("{blue,bold}Usage:{normal} watch [options] <path>\n");
            fossil_io_printf("{blue,bold}Options:{normal}\n");
            fossil_io_printf("  {cyan}-r, --recursive{normal}      Include subdirs\n");
            fossil_io_printf("  {cyan}-e, --events <list>{normal}  Filter events: create/modify/delete\n");
            fossil_io_printf("  {cyan}-t, --interval <n>{normal}   Poll interval in seconds\n");
        } else if (fossil_io_cstring_equals(command, "chat")) {
            fossil_io_printf("{blue,bold}Usage:{normal} chat [options]\n");
            fossil_io_printf("{blue,bold}Options:{normal}\n");
            fossil_io_printf("  {cyan}-f, --file <path>{normal}  Use file content\n");
            fossil_io_printf("  {cyan}-m, --model <name>{normal} Select model\n");
            fossil_io_printf("  {cyan}-s, --system <role>{normal} AI persona\n");
            fossil_io_printf("  {cyan}--save <path>{normal}      Save chat transcript\n");
            fossil_io_printf("  {cyan}--context{normal}          Keep session history\n");
        } else if (fossil_io_cstring_equals(command, "ask")) {
            fossil_io_printf("{blue,bold}Usage:{normal} ask [options]\n");
            fossil_io_printf("{blue,bold}Options:{normal}\n");
            fossil_io_printf("  {cyan}-f, --file <path>{normal}  Provide file context\n");
            fossil_io_printf("  {cyan}--explain{normal}          Force explanation\n");
            fossil_io_printf("  {cyan}--analyze{normal}          Deep analysis\n");
            fossil_io_printf("  {cyan}-q, --quiet{normal}        Minimal output\n");
        } else if (fossil_io_cstring_equals(command, "summery")) {
            fossil_io_printf("{blue,bold}Usage:{normal} summery [options] <file>\n");
            fossil_io_printf("{blue,bold}Options:{normal}\n");
            fossil_io_printf("  {cyan}-f, --file <path>{normal}  Use file content\n");
            fossil_io_printf("  {cyan}--depth <level>{normal}    Summary depth\n");
            fossil_io_printf("  {cyan}-q, --quiet{normal}        Minimal output\n");
            fossil_io_printf("  {cyan}--color{normal}            Highlight key items\n");
            fossil_io_printf("  {cyan}--time{normal}             Include timestamps\n");
        } else if (fossil_io_cstring_equals(command, "help")) {
            fossil_io_printf("{blue,bold}Usage:{normal} help [command]\n");
            fossil_io_printf("{blue,bold}Options:{normal}\n");
            fossil_io_printf("  {cyan}--examples{normal}  Show usage examples\n");
            fossil_io_printf("  {cyan}--man{normal}       Show full manual\n");
        } else if (fossil_io_cstring_equals(command, "rewrite")) {
            fossil_io_printf("{blue,bold}Usage:{normal} rewrite [options] <path> [content]\n");
            fossil_io_printf("{blue,bold}Options:{normal}\n");
            fossil_io_printf("  {cyan}--in-place{normal}       Modify file directly (default)\n");
            fossil_io_printf("  {cyan}-a, --append{normal}     Append content instead of overwriting\n");
            fossil_io_printf("  {cyan}--size <n>{normal}       Truncate or extend file to exact size\n");
            fossil_io_printf("  {cyan}--access-time{normal}    Update file access time\n");
            fossil_io_printf("  {cyan}--mod-time{normal}       Update file modification time\n");
            fossil_io_printf("  {cyan}<content>{normal}        Optional content to write\n");
        } else if (fossil_io_cstring_equals(command, "introspect")) {
            fossil_io_printf("{blue,bold}Usage:{normal} introspect [options] <path>\n");
            fossil_io_printf("{blue,bold}Options:{normal}\n");
            fossil_io_printf("  {cyan}--head <n>{normal}       Show first n lines\n");
            fossil_io_printf("  {cyan}--tail <n>{normal}       Show last n lines\n");
            fossil_io_printf("  {cyan}--count{normal}          Count lines, words, bytes\n");
            fossil_io_printf("  {cyan}--type{normal}           Show file type or MIME\n");
            fossil_io_printf("  {cyan}--fson{normal}           Output metadata in FSON format\n");
        } else if (fossil_io_cstring_equals(command, "grammar")) {
            fossil_io_printf("{blue,bold}Usage:{normal} grammar [options] <file>\n");
            fossil_io_printf("{blue,bold}Options:{normal}\n");
            fossil_io_printf("  {cyan}--check{normal}           Run grammar check\n");
            fossil_io_printf("  {cyan}--fix{normal}             Auto-correct grammar\n");
            fossil_io_printf("  {cyan}--sanitize{normal}        Remove rot-brain/meme language\n");
            fossil_io_printf("  {cyan}--suggest{normal}         Suggest alternatives\n");
            fossil_io_printf("  {cyan}--tone{normal}            Detect tone\n");
            fossil_io_printf("  {cyan}--detect <type>{normal}   Run detectors: ragebait, clickbait, spam, woke, bot, sarcasm, formal, snowflake, offensive, neutral, hype, quality, political, conspiracy, marketing, technobabble\n");
        } else {
            fossil_io_fprintf(FOSSIL_STDERR, "{red,bold}Unknown command: %s{normal}\n", command);
            return 1;
        }

        if (clikely(show_examples)) {
            fossil_io_printf("\n{blue,bold}Example usage:{normal}\n");
            if (fossil_io_cstring_equals(command, "show")) fossil_io_printf("  {cyan}show -a -l /home/user{normal}\n");
            else if (fossil_io_cstring_equals(command, "move")) fossil_io_printf("  {cyan}move -f file1.txt file2.txt{normal}\n");
            else if (fossil_io_cstring_equals(command, "copy")) fossil_io_printf("  {cyan}copy -r src_dir dest_dir{normal}\n");
            else if (fossil_io_cstring_equals(command, "remove") || fossil_io_cstring_equals(command, "delete")) 
                fossil_io_printf("  {cyan}%s -r --trash old_dir{normal}\n", command);
            else if (fossil_io_cstring_equals(command, "rename")) fossil_io_printf("  {cyan}rename -i old.txt new.txt{normal}\n");
            else if (fossil_io_cstring_equals(command, "create")) fossil_io_printf("  {cyan}create -p -t dir new_folder{normal}\n");
            else if (fossil_io_cstring_equals(command, "search")) fossil_io_printf("  {cyan}search -r -n report -i ./docs{normal}\n");
            else if (fossil_io_cstring_equals(command, "archive")) fossil_io_printf("  {cyan}archive -c -f zip backup.zip{normal}\n");
            else if (fossil_io_cstring_equals(command, "view")) fossil_io_printf("  {cyan}view -n -s file.txt{normal}\n");
            else if (fossil_io_cstring_equals(command, "compare")) fossil_io_printf("  {cyan}compare -t file1.txt file2.txt{normal}\n");
            else if (fossil_io_cstring_equals(command, "sync")) fossil_io_printf("  {cyan}sync -r -u --delete src/ dest/{normal}\n");
            else if (fossil_io_cstring_equals(command, "watch")) fossil_io_printf("  {cyan}watch -r -e create,modify -t 2 logs/{normal}\n");
            else if (fossil_io_cstring_equals(command, "chat")) fossil_io_printf("  {cyan}chat -m jellyfish -s assistant{normal}\n");
            else if (fossil_io_cstring_equals(command, "ask")) fossil_io_printf("  {cyan}ask -f file.txt --explain{normal}\n");
            else if (fossil_io_cstring_equals(command, "summery")) fossil_io_printf("  {cyan}summery -f report.txt --color --depth 3{normal}\n");
            else if (fossil_io_cstring_equals(command, "help")) fossil_io_printf("  {cyan}help show --examples{normal}\n");
            else if (fossil_io_cstring_equals(command, "rewrite")) fossil_io_printf("  {cyan}rewrite --in-place -a file.txt \"New content\"{normal}\n");
            else if (fossil_io_cstring_equals(command, "introspect")) fossil_io_printf("  {cyan}introspect --head 10 --type file.txt{normal}\n");
            else if (fossil_io_cstring_equals(command, "grammar")) fossil_io_printf("  {cyan}grammar --check file.txt{normal}\n");
        }

        if (cunlikely(full_manual)) {
            fossil_io_printf("\n{blue,bold}Full manual for '%s' not implemented in this demo.{normal}\n", command);
        }
    }

    return 0;
}
