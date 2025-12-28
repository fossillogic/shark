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


__attribute__((unused))
static void fossil_shark_watch_file(const char *path, const char *events, struct stat *prev_stat)
{
    struct stat curr_stat;
    int rc = stat(path, &curr_stat);
    if (rc != 0) {
        if (errno == ENOENT && fossil_io_cstring_icontains(events, "delete")) {
            cstring del_msg = fossil_io_cstring_format("{red}File deleted:{normal} %s\n", path);
            fossil_io_file_write(FOSSIL_STDOUT, del_msg, fossil_io_cstring_length(del_msg), 1);
            fossil_io_cstring_free(del_msg);
        }
        return;
    }

    // Check modification time
    if (curr_stat.st_mtime != prev_stat->st_mtime && fossil_io_cstring_icontains(events, "modify")) {
        cstring mod_msg = fossil_io_cstring_format("{yellow}File modified:{normal} %s\n", path);
        fossil_io_file_write(FOSSIL_STDOUT, mod_msg, fossil_io_cstring_length(mod_msg), 1);
        fossil_io_cstring_free(mod_msg);
    }

    // Check size change
    if (curr_stat.st_size != prev_stat->st_size && fossil_io_cstring_icontains(events, "modify")) {
        cstring size_msg = fossil_io_cstring_format("{cyan}File size changed:{normal} %s\n", path);
        fossil_io_file_write(FOSSIL_STDOUT, size_msg, fossil_io_cstring_length(size_msg), 1);
        fossil_io_cstring_free(size_msg);
    }

    *prev_stat = curr_stat;
}

#if !defined(_WIN32) && !defined(_WIN64)
__attribute__((unused))
static void fossil_shark_watch_dir(const char *dir_path, const char *events, int interval)
{
    fossil_io_dir_iter_t it;
    if (fossil_io_dir_iter_open(&it, dir_path) != 0)
        return;

    while (fossil_io_dir_iter_next(&it) > 0) {
        fossil_io_dir_entry_t *entry = &it.current;
        if (strcmp(entry->name, ".") == 0 || strcmp(entry->name, "..") == 0)
            continue;

        if (entry->type == 1) { // Directory
            fossil_shark_watch_dir(entry->path, events, interval);
        } else if (entry->type == 0) { // File
            struct stat st;
            if (stat(entry->path, &st) == 0) {
                struct stat prev_stat = st;
                while (1) {
                    sleep(interval);
                    fossil_shark_watch_file(entry->path, events, &prev_stat);
                }
            }
        }
    }
    fossil_io_dir_iter_close(&it);
}
#endif

#if defined(_WIN32) || defined(_WIN64)

static wchar_t *fossil_utf8_to_wide(const char *s)
{
    if (!s) return NULL;
    int len = MultiByteToWideChar(CP_UTF8, 0, s, -1, NULL, 0);
    wchar_t *w = (wchar_t *)malloc(len * sizeof(wchar_t));
    if (!w) return NULL;
    MultiByteToWideChar(CP_UTF8, 0, s, -1, w, len);
    return w;
}

static int fossil_shark_watch_windows_recursive(
    const char *path,
    const char *events
)
{
    wchar_t *wpath = fossil_utf8_to_wide(path);
    if (!wpath)
        return ERROR_NOT_ENOUGH_MEMORY;

    HANDLE dir = CreateFileW(
        wpath,
        FILE_LIST_DIRECTORY,
        FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
        NULL,
        OPEN_EXISTING,
        FILE_FLAG_BACKUP_SEMANTICS,
        NULL
    );

    free(wpath);

    if (dir == INVALID_HANDLE_VALUE)
        return GetLastError();

    BYTE buffer[64 * 1024];
    DWORD bytes;

    while (1) {
        if (!ReadDirectoryChangesW(
                dir,
                buffer,
                sizeof(buffer),
                TRUE,  /* recursive */
                FILE_NOTIFY_CHANGE_FILE_NAME |
                FILE_NOTIFY_CHANGE_DIR_NAME  |
                FILE_NOTIFY_CHANGE_SIZE      |
                FILE_NOTIFY_CHANGE_LAST_WRITE,
                &bytes,
                NULL,
                NULL))
            break;

        FILE_NOTIFY_INFORMATION *fni =
            (FILE_NOTIFY_INFORMATION *)buffer;

        do {
            char filename[1024];
            int flen = WideCharToMultiByte(
                CP_UTF8, 0,
                fni->FileName,
                fni->FileNameLength / sizeof(WCHAR),
                filename,
                sizeof(filename) - 1,
                NULL,
                NULL
            );
            filename[flen] = '\0';

            bool emit = false;
            const char *etype = NULL;

            switch (fni->Action) {
            case FILE_ACTION_ADDED:
                emit = fossil_io_cstring_icontains(events, "create");
                etype = "created";
                break;
            case FILE_ACTION_REMOVED:
                emit = fossil_io_cstring_icontains(events, "delete");
                etype = "deleted";
                break;
            case FILE_ACTION_MODIFIED:
                emit = fossil_io_cstring_icontains(events, "modify");
                etype = "modified";
                break;
            case FILE_ACTION_RENAMED_OLD_NAME:
            case FILE_ACTION_RENAMED_NEW_NAME:
                emit = fossil_io_cstring_icontains(events, "rename");
                etype = "renamed";
                break;
            }

            if (emit) {
                cstring msg = fossil_io_cstring_format(
                    "{yellow}%s:{normal} %s\n",
                    etype, filename);
                fossil_io_file_write(
                    FOSSIL_STDOUT,
                    msg,
                    fossil_io_cstring_length(msg),
                    1);
                fossil_io_cstring_free(msg);
            }

            if (!fni->NextEntryOffset)
                break;

            fni = (FILE_NOTIFY_INFORMATION *)(
                (BYTE *)fni + fni->NextEntryOffset
            );
        } while (1);
    }

    CloseHandle(dir);
    return 0;
}

static int fossil_shark_watch_windows(
    const char *path,
    const char *events
)
{
    wchar_t *wpath = fossil_utf8_to_wide(path);
    if (!wpath)
        return ERROR_NOT_ENOUGH_MEMORY;

    HANDLE dir = CreateFileW(
        wpath,
        FILE_LIST_DIRECTORY,
        FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
        NULL,
        OPEN_EXISTING,
        FILE_FLAG_BACKUP_SEMANTICS,
        NULL
    );

    free(wpath);

    if (dir == INVALID_HANDLE_VALUE)
        return GetLastError();

    BYTE buffer[64 * 1024];
    DWORD bytes;

    while (1) {
        if (!ReadDirectoryChangesW(
                dir,
                buffer,
                sizeof(buffer),
                FALSE, /* non-recursive */
                FILE_NOTIFY_CHANGE_FILE_NAME |
                FILE_NOTIFY_CHANGE_DIR_NAME  |
                FILE_NOTIFY_CHANGE_SIZE      |
                FILE_NOTIFY_CHANGE_LAST_WRITE,
                &bytes,
                NULL,
                NULL))
            break;

        FILE_NOTIFY_INFORMATION *fni =
            (FILE_NOTIFY_INFORMATION *)buffer;

        do {
            char filename[1024];
            int flen = WideCharToMultiByte(
                CP_UTF8, 0,
                fni->FileName,
                fni->FileNameLength / sizeof(WCHAR),
                filename,
                sizeof(filename) - 1,
                NULL,
                NULL
            );
            filename[flen] = '\0';

            bool emit = false;
            const char *etype = NULL;

            switch (fni->Action) {
            case FILE_ACTION_ADDED:
                emit = fossil_io_cstring_icontains(events, "create");
                etype = "created";
                break;
            case FILE_ACTION_REMOVED:
                emit = fossil_io_cstring_icontains(events, "delete");
                etype = "deleted";
                break;
            case FILE_ACTION_MODIFIED:
                emit = fossil_io_cstring_icontains(events, "modify");
                etype = "modified";
                break;
            case FILE_ACTION_RENAMED_OLD_NAME:
            case FILE_ACTION_RENAMED_NEW_NAME:
                emit = fossil_io_cstring_icontains(events, "rename");
                etype = "renamed";
                break;
            }

            if (emit) {
                cstring msg = fossil_io_cstring_format(
                    "{yellow}%s:{normal} %s\n",
                    etype,
                    filename
                );
                fossil_io_file_write(
                    FOSSIL_STDOUT,
                    msg,
                    fossil_io_cstring_length(msg),
                    1
                );
                fossil_io_cstring_free(msg);
            }

            if (!fni->NextEntryOffset)
                break;

            fni = (FILE_NOTIFY_INFORMATION *)(
                (BYTE *)fni + fni->NextEntryOffset
            );
        } while (1);
    }

    CloseHandle(dir);
    return 0;
}

#endif

int fossil_shark_watch(const char *path, bool recursive,
                       const char *events, int interval)
{
    if (interval <= 0) {
        interval = 1; /* safety default */
    }

#if defined(_WIN32) || defined(_WIN64)

    cstring msg = fossil_io_cstring_format(
        "{green,bold}Watching %s every %d seconds...{reset}%s\n",
        path,
        interval,
        recursive ? " (recursive enabled)" : ""
    );
    fossil_io_file_write(
        FOSSIL_STDOUT,
        msg,
        fossil_io_cstring_length(msg),
        1
    );
    fossil_io_cstring_free(msg);

    while (1) {
        if (recursive) {
            fossil_shark_watch_windows_recursive(path, events);
        } else {
            fossil_shark_watch_windows(path, events);
        }

        /* Windows sleep uses milliseconds */
        Sleep((DWORD)(interval * 1000));
    }

#else /* POSIX */

    struct stat st;
    if (stat(path, &st) != 0) {
        cstring err_msg = fossil_io_cstring_format(
            "{red,bold}Failed to stat path:{reset} %s\n", path
        );
        fossil_io_file_write(
            FOSSIL_STDERR,
            err_msg,
            fossil_io_cstring_length(err_msg),
            1
        );
        fossil_io_cstring_free(err_msg);
        return errno;
    }

    cstring msg = fossil_io_cstring_format(
        "{green,bold}Watching %s every %d seconds...{reset}%s\n",
        path,
        interval,
        recursive ? " (recursive enabled)" : ""
    );
    fossil_io_file_write(
        FOSSIL_STDOUT,
        msg,
        fossil_io_cstring_length(msg),
        1
    );
    fossil_io_cstring_free(msg);

    if (recursive && S_ISDIR(st.st_mode)) {
        fossil_shark_watch_dir(path, events, interval);
    } else {
        struct stat prev_stat = st;
        while (1) {
            sleep(interval);
            fossil_shark_watch_file(path, events, &prev_stat);
        }
    }

#endif

    return 0;
}
