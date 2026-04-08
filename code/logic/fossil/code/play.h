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
#ifndef FOSSIL_APP_COMMAND_PLAY_H
#define FOSSIL_APP_COMMAND_PLAY_H

#include "common.h"

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * Launch a text-based game from the Spino tool.
 * @param game_name Name of the game to play (e.g., "guess_number", "tic_tac_toe").
 * @param rounds Number of rounds to play.
 * @return 0 on success, non-zero on failure.
 */
int fossil_spino_play(const char* game_name, int rounds);

#ifdef __cplusplus
}
#endif

#endif /* FOSSIL_APP_CODE_H */
