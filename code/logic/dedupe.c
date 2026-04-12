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
#include "fossil/code/dedupe.h"

#define MAX_HASH_LEN 128

int fossil_spino_dedupe(
    const char* dir_path,
    bool use_hash,
    bool interactive,
    bool delete_files,
    bool link_files,
    bool output_json
)
{
    if (!dir_path) return -1;

    fossil_io_filesys_obj_t entries[1024];
    size_t count = 0;
    int rc = fossil_io_filesys_dir_list(dir_path, entries, 1024, &count);
    if (rc < 0) return rc;

    typedef struct file_node_t {
        char path[FOSSIL_FILESYS_MAX_PATH];
        char hash[MAX_HASH_LEN];
        struct file_node_t* next;
    } file_node_t;

    file_node_t* head = NULL;

    for (size_t i = 0; i < count; ++i) {
        fossil_io_filesys_obj_t* obj = &entries[i];

        if (obj->type != FOSSIL_FILESYS_TYPE_FILE) continue;

        char file_hash[MAX_HASH_LEN] = {0};
        if (use_hash) {
            // Read file contents
            fossil_io_filesys_file_t f = {0};
            if (fossil_io_filesys_file_open(&f, obj->path, "rb") != 0) continue;

            uint8_t* buffer = malloc(obj->size);
            if (!buffer) { fossil_io_filesys_file_close(&f); continue; }
            size_t read_bytes = fossil_io_filesys_file_read(&f, buffer, 1, obj->size);
            fossil_io_filesys_file_close(&f);

            if (read_bytes != obj->size) { free(buffer); continue; }

            fossil_cryptic_hash_compute("xor", "auto", "hex", file_hash, sizeof(file_hash), buffer, obj->size);
            free(buffer);
        } else {
            // Use size+mtime as hash surrogate
            snprintf(file_hash, sizeof(file_hash),
         "%zu-%lld", obj->size, (long long)obj->modified_at);
        }

        // Compare against previously seen files
        file_node_t* node = head;
        bool duplicate_found = false;
        while (node) {
            if (strcmp(node->hash, file_hash) == 0) {
                duplicate_found = true;

                if (output_json)
                    fossil_io_printf("{\"duplicate\":\"%s\",\"original\":\"%s\"}\n", obj->path, node->path);
                else
                    fossil_io_printf("Duplicate found: %s -> %s\n", obj->path, node->path);

                bool do_delete = delete_files;
                if (interactive) {
                    fossil_io_printf("Delete %s? (y/n): ", obj->path);
                    int c = getchar();
                    while (getchar() != '\n');
                    do_delete = (c == 'y' || c == 'Y');
                }

                if (do_delete) {
                    fossil_io_filesys_remove(obj->path, false);
                    if (link_files) fossil_io_filesys_link_create(node->path, obj->path, true);
                }

                break;
            }
            node = node->next;
        }

        if (!duplicate_found) {
            file_node_t* new_node = malloc(sizeof(file_node_t));
            if (!new_node) continue;
            strncpy(new_node->path, obj->path, sizeof(new_node->path));
            strncpy(new_node->hash, file_hash, sizeof(new_node->hash));
            new_node->next = head;
            head = new_node;
        }
    }

    // Free linked list
    file_node_t* tmp;
    while (head) {
        tmp = head;
        head = head->next;
        free(tmp);
    }

    return 0;
}
