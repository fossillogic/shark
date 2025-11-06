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
