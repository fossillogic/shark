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


// Helper function to safely create a path by combining directory and filename
static int fossil_fpath_create_path_safe(char *dest, size_t dest_size, ccstring base_path, ccstring suffix) {
    if (!dest || !base_path || !suffix || dest_size == 0) {
        return -1;
    }
    
    // Clear destination buffer
    fossil_sys_memory_zero(dest, dest_size);
    
    // Find the last directory separator to get the directory part
    ccstring last_sep = cnull;
    for (ccstring p = base_path; *p; p++) {
        if (*p == '/' || *p == '\\') {
            last_sep = p;
        }
    }
    
    // Calculate required size
    size_t dir_len = last_sep ? (size_t)(last_sep - base_path + 1) : 0;
    size_t base_name_len = fossil_io_cstring_length(base_path) - dir_len;
    size_t suffix_len = fossil_io_cstring_length(suffix);
    size_t required_size = dir_len + base_name_len + suffix_len + 1;
    
    if (required_size > dest_size) {
        return -1; // Buffer too small
    }
    
    // Copy directory part if it exists
    if (dir_len > 0) {
        fossil_sys_memory_copy(dest, (fossil_sys_memory_t)base_path, dir_len);
    }
    
    // Copy base filename (without extension if present)
    ccstring base_name = base_path + dir_len;
    ccstring dot = cnull;
    for (ccstring p = base_name; *p; p++) {
        if (*p == '.') {
            dot = p;
        }
    }
    
    size_t name_copy_len = dot ? (size_t)(dot - base_name) : base_name_len;
    fossil_sys_memory_copy(dest + dir_len, (fossil_sys_memory_t)base_name, name_copy_len);
    
    // Append suffix
    fossil_sys_memory_copy(dest + dir_len + name_copy_len, (fossil_sys_memory_t)suffix, suffix_len);
    
    return 0;
}


// Helper function to convert string format to enum type
static fossil_io_archive_type_t get_archive_type_from_format(ccstring format) {
    if (!format) return FOSSIL_IO_ARCHIVE_UNKNOWN;
    
    if (fossil_io_cstring_equals(format, "zip")) return FOSSIL_IO_ARCHIVE_ZIP;
    if (fossil_io_cstring_equals(format, "tar")) return FOSSIL_IO_ARCHIVE_TAR;
    if (fossil_io_cstring_equals(format, "gz") || fossil_io_cstring_equals(format, "tar.gz")) return FOSSIL_IO_ARCHIVE_TARGZ;
    if (fossil_io_cstring_equals(format, "bz2") || fossil_io_cstring_equals(format, "tar.bz2")) return FOSSIL_IO_ARCHIVE_TARBZ2;
    if (fossil_io_cstring_equals(format, "xz") || fossil_io_cstring_equals(format, "tar.xz")) return FOSSIL_IO_ARCHIVE_TARXZ;
    if (fossil_io_cstring_equals(format, "7z")) return FOSSIL_IO_ARCHIVE_7Z;
    if (fossil_io_cstring_equals(format, "rar")) return FOSSIL_IO_ARCHIVE_RAR;
    
    return FOSSIL_IO_ARCHIVE_UNKNOWN;
}

int fossil_shark_archive(ccstring path, bool create, bool extract,
                         bool list, ccstring format, ccstring password) {
    if (!path) {
        fossil_io_printf("{red}Error: Archive path must be specified.{normal}\n");
        return 1;
    }

    if ((!create && !extract && !list) || (create + extract + list > 1)) {
        fossil_io_printf("{red}Error: Specify exactly one operation: create, extract, or list.{normal}\n");
        return 1;
    }

    // Check if file exists for extract and list operations
    if ((extract || list) && !fossil_io_file_file_exists(path)) {
        fossil_io_printf("{red}Error: Archive file '%s' does not exist.{normal}\n", path);
        return 1;
    }

    // Check permissions
    if (extract || list) {
        if (!fossil_io_file_is_readable(path)) {
            fossil_io_printf("{red}Error: Archive file '%s' is not readable.{normal}\n", path);
            return 1;
        }
    }

    // Validate and sanitize inputs using safe allocation
    char *sanitized_path = (char*)fossil_sys_memory_alloc(1024);
    char *sanitized_format = (char*)fossil_sys_memory_alloc(64);
    char *sanitized_password = (char*)fossil_sys_memory_alloc(256);
    
    if (cunlikely(!sanitized_path || !sanitized_format || !sanitized_password)) {
        fossil_io_printf("{red}Error: Memory allocation failed.{normal}\n");
        if (cnotnull(sanitized_path)) fossil_sys_memory_free(sanitized_path);
        if (cnotnull(sanitized_format)) fossil_sys_memory_free(sanitized_format);
        if (cnotnull(sanitized_password)) fossil_sys_memory_free(sanitized_password);
        return 1;
    }
    
    fossil_sys_memory_zero(sanitized_path, 1024);
    fossil_sys_memory_zero(sanitized_format, 64);
    fossil_sys_memory_zero(sanitized_password, 256);
    
    if (fossil_io_validate_sanitize_string(path, sanitized_path, 1024, FOSSIL_CTX_FILENAME) & 
        (FOSSIL_SAN_SHELL | FOSSIL_SAN_PATH)) {
        fossil_io_printf("{red}Error: Suspicious path detected.{normal}\n");
        fossil_sys_memory_free(sanitized_path);
        fossil_sys_memory_free(sanitized_format);
        fossil_sys_memory_free(sanitized_password);
        return 1;
    }
    
    ccstring fmt = cunwrap_or(format, "tar");
    if (fossil_io_validate_sanitize_string(fmt, sanitized_format, 64, FOSSIL_CTX_GENERIC) & 
        FOSSIL_SAN_SHELL) {
        fossil_io_printf("{red}Error: Invalid format specification.{normal}\n");
        fossil_sys_memory_free(sanitized_path);
        fossil_sys_memory_free(sanitized_format);
        fossil_sys_memory_free(sanitized_password);
        return 1;
    }
    
    if (cnotnull(password) && fossil_io_validate_is_weak_password(password, cnull, cnull)) {
        fossil_io_printf("{yellow}Warning: Password appears to be weak.{normal}\n");
    }
    
    if (cnotnull(password)) {
        if (fossil_io_validate_sanitize_string(password, sanitized_password, 256, FOSSIL_CTX_GENERIC) & 
            FOSSIL_SAN_SHELL) {
            fossil_io_printf("{red}Error: Invalid characters in password.{normal}\n");
            fossil_sys_memory_free(sanitized_path);
            fossil_sys_memory_free(sanitized_format);
            fossil_sys_memory_free(sanitized_password);
            return 1;
        }
    }

    // Determine archive type
    fossil_io_archive_type_t archive_type = get_archive_type_from_format(sanitized_format);
    if (archive_type == FOSSIL_IO_ARCHIVE_UNKNOWN && !list && !extract) {
        fossil_io_printf("{red}Error: Unsupported format: %s{normal}\n", sanitized_format);
        fossil_sys_memory_free(sanitized_path);
        fossil_sys_memory_free(sanitized_format);
        fossil_sys_memory_free(sanitized_password);
        return 1;
    }

    // For extract and list operations, auto-detect archive type if unknown
    if ((extract || list) && archive_type == FOSSIL_IO_ARCHIVE_UNKNOWN) {
        archive_type = fossil_io_archive_get_type(sanitized_path);
        if (archive_type == FOSSIL_IO_ARCHIVE_UNKNOWN) {
            fossil_io_printf("{red}Error: Cannot determine archive type for: %s{normal}\n", sanitized_path);
            fossil_sys_memory_free(sanitized_path);
            fossil_sys_memory_free(sanitized_format);
            fossil_sys_memory_free(sanitized_password);
            return 1;
        }
    }

    char *log_filename = (char*)fossil_sys_memory_alloc(1024);
    if (cunlikely(!log_filename)) {
        fossil_io_printf("{red}Error: Failed to allocate log filename buffer.{normal}\n");
        fossil_sys_memory_free(sanitized_path);
        fossil_sys_memory_free(sanitized_format);
        fossil_sys_memory_free(sanitized_password);
        return 1;
    }
    
    fossil_sys_memory_zero(log_filename, 1024);
    fossil_fpath_create_path_safe(log_filename, 1024, sanitized_path, ".archive.log");
    
    fossil_io_file_t log_stream;
    COption log_option = cnone();
    if (fossil_io_file_open(&log_stream, log_filename, "w") == 0) {
        log_option = csome(&log_stream);
        char *log_msg = (char*)fossil_sys_memory_alloc(1000);
        if (cnotnull(log_msg)) {
            fossil_sys_memory_zero(log_msg, 1000);
            fossil_sys_memory_copy(log_msg, "Archive operation started: ", 27);
            if (create) {
                fossil_sys_memory_copy(log_msg + 27, "create", 6);
            } else if (extract) {
                fossil_sys_memory_copy(log_msg + 27, "extract", 7);
            } else {
                fossil_sys_memory_copy(log_msg + 27, "list", 4);
            }
            size_t len = fossil_io_cstring_length(log_msg);
            log_msg[len] = '\n';
            log_msg[len + 1] = '\0';
            fossil_io_file_write(&log_stream, log_msg, fossil_io_cstring_length(log_msg), 1);
            fossil_sys_memory_free(log_msg);
        }
    }

    fossil_io_archive_t *archive = NULL;
    int ret = 0;

    if (create) {
        // Interactive confirmation for create operation
        fossil_io_printf("{cyan}Creating archive: %s (format: %s){normal}\n", sanitized_path, sanitized_format);
        fossil_io_printf("Are you sure you want to create this archive? (y/N): ");
        fossil_io_flush();
        
        char *confirm = (char*)fossil_sys_memory_alloc(10);
        if (cunlikely(!confirm)) {
            fossil_io_printf("{red}Error: Failed to allocate confirmation buffer.{normal}\n");
            fossil_sys_memory_free(log_filename);
            fossil_sys_memory_free(sanitized_path);
            fossil_sys_memory_free(sanitized_format);
            fossil_sys_memory_free(sanitized_password);
            if (log_option.is_some) {
                fossil_io_file_close((fossil_io_file_t*)log_option.value);
            }
            return 1;
        }
        
        fossil_sys_memory_zero(confirm, 10);
        if (fossil_io_gets(confirm, 10) == 0) {
            fossil_io_trim(confirm);
            if (confirm[0] != 'y' && confirm[0] != 'Y') {
                fossil_io_printf("Operation cancelled.\n");
                fossil_sys_memory_free(confirm);
                fossil_sys_memory_free(log_filename);
                fossil_sys_memory_free(sanitized_path);
                fossil_sys_memory_free(sanitized_format);
                fossil_sys_memory_free(sanitized_password);
                if (log_option.is_some) {
                    fossil_io_file_close((fossil_io_file_t*)log_option.value);
                }
                return 0;
            }
        }
        fossil_sys_memory_free(confirm);
        
        // Create archive using new API
        archive = fossil_io_archive_create(sanitized_path, archive_type, FOSSIL_IO_COMPRESSION_NORMAL);
        if (!archive) {
            fossil_io_printf("{red}Error: Failed to create archive{normal}\n");
            ret = 1;
        } else {
            // Add current directory contents (cross-platform)
            if (!fossil_io_archive_add_directory(archive, ".", "")) {
                fossil_io_printf("{red}Error: Failed to add files to archive{normal}\n");
                ret = 1;
            } else {
                fossil_io_printf("{blue}Archive created successfully{normal}\n");
            }
        }
        
    } else if (extract) {
        fossil_io_printf("{cyan}Extracting archive: %s{normal}\n", sanitized_path);
        
        // Show progress during extraction
        fossil_io_show_progress(0);
        
        // Open archive for reading
        archive = fossil_io_archive_open(sanitized_path, archive_type, FOSSIL_IO_ARCHIVE_READ, FOSSIL_IO_COMPRESSION_NONE);
        if (!archive) {
            fossil_io_printf("{red}Error: Failed to open archive for extraction{normal}\n");
            ret = 1;
        } else {
            fossil_io_show_progress(50);

            if (!fossil_io_archive_extract_all(archive, ".")) {
                fossil_io_printf("{red}Error: Failed to extract archive{normal}\n");
                ret = 1;
            } else {
                fossil_io_printf("{blue}Archive extracted successfully{normal}\n");
            }
        }
        
        fossil_io_show_progress(100);
        fossil_io_printf("\n");
        
    } else if (list) {
        fossil_io_printf("{cyan}Listing contents of archive: %s{normal}\n", sanitized_path);
        
        // Open archive for reading
        archive = fossil_io_archive_open(sanitized_path, archive_type, FOSSIL_IO_ARCHIVE_READ, FOSSIL_IO_COMPRESSION_NONE);
        if (!archive) {
            fossil_io_printf("{red}Error: Failed to open archive for listing{normal}\n");
            ret = 1;
        } else {
            // Print archive contents
            fossil_io_archive_print(archive);
            
            // Get and display archive statistics
            fossil_io_archive_stats_t stats;
            if (fossil_io_archive_get_stats(archive, &stats)) {
                fossil_io_printf("\n{blue}Archive Statistics:{normal}\n");
                fossil_io_printf("Total entries: %zu\n", stats.total_entries);
                fossil_io_printf("Total size: %zu bytes\n", stats.total_size);
                fossil_io_printf("Compressed size: %zu bytes\n", stats.compressed_size);
                fossil_io_printf("Compression ratio: %.2f%%\n", stats.compression_ratio * 100);
            }
        }
    }

    // Close archive if opened
    if (archive) {
        fossil_io_archive_close(archive);
    }
    
    // Log the result
    if (log_option.is_some) {
        char *result_msg = (char*)fossil_sys_memory_alloc(200);
        if (cnotnull(result_msg)) {
            fossil_sys_memory_zero(result_msg, 200);
            size_t written = 0;
            fossil_sys_memory_copy(result_msg, "Operation completed with return code: ", 38);
            written += 38;
            if (ret == 0) {
                fossil_sys_memory_copy(result_msg + written, "0", 1);
                written += 1;
            } else {
                fossil_sys_memory_copy(result_msg + written, "1", 1);
                written += 1;
            }
            result_msg[written] = '\n';
            result_msg[written + 1] = '\0';
            fossil_io_file_write((fossil_io_file_t*)log_option.value, result_msg, fossil_io_cstring_length(result_msg), 1);
            fossil_sys_memory_free(result_msg);
        }
        fossil_io_file_close((fossil_io_file_t*)log_option.value);
    }

    // Clean up allocated memory
    fossil_sys_memory_free(log_filename);
    fossil_sys_memory_free(sanitized_path);
    fossil_sys_memory_free(sanitized_format);
    fossil_sys_memory_free(sanitized_password);
    
    return ret;
}
