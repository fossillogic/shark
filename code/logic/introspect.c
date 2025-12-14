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


static ccstring get_mime_type(ccstring path) {
    ccstring ext = strrchr(path, '.');
    if (!cnotnull(ext)) {
        // Check for extensionless Meson build files
        if (fossil_io_cstring_iequals(path, "meson.build") || fossil_io_cstring_iequals(path, "meson_options.txt") || fossil_io_cstring_iequals(path, "meson.options"))
            return "text/x-meson";
        return "application/octet-stream";
    }
    ext++;
    // Meson build files
    if (fossil_io_cstring_iequals(ext, "build") && fossil_io_cstring_icontains(path, "meson.build")) return "text/x-meson";
    if (fossil_io_cstring_iequals(ext, "txt") && fossil_io_cstring_icontains(path, "meson_options.txt")) return "text/x-meson";
    if (fossil_io_cstring_iequals(ext, "options") && fossil_io_cstring_icontains(path, "meson.options")) return "text/x-meson";
    // Build system files
    if (fossil_io_cstring_iequals(ext, "mk") || fossil_io_cstring_iequals(ext, "makefile")) return "text/x-makefile";
    if (fossil_io_cstring_iequals(ext, "cmake")) return "text/x-cmake";
    if (fossil_io_cstring_iequals(ext, "ninja")) return "text/x-ninja";
    if (fossil_io_cstring_iequals(ext, "gradle")) return "text/x-gradle";
    if (fossil_io_cstring_iequals(ext, "pom") || fossil_io_cstring_iequals(ext, "pom.xml")) return "application/xml";
    if (fossil_io_cstring_iequals(ext, "bazel") || fossil_io_cstring_iequals(ext, "bzl")) return "text/x-bazel";
    if (fossil_io_cstring_iequals(ext, "build")) return "text/plain";
    if (fossil_io_cstring_iequals(ext, "workspace")) return "text/plain";
    if (fossil_io_cstring_iequals(ext, "sln")) return "text/x-msbuild";
    if (fossil_io_cstring_iequals(ext, "vcxproj")) return "application/xml";
    if (fossil_io_cstring_iequals(ext, "csproj")) return "application/xml";
    if (fossil_io_cstring_iequals(ext, "xcodeproj")) return "text/plain";
    if (fossil_io_cstring_iequals(ext, "pro")) return "text/x-qmake";
    if (fossil_io_cstring_iequals(ext, "gyp")) return "application/json";
    if (fossil_io_cstring_iequals(ext, "gypi")) return "application/json";
    if (fossil_io_cstring_iequals(ext, "am")) return "text/x-makefile";
    if (fossil_io_cstring_iequals(ext, "in")) return "text/plain";
    // Existing types
    if (fossil_io_cstring_iequals(ext, "txt")) return "text/plain";
    if (fossil_io_cstring_iequals(ext, "c") || fossil_io_cstring_iequals(ext, "h")) return "text/plain";
    if (fossil_io_cstring_iequals(ext, "cpp") || fossil_io_cstring_iequals(ext, "hpp")) return "text/x-c++src";
    if (fossil_io_cstring_iequals(ext, "py")) return "text/x-python";
    if (fossil_io_cstring_iequals(ext, "js")) return "application/javascript";
    if (fossil_io_cstring_iequals(ext, "ts")) return "application/typescript";
    if (fossil_io_cstring_iequals(ext, "html") || fossil_io_cstring_iequals(ext, "htm")) return "text/html";
    if (fossil_io_cstring_iequals(ext, "css")) return "text/css";
    if (fossil_io_cstring_iequals(ext, "json")) return "application/json";
    if (fossil_io_cstring_iequals(ext, "xml")) return "application/xml";
    if (fossil_io_cstring_iequals(ext, "csv")) return "text/csv";
    if (fossil_io_cstring_iequals(ext, "md")) return "text/markdown";
    if (fossil_io_cstring_iequals(ext, "yml") || fossil_io_cstring_iequals(ext, "yaml")) return "application/x-yaml";
    if (fossil_io_cstring_iequals(ext, "ini")) return "text/plain";
    if (fossil_io_cstring_iequals(ext, "conf")) return "text/plain";
    if (fossil_io_cstring_iequals(ext, "log")) return "text/plain";
    if (fossil_io_cstring_iequals(ext, "sh")) return "application/x-sh";
    if (fossil_io_cstring_iequals(ext, "bat")) return "application/x-msdos-program";
    if (fossil_io_cstring_iequals(ext, "exe")) return "application/vnd.microsoft.portable-executable";
    if (fossil_io_cstring_iequals(ext, "dll")) return "application/vnd.microsoft.portable-executable";
    if (fossil_io_cstring_iequals(ext, "so")) return "application/x-sharedlib";
    if (fossil_io_cstring_iequals(ext, "bin")) return "application/octet-stream";
    if (fossil_io_cstring_iequals(ext, "jar")) return "application/java-archive";
    if (fossil_io_cstring_iequals(ext, "war")) return "application/java-archive";
    if (fossil_io_cstring_iequals(ext, "class")) return "application/java-vm";
    if (fossil_io_cstring_iequals(ext, "go")) return "text/x-go";
    if (fossil_io_cstring_iequals(ext, "rs")) return "text/x-rustsrc";
    if (fossil_io_cstring_iequals(ext, "swift")) return "text/x-swift";
    if (fossil_io_cstring_iequals(ext, "php")) return "application/x-httpd-php";
    if (fossil_io_cstring_iequals(ext, "rb")) return "text/x-ruby";
    if (fossil_io_cstring_iequals(ext, "pl")) return "text/x-perl";
    if (fossil_io_cstring_iequals(ext, "lua")) return "text/x-lua";
    if (fossil_io_cstring_iequals(ext, "sql")) return "application/sql";
    if (fossil_io_cstring_iequals(ext, "jpg") || fossil_io_cstring_iequals(ext, "jpeg")) return "image/jpeg";
    if (fossil_io_cstring_iequals(ext, "png")) return "image/png";
    if (fossil_io_cstring_iequals(ext, "gif")) return "image/gif";
    if (fossil_io_cstring_iequals(ext, "bmp")) return "image/bmp";
    if (fossil_io_cstring_iequals(ext, "svg")) return "image/svg+xml";
    if (fossil_io_cstring_iequals(ext, "ico")) return "image/x-icon";
    if (fossil_io_cstring_iequals(ext, "webp")) return "image/webp";
    if (fossil_io_cstring_iequals(ext, "tif") || fossil_io_cstring_iequals(ext, "tiff")) return "image/tiff";
    if (fossil_io_cstring_iequals(ext, "zip")) return "application/zip";
    if (fossil_io_cstring_iequals(ext, "tar")) return "application/x-tar";
    if (fossil_io_cstring_iequals(ext, "gz")) return "application/gzip";
    if (fossil_io_cstring_iequals(ext, "bz2")) return "application/x-bzip2";
    if (fossil_io_cstring_iequals(ext, "xz")) return "application/x-xz";
    if (fossil_io_cstring_iequals(ext, "7z")) return "application/x-7z-compressed";
    if (fossil_io_cstring_iequals(ext, "rar")) return "application/vnd.rar";
    if (fossil_io_cstring_iequals(ext, "pdf")) return "application/pdf";
    if (fossil_io_cstring_iequals(ext, "epub")) return "application/epub+zip";
    if (fossil_io_cstring_iequals(ext, "mp3")) return "audio/mpeg";
    if (fossil_io_cstring_iequals(ext, "wav")) return "audio/wav";
    if (fossil_io_cstring_iequals(ext, "ogg")) return "audio/ogg";
    if (fossil_io_cstring_iequals(ext, "flac")) return "audio/flac";
    if (fossil_io_cstring_iequals(ext, "aac")) return "audio/aac";
    if (fossil_io_cstring_iequals(ext, "m4a")) return "audio/mp4";
    if (fossil_io_cstring_iequals(ext, "mp4")) return "video/mp4";
    if (fossil_io_cstring_iequals(ext, "mov")) return "video/quicktime";
    if (fossil_io_cstring_iequals(ext, "avi")) return "video/x-msvideo";
    if (fossil_io_cstring_iequals(ext, "wmv")) return "video/x-ms-wmv";
    if (fossil_io_cstring_iequals(ext, "mkv")) return "video/x-matroska";
    if (fossil_io_cstring_iequals(ext, "webm")) return "video/webm";
    if (fossil_io_cstring_iequals(ext, "flv")) return "video/x-flv";
    if (fossil_io_cstring_iequals(ext, "apk")) return "application/vnd.android.package-archive";
    if (fossil_io_cstring_iequals(ext, "deb")) return "application/vnd.debian.binary-package";
    if (fossil_io_cstring_iequals(ext, "rpm")) return "application/x-rpm";
    if (fossil_io_cstring_iequals(ext, "iso")) return "application/x-iso9660-image";
    if (fossil_io_cstring_iequals(ext, "dmg")) return "application/x-apple-diskimage";
    if (fossil_io_cstring_iequals(ext, "psd")) return "image/vnd.adobe.photoshop";
    if (fossil_io_cstring_iequals(ext, "ai")) return "application/postscript";
    if (fossil_io_cstring_iequals(ext, "ps")) return "application/postscript";
    if (fossil_io_cstring_iequals(ext, "ttf")) return "font/ttf";
    if (fossil_io_cstring_iequals(ext, "otf")) return "font/otf";
    if (fossil_io_cstring_iequals(ext, "woff")) return "font/woff";
    if (fossil_io_cstring_iequals(ext, "woff2")) return "font/woff2";
    if (fossil_io_cstring_iequals(ext, "eot")) return "application/vnd.ms-fontobject";
    if (fossil_io_cstring_iequals(ext, "apk")) return "application/vnd.android.package-archive";
    if (fossil_io_cstring_iequals(ext, "ics")) return "text/calendar";
    if (fossil_io_cstring_iequals(ext, "vcf")) return "text/vcard";
    return "application/octet-stream";
}

int fossil_shark_introspect(ccstring path, int show_head_lines,
                             int show_tail_lines, bool count_lines_words_bytes,
                             bool show_file_type, bool output_fson)
{
    if (!cnotnull(path)) return -1;

    struct stat st;
    if (stat(path, &st) != 0) return errno;

    fossil_io_file_t file_stream;
    if (fossil_io_file_open(&file_stream, path, "r") != 0 &&
        (show_head_lines > 0 || show_tail_lines > 0 || count_lines_words_bytes)) {
        return errno;
    }

    unsigned long lines = 0, words = 0, bytes = 0;
    char buffer[4096];
    cstring *tail_buffer = cnull;
    int tail_index = 0;

    if (show_tail_lines > 0) {
        tail_buffer = (cstring *)fossil_sys_memory_alloc(sizeof(cstring) * show_tail_lines);
        if (!cnotnull(tail_buffer)) {
            if (fossil_io_file_is_open(&file_stream)) fossil_io_file_close(&file_stream);
            return -1;
        }
        for (int i = 0; i < show_tail_lines; i++)
            tail_buffer[i] = cnull;
    }

    int head_count = 0;
    while (fossil_io_file_is_open(&file_stream) &&
           fgets(buffer, sizeof(buffer), file_stream.file)) {
        size_t len = strlen(buffer);
        bytes += len;
        if (count_lines_words_bytes || show_head_lines > 0 || show_tail_lines > 0) lines++;

        if (count_lines_words_bytes) {
            // Count words roughly
            char *p = buffer;
            bool in_word = false;
            while (*p) {
                if (isspace((unsigned char)*p)) {
                    if (in_word) words++;
                    in_word = false;
                } else {
                    in_word = true;
                }
                p++;
            }
            if (in_word) words++;
        }

        if (show_head_lines > 0 && head_count < show_head_lines) {
            fossil_io_printf("{green,bold}%s{normal}", buffer);
            head_count++;
        }

        if (show_tail_lines > 0) {
            // store last N lines in circular buffer
            if (cnotnull(tail_buffer[tail_index]))
                fossil_io_cstring_free(tail_buffer[tail_index]);
            tail_buffer[tail_index] = fossil_io_cstring_dup(buffer);
            tail_index = (tail_index + 1) % show_tail_lines;
        }
    }

    if (show_tail_lines > 0) {
        fossil_io_printf("\n{yellow,bold}-- Tail last %d lines --{normal}\n", show_tail_lines);
        int start = tail_index;
        for (int i = 0; i < show_tail_lines; i++) {
            int idx = (start + i) % show_tail_lines;
            if (cnotnull(tail_buffer[idx])) fossil_io_printf("{yellow}%s{normal}", tail_buffer[idx]);
        }
    }

    if (fossil_io_file_is_open(&file_stream)) fossil_io_file_close(&file_stream);

    // Output metadata
    if (output_fson) {
        puts("{");
        fossil_io_printf("  {blue,bold}path: {cyan}cstr:{reset} \"%s\",\n", path);
        fossil_io_printf("  {blue,bold}size: {cyan}i64:{reset} %lld,\n", (long long)st.st_size);
        fossil_io_printf("  {blue,bold}lines: {cyan}u64:{reset} %lu,\n", lines);
        fossil_io_printf("  {blue,bold}words: {cyan}u64:{reset} %lu,\n", words);
        fossil_io_printf("  {blue,bold}bytes: {cyan}u64:{reset} %lu", bytes);
        if (show_file_type) {
            fossil_io_printf(",\n  {blue,bold}type: {cyan}cstr:{reset} \"%s\"", get_mime_type(path));
        }
        fossil_io_printf("\n}\n");
    } else {
        fossil_io_printf("{cyan,bold}File: %s{normal}\n", path);
        fossil_io_printf("{magenta}Size: %lld bytes{normal}\n", (long long)st.st_size);
        if (count_lines_words_bytes) {
            fossil_io_printf("{blue}Lines: %lu Words: %lu Bytes: %lu{normal}\n", lines, words, bytes);
        }
        if (show_file_type) {
            fossil_io_printf("{green}Type: %s{normal}\n", get_mime_type(path));
        }
    }

    if (cnotnull(tail_buffer)) {
        for (int i = 0; i < show_tail_lines; i++) {
            if (cnotnull(tail_buffer[i]))
                fossil_io_cstring_free(tail_buffer[i]);
        }
        fossil_sys_memory_free(tail_buffer);
        cdrop(tail_buffer);
    }

    return 0;
}
