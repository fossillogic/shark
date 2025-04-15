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
#include "fossil/code/lifecycle.h"

// Function implementations for lifecycle methods
void fossil_app_on_create(fossil_app_engine_t* app) {
    if (app == cnullptr) {
        fossil_io_error("%s\n", fossil_io_what(FOSSIL_ERROR_NULL_POINTER));
        return;
    }
    fossil_io_printf("Fossil App created\n");
    app->state = FOSSIL_APP_LIFECYCLE_STATE_CREATED;
}

void fossil_app_on_start(fossil_app_engine_t* app) {
    if (app == cnullptr) {
        fossil_io_error("%s\n", fossil_io_what(FOSSIL_ERROR_NULL_POINTER));
        return;
    }
    fossil_io_printf("Fossil App started\n");
    app->state = FOSSIL_APP_LIFECYCLE_STATE_STARTED;
}

void fossil_app_on_resume(fossil_app_engine_t* app) {
    if (app == cnullptr) {
        fossil_io_error("%s\n", fossil_io_what(FOSSIL_ERROR_NULL_POINTER));
        return;
    }
    fossil_io_printf("Fossil App resumed\n");
    app->state = FOSSIL_APP_LIFECYCLE_STATE_RESUMED;
}

void fossil_app_on_pause(fossil_app_engine_t* app) {
    if (app == cnullptr) {
        fossil_io_error("%s\n", fossil_io_what(FOSSIL_ERROR_NULL_POINTER));
        return;
    }
    fossil_io_printf("Fossil App paused\n");
    app->state = FOSSIL_APP_LIFECYCLE_STATE_PAUSED;
}

void fossil_app_on_timeout(fossil_app_engine_t* app) {
    if (app == cnullptr) {
        fossil_io_error("%s\n", fossil_io_what(FOSSIL_ERROR_NULL_POINTER));
        return;
    }
    fossil_io_printf("Fossil App timed out\n");
    app->state = FOSSIL_APP_LIFECYCLE_STATE_TIMEOUT;
}

void fossil_app_on_stop(fossil_app_engine_t* app) {
    if (app == cnullptr) {
        fossil_io_error("%s\n", fossil_io_what(FOSSIL_ERROR_NULL_POINTER));
        return;
    }
    fossil_io_printf("Fossil App stopped\n");
    app->state = FOSSIL_APP_LIFECYCLE_STATE_STOPPED;
}

void fossil_app_on_destroy(fossil_app_engine_t* app) {
    if (app == cnullptr) {
        fossil_io_error("%s\n", fossil_io_what(FOSSIL_ERROR_NULL_POINTER));
        return;
    }
    fossil_io_printf("Fossil App destroyed\n");
    app->state = FOSSIL_APP_LIFECYCLE_STATE_DESTROYED;
}

// Transition check to ensure valid state transitions
bool fossil_app_transition(fossil_app_engine_t* app, fossil_app_state_t new_state) {
    if (app == cnullptr) {
        fossil_io_error("%s\n", fossil_io_what(FOSSIL_ERROR_NULL_POINTER));
        return false;
    }
    switch (app->state) {
        case FOSSIL_APP_LIFECYCLE_STATE_CREATED:
            if (new_state == FOSSIL_APP_LIFECYCLE_STATE_STARTED) return true;
            break;
        case FOSSIL_APP_LIFECYCLE_STATE_STARTED:
            if (new_state == FOSSIL_APP_LIFECYCLE_STATE_RESUMED || new_state == FOSSIL_APP_LIFECYCLE_STATE_STOPPED) return true;
            break;
        case FOSSIL_APP_LIFECYCLE_STATE_RESUMED:
            if (new_state == FOSSIL_APP_LIFECYCLE_STATE_PAUSED || new_state == FOSSIL_APP_LIFECYCLE_STATE_STOPPED) return true;
            break;
        case FOSSIL_APP_LIFECYCLE_STATE_PAUSED:
            if (new_state == FOSSIL_APP_LIFECYCLE_STATE_RESUMED || new_state == FOSSIL_APP_LIFECYCLE_STATE_STOPPED) return true;
            break;
        case FOSSIL_APP_LIFECYCLE_STATE_TIMEOUT:
            if (new_state == FOSSIL_APP_LIFECYCLE_STATE_STOPPED) return true;
            break;
        case FOSSIL_APP_LIFECYCLE_STATE_STOPPED:
            if (new_state == FOSSIL_APP_LIFECYCLE_STATE_DESTROYED) return true;
            break;
        case FOSSIL_APP_LIFECYCLE_STATE_DESTROYED:
            return false; // No more valid transitions from destroyed state
    }
    return false;
}

// Initialize the Fossil App engine with the correct lifecycle methods
void fossil_app_init(fossil_app_engine_t* app) {
    if (app == cnullptr) {
        fossil_io_error("%s\n", fossil_io_what(FOSSIL_ERROR_NULL_POINTER));
        return;
    }

    fossil_sys_memory_set(app, 0, sizeof(fossil_app_engine_t));
    app->on_create  = fossil_app_on_create;
    app->on_start   = fossil_app_on_start;
    app->on_resume  = fossil_app_on_resume;
    app->on_pause   = fossil_app_on_pause;
    app->on_stop    = fossil_app_on_stop;
    app->on_timeout = fossil_app_on_timeout;
    app->on_destroy = fossil_app_on_destroy;
    app->transition = fossil_app_transition;
}

// Main loop for the Fossil App engine
void fossil_app_run(fossil_app_engine_t* app) {
    if (app == cnullptr) {
        fossil_io_error("%s\n", fossil_io_what(FOSSIL_ERROR_NULL_POINTER));
        return;
    }
    bool is_running = true;
    
    while (is_running) {
        switch (app->state) {
            case FOSSIL_APP_LIFECYCLE_STATE_CREATED:
                if (app->transition(app, FOSSIL_APP_LIFECYCLE_STATE_STARTED)) {
                    app->on_create(app);
                    app->on_start(app);
                }
                break;

            case FOSSIL_APP_LIFECYCLE_STATE_STARTED:
                if (app->transition(app, FOSSIL_APP_LIFECYCLE_STATE_RESUMED)) {
                    app->on_resume(app);
                } else if (app->transition(app, FOSSIL_APP_LIFECYCLE_STATE_STOPPED)) {
                    app->on_stop(app);
                }
                break;

            case FOSSIL_APP_LIFECYCLE_STATE_RESUMED:
                if (app->transition(app, FOSSIL_APP_LIFECYCLE_STATE_PAUSED)) {
                    app->on_pause(app);
                } else if (app->transition(app, FOSSIL_APP_LIFECYCLE_STATE_STOPPED)) {
                    app->on_stop(app);
                }
                break;

            case FOSSIL_APP_LIFECYCLE_STATE_PAUSED:
                if (app->transition(app, FOSSIL_APP_LIFECYCLE_STATE_RESUMED)) {
                    app->on_resume(app);
                } else if (app->transition(app, FOSSIL_APP_LIFECYCLE_STATE_STOPPED)) {
                    app->on_stop(app);
                }
                break;

            case FOSSIL_APP_LIFECYCLE_STATE_STOPPED:
                if (app->transition(app, FOSSIL_APP_LIFECYCLE_STATE_DESTROYED)) {
                    app->on_destroy(app);
                    is_running = false;  // End the loop
                }
                break;

            case FOSSIL_APP_LIFECYCLE_STATE_DESTROYED:
                is_running = false;  // Exit the lifecycle loop as the app is destroyed
                break;

            default:
                fossil_io_error("%s\n", fossil_io_what(FOSSIL_ERROR_UNKNOWN));
                is_running = false;  // Exit on unknown state
                break;
        }
    }
}
