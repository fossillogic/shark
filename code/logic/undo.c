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
#include "fossil/code/undo.h"

#include <stdio.h>
#include <string.h>
#include <stdbool.h>

typedef enum {
    FOSSIL_SPINO_OP_MOVE,
    FOSSIL_SPINO_OP_COPY,
    FOSSIL_SPINO_OP_RENAME,
    FOSSIL_SPINO_OP_REMOVE
} fossil_spino_op_type_t;

typedef struct fossil_spino_op {
    fossil_spino_op_type_t type;
    char src[FOSSIL_FILESYS_MAX_PATH];
    char dest[FOSSIL_FILESYS_MAX_PATH]; // For move/rename/copy
} fossil_spino_op_t;

// Simple undo log (fixed size for demo)
#define FOSSIL_SPINO_UNDO_LOG_MAX 1024
static fossil_spino_op_t _fossil_spino_undo_log[FOSSIL_SPINO_UNDO_LOG_MAX];
static int _fossil_spino_undo_count = 0;

/* ------------------------------------------------------------
    * Add operation to undo log (internal)
    * ------------------------------------------------------------ */
void fossil_spino_undo_log_add(fossil_spino_op_type_t type, const char* src, const char* dest) {
    if (_fossil_spino_undo_count >= FOSSIL_SPINO_UNDO_LOG_MAX) return;
    fossil_spino_op_t* op = &_fossil_spino_undo_log[_fossil_spino_undo_count++];
    op->type = type;
    strncpy(op->src, src, sizeof(op->src));
    if (dest) strncpy(op->dest, dest, sizeof(op->dest));
    else op->dest[0] = '\0';
}

int fossil_spino_undo(int last_n, const char* file_path, bool interactive, bool dry_run) {
    if (_fossil_spino_undo_count == 0) {
        fprintf(stderr, "Nothing to undo.\n");
        return -1;
    }

    int undone = 0;
    for (int i = _fossil_spino_undo_count - 1; i >= 0 && undone < last_n; i--) {
        fossil_spino_op_t* op = &_fossil_spino_undo_log[i];

        // Skip if a specific file_path is given
        if (file_path && strcmp(op->src, file_path) != 0 && strcmp(op->dest, file_path) != 0)
            continue;

        if (interactive) {
            printf("Undo operation %d: %s -> %s? [y/N] ", i, op->src, op->dest[0] ? op->dest : "(removed)");
            char answer = getchar();
            getchar(); // consume newline
            if (answer != 'y' && answer != 'Y') continue;
        }

        if (!dry_run) {
            int rc = 0;
            switch (op->type) {
                case FOSSIL_SPINO_OP_MOVE:
                    rc = fossil_io_filesys_move(op->dest, op->src);
                    break;
                case FOSSIL_SPINO_OP_RENAME:
                    rc = fossil_io_filesys_move(op->dest, op->src);
                    break;
                case FOSSIL_SPINO_OP_COPY:
                    rc = fossil_io_filesys_remove(op->dest, false);
                    break;
                case FOSSIL_SPINO_OP_REMOVE:
                    // Can't restore removed file without backup; skipping
                    fprintf(stderr, "Cannot undo removal for %s without backup\n", op->src);
                    rc = -2;
                    break;
            }
            if (rc != 0) {
                fprintf(stderr, "Failed to undo %s -> %s\n", op->src, op->dest[0] ? op->dest : "(removed)");
                continue;
            }
        }

        undone++;
    }

    return undone;
}
