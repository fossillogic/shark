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
#include <fossil/pizza/framework.h>

#include "fossil/code/app.h"


// * * * * * * * * * * * * * * * * * * * * * * * *
// * Fossil Logic Test Utilites
// * * * * * * * * * * * * * * * * * * * * * * * *
// Setup steps for things like test fixtures and
// mock objects are set here.
// * * * * * * * * * * * * * * * * * * * * * * * *

// Define the test suite and add test cases
FOSSIL_TEST_SUITE(c_move_suite);

// Setup function for the test suite
FOSSIL_SETUP(c_move_suite) {
    // Setup code here
}

// Teardown function for the test suite
FOSSIL_TEARDOWN(c_move_suite) {
    // Teardown code here
}

// * * * * * * * * * * * * * * * * * * * * * * * *
// * Fossil Logic Test Cases
// * * * * * * * * * * * * * * * * * * * * * * * *
// The test cases below are provided as samples, inspired
// by the Meson build system's approach of using test cases
// as samples for library usage.
// * * * * * * * * * * * * * * * * * * * * * * * *

FOSSIL_TEST_CASE(c_test_handle_move_success) {
    const char *source = "test_source.txt";
    const char *destination = "test_destination.txt";

    // Create a mock file to simulate the source
    int create_result = FOSSIL_SANITY_SYS_CREATE_FILE(source);
    ASSUME_ITS_EQUAL_I32(0, create_result);

    FILE *file = fopen(source, "w");
    ASSUME_NOT_CNULL(file);
    fprintf(file, "Test content");
    fclose(file);

    // Call the function to test (force = 1, interactive = 0)
    handle_move(source, destination, 1, 0);

    // Check if the destination file exists and contains the expected content
    ASSUME_ITS_EQUAL_I32(1, FOSSIL_SANITY_SYS_FILE_EXISTS(destination));

    FILE *dest_file = fopen(destination, "r");
    ASSUME_NOT_CNULL(dest_file);

    char buffer[256];
    fgets(buffer, sizeof(buffer), dest_file);
    ASSUME_ITS_EQUAL_CSTR("Test content", buffer);
    fclose(dest_file);

    // Cleanup
    remove(destination);
    remove(source);
}

FOSSIL_TEST_CASE(c_test_handle_move_failure) {
    const char *source = "non_existent_source.txt";
    const char *destination = "test_destination.txt";

    // Call the function to test with a non-existent source (force = 1, interactive = 0)
    handle_move(source, destination, 1, 0);

    // Check if the destination file was not created
    ASSUME_ITS_EQUAL_I32(0, FOSSIL_SANITY_SYS_FILE_EXISTS(destination));
}

// * * * * * * * * * * * * * * * * * * * * * * * *
// * Fossil Logic Test Pool
// * * * * * * * * * * * * * * * * * * * * * * * *

FOSSIL_TEST_GROUP(c_move_command_tests) {
    FOSSIL_TEST_ADD(c_move_suite, c_test_handle_move_success);
    FOSSIL_TEST_ADD(c_move_suite, c_test_handle_move_failure);

    FOSSIL_TEST_REGISTER(c_move_suite);
}
