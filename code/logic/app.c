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
    fossil_io_printf("{blue}Usage: {cyan}%s{blue} <command> [options]{reset}\n\n", app_name);
    
    fossil_io_printf("{blue}🗂 Core File Operations:{reset}\n");
    fossil_io_printf("{cyan}  show             {reset}Display files and directories\n");
    fossil_io_printf("{yellow}                   -a, --all         Show hidden files\n");
    fossil_io_printf("                   -l, --long        Detailed info\n");
    fossil_io_printf("                   -h, --human       Human-readable sizes\n");
    fossil_io_printf("                   -r, --recursive   Include subdirs\n");
    fossil_io_printf("                   -d, --depth <n>   Limit recursion depth\n");
    fossil_io_printf("                   --as <format>     Format: list, tree, graph\n");
    fossil_io_printf("                   --time            Show timestamps{reset}\n");

    fossil_io_printf("{cyan}  move             {reset}Move or rename files/directories\n");
    fossil_io_printf("{yellow}                   -f, --force       Overwrite without prompt\n");
    fossil_io_printf("                   -i, --interactive Ask before overwrite\n");
    fossil_io_printf("                   -b, --backup      Backup before move{reset}\n");

    fossil_io_printf("{cyan}  copy             {reset}Copy files or directories\n");
    fossil_io_printf("{yellow}                   -r, --recursive   Copy subdirectories\n");
    fossil_io_printf("                   -u, --update      Only copy newer\n");
    fossil_io_printf("                   -p, --preserve    Keep permissions/timestamps{reset}\n");

    fossil_io_printf("{cyan}  remove/delete    {reset}Delete files or directories\n");
    fossil_io_printf("{yellow}                   -r, --recursive   Delete contents\n");
    fossil_io_printf("                   -f, --force       No confirmation\n");
    fossil_io_printf("                   -i, --interactive Confirm per file\n");
    fossil_io_printf("                   --trash           Move to system trash{reset}\n");

    fossil_io_printf("{cyan}  rename           {reset}Rename files or directories\n");
    fossil_io_printf("{yellow}                   -f, --force       Overwrite target\n");
    fossil_io_printf("                   -i, --interactive Confirm before overwrite{reset}\n");

    fossil_io_printf("{cyan}  create           {reset}Create new directories or files\n");
    fossil_io_printf("{yellow}                   -p, --parents     Create parent dirs\n");
    fossil_io_printf("                   -t, --type        Creates either file or dir{reset}\n");

    fossil_io_printf("{cyan}  search           {reset}Find files by name or content\n");
    fossil_io_printf("{yellow}                   -r, --recursive   Include subdirs\n");
    fossil_io_printf("                   -n, --name        Match filename\n");
    fossil_io_printf("                   -c, --content     Search in file contents\n");
    fossil_io_printf("                   -i, --ignore-case Case-insensitive{reset}\n");

    fossil_io_printf("{cyan}  archive          {reset}Create, extract, or list archives\n");
    fossil_io_printf("{yellow}                   -c, --create      New archive\n");
    fossil_io_printf("                   -x, --extract     Extract contents\n");
    fossil_io_printf("                   -l, --list        List archive\n");
    fossil_io_printf("                   -f <format>       Format: zip/tar/gz\n");
    fossil_io_printf("                   -p, --password    Encrypt archive{reset}\n");

    fossil_io_printf("{cyan}  view             {reset}Output file contents to terminal\n");
    fossil_io_printf("{yellow}                   -n, --number      Number all lines\n");
    fossil_io_printf("                   -b, --non-blank   Number non-empty lines\n");
    fossil_io_printf("                   -s, --squeeze     Remove blank lines\n");
    fossil_io_printf("                   -h, --head <n>    First n lines\n");
    fossil_io_printf("                   -t, --tail <n>    Last n lines\n");
    fossil_io_printf("                   --time            Show timestamps{reset}\n");

    fossil_io_printf("{cyan}  compare          {reset}Compare two files/directories\n");
    fossil_io_printf("{yellow}                   -t, --text        Line diff\n");
    fossil_io_printf("                   -b, --binary      Binary diff\n");
    fossil_io_printf("                   --context <n>     Show context lines\n");
    fossil_io_printf("                   --ignore-case     Ignore case differences{reset}\n");

    fossil_io_printf("{cyan}  info             {reset}Show detailed metadata about a file or directory\n");
    fossil_io_printf("{yellow}                   -p, --permissions Show permissions\n");
    fossil_io_printf("                   -o, --owner       Show owner/group\n");
    fossil_io_printf("                   -s, --size        Show size\n");
    fossil_io_printf("                   -t, --timestamps  Show times{reset}\n");

    fossil_io_printf("{cyan}  sync             {reset}Synchronize files/directories\n");
    fossil_io_printf("{yellow}                   -r, --recursive   Include subdirs\n");
    fossil_io_printf("                   -u, --update      Copy only newer\n");
    fossil_io_printf("                   --delete          Remove extraneous files from target{reset}\n");

    fossil_io_printf("{cyan}  watch            {reset}Continuously monitor files or directories\n");
    fossil_io_printf("{yellow}                   -r, --recursive   Include subdirs\n");
    fossil_io_printf("                   -e, --events <list> Filter events: create/modify/delete\n");
    fossil_io_printf("                   -t, --interval <n> Poll interval in seconds{reset}\n");

    fossil_io_printf("{cyan}  help             {reset}Display help for supported commands\n");
    fossil_io_printf("{yellow}                   --examples        Show usage examples\n");
    fossil_io_printf("                   --man             Full manual{reset}\n\n");

    fossil_io_printf("{blue}🤖 AI Commands (Jellyfish Integration):{reset}\n");
    fossil_io_printf("{cyan}  chat             {reset}Start an interactive AI chat session\n");
    fossil_io_printf("{yellow}                   -f, --file <path> Use file content\n");
    fossil_io_printf("                   -m, --model <name> Select model\n");
    fossil_io_printf("                   -s, --system <role> AI persona\n");
    fossil_io_printf("                   --save <path>     Save chat transcript\n");
    fossil_io_printf("                   --context         Keep session history{reset}\n");

    fossil_io_printf("{cyan}  ask              {reset}Ask Jellyfish AI a one-shot question\n");
    fossil_io_printf("{yellow}                   -f, --file <path> Provide file context\n");
    fossil_io_printf("                   --explain         Force explanation\n");
    fossil_io_printf("                   --analyze         Deep analysis\n");
    fossil_io_printf("                   -q, --quiet       Minimal output{reset}\n");

    fossil_io_printf("{cyan}  summery          {reset}Generate AI summary of file/directory\n");
    fossil_io_printf("{yellow}                   -f, --file <path> Use file content\n");
    fossil_io_printf("                   --depth <level>   Summary depth\n");
    fossil_io_printf("                   -q, --quiet       Minimal output\n");
    fossil_io_printf("                   --color           Highlight key items\n");
    fossil_io_printf("                   --time            Include timestamps{reset}\n\n");

    fossil_io_printf("{blue}🌍 Global Options:{reset}\n");
    fossil_io_printf("{cyan}  --help           {reset}Display help information\n");
    fossil_io_printf("{cyan}  --version        {reset}Display current version\n");
    fossil_io_printf("{cyan}  --name           {reset}Display app name\n");
    fossil_io_printf("{cyan}  --verbose        {reset}Enable detailed output\n");
    fossil_io_printf("{cyan}  --color          {reset}Set color mode: enable, disable, auto\n");
    fossil_io_printf("{cyan}  --clear          {reset}Clear terminal screen\n");
    
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
        }
        // File Operations Commands
        else if (fossil_io_cstring_compare(argv[i], "show") == 0) {
            // Parse show command arguments and call fossil_shark_show
            const char *path = "."; // default current directory
            bool show_all = false, long_format = false, human_readable = false;
            bool recursive = false, show_time = false;
            const char *format = "list";
            int depth = -1;
            
            // Parse flags
            for (int j = i + 1; j < argc && argv[j][0] == '-'; j++) {
            if (fossil_io_cstring_compare(argv[j], "-a") == 0 || fossil_io_cstring_compare(argv[j], "--all") == 0) {
                show_all = true;
            } else if (fossil_io_cstring_compare(argv[j], "-l") == 0 || fossil_io_cstring_compare(argv[j], "--long") == 0) {
                long_format = true;
            } else if (fossil_io_cstring_compare(argv[j], "-h") == 0 || fossil_io_cstring_compare(argv[j], "--human") == 0) {
                human_readable = true;
            } else if (fossil_io_cstring_compare(argv[j], "-r") == 0 || fossil_io_cstring_compare(argv[j], "--recursive") == 0) {
                recursive = true;
            } else if (fossil_io_cstring_compare(argv[j], "--time") == 0) {
                show_time = true;
            } else if (fossil_io_cstring_compare(argv[j], "--as") == 0 && j + 1 < argc) {
                format = argv[++j];
            } else if (fossil_io_cstring_compare(argv[j], "-d") == 0 || fossil_io_cstring_compare(argv[j], "--depth") == 0) {
                if (j + 1 < argc) depth = atoi(argv[++j]);
            } else {
                path = argv[j];
            }
            i = j;
            }
            if (i + 1 < argc && argv[i + 1][0] != '-') path = argv[++i];
            fossil_shark_show(path, show_all, long_format, human_readable, recursive, format, show_time, depth);
            
        } else if (fossil_io_cstring_compare(argv[i], "move") == 0) {
            // Parse move command arguments and call fossil_shark_move
            const char *src = NULL, *dest = NULL;
            bool force = false, interactive = false, backup = false;
            
            for (int j = i + 1; j < argc; j++) {
            if (fossil_io_cstring_compare(argv[j], "-f") == 0 || fossil_io_cstring_compare(argv[j], "--force") == 0) {
                force = true;
            } else if (fossil_io_cstring_compare(argv[j], "-i") == 0 || fossil_io_cstring_compare(argv[j], "--interactive") == 0) {
                interactive = true;
            } else if (fossil_io_cstring_compare(argv[j], "-b") == 0 || fossil_io_cstring_compare(argv[j], "--backup") == 0) {
                backup = true;
            } else if (!src) {
                src = argv[j];
            } else if (!dest) {
                dest = argv[j];
            }
            i = j;
            }
            if (src && dest) fossil_shark_move(src, dest, force, interactive, backup);
            
        } else if (fossil_io_cstring_compare(argv[i], "copy") == 0) {
            // Parse copy command arguments and call fossil_shark_copy
            const char *src = NULL, *dest = NULL;
            bool recursive = false, update = false, preserve = false;
            
            for (int j = i + 1; j < argc; j++) {
            if (fossil_io_cstring_compare(argv[j], "-r") == 0 || fossil_io_cstring_compare(argv[j], "--recursive") == 0) {
                recursive = true;
            } else if (fossil_io_cstring_compare(argv[j], "-u") == 0 || fossil_io_cstring_compare(argv[j], "--update") == 0) {
                update = true;
            } else if (fossil_io_cstring_compare(argv[j], "-p") == 0 || fossil_io_cstring_compare(argv[j], "--preserve") == 0) {
                preserve = true;
            } else if (!src) {
                src = argv[j];
            } else if (!dest) {
                dest = argv[j];
            }
            i = j;
            }
            if (src && dest) fossil_shark_copy(src, dest, recursive, update, preserve);
            
        } else if (fossil_io_cstring_compare(argv[i], "remove") == 0 || 
               fossil_io_cstring_compare(argv[i], "delete") == 0) {
            // Parse remove command arguments and call fossil_shark_remove
            const char *path = NULL;
            bool recursive = false, force = false, interactive = false, use_trash = false;
            
            for (int j = i + 1; j < argc; j++) {
            if (fossil_io_cstring_compare(argv[j], "-r") == 0 || fossil_io_cstring_compare(argv[j], "--recursive") == 0) {
                recursive = true;
            } else if (fossil_io_cstring_compare(argv[j], "-f") == 0 || fossil_io_cstring_compare(argv[j], "--force") == 0) {
                force = true;
            } else if (fossil_io_cstring_compare(argv[j], "-i") == 0 || fossil_io_cstring_compare(argv[j], "--interactive") == 0) {
                interactive = true;
            } else if (fossil_io_cstring_compare(argv[j], "--trash") == 0) {
                use_trash = true;
            } else if (!path) {
                path = argv[j];
            }
            i = j;
            }
            if (path) fossil_shark_remove(path, recursive, force, interactive, use_trash);
            
        } else if (fossil_io_cstring_compare(argv[i], "rename") == 0) {
            // Parse rename command arguments and call fossil_shark_rename
            const char *old_name = NULL, *new_name = NULL;
            bool force = false, interactive = false;
            
            for (int j = i + 1; j < argc; j++) {
            if (fossil_io_cstring_compare(argv[j], "-f") == 0 || fossil_io_cstring_compare(argv[j], "--force") == 0) {
                force = true;
            } else if (fossil_io_cstring_compare(argv[j], "-i") == 0 || fossil_io_cstring_compare(argv[j], "--interactive") == 0) {
                interactive = true;
            } else if (!old_name) {
                old_name = argv[j];
            } else if (!new_name) {
                new_name = argv[j];
            }
            i = j;
            }
            if (old_name && new_name) fossil_shark_rename(old_name, new_name, force, interactive);
            
        } else if (fossil_io_cstring_compare(argv[i], "create") == 0) {
            // Parse create command arguments and call fossil_shark_create
            const char *path = NULL, *type = "dir";
            bool create_parents = false;
            
            for (int j = i + 1; j < argc; j++) {
            if (fossil_io_cstring_compare(argv[j], "-p") == 0 || fossil_io_cstring_compare(argv[j], "--parents") == 0) {
                create_parents = true;
            } else if (fossil_io_cstring_compare(argv[j], "-t") == 0 || fossil_io_cstring_compare(argv[j], "--type") == 0) {
                if (j + 1 < argc) type = argv[++j];
            } else if (!path) {
                path = argv[j];
            }
            i = j;
            }
            if (path) fossil_shark_create(path, create_parents, type);
            
        } else if (fossil_io_cstring_compare(argv[i], "search") == 0) {
            // Parse search command arguments and call fossil_shark_search
            const char *path = ".", *name_pattern = NULL, *content_pattern = NULL;
            bool recursive = false, ignore_case = false;
            
            for (int j = i + 1; j < argc; j++) {
            if (fossil_io_cstring_compare(argv[j], "-r") == 0 || fossil_io_cstring_compare(argv[j], "--recursive") == 0) {
                recursive = true;
            } else if (fossil_io_cstring_compare(argv[j], "-i") == 0 || fossil_io_cstring_compare(argv[j], "--ignore-case") == 0) {
                ignore_case = true;
            } else if (fossil_io_cstring_compare(argv[j], "-n") == 0 || fossil_io_cstring_compare(argv[j], "--name") == 0) {
                if (j + 1 < argc) name_pattern = argv[++j];
            } else if (fossil_io_cstring_compare(argv[j], "-c") == 0 || fossil_io_cstring_compare(argv[j], "--content") == 0) {
                if (j + 1 < argc) content_pattern = argv[++j];
            } else if (argv[j][0] != '-') {
                path = argv[j];
            }
            i = j;
            }
            fossil_shark_search(path, recursive, name_pattern, content_pattern, ignore_case);
            
        } else if (fossil_io_cstring_compare(argv[i], "archive") == 0) {
            // Parse archive command arguments and call fossil_shark_archive
            const char *path = NULL, *format = "zip", *password = NULL;
            bool create = false, extract = false, list = false;
            
            for (int j = i + 1; j < argc; j++) {
            if (fossil_io_cstring_compare(argv[j], "-c") == 0 || fossil_io_cstring_compare(argv[j], "--create") == 0) {
                create = true;
            } else if (fossil_io_cstring_compare(argv[j], "-x") == 0 || fossil_io_cstring_compare(argv[j], "--extract") == 0) {
                extract = true;
            } else if (fossil_io_cstring_compare(argv[j], "-l") == 0 || fossil_io_cstring_compare(argv[j], "--list") == 0) {
                list = true;
            } else if (fossil_io_cstring_compare(argv[j], "-f") == 0 && j + 1 < argc) {
                format = argv[++j];
            } else if (fossil_io_cstring_compare(argv[j], "-p") == 0 || fossil_io_cstring_compare(argv[j], "--password") == 0) {
                if (j + 1 < argc) password = argv[++j];
            } else if (!path) {
                path = argv[j];
            }
            i = j;
            }
            if (path) fossil_shark_archive(path, create, extract, list, format, password);
            
        } else if (fossil_io_cstring_compare(argv[i], "view") == 0) {
            // Parse view command arguments and call fossil_shark_view
            const char *path = NULL;
            bool number_lines = false, number_non_blank = false, squeeze_blank = false, show_time = false;
            int head_lines = 0, tail_lines = 0;
            
            for (int j = i + 1; j < argc; j++) {
            if (fossil_io_cstring_compare(argv[j], "-n") == 0 || fossil_io_cstring_compare(argv[j], "--number") == 0) {
                number_lines = true;
            } else if (fossil_io_cstring_compare(argv[j], "-b") == 0 || fossil_io_cstring_compare(argv[j], "--non-blank") == 0) {
                number_non_blank = true;
            } else if (fossil_io_cstring_compare(argv[j], "-s") == 0 || fossil_io_cstring_compare(argv[j], "--squeeze") == 0) {
                squeeze_blank = true;
            } else if (fossil_io_cstring_compare(argv[j], "--time") == 0) {
                show_time = true;
            } else if (fossil_io_cstring_compare(argv[j], "-h") == 0 || fossil_io_cstring_compare(argv[j], "--head") == 0) {
                if (j + 1 < argc) head_lines = atoi(argv[++j]);
            } else if (fossil_io_cstring_compare(argv[j], "-t") == 0 || fossil_io_cstring_compare(argv[j], "--tail") == 0) {
                if (j + 1 < argc) tail_lines = atoi(argv[++j]);
            } else if (!path) {
                path = argv[j];
            }
            i = j;
            }
            if (path) fossil_shark_view(path, number_lines, number_non_blank, squeeze_blank, head_lines, tail_lines, show_time);
            
        } else if (fossil_io_cstring_compare(argv[i], "compare") == 0) {
            // Parse compare command arguments and call fossil_shark_compare
            const char *path1 = NULL, *path2 = NULL;
            bool text_diff = false, binary_diff = false, ignore_case = false;
            int context_lines = 3;
            
            for (int j = i + 1; j < argc; j++) {
            if (fossil_io_cstring_compare(argv[j], "-t") == 0 || fossil_io_cstring_compare(argv[j], "--text") == 0) {
                text_diff = true;
            } else if (fossil_io_cstring_compare(argv[j], "-b") == 0 || fossil_io_cstring_compare(argv[j], "--binary") == 0) {
                binary_diff = true;
            } else if (fossil_io_cstring_compare(argv[j], "--ignore-case") == 0) {
                ignore_case = true;
            } else if (fossil_io_cstring_compare(argv[j], "--context") == 0 && j + 1 < argc) {
                context_lines = atoi(argv[++j]);
            } else if (!path1) {
                path1 = argv[j];
            } else if (!path2) {
                path2 = argv[j];
            }
            i = j;
            }
            if (path1 && path2) fossil_shark_compare(path1, path2, text_diff, binary_diff, context_lines, ignore_case);
            
        } else if (fossil_io_cstring_compare(argv[i], "help") == 0) {
            // Parse help command arguments and call fossil_shark_help
            const char *command = NULL;
            bool show_examples = false, full_manual = false;
            
            for (int j = i + 1; j < argc; j++) {
            if (fossil_io_cstring_compare(argv[j], "--examples") == 0) {
                show_examples = true;
            } else if (fossil_io_cstring_compare(argv[j], "--man") == 0) {
                full_manual = true;
            } else if (!command && argv[j][0] != '-') {
                command = argv[j];
            }
            i = j;
            }
            fossil_shark_help(command, show_examples, full_manual);
        }
        // AI Commands
        else if (fossil_io_cstring_compare(argv[i], "chat") == 0) {
            // Parse chat command arguments and call fossil_shark_chat
            const char *file_path = NULL, *model_name = NULL, *system_role = NULL, *save_path = NULL;
            bool keep_context = false;
            
            for (int j = i + 1; j < argc; j++) {
            if (fossil_io_cstring_compare(argv[j], "-f") == 0 || fossil_io_cstring_compare(argv[j], "--file") == 0) {
                if (j + 1 < argc) file_path = argv[++j];
            } else if (fossil_io_cstring_compare(argv[j], "-m") == 0 || fossil_io_cstring_compare(argv[j], "--model") == 0) {
                if (j + 1 < argc) model_name = argv[++j];
            } else if (fossil_io_cstring_compare(argv[j], "-s") == 0 || fossil_io_cstring_compare(argv[j], "--system") == 0) {
                if (j + 1 < argc) system_role = argv[++j];
            } else if (fossil_io_cstring_compare(argv[j], "--save") == 0 && j + 1 < argc) {
                save_path = argv[++j];
            } else if (fossil_io_cstring_compare(argv[j], "--context") == 0) {
                keep_context = true;
            }
            i = j;
            }
            fossil_shark_chat(file_path, model_name, system_role, save_path, keep_context);
            
        } else if (fossil_io_cstring_compare(argv[i], "ask") == 0) {
            // Parse ask command arguments and call fossil_shark_ask
            const char *file_path = NULL;
            bool explain = false, analyze = false, quiet = false;
            
            for (int j = i + 1; j < argc; j++) {
            if (fossil_io_cstring_compare(argv[j], "-f") == 0 || fossil_io_cstring_compare(argv[j], "--file") == 0) {
                if (j + 1 < argc) file_path = argv[++j];
            } else if (fossil_io_cstring_compare(argv[j], "--explain") == 0) {
                explain = true;
            } else if (fossil_io_cstring_compare(argv[j], "--analyze") == 0) {
                analyze = true;
            } else if (fossil_io_cstring_compare(argv[j], "-q") == 0 || fossil_io_cstring_compare(argv[j], "--quiet") == 0) {
                quiet = true;
            }
            i = j;
            }
            fossil_shark_ask(file_path, explain, analyze, quiet);
            
        } else if (fossil_io_cstring_compare(argv[i], "summery") == 0) {
            // Parse summery command arguments and call fossil_shark_summery
            const char *file_path = NULL;
            int depth = 5;
            bool quiet = false, color = false, show_time = false;
            
            for (int j = i + 1; j < argc; j++) {
            if (fossil_io_cstring_compare(argv[j], "-f") == 0 || fossil_io_cstring_compare(argv[j], "--file") == 0) {
                if (j + 1 < argc) file_path = argv[++j];
            } else if (fossil_io_cstring_compare(argv[j], "--depth") == 0 && j + 1 < argc) {
                depth = atoi(argv[++j]);
            } else if (fossil_io_cstring_compare(argv[j], "-q") == 0 || fossil_io_cstring_compare(argv[j], "--quiet") == 0) {
                quiet = true;
            } else if (fossil_io_cstring_compare(argv[j], "--color") == 0) {
                color = true;
            } else if (fossil_io_cstring_compare(argv[j], "--time") == 0) {
                show_time = true;
            }
            i = j;
            }
            fossil_shark_summery(file_path, depth, quiet, color, show_time);
            
        } else if (fossil_io_cstring_compare(argv[i], "sync") == 0) {
            // Parse sync command arguments and call fossil_shark_sync
            const char *src = NULL, *dest = NULL;
            bool recursive = false, update = false, delete_flag = false;
            for (int j = i + 1; j < argc; j++) {
            if (fossil_io_cstring_compare(argv[j], "-r") == 0 || fossil_io_cstring_compare(argv[j], "--recursive") == 0) {
                recursive = true;
            } else if (fossil_io_cstring_compare(argv[j], "-u") == 0 || fossil_io_cstring_compare(argv[j], "--update") == 0) {
                update = true;
            } else if (fossil_io_cstring_compare(argv[j], "--delete") == 0) {
                delete_flag = true;
            } else if (!src) {
                src = argv[j];
            } else if (!dest) {
                dest = argv[j];
            }
            i = j;
            }
            if (src && dest) fossil_shark_sync(src, dest, recursive, update, delete_flag);

        } else if (fossil_io_cstring_compare(argv[i], "watch") == 0) {
            // Parse watch command arguments and call fossil_shark_watch
            const char *path = NULL, *events = NULL;
            bool recursive = false;
            int interval = 1;
            for (int j = i + 1; j < argc; j++) {
            if (fossil_io_cstring_compare(argv[j], "-r") == 0 || fossil_io_cstring_compare(argv[j], "--recursive") == 0) {
                recursive = true;
            } else if (fossil_io_cstring_compare(argv[j], "-e") == 0 || fossil_io_cstring_compare(argv[j], "--events") == 0) {
                if (j + 1 < argc) events = argv[++j];
            } else if (fossil_io_cstring_compare(argv[j], "-t") == 0 || fossil_io_cstring_compare(argv[j], "--interval") == 0) {
                if (j + 1 < argc) interval = atoi(argv[++j]);
            } else if (!path) {
                path = argv[j];
            }
            i = j;
            }
            if (path) fossil_shark_watch(path, recursive, events, interval);

        } else {
            fossil_io_printf("{red}Unknown command: %s{reset}\n", argv[i]);
            return false;
        }
    }
    return 0;
}
