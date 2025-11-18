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

static char *read_file(const char *path) {
    fossil_fstream_t stream;
    if (fossil_fstream_open(&stream, path, "rb") != 0)
        return cnull;

    if (fossil_fstream_seek(&stream, 0, SEEK_END) != 0) {
        fossil_fstream_close(&stream);
        return cnull;
    }
    int32_t size = fossil_fstream_tell(&stream);
    fossil_fstream_rewind(&stream);

    char *buf = malloc(size + 1);
    if (!buf) {
        fossil_fstream_close(&stream);
        return cnull;
    }

    size_t read = fossil_fstream_read(&stream, buf, 1, size);
    if (read != (size_t)size) {
        free(buf);
        fossil_fstream_close(&stream);
        return cnull;
    }
    buf[size] = cterm;
    fossil_fstream_close(&stream);
    return buf;
}

/**
 * Perform grammar analysis, correction, sanitization, tone detection, and content-risk scanning.
 * Uses SOAP API for advanced grammar and content analysis.
 *
 * @return 0 on success, non-zero on error
 */
int fossil_shark_grammar(ccstring file_path, bool check, bool fix,
                         bool sanitize, bool suggest, bool tone,
                         ccstring detect_type)
{
    if (!file_path) {
        fossil_io_printf("{red,bold}[error]{normal} grammar: invalid NULL path\n");
        return 1;
    }

    cstring input = read_file(file_path);
    if (!input) {
        input = fossil_io_cstring_dup(file_path);
        if (!input) {
            fossil_io_printf("{red,bold}[error]{normal} grammar: memory allocation failed\n");
            return 1;
        }
    } else {
        size_t len = strlen(input);
        while (len > 0 && input[len - 1] == '\0') {
            len--;
        }
        input[len] = cterm;
    }

    cstring work = fossil_io_cstring_dup(input);
    if (!work) {
        fossil_io_cstring_free(input);
        return 1;
    }

    fossil_io_printf("{cyan,bold}=== Grammar Analysis ==={normal}\n");

    if (sanitize) {
        cstring clean = fossil_io_soap_sanitize(work);
        if (clean) {
            fossil_io_printf("{green}[sanitize]{normal} Applied meme-language sanitization.\n");
            fossil_io_cstring_free(work);
            work = clean;
        }
    }

    if (check) {
        int status = fossil_io_soap_check_grammar(work);
        if (status == 0)
            fossil_io_printf("{green}[check]{normal} Grammar OK.\n");
        else
            fossil_io_printf("{yellow}[check]{normal} Grammar issues detected: %d\n", status);
    }

    if (fix) {
        cstring fixed = fossil_io_soap_correct_grammar(work);
        if (fixed) {
            fossil_io_printf("{green}[fix]{normal} Auto-corrected grammar.\n");
            fossil_io_cstring_free(work);
            work = fixed;
        }
    }

    if (suggest) {
        cstring sug = fossil_io_soap_suggest(work);
        if (sug) {
            fossil_io_printf("{magenta,bold}=== Suggestions ==={normal}\n%s\n", sug);
            fossil_io_cstring_free(sug);
        }
    }

    if (tone) {
        ccstring t = fossil_io_soap_detect_tone(work);
        fossil_io_printf("{blue}[tone]{normal} Detected tone: %s\n", t ? t : "unknown");
    }

    if (detect_type) {
        fossil_io_printf("{yellow,bold}=== Detector: %s ==={normal}\n", detect_type);

        int result = 0;
        bool is_neutral = fossil_io_cstring_equals(detect_type, "neutral");

        if (fossil_io_cstring_equals(detect_type, "ragebait"))
            result = fossil_io_soap_detect_ragebait(work);
        else if (fossil_io_cstring_equals(detect_type, "clickbait"))
            result = fossil_io_soap_detect_clickbait(work);
        else if (fossil_io_cstring_equals(detect_type, "spam"))
            result = fossil_io_soap_detect_spam(work);
        else if (fossil_io_cstring_equals(detect_type, "woke"))
            result = fossil_io_soap_detect_woke(work);
        else if (fossil_io_cstring_equals(detect_type, "bot"))
            result = fossil_io_soap_detect_bot(work);
        else if (fossil_io_cstring_equals(detect_type, "sarcasm"))
            result = fossil_io_soap_detect_sarcasm(work);
        else if (fossil_io_cstring_equals(detect_type, "formal"))
            result = fossil_io_soap_detect_formal(work);
        else if (fossil_io_cstring_equals(detect_type, "snowflake"))
            result = fossil_io_soap_detect_snowflake(work);
        else if (fossil_io_cstring_equals(detect_type, "offensive"))
            result = fossil_io_soap_detect_offensive(work);
        else if (fossil_io_cstring_equals(detect_type, "neutral"))
            result = fossil_io_soap_detect_neutral(work);
        else if (fossil_io_cstring_equals(detect_type, "hype"))
            result = fossil_io_soap_detect_hype(work);
        else if (fossil_io_cstring_equals(detect_type, "quality"))
            result = fossil_io_soap_detect_quality(work);
        else if (fossil_io_cstring_equals(detect_type, "political"))
            result = fossil_io_soap_detect_political(work);
        else if (fossil_io_cstring_equals(detect_type, "conspiracy"))
            result = fossil_io_soap_detect_conspiracy(work);
        else if (fossil_io_cstring_equals(detect_type, "marketing"))
            result = fossil_io_soap_detect_marketing(work);
        else if (fossil_io_cstring_equals(detect_type, "technobabble"))
            result = fossil_io_soap_detect_technobabble(work);
        else {
            fossil_io_printf("{red,bold}[detect]{normal} Unknown detector type: %s\n", detect_type);
            fossil_io_cstring_free(work);
            fossil_io_cstring_free(input);
            return 2;
        }

        fossil_io_printf("{yellow}[detect:%s]{normal} %s\n",
            detect_type,
            result
                ? (is_neutral ? "{green}MATCH FOUND{normal}" : "{red,bold}MATCH FOUND{normal}")
                : "{green}clean{normal}");
    }

    fossil_io_cstring_free(work);
    fossil_io_cstring_free(input);
    return 0;
}
