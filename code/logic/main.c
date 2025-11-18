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
#include "fossil/code/app.h"

/**
 * Main entry point for the application.
 * This function initializes the application and starts the main execution loop.
 * It processes command-line arguments and passes them to the application entry
 * function defined in the fossil/app.h header.
 * 
 * @param argc The number of command-line arguments passed to the program.
 * @param argv The array of command-line arguments passed to the program.
 *
 * @return Returns 0 if the application runs successfully. If an error occurs,
 *         a non-zero value is returned to indicate the type of error.
 */
int main(int argc, char** argv) {
    return app_entry(argc, argv);
}
