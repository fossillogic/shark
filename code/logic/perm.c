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

#include <sys/stat.h>
#include <unistd.h>
#include <pwd.h>
#include <grp.h>
#include <dirent.h>
#include <errno.h>

/* ------------------------------------------------------------
 * Helper: Convert permission string (e.g., "rwx") to mode bits
 * ------------------------------------------------------------ */
static mode_t perm_string_to_mode(const char* perm_str) {
    mode_t mode = 0;
    if (!perm_str) return mode;

    for (size_t i = 0; i < strlen(perm_str); i++) {
        switch (perm_str[i]) {
            case 'r': mode |= S_IRUSR | S_IRGRP | S_IROTH; break;
            case 'w': mode |= S_IWUSR | S_IWGRP | S_IWOTH; break;
            case 'x': mode |= S_IXUSR | S_IXGRP | S_IXOTH; break;
            default: break;
        }
    }
    return mode;
}

/* ------------------------------------------------------------
 * Helper: Change permissions of a single file
 * ------------------------------------------------------------ */
static int fossil_shark_perm_apply(
    const char* path,
    const char* user,
    const char* group,
    const char* grant_perm,
    const char* revoke_perm
) {
    struct stat st;
    if (stat(path, &st) != 0) {
        perror("stat failed");
        return -1;
    }

    mode_t mode = st.st_mode;

    /* Grant permissions */
    if (grant_perm) {
        mode |= perm_string_to_mode(grant_perm);
    }

    /* Revoke permissions */
    if (revoke_perm) {
        mode &= ~perm_string_to_mode(revoke_perm);
    }

    if (chmod(path, mode) != 0) {
        perror("chmod failed");
        return -1;
    }

    /* Change ownership if requested */
    if (user || group) {
        uid_t uid = user ? getpwnam(user)->pw_uid : st.st_uid;
        gid_t gid = group ? getgrnam(group)->gr_gid : st.st_gid;

        if (chown(path, uid, gid) != 0) {
            perror("chown failed");
            return -1;
        }
    }

    return 0;
}

/* ------------------------------------------------------------
 * Recursive permission application
 * ------------------------------------------------------------ */
static int fossil_shark_perm_recursive(
    const char* path,
    const char* user,
    const char* group,
    const char* grant_perm,
    const char* revoke_perm
) {
    int ret = fossil_shark_perm_apply(path, user, group, grant_perm, revoke_perm);
    if (ret != 0) return ret;

    struct stat st;
    if (stat(path, &st) != 0) return ret;

    if (S_ISDIR(st.st_mode)) {
        DIR* dir = opendir(path);
        if (!dir) return -1;
        struct dirent* entry;
        while ((entry = readdir(dir))) {
            if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) continue;
            char child_path[1024];
            snprintf(child_path, sizeof(child_path), "%s/%s", path, entry->d_name);
            int child_ret = fossil_shark_perm_recursive(child_path, user, group, grant_perm, revoke_perm);
            if (child_ret != 0) {
                closedir(dir);
                return child_ret;
            }
        }
        closedir(dir);
    }
    return 0;
}

/* ------------------------------------------------------------
 * Main fossil_shark_perm function
 * ------------------------------------------------------------ */
int fossil_shark_perm(
    const char* path,
    const char* user,
    const char* group,
    const char* grant_perm,
    const char* revoke_perm,
    bool list,
    bool recursive
) {
    if (!path) {
        fprintf(stderr, "Error: path must be provided.\n");
        return -1;
    }

    /* List current permissions if requested */
    if (list) {
        struct stat st;
        if (stat(path, &st) != 0) {
            perror("stat failed");
            return -1;
        }
        printf("Path: %s\n", path);
        printf("Permissions: %o\n", st.st_mode & 0777);
        printf("Owner UID: %d\n", st.st_uid);
        printf("Group GID: %d\n", st.st_gid);
        return 0;
    }

    /* Apply permissions */
    if (recursive) {
        return fossil_shark_perm_recursive(path, user, group, grant_perm, revoke_perm);
    } else {
        return fossil_shark_perm_apply(path, user, group, grant_perm, revoke_perm);
    }
}
