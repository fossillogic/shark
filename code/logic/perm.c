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
#include "fossil/code/perm.h"

/* ------------------------------------------------------------
 * Helper: Convert permission string ("rwx") to struct
 * ------------------------------------------------------------ */
static fossil_io_filesys_perms_t perm_string_to_struct(const char* perm_str)
{
    fossil_io_filesys_perms_t perms = {0};

    if (!perm_str) return perms;

    for (size_t i = 0; i < strlen(perm_str); i++)
    {
        switch (perm_str[i])
        {
            case 'r': perms.read = true; break;
            case 'w': perms.write = true; break;
            case 'x': perms.execute = true; break;
            default: break;
        }
    }

    return perms;
}

/* ------------------------------------------------------------
 * Apply permissions to a single object
 * ------------------------------------------------------------ */
static int fossil_spino_perm_apply(
    const fossil_io_filesys_obj_t* obj,
    const char* user,
    const char* group,
    const char* grant_perm,
    const char* revoke_perm
)
{
    if (!obj) return -1;

    fossil_io_filesys_perms_t perms = obj->perms;

    /* Apply grants */
    if (grant_perm)
    {
        fossil_io_filesys_perms_t g = perm_string_to_struct(grant_perm);
        if (g.read)    perms.read = true;
        if (g.write)   perms.write = true;
        if (g.execute) perms.execute = true;
    }

    /* Apply revokes */
    if (revoke_perm)
    {
        fossil_io_filesys_perms_t r = perm_string_to_struct(revoke_perm);
        if (r.read)    perms.read = false;
        if (r.write)   perms.write = false;
        if (r.execute) perms.execute = false;
    }

    int rc = 0;

    /* Apply permissions based on type */
    if (obj->type == FOSSIL_FILESYS_TYPE_FILE)
    {
        rc = fossil_io_filesys_file_set_perms(obj->path, perms);
    }
    else if (obj->type == FOSSIL_FILESYS_TYPE_DIR)
    {
        rc = fossil_io_filesys_dir_set_perms(obj->path, perms);
    }
    else if (obj->type == FOSSIL_FILESYS_TYPE_LINK)
    {
        rc = fossil_io_filesys_link_set_perms(obj->path, perms);
    }

    if (rc != 0)
    {
        fossil_io_fprintf(FOSSIL_STDERR, "Failed to set permissions on %s\n", obj->path);
        return rc;
    }

    /* Apply ownership if requested */
    if (user || group)
    {
        if (obj->type == FOSSIL_FILESYS_TYPE_FILE)
        {
            rc = fossil_io_filesys_file_set_owner(obj->path, user, group);
        }
        else if (obj->type == FOSSIL_FILESYS_TYPE_DIR)
        {
            rc = fossil_io_filesys_dir_set_owner(obj->path, user, group);
        }
        else if (obj->type == FOSSIL_FILESYS_TYPE_LINK)
        {
            rc = fossil_io_filesys_link_set_owner(obj->path, user, group);
        }

        if (rc != 0)
        {
            fossil_io_fprintf(FOSSIL_STDERR, "Failed to set owner/group on %s\n", obj->path);
            return rc;
        }
    }

    return 0;
}

/* ------------------------------------------------------------
 * Recursive callback
 * ------------------------------------------------------------ */
typedef struct
{
    const char* user;
    const char* group;
    const char* grant_perm;
    const char* revoke_perm;
} perm_ctx_t;

static int perm_walk_cb(const fossil_io_filesys_obj_t* obj, void* user_data)
{
    perm_ctx_t* ctx = (perm_ctx_t*)user_data;

    return fossil_spino_perm_apply(
        obj,
        ctx->user,
        ctx->group,
        ctx->grant_perm,
        ctx->revoke_perm
    );
}

/* ------------------------------------------------------------
 * Main fossil_spino_perm
 * ------------------------------------------------------------ */
int fossil_spino_perm(
    const char* path,
    const char* user,
    const char* group,
    const char* grant_perm,
    const char* revoke_perm,
    bool list,
    bool recursive
)
{
    if (!path)
    {
        fossil_io_fprintf(FOSSIL_IO_ERROR, "Error: path must be provided.\n");
        return -1;
    }

    fossil_io_filesys_obj_t obj;
    if (fossil_io_filesys_stat(path, &obj) != 0)
    {
        fossil_io_fprintf(FOSSIL_IO_ERROR, "Error: failed to stat path: %s\n", path);
        return -1;
    }

    /* --------------------------------------------------------
     * LIST MODE
     * -------------------------------------------------------- */
    if (list)
    {
        fossil_io_printf("Path: %s\n", obj.path);
        fossil_io_printf("Type: %s\n", fossil_io_filesys_type_string(obj.type));
        fossil_io_printf("Permissions: [r=%d w=%d x=%d]\n",
               obj.perms.read,
               obj.perms.write,
               obj.perms.execute);
        fossil_io_printf("Owner: %s\n", obj.owner);
        fossil_io_printf("Group: %s\n", obj.group);
        fossil_io_printf("Size: %zu\n", obj.size);
        return 0;
    }

    /* --------------------------------------------------------
     * APPLY MODE
     * -------------------------------------------------------- */
    if (recursive)
    {
        perm_ctx_t ctx = {
            .user = user,
            .group = group,
            .grant_perm = grant_perm,
            .revoke_perm = revoke_perm
        };

        return fossil_io_filesys_dir_walk(path, perm_walk_cb, &ctx);
    }
    else
    {
        return fossil_spino_perm_apply(
            &obj,
            user,
            group,
            grant_perm,
            revoke_perm
        );
    }
}
