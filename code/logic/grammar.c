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
#include "fossil/code/grammar.h"

static char *read_file(const char *path)
{
    fossil_io_filesys_file_t file;
    if (fossil_io_filesys_file_open(&file, path, "rb") != 0)
        return cnull;

    if (fossil_io_filesys_file_seek(&file, 0, SEEK_END) != 0)
    {
        fossil_io_filesys_file_close(&file);
        return cnull;
    }
    int64_t size = fossil_io_filesys_file_tell(&file);
    if (size < 0)
    {
        fossil_io_filesys_file_close(&file);
        return cnull;
    }
    if (fossil_io_filesys_file_seek(&file, 0, SEEK_SET) != 0)
    {
        fossil_io_filesys_file_close(&file);
        return cnull;
    }

    char *buf = malloc((size_t)size + 1);
    if (!buf)
    {
        fossil_io_filesys_file_close(&file);
        return cnull;
    }

    size_t read = fossil_io_filesys_file_read(&file, buf, 1, (size_t)size);
    if (read != (size_t)size)
    {
        free(buf);
        fossil_io_filesys_file_close(&file);
        return cnull;
    }
    buf[size] = cterm;
    fossil_io_filesys_file_close(&file);
    return buf;
}

int fossil_shark_grammar(ccstring file_path,
                         bool check,
                         bool correct,
                         bool tone,
                         bool sanitize,
                         bool suggest,
                         bool summarize,
                         bool score,
                         ccstring detect_type,
                         int reflow_width,
                         ccstring capitalize_mode,
                         bool format,
                         bool declutter,
                         bool punctuate)
{
    if (!file_path)
    {
        fossil_io_printf("{red,bold}[error]{normal} grammar: invalid NULL path\n");
        return 1;
    }

    cstring input = read_file(file_path);
    if (!input)
    {
        input = fossil_io_cstring_dup(file_path);
        if (!input)
        {
            fossil_io_printf("{red,bold}[error]{normal} grammar: memory allocation failed\n");
            return 1;
        }
    }

    cstring work = fossil_io_cstring_dup(input);
    if (!work)
    {
        fossil_io_cstring_free(input);
        return 1;
    }

    fossil_io_printf("{cyan,bold}=== Grammar Analysis ==={normal}\n");

    // Declutter
    if (declutter)
    {
        cstring decluttered = fossil_io_soap_declutter(work);
        if (decluttered)
        {
            fossil_io_printf("{green}[declutter]{normal} Decluttered text.\n");
            fossil_io_cstring_free(work);
            work = decluttered;
        }
    }

    // Sanitize
    if (sanitize)
    {
        cstring clean = fossil_io_soap_sanitize(work);
        if (clean)
        {
            fossil_io_printf("{green}[sanitize]{normal} Applied sanitization.\n");
            fossil_io_cstring_free(work);
            work = clean;
        }
    }

    // Normalize punctuation
    if (punctuate)
    {
        cstring punct = fossil_io_soap_punctuate(work);
        if (punct)
        {
            fossil_io_printf("{green}[punctuate]{normal} Normalized punctuation.\n");
            fossil_io_cstring_free(work);
            work = punct;
        }
    }

    // Capitalize
    if (capitalize_mode)
    {
        int mode = 0;
        if (strcmp(capitalize_mode, "sentence") == 0)
            mode = 0;
        else if (strcmp(capitalize_mode, "title") == 0)
            mode = 1;
        cstring cap = fossil_io_soap_capitalize(work, mode);
        if (cap)
        {
            fossil_io_printf("{green}[capitalize]{normal} Applied capitalization (%s-case).\n", capitalize_mode);
            fossil_io_cstring_free(work);
            work = cap;
        }
    }

    // Reflow
    if (reflow_width > 0)
    {
        cstring reflowed = fossil_io_soap_reflow(work, reflow_width);
        if (reflowed)
        {
            fossil_io_printf("{green}[reflow]{normal} Reflowed text to width %d.\n", reflow_width);
            fossil_io_cstring_free(work);
            work = reflowed;
        }
    }

    // Format
    if (format)
    {
        cstring formatted = fossil_io_soap_format(work);
        if (formatted)
        {
            fossil_io_printf("{green}[format]{normal} Pretty-printed text.\n");
            fossil_io_cstring_free(work);
            work = formatted;
        }
    }

    // Grammar/style check
    if (check)
    {
        fossil_io_soap_grammar_style_t style = fossil_io_soap_analyze_grammar_style(work);
        fossil_io_printf("{green}[check]{normal} Grammar OK: %s, Passive voice: %d%%, Style: %s\n",
                         style.grammar_ok ? "yes" : "no",
                         style.passive_voice_pct,
                         style.style ? style.style : "unknown");
    }

    // Correct grammar
    if (correct)
    {
        cstring fixed = fossil_io_soap_correct_grammar(work);
        if (fixed)
        {
            fossil_io_printf("{green}[correct]{normal} Auto-corrected grammar:\n%s\n", fixed);
            fossil_io_cstring_free(work);
            work = fixed;
        }
    }

    // Suggest improvements
    if (suggest)
    {
        cstring sug = fossil_io_soap_suggest(work);
        if (sug)
        {
            fossil_io_printf("{magenta,bold}=== Suggestions ==={normal}\n%s\n", sug);
            fossil_io_cstring_free(sug);
        }
    }

    // Summarize
    if (summarize)
    {
        cstring summary = fossil_io_soap_summarize(work);
        if (summary)
        {
            fossil_io_printf("{cyan}[summary]{normal} %s\n", summary);
            fossil_io_cstring_free(summary);
        }
    }

    // Score
    if (score)
    {
        fossil_io_soap_scores_t scores = fossil_io_soap_score(work);
        const char *read_label = fossil_io_soap_readability_label(scores.readability);
        fossil_io_printf("{yellow}[score]{normal} Readability: %d (%s), Clarity: %d, Quality: %d\n",
                         scores.readability, read_label, scores.clarity, scores.quality);
    }

    // Tone/style detection
    if (tone)
    {
        fossil_io_soap_grammar_style_t style = fossil_io_soap_analyze_grammar_style(work);
        fossil_io_printf("{blue}[tone]{normal} Detected style: %s\n", style.style ? style.style : "unknown");
    }

    // Trait detection
    if (detect_type)
    {
        fossil_io_printf("{yellow,bold}=== Detector: %s ==={normal}\n", detect_type);
        int result = fossil_io_soap_detect(work, detect_type);
        fossil_io_printf("{yellow}[detect:%s]{normal} %s\n",
                         detect_type,
                         result ? "{red,bold}MATCH FOUND{normal}" : "{green}clean{normal}");
    }

    fossil_io_cstring_free(work);
    fossil_io_cstring_free(input);
    return 0;
}
