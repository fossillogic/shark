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


int fossil_shark_help(ccstring command, bool show_examples, bool full_manual) {
    // Show overview if command is NULL or "all"
    if (!command || fossil_io_cstring_equals(command, "all")) {
        fossil_io_printf("{black,bold,underline}Fossil Shark - Command Overview:{normal}\n");
        fossil_io_printf("{black,italic}------------------------------------------------------------{normal}\n");
        fossil_io_printf("{blue,bold,underline}Core File Commands:{normal}\n");
        fossil_io_printf("  {cyan,bold}show{normal}        - Display files and directories\n");
        fossil_io_printf("  {cyan,bold}move{normal}        - Move or rename files/directories\n");
        fossil_io_printf("  {cyan,bold}copy{normal}        - Copy files or directories\n");
        fossil_io_printf("  {cyan,bold}remove{normal}      - Delete files or directories\n");
        fossil_io_printf("  {cyan,bold}delete{normal}      - Delete files or directories (alias for remove)\n");
        fossil_io_printf("  {cyan,bold}rename{normal}      - Rename files or directories\n");
        fossil_io_printf("  {cyan,bold}create{normal}      - Create new directories or files\n");
        fossil_io_printf("  {cyan,bold}search{normal}      - Find files by name or content\n");
        fossil_io_printf("  {cyan,bold}archive{normal}     - Create, extract, or list archives\n");
        fossil_io_printf("  {cyan,bold}view{normal}        - Output file contents to terminal\n");
        fossil_io_printf("  {cyan,bold}compare{normal}     - Compare files or directories\n");
        fossil_io_printf("  {cyan,bold}help{normal}        - Show help and usage\n");
        fossil_io_printf("  {cyan,bold}sync{normal}        - Synchronize files/directories\n");
        fossil_io_printf("  {cyan,bold}watch{normal}       - Monitor files/directories\n");
        fossil_io_printf("  {cyan,bold}rewrite{normal}     - Modify or update file contents\n");
        fossil_io_printf("  {cyan,bold}introspect{normal}  - Examine file contents, type, or metadata\n");
        fossil_io_printf("  {cyan,bold}grammar{normal}     - Grammar analysis and correction\n");
        fossil_io_printf("\n{blue,bold,underline}Global Flags & Special Commands:{normal}\n");
        fossil_io_printf("  {cyan,bold}--help{normal}      - Show command help\n");
        fossil_io_printf("  {cyan,bold}--version{normal}   - Display Shark Tool version\n");
        fossil_io_printf("  {cyan,bold}--name{normal}      - Show application name\n");
        fossil_io_printf("  {cyan,bold}--verbose{normal}   - Enable detailed output\n");
        fossil_io_printf("  {cyan,bold}--color{normal}     - Colorize output where applicable\n");
        fossil_io_printf("  {cyan,bold}--clear{normal}     - Clear the terminal screen\n");
        fossil_io_printf("{black,italic}------------------------------------------------------------{normal}\n");
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
            fossil_io_printf("  {cyan,bold}-r, --recursive{normal}  Include subdirs\n");
            fossil_io_printf("  {cyan,bold}-d, --depth <n>{normal}  Limit recursion depth\n");
            fossil_io_printf("  {cyan,bold}--as <format>{normal}    Output format: list, tree, graph, tiles\n");
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
            fossil_io_printf("  {cyan,bold}-p, --parents{normal}    Create parent dirs\n");
            fossil_io_printf("  {cyan,bold}-t, --type <type>{normal}  Create file or dir (default: dir)\n");
        } else if (fossil_io_cstring_equals(command, "search")) {
            fossil_io_printf("{blue,bold,underline}Usage:{normal} {green}search [options] <path>{normal}\n");
            fossil_io_printf("{blue,bold,underline}Options:{normal}\n");
            fossil_io_printf("  {cyan,bold}-r, --recursive{normal}  Include subdirs\n");
            fossil_io_printf("  {cyan,bold}-n, --name <pattern>{normal}   Match filename\n");
            fossil_io_printf("  {cyan,bold}-c, --content <pattern>{normal} Search in file contents\n");
            fossil_io_printf("  {cyan,bold}-i, --ignore-case{normal} Case-insensitive\n");
        } else if (fossil_io_cstring_equals(command, "archive")) {
            fossil_io_printf("{blue,bold,underline}Usage:{normal} {green}archive [options] <path>{normal}\n");
            fossil_io_printf("{blue,bold,underline}Options:{normal}\n");
            fossil_io_printf("  {cyan,bold}-c, --create{normal}     New archive\n");
            fossil_io_printf("  {cyan,bold}-x, --extract{normal}    Extract contents\n");
            fossil_io_printf("  {cyan,bold}-l, --list{normal}       List archive\n");
            fossil_io_printf("  {cyan,bold}-f <format>{normal}      Format: zip/tar/gz\n");
            fossil_io_printf("  {cyan,bold}-p, --password <pw>{normal}   Encrypt archive\n");
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
        } else if (fossil_io_cstring_equals(command, "help")) {
            fossil_io_printf("{blue,bold,underline}Usage:{normal} {green}help [command]{normal}\n");
            fossil_io_printf("{blue,bold,underline}Options:{normal}\n");
            fossil_io_printf("  {cyan,bold}--examples{normal}  Show usage examples\n");
            fossil_io_printf("  {cyan,bold}--man{normal}       Show full manual\n");
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
        } else if (fossil_io_cstring_equals(command, "rewrite")) {
            fossil_io_printf("{blue,bold,underline}Usage:{normal} {green}rewrite [options] <path> [content]{normal}\n");
            fossil_io_printf("{blue,bold,underline}Options:{normal}\n");
            fossil_io_printf("  {cyan,bold}-a, --append{normal}          Append instead of overwrite\n");
            fossil_io_printf("  {cyan,bold}--in-place{normal}            In-place edit (default)\n");
            fossil_io_printf("  {cyan,bold}--access-time{normal}         Update access time\n");
            fossil_io_printf("  {cyan,bold}--mod-time{normal}            Update modification time\n");
            fossil_io_printf("  {cyan,bold}--size <n>{normal}            Set exact file size\n");
        } else if (fossil_io_cstring_equals(command, "introspect")) {
            fossil_io_printf("{blue,bold,underline}Usage:{normal} {green}introspect [options] <path>{normal}\n");
            fossil_io_printf("{blue,bold,underline}Options:{normal}\n");
            fossil_io_printf("  {cyan,bold}--head <n>{normal}            Show first n lines\n");
            fossil_io_printf("  {cyan,bold}--tail <n>{normal}            Show last n lines\n");
            fossil_io_printf("  {cyan,bold}--count{normal}               Count lines/words/bytes\n");
            fossil_io_printf("  {cyan,bold}--type{normal}                Show file type\n");
            fossil_io_printf("  {cyan,bold}--fson{normal}                FSON output\n");
        } else if (fossil_io_cstring_equals(command, "grammar")) {
            fossil_io_printf("{blue,bold,underline}Usage:{normal} {green}grammar [options] <file>{normal}\n");
            fossil_io_printf("{blue,bold,underline}Options:{normal}\n");
            fossil_io_printf("  {cyan,bold}--check{normal}           Run grammar check\n");
            fossil_io_printf("  {cyan,bold}--fix{normal}             Auto-correct grammar\n");
            fossil_io_printf("  {cyan,bold}--sanitize{normal}        Remove rot-brain/meme language\n");
            fossil_io_printf("  {cyan,bold}--suggest{normal}         Suggest alternatives\n");
            fossil_io_printf("  {cyan,bold}--tone{normal}            Detect tone\n");
            fossil_io_printf("  {cyan,bold}--detect <type>{normal}   Run detectors: ragebait, clickbait, spam, woke, bot, sarcasm, formal, snowflake, offensive, neutral, hype, quality, political, conspiracy, marketing, technobabble\n");
        } else if (fossil_io_cstring_equals(command, "--help")) {
            fossil_io_printf("{blue,bold,underline}Usage:{normal} {green}--help{normal}\n");
            fossil_io_printf("{blue,bold,underline}Description:{normal} Show command help\n");
        } else if (fossil_io_cstring_equals(command, "--version")) {
            fossil_io_printf("{blue,bold,underline}Usage:{normal} {green}--version{normal}\n");
            fossil_io_printf("{blue,bold,underline}Description:{normal} Display Shark Tool version\n");
        } else if (fossil_io_cstring_equals(command, "--name")) {
            fossil_io_printf("{blue,bold,underline}Usage:{normal} {green}--name{normal}\n");
            fossil_io_printf("{blue,bold,underline}Description:{normal} Show application name\n");
        } else if (fossil_io_cstring_equals(command, "--verbose")) {
            fossil_io_printf("{blue,bold,underline}Usage:{normal} {green}--verbose{normal}\n");
            fossil_io_printf("{blue,bold,underline}Description:{normal} Enable detailed output\n");
        } else if (fossil_io_cstring_equals(command, "--color")) {
            fossil_io_printf("{blue,bold,underline}Usage:{normal} {green}--color [enable|disable|auto]{normal}\n");
            fossil_io_printf("{blue,bold,underline}Description:{normal} Colorize output where applicable\n");
        } else if (fossil_io_cstring_equals(command, "--clear")) {
            fossil_io_printf("{blue,bold,underline}Usage:{normal} {green}--clear{normal}\n");
            fossil_io_printf("{blue,bold,underline}Description:{normal} Clear the terminal screen\n");
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
            else if (fossil_io_cstring_equals(command, "help"))
            fossil_io_printf("  {cyan,bold}shark help show --examples{normal}\n");
            else if (fossil_io_cstring_equals(command, "sync"))
            fossil_io_printf("  {cyan,bold}shark sync -ru src/ dest/{normal}\n");
            else if (fossil_io_cstring_equals(command, "watch"))
            fossil_io_printf("  {cyan,bold}shark watch -r -e create,delete src/{normal}\n");
            else if (fossil_io_cstring_equals(command, "rewrite"))
            fossil_io_printf("  {cyan,bold}shark rewrite --in-place --append log.txt \"New entry\"{normal}\n");
            else if (fossil_io_cstring_equals(command, "introspect"))
            fossil_io_printf("  {cyan,bold}shark introspect --type --fson report.pdf{normal}\n");
            else if (fossil_io_cstring_equals(command, "grammar"))
            fossil_io_printf("  {cyan,bold}shark grammar --check --tone notes.txt{normal}\n");
        }

        // Manual page output omitted for brevity, but should be updated similarly.
        if (cunlikely(full_manual)) {
            fossil_io_printf("\n{blue,bold,underline}Manual Page for '{cyan}%s{normal}{blue,bold,underline}':{normal}\n", command);
            fossil_io_printf("{cyan,italic}------------------------------------------------------------{normal}\n");
            fossil_io_printf("{blue,bold}NAME{normal}\n  {cyan}%s{normal} - ", command);
            fossil_io_printf("{cyan}See command overview and options above.{normal}\n");
            fossil_io_printf("{blue,bold}SYNOPSIS{normal}\n  {cyan}shark %s [options] ...{normal}\n", command);
            fossil_io_printf("{blue,bold}DESCRIPTION{normal}\n  {cyan}See command overview and options above.{normal}\n");
            fossil_io_printf("{blue,bold}OPTIONS{normal}\n  {cyan}See command overview and options above.{normal}\n");
            fossil_io_printf("{blue,bold}EXAMPLE{normal}\n  {cyan}See example usage above.{normal}\n");
            fossil_io_printf("\n{blue,bold}SEE ALSO{normal}\n  {cyan}For more information, visit the official documentation or use 'shark help %s --examples'.{normal}\n", command);
            fossil_io_printf("{cyan,italic}------------------------------------------------------------{normal}\n");
        }
    }
    return 0;
}
