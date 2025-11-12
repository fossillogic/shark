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
 * Start an interactive AI chat session
 */
int fossil_shark_chat(ccstring file_path, ccstring model_name,
                      ccstring system_role, ccstring save_path,
                      bool keep_context) {
    fossil_ai_jellyfish_chain_t chain = {0};
    
    // Initialize the chat session
    if (fossil_ai_iochat_start(cunwrap_or(model_name, "default"), &chain) != 0) {
        fossil_io_printf("{red}Error: Failed to initialize chat session{normal}\n");
        return 1;
    }

    // Load context from file if provided
    if (cnotnull(file_path)) {
        if (fossil_ai_iochat_import_context(&chain, file_path) != 0) {
            fossil_io_printf("{red}Error: Could not load context from %s{normal}\n", file_path);
            fossil_ai_iochat_end(&chain);
            return 1;
        }
        fossil_io_printf("{cyan}Loaded context from file: %s{normal}\n", file_path);
    }

    // Inject system message if role is provided
    if (cnotnull(system_role)) {
        char system_msg[256];
        snprintf(system_msg, sizeof(system_msg), "System role: %s", system_role);
        fossil_ai_iochat_inject_system_message(&chain, system_msg);
    }

    char *display_model = fossil_sys_memory_strdup(cunwrap_or(model_name, "default"));
    char *display_role = fossil_sys_memory_strdup(cunwrap_or(system_role, "assistant"));
    
    fossil_io_printf("{cyan}Starting AI chat session (Model: %s, Role: %s){normal}\n",
           display_model, display_role);

    char *input_buffer = (char*)fossil_sys_memory_alloc(1024);
    char *sanitized_input = (char*)fossil_sys_memory_alloc(1024);
    char *response_buffer = (char*)fossil_sys_memory_alloc(1024);
    
    if (cunlikely(input_buffer == cnull || sanitized_input == cnull || response_buffer == cnull)) {
        cpanic("Failed to allocate memory for input buffers");
    }
    
    while (1) {
        fossil_io_printf("{cyan}You: {normal}");
        if (fossil_io_gets(input_buffer, 1024) != 0) break;

        fossil_io_trim(input_buffer);

        if (strcmp(input_buffer, "exit") == 0 || strcmp(input_buffer, "quit") == 0) {
            break;
        }

        int sanitize_result = fossil_io_validate_sanitize_string(input_buffer, sanitized_input, 
                                                               1024, 
                                                               FOSSIL_CTX_GENERIC);
        
        if (sanitize_result & FOSSIL_SAN_SCRIPT || sanitize_result & FOSSIL_SAN_SQL || 
            sanitize_result & FOSSIL_SAN_SHELL) {
            fossil_io_printf("{red}Warning: Potentially harmful input detected and sanitized.{normal}\n");
        }

        // Generate AI response using the jellyfish chain
        if (fossil_ai_iochat_respond(&chain, sanitized_input, response_buffer, 1024) == 0) {
            fossil_io_printf("{blue}%s{normal}\n", response_buffer);
            
            // Learn from the interaction if keeping context
            if (keep_context) {
                fossil_ai_iochat_learn_response(&chain, sanitized_input, response_buffer);
            }
        } else {
            // Fallback response if AI system fails
            const char *ai_name = cunwrap_or(model_name, "Jellyfish");
            snprintf(response_buffer, 1024, "[%s AI] I understand you said: '%s'", ai_name, sanitized_input);
            fossil_io_printf("{blue}%s{normal}\n", response_buffer);
        }
    }

    // Export conversation history if save path is provided
    if (cnotnull(save_path)) {
        if (fossil_ai_iochat_export_history(&chain, save_path) != 0) {
            fossil_io_printf("{red}Warning: Could not save conversation to %s{normal}\n", save_path);
        } else {
            fossil_io_printf("{cyan}Conversation saved to: %s{normal}\n", save_path);
        }
    }

    // Display session summary
    char summary[512];
    if (fossil_ai_iochat_summarize_session(&chain, summary, sizeof(summary)) == 0) {
        fossil_io_printf("{cyan}Session Summary: %s{normal}\n", summary);
    }

    int turn_count = fossil_ai_iochat_turn_count(&chain);
    fossil_io_printf("{cyan}Total conversation turns: %d{normal}\n", turn_count);

    // Cleanup
    if (cnotnull(input_buffer)) {
        fossil_sys_memory_free(input_buffer);
        cnullify(input_buffer);
    }
    if (cnotnull(sanitized_input)) {
        fossil_sys_memory_free(sanitized_input);
        cnullify(sanitized_input);
    }
    if (cnotnull(response_buffer)) {
        fossil_sys_memory_free(response_buffer);
        cnullify(response_buffer);
    }
    if (cnotnull(display_model)) {
        fossil_sys_memory_free(display_model);
        cnullify(display_model);
    }
    if (cnotnull(display_role)) {
        fossil_sys_memory_free(display_role);
        cnullify(display_role);
    }

    // End the chat session
    fossil_ai_iochat_end(&chain);

    fossil_io_printf("{cyan}Chat session ended.{normal}\n");
    return 0;
}
