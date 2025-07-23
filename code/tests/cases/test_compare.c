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
FOSSIL_TEST_SUITE(c_compare_suite);

// Setup function for the test suite
FOSSIL_SETUP(c_compare_suite) {
    // Setup code here
}

// Teardown function for the test suite
FOSSIL_TEARDOWN(c_compare_suite) {
    // Teardown code here
}

// * * * * * * * * * * * * * * * * * * * * * * * *
// * Fossil Logic Test Cases
// * * * * * * * * * * * * * * * * * * * * * * * *
// The test cases below are provided as samples, inspired
// by the Meson build system's approach of using test cases
// as samples for library usage.
// * * * * * * * * * * * * * * * * * * * * * * * *

FOSSIL_TEST_CASE(c_test_handle_compare_success) {
    const char *file1 = "test_compare_file1.txt";
    const char *file2 = "test_compare_file2.txt";

    // Create two identical files
    FILE *f1 = fopen(file1, "w");
    FILE *f2 = fopen(file2, "w");
    ASSUME_NOT_CNULL(f1);
    ASSUME_NOT_CNULL(f2);
    fprintf(f1, "Compare test content");
    fprintf(f2, "Compare test content");
    fclose(f1);
    fclose(f2);

    // Call the function to test (recursive = 0, brief = 0, ignore_case = 0)
    handle_compare(file1, file2, 0, 0, 0);

    // Cleanup
    remove(file1);
    remove(file2);
}

FOSSIL_TEST_CASE(c_test_handle_compare_failure) {
    const char *file1 = "test_compare_file1.txt";
    const char *file2 = "test_compare_file2.txt";

    // Create two different files
    FILE *f1 = fopen(file1, "w");
    FILE *f2 = fopen(file2, "w");
    ASSUME_NOT_CNULL(f1);
    ASSUME_NOT_CNULL(f2);
    fprintf(f1, "Compare test content A");
    fprintf(f2, "Compare test content B");
    fclose(f1);
    fclose(f2);

    // Call the function to test (recursive = 0, brief = 0, ignore_case = 0)
    handle_compare(file1, file2, 0, 0, 0);

    // Cleanup
    remove(file1);
    remove(file2);
}

// * * * * * * * * * * * * * * * * * * * * * * * *
// * Fossil Logic Test Pool
// * * * * * * * * * * * * * * * * * * * * * * * *

FOSSIL_TEST_GROUP(c_compare_command_tests) {
    FOSSIL_TEST_ADD(c_compare_suite, c_test_handle_compare_success);
    FOSSIL_TEST_ADD(c_compare_suite, c_test_handle_compare_failure);

    FOSSIL_TEST_REGISTER(c_compare_suite);
}
