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

/**
 * Display help information and usage examples
 */
int fossil_shark_help(ccstring command, bool show_examples, bool full_manual) {
    // Show overview if command is NULL or "all" (default to "all" if command is NULL)
    if (!command || fossil_io_cstring_equals(command, "all")) {
        fossil_io_printf("{pos:top}{cyan,bold,underline}Fossil Shark - Command Overview:{normal}\n");
        fossil_io_printf("{cyan,italic}------------------------------------------------------------{normal}\n");
        fossil_io_printf("{blue,bold,underline}Core File Commands:{normal}\n");
        fossil_io_printf("  {cyan,bold}show{normal}      - Display files and directories\n");
        fossil_io_printf("  {cyan,bold}move{normal}      - Move or rename files/directories\n");
        fossil_io_printf("  {cyan,bold}copy{normal}      - Copy files or directories\n");
        fossil_io_printf("  {cyan,bold}remove{normal}    - Delete files or directories\n");
        fossil_io_printf("  {cyan,bold}delete{normal}    - Delete files or directories (alias for remove)\n");
        fossil_io_printf("  {cyan,bold}rename{normal}    - Rename files or directories\n");
        fossil_io_printf("  {cyan,bold}create{normal}    - Create files or directories\n");
        fossil_io_printf("  {cyan,bold}search{normal}    - Find files by name or content\n");
        fossil_io_printf("  {cyan,bold}archive{normal}   - Create, extract, or list archives\n");
        fossil_io_printf("  {cyan,bold}view{normal}      - Output file contents\n");
        fossil_io_printf("  {cyan,bold}compare{normal}   - Compare files or directories\n");
        fossil_io_printf("  {cyan,bold}sync{normal}      - Synchronize files/directories\n");
        fossil_io_printf("  {cyan,bold}watch{normal}     - Monitor files/directories\n");
        fossil_io_printf("  {cyan,bold}help{normal}      - Show help and usage\n");
        fossil_io_printf("  {cyan,bold}grammar{normal}   - Grammar analysis and correction\n");
        fossil_io_printf("\n{blue,bold,underline}AI Commands:{normal}\n");
        fossil_io_printf("  {cyan,bold}chat{normal}      - Interactive AI chat session\n");
        fossil_io_printf("  {cyan,bold}ask{normal}       - Ask AI a single question\n");
        fossil_io_printf("  {cyan,bold}summery{normal}   - Generate AI summary of file or directory\n");
        fossil_io_printf("\n{blue,bold,underline}Global Flags:{normal}\n");
        fossil_io_printf("  {cyan,bold}--help{normal}    - Show command help\n");
        fossil_io_printf("  {cyan,bold}--version{normal} - Display Shark Tool version\n");
        fossil_io_printf("  {cyan,bold}-v, --verbose{normal} - Enable detailed output\n");
        fossil_io_printf("  {cyan,bold}-q, --quiet{normal}   - Suppress standard output\n");
        fossil_io_printf("  {cyan,bold}--dry-run{normal}     - Simulate actions without changes\n");
        fossil_io_printf("  {cyan,bold}--color{normal}       - Colorize output where applicable\n");
        fossil_io_printf("  {cyan,bold}--time{normal}        - Display timestamps in output\n");
        fossil_io_printf("{cyan,italic}------------------------------------------------------------{normal}\n");

        // Always return after showing overview (default is "all")
        return 0;
    }

    // Detailed help for specific command
    if (clikely(cnotnull(command))) {
        if (fossil_io_cstring_equals(command, "show")) {
            fossil_io_printf("{blue,bold,underline}Usage:{normal} {green}show [options] <path>{normal}\n");
            fossil_io_printf("{blue,bold,underline}Options:{normal}\n");
            fossil_io_printf("  {cyan,bold}-a, --all{normal}        Show hidden files\n");
            fossil_io_printf("  {cyan,bold}-l, --long{normal}       Detailed info\n");
            fossil_io_printf("  {cyan,bold}-h, --human{normal}      Human-readable sizes\n");
            fossil_io_printf("  {cyan,bold}-r, --recursive{normal}  Include subdirectories\n");
            fossil_io_printf("  {cyan,bold}-d, --depth <n>{normal}  Limit recursion depth\n");
            fossil_io_printf("  {cyan,bold}--as <format>{normal}    Output format: list, tree, graph\n");
            fossil_io_printf("  {cyan,bold}--time{normal}           Show timestamps\n");
        } else if (fossil_io_cstring_equals(command, "move")) {
            fossil_io_printf("{blue,bold,underline}Usage:{normal} {green}move [options] <src> <dest>{normal}\n");
            fossil_io_printf("{blue,bold,underline}Options:{normal}\n");
            fossil_io_printf("  {cyan,bold}-f, --force{normal}      Overwrite without prompt\n");
            fossil_io_printf("  {cyan,bold}-i, --interactive{normal} Ask before overwrite\n");
            fossil_io_printf("  {cyan,bold}-b, --backup{normal}     Backup before move\n");
        } else if (fossil_io_cstring_equals(command, "copy")) {
            fossil_io_printf("{blue,bold,underline}Usage:{normal} {green}copy [options] <src> <dest>{normal}\n");
            fossil_io_printf("{blue,bold,underline}Options:{normal}\n");
            fossil_io_printf("  {cyan,bold}-r, --recursive{normal}  Copy subdirectories\n");
            fossil_io_printf("  {cyan,bold}-u, --update{normal}     Only copy newer files\n");
            fossil_io_printf("  {cyan,bold}-p, --preserve{normal}   Keep permissions/timestamps\n");
        } else if (fossil_io_cstring_equals(command, "remove") || fossil_io_cstring_equals(command, "delete")) {
            fossil_io_printf("{blue,bold,underline}Usage:{normal} {green}%s [options] <path>{normal}\n", command);
            fossil_io_printf("{blue,bold,underline}Options:{normal}\n");
            fossil_io_printf("  {cyan,bold}-r, --recursive{normal}  Delete contents\n");
            fossil_io_printf("  {cyan,bold}-f, --force{normal}      No confirmation\n");
            fossil_io_printf("  {cyan,bold}-i, --interactive{normal} Confirm per file\n");
            fossil_io_printf("  {cyan,bold}--trash{normal}          Move to system trash\n");
        } else if (fossil_io_cstring_equals(command, "rename")) {
            fossil_io_printf("{blue,bold,underline}Usage:{normal} {green}rename [options] <old_name> <new_name>{normal}\n");
            fossil_io_printf("{blue,bold,underline}Options:{normal}\n");
            fossil_io_printf("  {cyan,bold}-f, --force{normal}      Overwrite target\n");
            fossil_io_printf("  {cyan,bold}-i, --interactive{normal} Confirm before overwrite\n");
        } else if (fossil_io_cstring_equals(command, "create")) {
            fossil_io_printf("{blue,bold,underline}Usage:{normal} {green}create [options] <path>{normal}\n");
            fossil_io_printf("{blue,bold,underline}Options:{normal}\n");
            fossil_io_printf("  {cyan,bold}-p, --parents{normal}    Create parent directories\n");
            fossil_io_printf("  {cyan,bold}-t, --type{normal}       Creates either file or dir\n");
        } else if (fossil_io_cstring_equals(command, "search")) {
            fossil_io_printf("{blue,bold,underline}Usage:{normal} {green}search [options] <path>{normal}\n");
            fossil_io_printf("{blue,bold,underline}Options:{normal}\n");
            fossil_io_printf("  {cyan,bold}-r, --recursive{normal}  Include subdirectories\n");
            fossil_io_printf("  {cyan,bold}-n, --name{normal}       Match filename\n");
            fossil_io_printf("  {cyan,bold}-c, --content{normal}    Search in file contents\n");
            fossil_io_printf("  {cyan,bold}-i, --ignore-case{normal} Case-insensitive\n");
        } else if (fossil_io_cstring_equals(command, "archive")) {
            fossil_io_printf("{blue,bold,underline}Usage:{normal} {green}archive [options] <path>{normal}\n");
            fossil_io_printf("{blue,bold,underline}Options:{normal}\n");
            fossil_io_printf("  {cyan,bold}-c, --create{normal}     New archive\n");
            fossil_io_printf("  {cyan,bold}-x, --extract{normal}    Extract contents\n");
            fossil_io_printf("  {cyan,bold}-l, --list{normal}       List archive\n");
            fossil_io_printf("  {cyan,bold}-f <format>{normal}      Format: zip/tar/gz\n");
            fossil_io_printf("  {cyan,bold}-p, --password{normal}   Encrypt archive\n");
        } else if (fossil_io_cstring_equals(command, "view")) {
            fossil_io_printf("{blue,bold,underline}Usage:{normal} {green}view [options] <file>{normal}\n");
            fossil_io_printf("{blue,bold,underline}Options:{normal}\n");
            fossil_io_printf("  {cyan,bold}-n, --number{normal}      Number all lines\n");
            fossil_io_printf("  {cyan,bold}-b, --non-blank{normal}   Number non-empty lines\n");
            fossil_io_printf("  {cyan,bold}-s, --squeeze{normal}     Remove blank lines\n");
            fossil_io_printf("  {cyan,bold}-h, --head <n>{normal}    First n lines\n");
            fossil_io_printf("  {cyan,bold}-t, --tail <n>{normal}    Last n lines\n");
            fossil_io_printf("  {cyan,bold}--time{normal}            Show timestamps\n");
        } else if (fossil_io_cstring_equals(command, "compare")) {
            fossil_io_printf("{blue,bold,underline}Usage:{normal} {green}compare [options] <path1> <path2>{normal}\n");
            fossil_io_printf("{blue,bold,underline}Options:{normal}\n");
            fossil_io_printf("  {cyan,bold}-t, --text{normal}       Line diff\n");
            fossil_io_printf("  {cyan,bold}-b, --binary{normal}     Binary diff\n");
            fossil_io_printf("  {cyan,bold}--context <n>{normal}    Show context lines\n");
            fossil_io_printf("  {cyan,bold}--ignore-case{normal}    Ignore case differences\n");
        } else if (fossil_io_cstring_equals(command, "sync")) {
            fossil_io_printf("{blue,bold,underline}Usage:{normal} {green}sync [options] <src> <dest>{normal}\n");
            fossil_io_printf("{blue,bold,underline}Options:{normal}\n");
            fossil_io_printf("  {cyan,bold}-r, --recursive{normal}  Include subdirs\n");
            fossil_io_printf("  {cyan,bold}-u, --update{normal}     Copy only newer\n");
            fossil_io_printf("  {cyan,bold}--delete{normal}         Remove extraneous files from target\n");
        } else if (fossil_io_cstring_equals(command, "watch")) {
            fossil_io_printf("{blue,bold,underline}Usage:{normal} {green}watch [options] <path>{normal}\n");
            fossil_io_printf("{blue,bold,underline}Options:{normal}\n");
            fossil_io_printf("  {cyan,bold}-r, --recursive{normal}      Include subdirs\n");
            fossil_io_printf("  {cyan,bold}-e, --events <list>{normal}  Filter events: create/modify/delete\n");
            fossil_io_printf("  {cyan,bold}-t, --interval <n>{normal}   Poll interval in seconds\n");
        } else if (fossil_io_cstring_equals(command, "ask")) {
            fossil_io_printf("{blue,bold,underline}Command:{normal} {green}ask{normal}\n");
            fossil_io_printf("{blue,bold,underline}Description:{normal} Run a one-shot prompt against a module or chain.\n");
            fossil_io_printf("{blue,bold,underline}Common Flags:{normal}\n");
            fossil_io_printf("  {cyan,bold}-m, --model <id>{normal}    Model to use\n");
            fossil_io_printf("  {cyan,bold}-f, --file <path>{normal}   Provide file context\n");
            fossil_io_printf("  {cyan,bold}--explain{normal}           Request explanation\n");
        } else if (fossil_io_cstring_equals(command, "chat")) {
            fossil_io_printf("{blue,bold,underline}Command:{normal} {green}chat{normal}\n");
            fossil_io_printf("{blue,bold,underline}Description:{normal} Interactive conversation session with a local module.\n");
            fossil_io_printf("{blue,bold,underline}Common Flags:{normal}\n");
            fossil_io_printf("  {cyan,bold}--context{normal}           Keep conversation history\n");
            fossil_io_printf("  {cyan,bold}--save <file>{normal}       Save chat transcript\n");
            fossil_io_printf("  {cyan,bold}-m, --model <id>{normal}    Model to use\n");
        } else if (fossil_io_cstring_equals(command, "summery")) {
            fossil_io_printf("{blue,bold,underline}Command:{normal} {green}summary{normal}\n");
            fossil_io_printf("{blue,bold,underline}Description:{normal} Summarize datasets, chains, logs, or model states.\n");
            fossil_io_printf("{blue,bold,underline}Common Flags:{normal}\n");
            fossil_io_printf("  {cyan,bold}-f, --file <path>{normal}   File to summarize\n");
            fossil_io_printf("  {cyan,bold}--depth <n>{normal}         Summary depth\n");
            fossil_io_printf("  {cyan,bold}--time{normal}              Show timestamps\n");
        } else if (fossil_io_cstring_equals(command, "help")) {
            fossil_io_printf("{blue,bold,underline}Usage:{normal} {green}help [command]{normal}\n");
            fossil_io_printf("{blue,bold,underline}Options:{normal}\n");
            fossil_io_printf("  {cyan,bold}--examples{normal}  Show usage examples\n");
            fossil_io_printf("  {cyan,bold}--man{normal}       Show full manual\n");
        } else if (fossil_io_cstring_equals(command, "rewrite")) {
            fossil_io_printf("{blue,bold,underline}Usage:{normal} {green}rewrite [options] <path> [content]{normal}\n");
            fossil_io_printf("{blue,bold,underline}Options:{normal}\n");
            fossil_io_printf("  {cyan,bold}--in-place{normal}       Modify file directly (default)\n");
            fossil_io_printf("  {cyan,bold}-a, --append{normal}     Append content instead of overwriting\n");
            fossil_io_printf("  {cyan,bold}--size <n>{normal}       Truncate or extend file to exact size\n");
            fossil_io_printf("  {cyan,bold}--access-time{normal}    Update file access time\n");
            fossil_io_printf("  {cyan,bold}--mod-time{normal}       Update file modification time\n");
            fossil_io_printf("  {cyan,bold}<content>{normal}        Optional content to write\n");
        } else if (fossil_io_cstring_equals(command, "introspect")) {
            fossil_io_printf("{blue,bold,underline}Usage:{normal} {green}introspect [options] <path>{normal}\n");
            fossil_io_printf("{blue,bold,underline}Options:{normal}\n");
            fossil_io_printf("  {cyan,bold}--head <n>{normal}       Show first n lines\n");
            fossil_io_printf("  {cyan,bold}--tail <n>{normal}       Show last n lines\n");
            fossil_io_printf("  {cyan,bold}--count{normal}          Count lines, words, bytes\n");
            fossil_io_printf("  {cyan,bold}--type{normal}           Show file type or MIME\n");
            fossil_io_printf("  {cyan,bold}--fson{normal}           Output metadata in FSON format\n");
        } else if (fossil_io_cstring_equals(command, "grammar")) {
            fossil_io_printf("{blue,bold,underline}Usage:{normal} {green}grammar [options] <file>{normal}\n");
            fossil_io_printf("{blue,bold,underline}Options:{normal}\n");
            fossil_io_printf("  {cyan,bold}--check{normal}           Run grammar check\n");
            fossil_io_printf("  {cyan,bold}--fix{normal}             Auto-correct grammar\n");
            fossil_io_printf("  {cyan,bold}--sanitize{normal}        Remove rot-brain/meme language\n");
            fossil_io_printf("  {cyan,bold}--suggest{normal}         Suggest alternatives\n");
            fossil_io_printf("  {cyan,bold}--tone{normal}            Detect tone\n");
            fossil_io_printf("  {cyan,bold}--detect <type>{normal}   Run detectors: ragebait, clickbait, spam, woke, bot, sarcasm, formal, snowflake, offensive, neutral, hype, quality, political, conspiracy, marketing, technobabble\n");
        } else {
            fossil_io_fprintf(FOSSIL_STDERR, "{red,bold,blink}Unknown command: %s{normal}\n", command);
            return 1;
        }

        if (clikely(show_examples)) {
            fossil_io_printf("\n{blue,bold,underline}Example usage:{normal}\n");
            if (fossil_io_cstring_equals(command, "show"))
            fossil_io_printf("  {cyan,bold}shark show -alh --as=tree --time{normal}\n");
            else if (fossil_io_cstring_equals(command, "move"))
            fossil_io_printf("  {cyan,bold}shark move -i -b old.txt archive/old.txt{normal}\n");
            else if (fossil_io_cstring_equals(command, "copy"))
            fossil_io_printf("  {cyan,bold}shark copy -rp src/ backup/{normal}\n");
            else if (fossil_io_cstring_equals(command, "remove") || fossil_io_cstring_equals(command, "delete"))
            fossil_io_printf("  {cyan,bold}shark remove -r --trash temp/{normal}\n");
            else if (fossil_io_cstring_equals(command, "rename"))
            fossil_io_printf("  {cyan,bold}shark rename -i draft.md final.md{normal}\n");
            else if (fossil_io_cstring_equals(command, "create"))
            fossil_io_printf("  {cyan,bold}shark create -p -t dir logs/archive/2024/{normal}\n");
            else if (fossil_io_cstring_equals(command, "search"))
            fossil_io_printf("  {cyan,bold}shark search -rc \"config\"{normal}\n");
            else if (fossil_io_cstring_equals(command, "archive"))
            fossil_io_printf("  {cyan,bold}shark archive -c -f tar project.tar src/{normal}\n");
            else if (fossil_io_cstring_equals(command, "view"))
            fossil_io_printf("  {cyan,bold}shark view -n -h 20 --time notes.txt{normal}\n");
            else if (fossil_io_cstring_equals(command, "compare"))
            fossil_io_printf("  {cyan,bold}shark compare -t main_v1.c main_v2.c --context 5{normal}\n");
            else if (fossil_io_cstring_equals(command, "sync"))
            fossil_io_printf("  {cyan,bold}shark sync -ru src/ dest/{normal}\n");
            else if (fossil_io_cstring_equals(command, "watch"))
            fossil_io_printf("  {cyan,bold}shark watch -r -e create,delete src/{normal}\n");
            else if (fossil_io_cstring_equals(command, "rewrite"))
            fossil_io_printf("  {cyan,bold}shark rewrite -i --append log.txt \"New entry\"{normal}\n");
            else if (fossil_io_cstring_equals(command, "introspect"))
            fossil_io_printf("  {cyan,bold}shark introspect --mime report.pdf{normal}\n");
            else if (fossil_io_cstring_equals(command, "grammar"))
            fossil_io_printf("  {cyan,bold}shark grammar --check --tone notes.txt{normal}\n");
            else if (fossil_io_cstring_equals(command, "chat"))
            fossil_io_printf("  {cyan,bold}shark chat --context -m jelly.fish --save chat.txt{normal}\n");
            else if (fossil_io_cstring_equals(command, "ask"))
            fossil_io_printf("  {cyan,bold}shark ask -m jelly.fish -f script.sh --explain \"Is this script safe?\"{normal}\n");
            else if (fossil_io_cstring_equals(command, "summary"))
            fossil_io_printf("  {cyan,bold}shark summary -f report.txt --depth 2 --time{normal}\n");
        }

        if (cunlikely(full_manual)) {
            fossil_io_printf("\n{blue,bold,underline}Manual for '{green}%s{normal}{blue,bold,underline}':{normal}\n", command);
            fossil_io_printf("{cyan,italic}------------------------------------------------------------{normal}\n");
            fossil_io_printf("  This section provides a detailed manual for the '%s' command.\n", command);
            fossil_io_printf("  Usage, options, flags, and examples are described above.\n");
            fossil_io_printf("  For more information, visit the official documentation or use 'shark help %s --examples'.\n", command);
            fossil_io_printf("{cyan,italic}------------------------------------------------------------{normal}\n");
        }
    }

    return 0;
}
