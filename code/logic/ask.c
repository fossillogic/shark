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

/**
 * Ask AI a single question about a file or topic
 */
int fossil_shark_ask(ccstring file_path, bool explain,
                     bool analyze, bool quiet) {
    // Initialize jellyfish AI chain for learning and reasoning
    fossil_ai_jellyfish_chain_t ai_chain;
    fossil_ai_jellyfish_init(&ai_chain);
    
    cstring file_content = fossil_io_cstring_create(cempty);
    
    if (cnotnull(file_path)) {
        fossil_fstream_t file_stream;
        if (fossil_fstream_open(&file_stream, file_path, "r") != 0) {
            fossil_io_printf("{red}Error: Unable to open file %s{normal}\n", file_path);
            fossil_io_cstring_free(file_content);
            return 1;
        }
        
        size_t buffer_size = 4096;
        char *buffer = csafe_cast(char*, fossil_sys_memory_alloc(buffer_size));
        if (cunlikely(buffer == cnull)) {
            fossil_io_printf("{red}Error: Memory allocation failed{normal}\n");
            fossil_fstream_close(&file_stream);
            fossil_io_cstring_free(file_content);
            return 1;
        }
        
        size_t bytes_read;
        size_t total_bytes = 0;
        
        while ((bytes_read = fossil_fstream_read(&file_stream, buffer, sizeof(char), buffer_size - 1)) > 0) {
            buffer[bytes_read] = cterm;
            cstring new_content = fossil_io_cstring_concat(file_content, buffer);
            fossil_io_cstring_free(file_content);
            file_content = new_content;
            total_bytes += bytes_read;
        }
        fossil_fstream_close(&file_stream);
        fossil_sys_memory_free(buffer);
        cdrop(buffer);

        if (!quiet) {
            fossil_io_printf("{cyan}Loaded file content from %s (%zu bytes){normal}\n", file_path, total_bytes);
        }
        
        // Learn from file content
        if (fossil_io_cstring_length(file_content) > 0) {
            fossil_ai_jellyfish_learn(&ai_chain, file_path, file_content);
        }
    }

    cstring question = fossil_io_cstring_create(cempty);
    fossil_io_printf("{blue}Enter your question:{normal} ");
    
    size_t input_buffer_size = 1024;
    char *input_buffer = csafe_cast(char*, fossil_sys_memory_alloc(input_buffer_size));
    if (cunlikely(input_buffer == cnull)) {
        fossil_io_printf("{red}Error: Memory allocation failed{normal}\n");
        fossil_io_cstring_free(file_content);
        fossil_io_cstring_free(question);
        return 1;
    }
    
    if (fossil_io_gets(input_buffer, input_buffer_size) != 0) {
        fossil_io_printf("{red}Error: Failed to read question.{normal}\n");
        fossil_io_cstring_free(file_content);
        fossil_io_cstring_free(question);
        fossil_sys_memory_free(input_buffer);
        cdrop(input_buffer);
        return 1;
    }
    
    // Remove trailing whitespace and create cstring
    fossil_io_trim(input_buffer);
    fossil_io_cstring_free(question);
    question = fossil_io_cstring_create(input_buffer);
    fossil_sys_memory_free(input_buffer);
    cdrop(input_buffer);

    // Use jellyfish AI reasoning for response
    fossil_io_printf("\n{cyan,bold}[AI Response]{normal}\n");
    
    if (cnotnull(file_path) && fossil_io_cstring_length(file_content) > 0) {
        fossil_io_printf("{blue}Based on file '%s':{normal}\n", file_path);
    }
    
    // Get AI response using jellyfish reasoning
    const char *ai_response = fossil_ai_jellyfish_reason(&ai_chain, question);
    
    if (clikely(analyze)) {
        char output_buffer[512];
        float confidence;
        const fossil_ai_jellyfish_block_t *matched_block;
        
        if (fossil_ai_jellyfish_reason_verbose(&ai_chain, question, output_buffer, &confidence, &matched_block)) {
            fossil_io_printf("{cyan}Analysis:{normal} AI confidence: %.2f%% - %s\n", confidence * 100, output_buffer);
        } else {
            fossil_io_printf("{cyan}Analysis:{normal} No matching patterns found in knowledge base.\n");
        }
    }
    
    if (clikely(explain)) {
        const fossil_ai_jellyfish_block_t *best_match = fossil_ai_jellyfish_best_match(&ai_chain, question);
        if (best_match) {
            char explanation[256];
            fossil_ai_jellyfish_block_explain(best_match, explanation, sizeof(explanation));
            fossil_io_printf("{cyan}Explanation:{normal} %s\n", explanation);
        } else {
            fossil_io_printf("{cyan}Explanation:{normal} Reasoning based on general knowledge patterns.\n");
        }
    }
    
    if (cunlikely(!analyze && !explain)) {
        fossil_io_printf("{cyan}Answer:{normal} %s\n", ai_response);
    }
    
    // Clean up AI chain
    fossil_ai_jellyfish_cleanup(&ai_chain);

    fossil_io_cstring_free(file_content);
    fossil_io_cstring_free(question);
    return 0;
}
