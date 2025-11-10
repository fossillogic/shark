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
#include "fossil/code/commands.h"
#include <errno.h>

/**
 * Perform archive operations (create, extract, list)
 */
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
    if ((extract || list) && !fossil_fstream_file_exists(path)) {
        fossil_io_printf("{red}Error: Archive file '%s' does not exist.{normal}\n", path);
        return 1;
    }

    // Check permissions
    if (extract || list) {
        if (!fossil_fstream_is_readable(path)) {
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
    
    if (fossil_io_validate_sanitize_string(path, sanitized_path, 1024, FOSSIL_CONTEXT_FILESYSTEM) & 
        (FOSSIL_SAN_SHELL | FOSSIL_SAN_PATH)) {
        fossil_io_printf("{red}Error: Suspicious path detected.{normal}\n");
        fossil_sys_memory_free(sanitized_path);
        fossil_sys_memory_free(sanitized_format);
        fossil_sys_memory_free(sanitized_password);
        return 1;
    }
    
    ccstring fmt = cunwrap_or(format, "tar");
    if (fossil_io_validate_sanitize_string(fmt, sanitized_format, 64, FOSSIL_CONTEXT_ALPHANUMERIC) & 
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
        if (fossil_io_validate_sanitize_string(password, sanitized_password, 256, FOSSIL_CONTEXT_SAFE) & 
            FOSSIL_SAN_SHELL) {
            fossil_io_printf("{red}Error: Invalid characters in password.{normal}\n");
            fossil_sys_memory_free(sanitized_path);
            fossil_sys_memory_free(sanitized_format);
            fossil_sys_memory_free(sanitized_password);
            return 1;
        }
    }

    cstring cmd = fossil_io_cstring_create_safe(cempty, 4096);
    if (cunlikely(!cmd)) {
        fossil_io_printf("{red}Error: Failed to allocate command buffer.{normal}\n");
        fossil_sys_memory_free(sanitized_path);
        fossil_sys_memory_free(sanitized_format);
        fossil_sys_memory_free(sanitized_password);
        return 1;
    }

    // Create a log file for the operation
    fossil_fstream_t log_stream;
    char *log_filename = (char*)fossil_sys_memory_alloc(600);
    if (cunlikely(!log_filename)) {
        fossil_io_printf("{red}Error: Failed to allocate log filename buffer.{normal}\n");
        fossil_io_cstring_free_safe(&cmd);
        fossil_sys_memory_free(sanitized_path);
        fossil_sys_memory_free(sanitized_format);
        fossil_sys_memory_free(sanitized_password);
        return 1;
    }
    
    fossil_sys_memory_zero(log_filename, 600);
    snprintf(log_filename, 600, "%s.archive.log", sanitized_path);
    
    COption log_option = cnone();
    if (fossil_fstream_open(&log_stream, log_filename, "w") == 0) {
        log_option = csome(&log_stream);
        char *log_msg = (char*)fossil_sys_memory_alloc(1000);
        if (cnotnull(log_msg)) {
            fossil_sys_memory_zero(log_msg, 1000);
            snprintf(log_msg, 1000, "Archive operation started: %s%c", 
                    create ? "create" : extract ? "extract" : "list", cnewline);
            fossil_fstream_write(&log_stream, log_msg, strlen(log_msg), 1);
            fossil_sys_memory_free(log_msg);
        }
    }

    if (create) {
        // Interactive confirmation for create operation
        fossil_io_printf("{cyan}Creating archive: %s (format: %s){normal}\n", sanitized_path, sanitized_format);
        fossil_io_printf("Are you sure you want to create this archive? (y/N): ");
        
        char *confirm = (char*)fossil_sys_memory_alloc(10);
        if (cunlikely(!confirm)) {
            fossil_io_printf("{red}Error: Failed to allocate confirmation buffer.{normal}\n");
            fossil_io_cstring_free_safe(&cmd);
            fossil_sys_memory_free(log_filename);
            fossil_sys_memory_free(sanitized_path);
            fossil_sys_memory_free(sanitized_format);
            fossil_sys_memory_free(sanitized_password);
            if (log_option.is_some) {
                fossil_fstream_close((fossil_fstream_t*)log_option.value);
            }
            return 1;
        }
        
        fossil_sys_memory_zero(confirm, 10);
        if (fossil_io_gets(confirm, 10) == 0) {
            fossil_io_trim(confirm);
            if (confirm[0] != 'y' && confirm[0] != 'Y') {
                fossil_io_printf("Operation cancelled.\n");
                fossil_io_cstring_free_safe(&cmd);
                fossil_sys_memory_free(confirm);
                fossil_sys_memory_free(log_filename);
                fossil_sys_memory_free(sanitized_path);
                fossil_sys_memory_free(sanitized_format);
                fossil_sys_memory_free(sanitized_password);
                if (log_option.is_some) {
                    fossil_fstream_close((fossil_fstream_t*)log_option.value);
                }
                return 0;
            }
        }
        fossil_sys_memory_free(confirm);
        
        if (fossil_io_cstring_equals(sanitized_format, "zip")) {
            if (cnotnull(password)) {
                cstring temp_cmd = fossil_io_cstring_format_safe(4096, "zip -P %s -r %s .", sanitized_password, sanitized_path);
                if (cnotnull(temp_cmd)) {
                    fossil_io_cstring_free_safe(&cmd);
                    cmd = temp_cmd;
                    fossil_io_printf("{blue}Using password protection{normal}\n");
                }
            } else {
                cstring temp_cmd = fossil_io_cstring_format_safe(4096, "zip -r %s .", sanitized_path);
                if (cnotnull(temp_cmd)) {
                    fossil_io_cstring_free_safe(&cmd);
                    cmd = temp_cmd;
                }
            }
        } else if (fossil_io_cstring_equals(sanitized_format, "tar") || fossil_io_cstring_equals(sanitized_format, "gz")) {
            cstring temp_cmd = fossil_io_cstring_format_safe(4096, "tar czf %s .", sanitized_path);
            if (cnotnull(temp_cmd)) {
                fossil_io_cstring_free_safe(&cmd);
                cmd = temp_cmd;
            }
        } else {
            fossil_io_printf("{red}Error: Unsupported format: %s{normal}\n", sanitized_format);
            fossil_io_cstring_free_safe(&cmd);
            fossil_sys_memory_free(log_filename);
            fossil_sys_memory_free(sanitized_path);
            fossil_sys_memory_free(sanitized_format);
            fossil_sys_memory_free(sanitized_password);
            if (log_option.is_some) {
                fossil_fstream_close((fossil_fstream_t*)log_option.value);
            }
            return 1;
        }
    } else if (extract) {
        fossil_io_printf("{cyan}Extracting archive: %s (format: %s){normal}\n", sanitized_path, sanitized_format);
        
        // Show progress during extraction
        fossil_io_show_progress(0);
        
        if (fossil_io_cstring_equals(sanitized_format, "zip")) {
            if (cnotnull(password)) {
                cstring temp_cmd = fossil_io_cstring_format_safe(4096, "unzip -P %s %s", sanitized_password, sanitized_path);
                if (cnotnull(temp_cmd)) {
                    fossil_io_cstring_free_safe(&cmd);
                    cmd = temp_cmd;
                    fossil_io_printf("{blue}Using password for extraction{normal}\n");
                }
            } else {
                cstring temp_cmd = fossil_io_cstring_format_safe(4096, "unzip %s", sanitized_path);
                if (cnotnull(temp_cmd)) {
                    fossil_io_cstring_free_safe(&cmd);
                    cmd = temp_cmd;
                }
            }
        } else if (fossil_io_cstring_equals(sanitized_format, "tar") || fossil_io_cstring_equals(sanitized_format, "gz")) {
            cstring temp_cmd = fossil_io_cstring_format_safe(4096, "tar xzf %s", sanitized_path);
            if (cnotnull(temp_cmd)) {
                fossil_io_cstring_free_safe(&cmd);
                cmd = temp_cmd;
            }
        } else {
            fossil_io_printf("{red}Error: Unsupported format: %s{normal}\n", sanitized_format);
            fossil_io_cstring_free_safe(&cmd);
            fossil_sys_memory_free(log_filename);
            fossil_sys_memory_free(sanitized_path);
            fossil_sys_memory_free(sanitized_format);
            fossil_sys_memory_free(sanitized_password);
            if (log_option.is_some) {
                fossil_fstream_close((fossil_fstream_t*)log_option.value);
            }
            return 1;
        }
        
        fossil_io_show_progress(50);
        
    } else if (list) {
        fossil_io_printf("{cyan}Listing contents of archive: %s (format: %s){normal}\n", sanitized_path, sanitized_format);
        if (fossil_io_cstring_equals(sanitized_format, "zip")) {
            cstring temp_cmd = fossil_io_cstring_format_safe(4096, "unzip -l %s", sanitized_path);
            if (cnotnull(temp_cmd)) {
                fossil_io_cstring_free_safe(&cmd);
                cmd = temp_cmd;
            }
        } else if (fossil_io_cstring_equals(sanitized_format, "tar") || fossil_io_cstring_equals(sanitized_format, "gz")) {
            cstring temp_cmd = fossil_io_cstring_format_safe(4096, "tar tzf %s", sanitized_path);
            if (cnotnull(temp_cmd)) {
                fossil_io_cstring_free_safe(&cmd);
                cmd = temp_cmd;
            }
        } else {
            fossil_io_printf("{red}Error: Unsupported format: %s{normal}\n", sanitized_format);
            fossil_io_cstring_free_safe(&cmd);
            fossil_sys_memory_free(log_filename);
            fossil_sys_memory_free(sanitized_path);
            fossil_sys_memory_free(sanitized_format);
            fossil_sys_memory_free(sanitized_password);
            if (log_option.is_some) {
                fossil_fstream_close((fossil_fstream_t*)log_option.value);
            }
            return 1;
        }
    }

    int ret = system(cmd);
    fossil_io_cstring_free_safe(&cmd);
    
    if (extract) {
        fossil_io_show_progress(100);
        fossil_io_printf("%c", cnewline);
    }
    
    // Log the result
    if (log_option.is_some) {
        char *result_msg = (char*)fossil_sys_memory_alloc(200);
        if (cnotnull(result_msg)) {
            fossil_sys_memory_zero(result_msg, 200);
            snprintf(result_msg, 200, "Operation completed with return code: %d%c", ret, cnewline);
            fossil_fstream_write((fossil_fstream_t*)log_option.value, result_msg, strlen(result_msg), 1);
            fossil_sys_memory_free(result_msg);
        }
        fossil_fstream_close((fossil_fstream_t*)log_option.value);
    }
    
    if (cunlikely(ret != 0)) {
        fossil_io_printf("{red}Error: Archive command failed{normal}\n");
        fossil_sys_memory_free(log_filename);
        fossil_sys_memory_free(sanitized_path);
        fossil_sys_memory_free(sanitized_format);
        fossil_sys_memory_free(sanitized_password);
        return ret;
    }

    // Verify the result for create operations
    if (clikely(create) && fossil_fstream_file_exists(sanitized_path)) {
        fossil_fstream_t created_file;
        if (fossil_fstream_open(&created_file, sanitized_path, "r") == 0) {
            int32_t size = fossil_fstream_get_size(&created_file);
            fossil_io_printf("{blue}Archive created successfully (size: %d bytes){normal}\n", size);
            fossil_fstream_close(&created_file);
        }
    } else {
        fossil_io_printf("{blue}Archive operation completed successfully{normal}\n");
    }

    // Clean up allocated memory
    fossil_sys_memory_free(log_filename);
    fossil_sys_memory_free(sanitized_path);
    fossil_sys_memory_free(sanitized_format);
    fossil_sys_memory_free(sanitized_password);
    
    return 0;
}
