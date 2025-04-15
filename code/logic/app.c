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
#include "fossil/code/lifecycle.h"


// Fossil App example implementation (Custom application)
void custom_app_on_create(fossil_app_engine_t* app) {
    fossil_io_printf("Custom App Created\n");
    app->state = FOSSIL_APP_LIFECYCLE_STATE_CREATED;
}

void custom_app_on_start(fossil_app_engine_t* app) {
    fossil_io_printf("Custom App Started\n");
    app->state = FOSSIL_APP_LIFECYCLE_STATE_STARTED;
}

bool app_entry(int argc, char** argv) {
    (void)argc; // Unused parameter
    (void)argv; // Unused parameter

    fossil_io_printf("Custom App Entry\n");
    fossil_app_engine_t app;
    fossil_app_init(&app);
    
    // Overriding with custom app-specific behavior
    app.on_create = custom_app_on_create;
    app.on_start = custom_app_on_start;

    // Run the Fossil App engine
    fossil_app_run(&app);
    return 0;
}
