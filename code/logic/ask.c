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

int fossil_shark_ask(const char *model_id, const char *file_path, bool explain) {
    fossil_io_printf("{cyan,bold}=== One-Shot AI Prompt ==={normal}\n");
    fossil_io_printf("{green}Model:{normal} %s\n", model_id ? model_id : "default");
    if (file_path)
        fossil_io_printf("{green}File Context:{normal} %s\n", file_path);
    else
        fossil_io_printf("{green}File Context:{normal} None provided\n");
    fossil_io_printf("{green}Explain Response:{normal} %s\n", explain ? "Yes" : "No");

    // --- Jellyfish AI Git-chain Hybrid usage ---
    fossil_ai_jellyfish_chain_t chain;
    fossil_ai_jellyfish_init(&chain);

    // Simulate a prompt/response pair
    const char *input = "What is the meaning of life?";
    const char *output = "42";

    fossil_ai_jellyfish_learn(&chain, input, output);

    // Reason over the chain
    char response[256] = {0};
    float confidence = 0.0f;
    const fossil_ai_jellyfish_block_t *matched_block = NULL;
    bool found = fossil_ai_jellyfish_reason_verbose(&chain, input, response, &confidence, &matched_block);

    fossil_io_printf("{yellow}Processing prompt...{normal}\n");
    if (found) {
        fossil_io_printf("{blue}AI Response:{normal} %s\n", response);
        if (explain && matched_block) {
            char explanation[256] = {0};
            fossil_ai_jellyfish_block_explain(matched_block, explanation, sizeof(explanation));
            fossil_io_printf("{blue}Explanation:{normal} %s\n", explanation);
        }
    } else {
        fossil_io_printf("{red}AI Response:{normal} Unknown\n");
    }

    return 0;
}
