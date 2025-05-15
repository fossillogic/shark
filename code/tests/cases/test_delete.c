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
FOSSIL_TEST_SUITE(c_delete_suite);

// Setup function for the test suite
FOSSIL_SETUP(c_delete_suite) {
    // Setup code here
}

// Teardown function for the test suite
FOSSIL_TEARDOWN(c_delete_suite) {
    // Teardown code here
}

// * * * * * * * * * * * * * * * * * * * * * * * *
// * Fossil Logic Test Cases
// * * * * * * * * * * * * * * * * * * * * * * * *
// The test cases below are provided as samples, inspired
// by the Meson build system's approach of using test cases
// as samples for library usage.
// * * * * * * * * * * * * * * * * * * * * * * * *

FOSSIL_TEST_CASE(c_test_handle_delete_success) {
    const char *target = "test_file_to_delete.txt";

    // Create a mock file to simulate the target
    int create_result = FOSSIL_SANITY_SYS_CREATE_FILE(target);
    ASSUME_ITS_EQUAL_I32(create_result, 0);

    // Ensure the file exists before deletion
    ASSUME_ITS_TRUE(FOSSIL_SANITY_SYS_FILE_EXISTS(target));

    // Call the function to test
    handle_delete(target);

    // Ensure the file no longer exists
    ASSUME_ITS_FALSE(FOSSIL_SANITY_SYS_FILE_EXISTS(target));
}

FOSSIL_TEST_CASE(c_test_handle_delete_failure) {
    const char *target = "non_existent_file_to_delete.txt";

    // Ensure the file does not exist before deletion
    ASSUME_ITS_FALSE(FOSSIL_SANITY_SYS_FILE_EXISTS(target));

    // Call the function to test
    handle_delete(target);

    // Ensure the file still does not exist
    ASSUME_ITS_FALSE(FOSSIL_SANITY_SYS_FILE_EXISTS(target));
}

// * * * * * * * * * * * * * * * * * * * * * * * *
// * Fossil Logic Test Pool
// * * * * * * * * * * * * * * * * * * * * * * * *

FOSSIL_TEST_GROUP(c_delete_command_tests) {
    FOSSIL_TEST_ADD(c_delete_suite, c_test_handle_delete_success);
    FOSSIL_TEST_ADD(c_delete_suite, c_test_handle_delete_failure);

    FOSSIL_TEST_REGISTER(c_delete_suite);
}
