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
#include "fossil/code/alias.h"

#define FOSSIL_SHARK_ALIAS_MAX 128
#define FOSSIL_SHARK_ALIAS_NAME_MAX 64
#define FOSSIL_SHARK_ALIAS_CMD_MAX 256

typedef struct {
    char name[FOSSIL_SHARK_ALIAS_NAME_MAX];
    char cmd[FOSSIL_SHARK_ALIAS_CMD_MAX];
    bool global_scope;
} fossil_shark_alias_t;

static fossil_shark_alias_t _fossil_shark_aliases[FOSSIL_SHARK_ALIAS_MAX];
static int _fossil_shark_alias_count = 0;

/* ------------------------------------------------------------
 * Find alias by name (internal)
 * ------------------------------------------------------------ */
static int fossil_shark_alias_find(const char* name) {
    for (int i = 0; i < _fossil_shark_alias_count; i++) {
        if (strcmp(_fossil_shark_aliases[i].name, name) == 0) {
            return i;
        }
    }
    return -1;
}

/* ------------------------------------------------------------
 * Add or update an alias
 * ------------------------------------------------------------ */
static int fossil_shark_alias_set(const char* name, const char* cmd, bool global_scope) {
    int idx = fossil_spino_alias_find(name);
    if (idx >= 0) {
        strncpy(_fossil_shark_aliases[idx].cmd, cmd, FOSSIL_SHARK_ALIAS_CMD_MAX);
        _fossil_shark_aliases[idx].global_scope = global_scope;
        return 0;
    }
    if (_fossil_shark_alias_count >= FOSSIL_SHARK_ALIAS_MAX) {
        fossil_io_fprintf(FOSSIL_STDERR, "Alias table full\n");
        return -1;
    }
    fossil_spino_alias_t* alias = &_fossil_shark_aliases[_fossil_shark_alias_count++];
    strncpy(alias->name, name, FOSSIL_SHARK_ALIAS_NAME_MAX);
    strncpy(alias->cmd, cmd, FOSSIL_SHARK_ALIAS_CMD_MAX);
    alias->global_scope = global_scope;
    return 0;
}

/* ------------------------------------------------------------
 * Remove alias by name
 * ------------------------------------------------------------ */
static int fossil_shark_alias_remove(const char* name) {
    int idx = fossil_shark_alias_find(name);
    if (idx < 0) return -1;
    for (int i = idx; i < _fossil_shark_alias_count - 1; i++) {
        _fossil_shark_aliases[i] = _fossil_shark_aliases[i + 1];
    }
    _fossil_shark_alias_count--;
    return 0;
}

/* ------------------------------------------------------------
 * List all aliases
 * ------------------------------------------------------------ */
static void fossil_shark_alias_list(void) {
    fossil_io_printf("Aliases:\n");
    for (int i = 0; i < _fossil_shark_alias_count; i++) {
        fossil_io_printf("  %s = %s %s\n",
            _fossil_shark_aliases[i].name,
            _fossil_shark_aliases[i].cmd,
            _fossil_shark_aliases[i].global_scope ? "(global)" : "(local)");
    }
}

int fossil_shark_alias(const char* set_alias, const char* remove_alias, bool list, bool global_scope) {
    int rc = 0;

    if (set_alias) {
        const char* eq = strchr(set_alias, '=');
        if (!eq || eq == set_alias) {
            fossil_io_fprintf(FOSSIL_STDERR, "Invalid alias format: %s\n", set_alias);
            return -1;
        }
        char name[FOSSIL_SHARK_ALIAS_NAME_MAX] = {0};
        char cmd[FOSSIL_SHARK_ALIAS_CMD_MAX] = {0};
        size_t name_len = eq - set_alias;
        if (name_len >= FOSSIL_SHARK_ALIAS_NAME_MAX) name_len = FOSSIL_SHARK_ALIAS_NAME_MAX - 1;
        strncpy(name, set_alias, name_len);
        strncpy(cmd, eq + 1, FOSSIL_SHARK_ALIAS_CMD_MAX - 1);
        rc = fossil_shark_alias_set(name, cmd, global_scope);
    }

    if (remove_alias) {
        rc = fossil_shark_alias_remove(remove_alias);
    }

    if (list) {
        fossil_shark_alias_list();
    }

    return rc;
}
