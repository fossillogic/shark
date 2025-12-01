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

static int file_hash(ccstring path, cstring out, size_t out_len) {
    fossil_io_file_t file;
    if (fossil_io_file_open(&file, path, "rb") != 0) return -1;

    unsigned char buf[4096];
    size_t n;
    int rc = 0;

    fossil_cryptic_hash_compute("sha256", "auto", "hex", out, out_len, cnull, 0);

    while ((n = fossil_io_file_read(&file, buf, 1, sizeof(buf))) > 0) {
        rc = fossil_cryptic_hash_compute("sha256", "auto", "hex", out, out_len, buf, n);
        if (rc != 0) break;
    }

    fossil_io_file_close(&file);
    return rc;
}

// Dedup: identical files → hardlink to first instance
static int perform_dedupe(ccstring dir) {
    fossil_io_dir_iter_t it;
    if (fossil_io_dir_iter_open(&it, dir) != 0) return -1;

    typedef struct { char hash[65], file[1024]; } entry_t;
    entry_t table[2048];
    int table_len = 0;

    while (fossil_io_dir_iter_next(&it) > 0) {
        fossil_io_dir_entry_t *e = &it.current;
        if (e->type != 0) continue; // Only files

        char hex[65];
        if (file_hash(e->path, hex, sizeof(hex)) == 0) {
            for (int i = 0; i < table_len; i++) {
                if (fossil_io_cstring_equals_safe(hex, table[i].hash, sizeof(hex))) {
                    unlink(e->path);
                    link(table[i].file, e->path);
                    goto next_file;
                }
            }
            fossil_io_cstring_copy_safe(hex, sizeof(table[table_len].hash));
            fossil_io_cstring_copy_safe(e->path, sizeof(table[table_len].file));
            strcpy(table[table_len].hash, hex);
            strcpy(table[table_len].file, e->path);
            table_len++;
        }
        next_file: ;
    }
    fossil_io_dir_iter_close(&it);
    return 0;
}

// Catalog: write metadata of all files
static int perform_catalog(ccstring dir) {
    char outpath[1024];
    snprintf(outpath, sizeof(outpath), "%s/.catalog.fson", dir);
    fossil_io_file_t out;
    if (fossil_io_file_open(&out, outpath, "w") != 0) return -1;

    fossil_io_dir_iter_t it;
    if (fossil_io_dir_iter_open(&it, dir) != 0) {
        fossil_io_file_close(&out);
        return -1;
    }

    char buf[2048];
    while (fossil_io_dir_iter_next(&it) > 0) {
        fossil_io_dir_entry_t *e = &it.current;
        int len = snprintf(buf, sizeof(buf), "file: %s\nsize: %llu\nmtime: %llu\n\n",
            e->path, (unsigned long long)e->size, (unsigned long long)e->modified);
        fossil_io_file_write(&out, buf, 1, len);
    }

    fossil_io_dir_iter_close(&it);
    fossil_io_file_close(&out);
    return 0;
}

// Index: filename + hash database
static int perform_index(ccstring dir) {
    char outpath[1024];
    snprintf(outpath, sizeof(outpath), "%s/.index.fson", dir);
    fossil_io_file_t out;
    if (fossil_io_file_open(&out, outpath, "w") != 0) return -1;

    fossil_io_dir_iter_t it;
    if (fossil_io_dir_iter_open(&it, dir) != 0) {
        fossil_io_file_close(&out);
        return -1;
    }

    char buf[2048];
    while (fossil_io_dir_iter_next(&it) > 0) {
        fossil_io_dir_entry_t *e = &it.current;
        if (e->type == 0) { // Only files
            char hex[65];
            if (file_hash(e->path, hex, sizeof(hex)) == 0) {
                int len = snprintf(buf, sizeof(buf), "file: %s\nhash: %s\n\n", e->path, hex);
                fossil_io_file_write(&out, buf, 1, len);
            }
        }
    }

    fossil_io_dir_iter_close(&it);
    fossil_io_file_close(&out);
    return 0;
}

// Snapshot: timestamped manifest
static int perform_snapshot(ccstring dir) {
    time_t now = time(NULL);

    char outpath[1024];
    snprintf(outpath, sizeof(outpath), "%s/.snapshot-%lld.fson", dir, (long long)now);
    fossil_io_file_t out;
    if (fossil_io_file_open(&out, outpath, "w") != 0) return -1;

    fossil_io_dir_iter_t it;
    if (fossil_io_dir_iter_open(&it, dir) != 0) {
        fossil_io_file_close(&out);
        return -1;
    }

    char buf[2048];
    while (fossil_io_dir_iter_next(&it) > 0) {
        fossil_io_dir_entry_t *e = &it.current;
        char hex[65] = cempty;
        if (e->type == 0 && file_hash(e->path, hex, sizeof(hex)) == 0) {
            // hash is in hex
        }
        int len = snprintf(buf, sizeof(buf), "file: %s\nsize: %llu\nmtime: %llu\nhash: %s\n\n",
            e->path, (unsigned long long)e->size, (unsigned long long)e->modified, hex);
        fossil_io_file_write(&out, buf, 1, len);
    }

    fossil_io_dir_iter_close(&it);
    fossil_io_file_close(&out);
    return 0;
}

// Prune: delete zero-byte files
static int perform_prune(ccstring dir) {
    fossil_io_dir_iter_t it;
    if (fossil_io_dir_iter_open(&it, dir) != 0) return -1;

    while (fossil_io_dir_iter_next(&it) > 0) {
        fossil_io_dir_entry_t *e = &it.current;
        if (e->type == 0 && e->size == 0) {
            fossil_io_file_delete(e->path);
        }
    }

    fossil_io_dir_iter_close(&it);
    return 0;
}

int fossil_shark_storage(ccstring path, bool dedupe, bool catalog,
                         bool index, bool snapshot, bool prune)
{
    if (cunlikely(path == cnull)) return -1;

    if (dedupe)  perform_dedupe(path);
    if (catalog) perform_catalog(path);
    if (index)   perform_index(path);
    if (snapshot) perform_snapshot(path);
    if (prune)   perform_prune(path);

    return 0;
}
