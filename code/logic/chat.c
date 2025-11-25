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

int fossil_shark_chat(const char *model_id, bool keep_context, const char *save_file) {
    fossil_io_printf("{cyan,bold}=== Interactive Chat Session ==={normal}\n");
    fossil_io_printf("{green}Model:{normal} %s\n", model_id ? model_id : "default");
    fossil_io_printf("{green}Keep Context:{normal} %s\n", keep_context ? "Yes" : "No");
    if (save_file)
        fossil_io_printf("{green}Save Transcript To:{normal} %s\n", save_file);
    else
        fossil_io_printf("{green}Save Transcript To:{normal} Not saving\n");

    // Jellyfish AI Git-chain hybrid session
    fossil_ai_jellyfish_chain_t chain;
    fossil_ai_jellyfish_init(&chain);

    char input_buf[FOSSIL_JELLYFISH_INPUT_SIZE];
    char output_buf[FOSSIL_JELLYFISH_OUTPUT_SIZE];
    int running = 1;

    fossil_io_file_t *input_stream = FOSSIL_STDIN;

    fossil_io_file_t transcript_stream;
    fossil_io_file_t *transcript = NULL;
    if (save_file) {
        if (fossil_io_file_open(&transcript_stream, save_file, "a") == 0) {
            transcript = &transcript_stream;
        } else {
            fossil_io_printf("{red}Failed to open transcript file: %s{normal}\n", save_file);
        }
    }

    while (running) {
        fossil_io_printf("{blue}You:{normal} ");
        if (!fossil_io_gets_from_stream(input_buf, sizeof(input_buf), input_stream)) {
            fossil_io_printf("{red}Input error or EOF. Exiting chat.{normal}\n");
            break;
        }
        fossil_io_trim(input_buf);
        if (strcmp(input_buf, "/exit") == 0 || strcmp(input_buf, "/quit") == 0) {
            fossil_io_printf("{magenta}Session ended.{normal}\n");
            break;
        }
        // Reason using Jellyfish chain
        float confidence = 0.0f;
        const fossil_ai_jellyfish_block_t *matched = NULL;
        bool found = fossil_ai_jellyfish_reason_verbose(&chain, input_buf, output_buf, &confidence, &matched);

        if (found && confidence > 0.0f) {
            fossil_io_printf("{yellow}AI:{normal} %s {gray}(confidence: %.2f){normal}\n", output_buf, confidence);
        } else {
            strcpy(output_buf, "Unknown");
            fossil_io_printf("{yellow}AI:{normal} Unknown\n");
        }

        // Learn new input/output pair if not found
        if (!found || confidence < 0.5f) {
            fossil_ai_jellyfish_learn(&chain, input_buf, output_buf);
        }

        // Optionally save transcript using fossil_io_file_write
        if (transcript && fossil_io_file_is_open(transcript)) {
            char transcript_line[1024];
            snprintf(transcript_line, sizeof(transcript_line), "You: %s\nAI: %s\n", input_buf, output_buf);
            fossil_io_file_write(transcript, transcript_line, 1, strlen(transcript_line));
            fossil_io_file_flush(transcript);
        }
    }

    // Optionally persist chain state
    if (save_file) {
        fossil_ai_jellyfish_save(&chain, save_file);
        if (transcript && fossil_io_file_is_open(transcript)) {
            fossil_io_file_close(transcript);
        }
    }

    return 0;
}
