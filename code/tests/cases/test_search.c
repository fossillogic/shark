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
FOSSIL_TEST_SUITE(c_search_suite);

// Setup function for the test suite
FOSSIL_SETUP(c_search_suite) {
    // Setup code here
}

// Teardown function for the test suite
FOSSIL_TEARDOWN(c_search_suite) {
    // Teardown code here
}

// * * * * * * * * * * * * * * * * * * * * * * * *
// * Fossil Logic Test Cases
// * * * * * * * * * * * * * * * * * * * * * * * *
// The test cases below are provided as samples, inspired
// by the Meson build system's approach of using test cases
// as samples for library usage.
// * * * * * * * * * * * * * * * * * * * * * * * *

FOSSIL_TEST_CASE(c_test_handle_search_found) {
    const char *file = "test_file_search.txt";
    const char *pattern = "match";

    // Create a mock file to simulate the target
    int create_result = FOSSIL_SANITY_SYS_CREATE_FILE(file);
    ASSUME_ITS_EQUAL_I32(create_result, 0);
    FILE *mock_file = fopen(file, "w");
    ASSUME_NOT_CNULL(mock_file);
    fprintf(mock_file, "This is a test line.\nAnother line with match.\nFinal line.");
    fclose(mock_file);

    // Call the function to test
    handle_search(file, pattern);

    // Cleanup
    remove(file);
}

FOSSIL_TEST_CASE(c_test_handle_search_not_found) {
    const char *file = "test_file_search.txt";
    const char *pattern = "notfound";

    // Create a mock file to simulate the target
    int create_result = FOSSIL_SANITY_SYS_CREATE_FILE(file);
    ASSUME_ITS_EQUAL_I32(create_result, 0);
    FILE *mock_file = fopen(file, "w");
    ASSUME_NOT_CNULL(mock_file);
    fprintf(mock_file, "This is a test line.\nAnother line.\nFinal line.");
    fclose(mock_file);

    // Call the function to test
    handle_search(file, pattern);

    // Cleanup
    remove(file);
}

FOSSIL_TEST_CASE(c_test_handle_search_file_not_found) {
    const char *file = "non_existent_file.txt";
    const char *pattern = "anything";

    // Ensure the file does not exist
    int file_exists = FOSSIL_SANITY_SYS_FILE_EXISTS(file);
    ASSUME_ITS_EQUAL_I32(file_exists, 0);

    // Call the function to test
    handle_search(file, pattern);

    // Ensure no file was created
    int file_still_exists = FOSSIL_SANITY_SYS_FILE_EXISTS(file);
    ASSUME_ITS_EQUAL_I32(file_still_exists, 0);
}

// * * * * * * * * * * * * * * * * * * * * * * * *
// * Fossil Logic Test Pool
// * * * * * * * * * * * * * * * * * * * * * * * *

FOSSIL_TEST_GROUP(c_search_command_tests) {
    FOSSIL_TEST_ADD(c_search_suite, c_test_handle_search_found);
    FOSSIL_TEST_ADD(c_search_suite, c_test_handle_search_not_found);
    FOSSIL_TEST_ADD(c_search_suite, c_test_handle_search_file_not_found);

    FOSSIL_TEST_REGISTER(c_search_suite);
}
