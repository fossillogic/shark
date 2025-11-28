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
        fossil_io_printf("  {cyan,bold}rewrite{normal}   - Rewrite file contents\n");
        fossil_io_printf("  {cyan,bold}introspect{normal}- Inspect file metadata/content\n");
        fossil_io_printf("  {cyan,bold}help{normal}      - Show help and usage\n");
        fossil_io_printf("  {cyan,bold}grammar{normal}   - Grammar analysis and correction\n");
        fossil_io_printf("\n{blue,bold,underline}AI Commands:{normal}\n");
        fossil_io_printf("  {cyan,bold}chat{normal}      - Interactive AI chat session\n");
        fossil_io_printf("  {cyan,bold}ask{normal}       - Ask AI a single question\n");
        fossil_io_printf("  {cyan,bold}summary{normal}   - Generate AI summary of file or directory\n");
        fossil_io_printf("\n{blue,bold,underline}Global Flags:{normal}\n");
        fossil_io_printf("  {cyan,bold}--help{normal}    - Show command help\n");
        fossil_io_printf("  {cyan,bold}--version{normal} - Display Shark Tool version\n");
        fossil_io_printf("  {cyan,bold}--name{normal}    - Show tool name\n");
        fossil_io_printf("  {cyan,bold}-v, --verbose{normal} - Enable detailed output\n");
        fossil_io_printf("  {cyan,bold}-q, --quiet{normal}   - Suppress standard output\n");
        fossil_io_printf("  {cyan,bold}--dry-run{normal}     - Simulate actions without changes\n");
        fossil_io_printf("  {cyan,bold}--color{normal}       - Colorize output where applicable\n");
        fossil_io_printf("  {cyan,bold}--clear{normal}       - Clear the screen\n");
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
        } else if (fossil_io_cstring_equals(command, "help")) {
            fossil_io_printf("{blue,bold,underline}Usage:{normal} {green}help [command]{normal}\n");
            fossil_io_printf("{blue,bold,underline}Options:{normal}\n");
            fossil_io_printf("  {cyan,bold}--examples{normal}  Show usage examples\n");
            fossil_io_printf("  {cyan,bold}--man{normal}       Show full manual\n");
        } else if (fossil_io_cstring_equals(command, "grammar")) {
            fossil_io_printf("{blue,bold,underline}Usage:{normal} {green}grammar [options] <file>{normal}\n");
            fossil_io_printf("{blue,bold,underline}Options:{normal}\n");
            fossil_io_printf("  {cyan,bold}--check{normal}           Run grammar check\n");
            fossil_io_printf("  {cyan,bold}--fix{normal}             Auto-correct grammar\n");
            fossil_io_printf("  {cyan,bold}--sanitize{normal}        Remove rot-brain/meme language\n");
            fossil_io_printf("  {cyan,bold}--suggest{normal}         Suggest alternatives\n");
            fossil_io_printf("  {cyan,bold}--tone{normal}            Detect tone\n");
            fossil_io_printf("  {cyan,bold}--detect <type>{normal}   Run detectors: ragebait, clickbait, spam, woke, bot, sarcasm, formal, snowflake, offensive, neutral, hype, quality, political, conspiracy, marketing, technobabble\n");
        } else if (fossil_io_cstring_equals(command, "summary")) {
            fossil_io_printf("{blue,bold,underline}Command:{normal} {green}summary{normal}\n");
            fossil_io_printf("{blue,bold,underline}Description:{normal} Generate summaries for files, directories, datasets, chains, logs, or model states.\n");
            fossil_io_printf("{blue,bold,underline}Common Flags:{normal}\n");
            fossil_io_printf("  {cyan,bold}-f, --file <path>{normal}    File to summarize\n");
            fossil_io_printf("  {cyan,bold}-l, --lines <n>{normal}      Limit number of lines analyzed\n");
            fossil_io_printf("  {cyan,bold}--auto{normal}               Auto-detect file type\n");
            fossil_io_printf("  {cyan,bold}--keywords{normal}           Extract keywords\n");
            fossil_io_printf("  {cyan,bold}--topics{normal}             Perform topic clustering\n");
            fossil_io_printf("  {cyan,bold}--stats{normal}              Show file statistics (lines, chars, entropy)\n");
            fossil_io_printf("  {cyan,bold}--fson{normal}               Output structured FSON summary\n");
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
            else if (fossil_io_cstring_equals(command, "summary"))
            fossil_io_printf("  {cyan,bold}shark summary -f report.txt -l 500 --keywords --stats --fson{normal}\n");
        }

        if (cunlikely(full_manual)) {
            fossil_io_printf("\n{blue,bold,underline}Manual Page for '{cyan}%s{normal}{blue,bold,underline}':{normal}\n", command);
            fossil_io_printf("{cyan,italic}------------------------------------------------------------{normal}\n");
            fossil_io_printf("{blue,bold}NAME{normal}\n  {cyan}%s{normal} - ", command);
            if (fossil_io_cstring_equals(command, "show")) {
                fossil_io_printf("{cyan}Display a list of files and directories in the specified path. Supports showing hidden files, long format for detailed information, human-readable sizes, recursive listing of subdirectories, and multiple output formats including list, tree, and graph. Useful for inspecting directory contents and file attributes.{normal}\n");
                fossil_io_printf("{blue,bold}SYNOPSIS{normal}\n  {cyan}shark show [options] <path>{normal}\n");
                fossil_io_printf("{blue,bold}DESCRIPTION{normal}\n  {cyan}Lists files and directories with options for hidden files, long format, human-readable sizes, recursion, and output format. Use this command to explore file system structure and details efficiently.{normal}\n");
                fossil_io_printf("{blue,bold}OPTIONS{normal}\n  {cyan}-a, --all        Show hidden files\n  -l, --long       Detailed info\n  -h, --human      Human-readable sizes\n  -r, --recursive  Include subdirectories\n  -d, --depth <n>  Limit recursion depth\n  --as <format>    Output format: list, tree, graph\n  --time           Show timestamps{normal}\n");
                fossil_io_printf("{blue,bold}EXAMPLE{normal}\n  {cyan}shark show -alh --as=tree --time{normal}\n");
            } else if (fossil_io_cstring_equals(command, "move")) {
                fossil_io_printf("{cyan}Move or rename files and directories from one location to another. Supports overwriting targets without prompt, interactive confirmation before overwrite, and automatic backup of files before moving. Ideal for reorganizing files and directories safely and efficiently.{normal}\n");
                fossil_io_printf("{blue,bold}SYNOPSIS{normal}\n  {cyan}shark move [options] <src> <dest>{normal}\n");
                fossil_io_printf("{blue,bold}DESCRIPTION{normal}\n  {cyan}Renames or moves files/directories. Supports force, interactive, and backup modes. Use this command to relocate files or change their names with safety options.{normal}\n");
                fossil_io_printf("{blue,bold}OPTIONS{normal}\n  {cyan}-f, --force      Overwrite without prompt\n  -i, --interactive  Ask before overwrite\n  -b, --backup     Backup before move{normal}\n");
                fossil_io_printf("{blue,bold}EXAMPLE{normal}\n  {cyan}shark move -i -b old.txt archive/old.txt{normal}\n");
            } else if (fossil_io_cstring_equals(command, "copy")) {
                fossil_io_printf("{cyan}Copy files or directories to a new location. Supports recursive copying of subdirectories, updating only newer files, and preserving file permissions and timestamps. Useful for creating backups or duplicating directory structures.{normal}\n");
                fossil_io_printf("{blue,bold}SYNOPSIS{normal}\n  {cyan}shark copy [options] <src> <dest>{normal}\n");
                fossil_io_printf("{blue,bold}DESCRIPTION{normal}\n  {cyan}Copies files or directories, with options for recursion, update, and preserving attributes. Use this command to duplicate files and directories with control over what gets copied.{normal}\n");
                fossil_io_printf("{blue,bold}OPTIONS{normal}\n  {cyan}-r, --recursive  Copy subdirectories\n  -u, --update     Only copy newer files\n  -p, --preserve   Keep permissions/timestamps{normal}\n");
                fossil_io_printf("{blue,bold}EXAMPLE{normal}\n  {cyan}shark copy -rp src/ backup/{normal}\n");
            } else if (fossil_io_cstring_equals(command, "remove") || fossil_io_cstring_equals(command, "delete")) {
                fossil_io_printf("{cyan}Delete files or directories from the file system. Supports recursive deletion of directory contents, force deletion without confirmation, interactive confirmation per file, and moving files to the system trash instead of permanent removal. Use with caution to avoid accidental data loss.{normal}\n");
                fossil_io_printf("{blue,bold}SYNOPSIS{normal}\n  {cyan}shark %s [options] <path>{normal}\n", command);
                fossil_io_printf("{blue,bold}DESCRIPTION{normal}\n  {cyan}Deletes files or directories. Supports recursion, force, interactive, and trash options. Use this command to clean up unwanted files and directories safely.{normal}\n");
                fossil_io_printf("{blue,bold}OPTIONS{normal}\n  {cyan}-r, --recursive  Delete contents\n  -f, --force      No confirmation\n  -i, --interactive  Confirm per file\n  --trash          Move to system trash{normal}\n");
                fossil_io_printf("{blue,bold}EXAMPLE{normal}\n  {cyan}shark remove -r --trash temp/{normal}\n");
            } else if (fossil_io_cstring_equals(command, "rename")) {
                fossil_io_printf("{cyan}Rename files or directories to a new name. Supports overwriting the target if it exists and interactive confirmation before overwriting. Useful for organizing and updating file names in bulk or individually.{normal}\n");
                fossil_io_printf("{blue,bold}SYNOPSIS{normal}\n  {cyan}shark rename [options] <old_name> <new_name>{normal}\n");
                fossil_io_printf("{blue,bold}DESCRIPTION{normal}\n  {cyan}Renames files or directories with optional force and interactive flags. Use this command to change file or directory names safely.{normal}\n");
                fossil_io_printf("{blue,bold}OPTIONS{normal}\n  {cyan}-f, --force      Overwrite target\n  -i, --interactive  Confirm before overwrite{normal}\n");
                fossil_io_printf("{blue,bold}EXAMPLE{normal}\n  {cyan}shark rename -i draft.md final.md{normal}\n");
            } else if (fossil_io_cstring_equals(command, "create")) {
                fossil_io_printf("{cyan}Create new files or directories at the specified path. Supports automatic creation of parent directories and specifying whether to create a file or directory. Useful for initializing new project structures or adding files in bulk.{normal}\n");
                fossil_io_printf("{blue,bold}SYNOPSIS{normal}\n  {cyan}shark create [options] <path>{normal}\n");
                fossil_io_printf("{blue,bold}DESCRIPTION{normal}\n  {cyan}Creates files or directories. Use -p for parent directories and -t for type. This command helps automate directory tree creation and file initialization.{normal}\n");
                fossil_io_printf("{blue,bold}OPTIONS{normal}\n  {cyan}-p, --parents    Create parent directories\n  -t, --type       Creates either file or dir{normal}\n");
                fossil_io_printf("{blue,bold}EXAMPLE{normal}\n  {cyan}shark create -p -t dir logs/archive/2024/{normal}\n");
            } else if (fossil_io_cstring_equals(command, "search")) {
                fossil_io_printf("{cyan}Search for files by name or content within a directory. Supports recursive search, matching filenames, searching within file contents, and case-insensitive matching. Useful for quickly locating files or text across large directory trees.{normal}\n");
                fossil_io_printf("{blue,bold}SYNOPSIS{normal}\n  {cyan}shark search [options] <path>{normal}\n");
                fossil_io_printf("{blue,bold}DESCRIPTION{normal}\n  {cyan}Finds files by name or content, with recursion and case-insensitive options. Use this command to locate files or specific text efficiently.{normal}\n");
                fossil_io_printf("{blue,bold}OPTIONS{normal}\n  {cyan}-r, --recursive  Include subdirectories\n  -n, --name       Match filename\n  -c, --content    Search in file contents\n  -i, --ignore-case  Case-insensitive{normal}\n");
                fossil_io_printf("{blue,bold}EXAMPLE{normal}\n  {cyan}shark search -rc \"config\"{normal}\n");
            } else if (fossil_io_cstring_equals(command, "archive")) {
                fossil_io_printf("{cyan}Create, extract, or list archive files such as zip, tar, or gz. Supports creating new archives, extracting contents, listing archive files, specifying archive format, and encrypting archives with a password. Useful for compressing, backing up, or distributing files and directories.{normal}\n");
                fossil_io_printf("{blue,bold}SYNOPSIS{normal}\n  {cyan}shark archive [options] <path>{normal}\n");
                fossil_io_printf("{blue,bold}DESCRIPTION{normal}\n  {cyan}Creates, extracts, or lists archives. Supports format and password options. Use this command to manage compressed files and backups.{normal}\n");
                fossil_io_printf("{blue,bold}OPTIONS{normal}\n  {cyan}-c, --create     New archive\n  -x, --extract    Extract contents\n  -l, --list       List archive\n  -f <format>      Format: zip/tar/gz\n  -p, --password   Encrypt archive{normal}\n");
                fossil_io_printf("{blue,bold}EXAMPLE{normal}\n  {cyan}shark archive -c -f tar project.tar src/{normal}\n");
            } else if (fossil_io_cstring_equals(command, "view")) {
                fossil_io_printf("{cyan}Output the contents of a file to the terminal. Supports line numbering, numbering only non-blank lines, squeezing blank lines, displaying only the first or last N lines, and showing timestamps. Useful for quickly inspecting file contents and structure.{normal}\n");
                fossil_io_printf("{blue,bold}SYNOPSIS{normal}\n  {cyan}shark view [options] <file>{normal}\n");
                fossil_io_printf("{blue,bold}DESCRIPTION{normal}\n  {cyan}Outputs file contents with line numbering, squeezing blanks, head/tail, and time options. Use this command to read files with enhanced formatting.{normal}\n");
                fossil_io_printf("{blue,bold}OPTIONS{normal}\n  {cyan}-n, --number      Number all lines\n  -b, --non-blank   Number non-empty lines\n  -s, --squeeze     Remove blank lines\n  -h, --head <n>    First n lines\n  -t, --tail <n>    Last n lines\n  --time            Show timestamps{normal}\n");
                fossil_io_printf("{blue,bold}EXAMPLE{normal}\n  {cyan}shark view -n -h 20 --time notes.txt{normal}\n");
            } else if (fossil_io_cstring_equals(command, "compare")) {
                fossil_io_printf("{cyan}Compare two files or directories to identify differences. Supports line-based text diff, binary diff, showing context lines around changes, and ignoring case differences. Useful for code review, file synchronization, and change tracking.{normal}\n");
                fossil_io_printf("{blue,bold}SYNOPSIS{normal}\n  {cyan}shark compare [options] <path1> <path2>{normal}\n");
                fossil_io_printf("{blue,bold}DESCRIPTION{normal}\n  {cyan}Compares files or directories with text/binary diff, context, and ignore-case options. Use this command to analyze changes and differences between files or folders.{normal}\n");
                fossil_io_printf("{blue,bold}OPTIONS{normal}\n  {cyan}-t, --text       Line diff\n  -b, --binary     Binary diff\n  --context <n>    Show context lines\n  --ignore-case    Ignore case differences{normal}\n");
                fossil_io_printf("{blue,bold}EXAMPLE{normal}\n  {cyan}shark compare -t main_v1.c main_v2.c --context 5{normal}\n");
            } else if (fossil_io_cstring_equals(command, "sync")) {
                fossil_io_printf("{cyan}Synchronize files and directories between two locations. Supports recursive synchronization, updating only newer files, and deleting extraneous files from the target. Useful for backups, mirroring, and keeping directories in sync.{normal}\n");
                fossil_io_printf("{blue,bold}SYNOPSIS{normal}\n  {cyan}shark sync [options] <src> <dest>{normal}\n");
                fossil_io_printf("{blue,bold}DESCRIPTION{normal}\n  {cyan}Synchronizes files/directories, supports recursion, update, and delete flags. Use this command to keep two directories identical and up-to-date.{normal}\n");
                fossil_io_printf("{blue,bold}OPTIONS{normal}\n  {cyan}-r, --recursive  Include subdirs\n  -u, --update     Copy only newer\n  --delete         Remove extraneous files from target{normal}\n");
                fossil_io_printf("{blue,bold}EXAMPLE{normal}\n  {cyan}shark sync -ru src/ dest/{normal}\n");
            } else if (fossil_io_cstring_equals(command, "watch")) {
                fossil_io_printf("{cyan}Monitor files or directories for changes and events such as creation, modification, or deletion. Supports recursive monitoring, filtering specific events, and setting polling intervals. Useful for automation, logging, and real-time notifications of file system activity.{normal}\n");
                fossil_io_printf("{blue,bold}SYNOPSIS{normal}\n  {cyan}shark watch [options] <path>{normal}\n");
                fossil_io_printf("{blue,bold}DESCRIPTION{normal}\n  {cyan}Monitors files/directories for events, with recursive and interval options. Use this command to track changes and respond to file system events.{normal}\n");
                fossil_io_printf("{blue,bold}OPTIONS{normal}\n  {cyan}-r, --recursive      Include subdirs\n  -e, --events <list>  Filter events: create/modify/delete\n  -t, --interval <n>   Poll interval in seconds{normal}\n");
                fossil_io_printf("{blue,bold}EXAMPLE{normal}\n  {cyan}shark watch -r -e create,delete src/{normal}\n");
            } else if (fossil_io_cstring_equals(command, "rewrite")) {
                fossil_io_printf("{cyan}Modify the contents of a file directly or by appending new content. Supports in-place modification, appending, truncating or extending file size, and updating access/modification times. Useful for editing logs, updating configuration files, or batch content changes.{normal}\n");
                fossil_io_printf("{blue,bold}SYNOPSIS{normal}\n  {cyan}shark rewrite [options] <path> [content]{normal}\n");
                fossil_io_printf("{blue,bold}DESCRIPTION{normal}\n  {cyan}Modifies file contents, supports in-place, append, size, and time options. Use this command to update files programmatically or manually.{normal}\n");
                fossil_io_printf("{blue,bold}OPTIONS{normal}\n  {cyan}--in-place       Modify file directly (default)\n  -a, --append     Append content instead of overwriting\n  --size <n>       Truncate or extend file to exact size\n  --access-time    Update file access time\n  --mod-time       Update file modification time\n  <content>        Optional content to write{normal}\n");
                fossil_io_printf("{blue,bold}EXAMPLE{normal}\n  {cyan}shark rewrite -i --append log.txt \"New entry\"{normal}\n");
            } else if (fossil_io_cstring_equals(command, "introspect")) {
                fossil_io_printf("{cyan}Inspect file metadata and content, including head/tail lines, counting lines/words/bytes, determining file type or MIME, and outputting metadata in FSON format. Useful for file analysis, reporting, and automation.{normal}\n");
                fossil_io_printf("{blue,bold}SYNOPSIS{normal}\n  {cyan}shark introspect [options] <path>{normal}\n");
                fossil_io_printf("{blue,bold}DESCRIPTION{normal}\n  {cyan}Inspects file metadata/content, supports head/tail, count, type, and FSON output. Use this command for detailed file analysis and reporting.{normal}\n");
                fossil_io_printf("{blue,bold}OPTIONS{normal}\n  {cyan}--head <n>       Show first n lines\n  --tail <n>       Show last n lines\n  --count          Count lines, words, bytes\n  --type           Show file type or MIME\n  --fson           Output metadata in FSON format{normal}\n");
                fossil_io_printf("{blue,bold}EXAMPLE{normal}\n  {cyan}shark introspect --mime report.pdf{normal}\n");
            } else if (fossil_io_cstring_equals(command, "help")) {
                fossil_io_printf("{cyan}Show help and usage information for commands. Supports displaying usage examples and the full manual for any command. Useful for learning command syntax and available options quickly.{normal}\n");
                fossil_io_printf("{blue,bold}SYNOPSIS{normal}\n  {cyan}shark help [command]{normal}\n");
                fossil_io_printf("{blue,bold}DESCRIPTION{normal}\n  {cyan}Shows help and usage for commands. Use --examples or --man for more details. This command is your entry point for learning about Shark's capabilities.{normal}\n");
                fossil_io_printf("{blue,bold}OPTIONS{normal}\n  {cyan}--examples  Show usage examples\n  --man       Show full manual{normal}\n");
                fossil_io_printf("{blue,bold}EXAMPLE{normal}\n  {cyan}shark help show --man{normal}\n");
            } else if (fossil_io_cstring_equals(command, "grammar")) {
                fossil_io_printf("{cyan}Analyze and correct grammar in files. Supports grammar checking, auto-correction, sanitizing language, suggesting alternatives, tone detection, and running various language detectors. Useful for improving writing quality and detecting problematic language in documents.{normal}\n");
                fossil_io_printf("{blue,bold}SYNOPSIS{normal}\n  {cyan}shark grammar [options] <file>{normal}\n");
                fossil_io_printf("{blue,bold}DESCRIPTION{normal}\n  {cyan}Analyzes and corrects grammar in files. Supports check, fix, sanitize, suggest, tone, and detect. Use this command to enhance document clarity and correctness.{normal}\n");
                fossil_io_printf("{blue,bold}OPTIONS{normal}\n  {cyan}--check           Run grammar check\n  --fix             Auto-correct grammar\n  --sanitize        Remove rot-brain/meme language\n  --suggest         Suggest alternatives\n  --tone            Detect tone\n  --detect <type>   Run detectors: ragebait, clickbait, spam, woke, bot, sarcasm, formal, snowflake, offensive, neutral, hype, quality, political, conspiracy, marketing, technobabble{normal}\n");
                fossil_io_printf("{blue,bold}EXAMPLE{normal}\n  {cyan}shark grammar --check --tone notes.txt{normal}\n");
            } else if (fossil_io_cstring_equals(command, "summary")) {
                fossil_io_printf("{cyan}Generate a structured summary of one or more files, including text, logs, code, or documentation. Supports keyword extraction, topic clustering, statistics, and FSON output.{normal}\n");
                fossil_io_printf("{blue,bold}SYNOPSIS{normal}\n  {cyan}shark summary [options] <file>{normal}\n");
                fossil_io_printf("{blue,bold}DESCRIPTION{normal}\n  {cyan}Produces an AI-driven structured summary of the specified files. Analyze specific number of lines, auto-detect file types, extract keywords, cluster topics, generate file statistics, or output in FSON format for programmatic use.{normal}\n");
                fossil_io_printf("{blue,bold}OPTIONS{normal}\n  {cyan}-l, --lines <n>     Limit the number of lines analyzed\n  --auto                 Auto-detect file type\n  --keywords             Extract keywords\n  --topics               Perform topic clustering\n  --stats                Show file statistics (chars/lines/entropy)\n  --fson                 Output structured summary in FSON format{normal}\n");
                fossil_io_printf("{blue,bold}EXAMPLE{normal}\n  {cyan}shark summary -l 200 --auto --keywords --topics --stats report.txt{normal}\n");
            } else {
                fossil_io_printf("{cyan}No manual available for this command.{normal}\n");
                fossil_io_printf("{blue,bold}DESCRIPTION{normal}\n  {cyan}No manual available for '%s'.{normal}\n", command);
            }
                fossil_io_printf("\n{blue,bold}SEE ALSO{normal}\n  {cyan}For more information, visit the official documentation or use 'shark help %s --examples'.{normal}\n", command);
                fossil_io_printf("{cyan,italic}------------------------------------------------------------{normal}\n");
        }
    }

    return 0;
}
