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
#ifndef FOSSIL_APP_LIFECYCLE_H
#define FOSSIL_APP_LIFECYCLE_H

#include <fossil/sys/framework.h>
#include <fossil/io/framework.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @enum fossil_app_state_t
 * @brief Enum for defining the various lifecycle states of the application.
 * This enumeration is used to represent the different states that the application
 * can be in during its lifecycle. Each state corresponds to a specific phase in
 * the application's lifecycle, such as creation, starting, resuming, pausing,
 * timing out, stopping, and destruction.
 */
typedef enum {
    FOSSIL_APP_LIFECYCLE_STATE_CREATED,   /**< The application has been created and is in the initial state.            */
    FOSSIL_APP_LIFECYCLE_STATE_STARTED,   /**< The application has started and is now running.                          */
    FOSSIL_APP_LIFECYCLE_STATE_RESUMED,   /**< The application has resumed from a paused state and is now active again. */
    FOSSIL_APP_LIFECYCLE_STATE_PAUSED,    /**< The application is paused and temporarily inactive.                      */
    FOSSIL_APP_LIFECYCLE_STATE_TIMEOUT,   /**< The application has timed out due to inactivity or other conditions.     */
    FOSSIL_APP_LIFECYCLE_STATE_STOPPED,   /**< The application has stopped and is no longer running.                    */
    FOSSIL_APP_LIFECYCLE_STATE_DESTROYED  /**< The application has been destroyed and is in the final state.            */
} fossil_app_state_t;

/**
 * @struct fossil_app_engine
 * @brief Struct for defining the Fossil App lifecycle methods and state.
 * This structure contains the current state of the application and pointers to
 * callback functions that are called during various phases of the application's
 * lifecycle. It also includes a method to transition the application state.
 */
typedef struct fossil_app_engine {
    fossil_app_state_t state;                      /**< Current state of the application, represented by the fossil_app_state_t enum. */
    void (*on_create )(struct fossil_app_engine*); /**< Callback function that is called when the application is created.             */
    void (*on_start  )(struct fossil_app_engine*); /**< Callback function that is called when the application starts.                 */
    void (*on_resume )(struct fossil_app_engine*); /**< Callback function that is called when the application resumes from a paused state. */
    void (*on_pause  )(struct fossil_app_engine*); /**< Callback function that is called when the application pauses.                 */
    void (*on_stop   )(struct fossil_app_engine*); /**< Callback function that is called when the application stops.                  */
    void (*on_timeout)(struct fossil_app_engine*); /**< Callback function that is called when the application times out.              */
    void (*on_destroy)(struct fossil_app_engine*); /**< Callback function that is called when the application is destroyed.           */
    bool (*transition)(struct fossil_app_engine*, fossil_app_state_t); /**< Method to transition the application state to a new state. */
} fossil_app_engine_t;

/**
 * @brief Callback function that is called when the application is created.
 * This function is intended to perform any initialization required when the
 * application is first created.
 * 
 * @param app Pointer to the application engine structure.
 */
void fossil_app_on_create(fossil_app_engine_t* app);

/**
 * @brief Callback function that is called when the application starts.
 * This function is intended to perform any actions required when the
 * application transitions from the created state to the started state.
 * 
 * @param app Pointer to the application engine structure.
 */
void fossil_app_on_start(fossil_app_engine_t* app);

/**
 * @brief Callback function that is called when the application resumes.
 * This function is intended to perform any actions required when the
 * application transitions from the paused state to the resumed state.
 * 
 * @param app Pointer to the application engine structure.
 */
void fossil_app_on_resume(fossil_app_engine_t* app);

/**
 * @brief Callback function that is called when the application pauses.
 * This function is intended to perform any actions required when the
 * application transitions from the resumed state to the paused state.
 * 
 * @param app Pointer to the application engine structure.
 */
void fossil_app_on_pause(fossil_app_engine_t* app);

/**
 * @brief Callback function that is called when the application times out.
 * This function is intended to perform any actions required when the
 * application transitions to a timeout state.
 * 
 * @param app Pointer to the application engine structure.
 */
void fossil_app_on_timeout(fossil_app_engine_t* app);

/**
 * @brief Callback function that is called when the application stops.
 * This function is intended to perform any actions required when the
 * application transitions from the started or resumed state to the stopped state.
 * 
 * @param app Pointer to the application engine structure.
 */
void fossil_app_on_stop(fossil_app_engine_t* app);

/**
 * @brief Callback function that is called when the application is destroyed.
 * This function is intended to perform any cleanup required when the
 * application is about to be destroyed.
 * 
 * @param app Pointer to the application engine structure.
 */
void fossil_app_on_destroy(fossil_app_engine_t* app);

/**
 * @brief Method to transition the application state.
 * This function is used to change the current state of the application to a new state.
 * It performs any necessary actions required for the state transition and updates
 * the state of the application engine structure.
 * 
 * @param app Pointer to the application engine structure.
 * @param new_state The new state to transition to.
 * @return true if the transition was successful, false otherwise.
 */
bool fossil_app_transition(fossil_app_engine_t* app, fossil_app_state_t new_state);

/**
 * @brief Method to run the Fossil App engine.
 * This function is the main loop for the Fossil App engine. It runs the application
 * lifecycle and calls the appropriate callback functions based on the current state
 * of the application.
 * 
 * @param app Pointer to the application engine structure.
 */
void fossil_app_init(fossil_app_engine_t* app);

/**
 * @brief Method to run the Fossil App engine.
 * This function is the main loop for the Fossil App engine. It runs the application
 * lifecycle and calls the appropriate callback functions based on the current state
 * of the application.
 * 
 * @param app Pointer to the application engine structure.
 */
void fossil_app_run(fossil_app_engine_t* app);

#ifdef __cplusplus
}
#endif

#endif /* FOSSIL_APP_LIFECYCLE_H */
