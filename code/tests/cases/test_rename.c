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
FOSSIL_TEST_SUITE(c_rename_suite);

// Setup function for the test suite
FOSSIL_SETUP(c_rename_suite) {
    // Setup code here
}

// Teardown function for the test suite
FOSSIL_TEARDOWN(c_rename_suite) {
    // Teardown code here
}

// * * * * * * * * * * * * * * * * * * * * * * * *
// * Fossil Logic Test Cases
// * * * * * * * * * * * * * * * * * * * * * * * *
// The test cases below are provided as samples, inspired
// by the Meson build system's approach of using test cases
// as samples for library usage.
// * * * * * * * * * * * * * * * * * * * * * * * *

FOSSIL_TEST_CASE(c_test_handle_rename_success) {
    const char *old_name = "test_source_rename.txt";
    const char *new_name = "test_destination_rename.txt";

    // Create a mock file to simulate the source
    int create_result = FOSSIL_SANITY_SYS_CREATE_FILE(old_name);
    ASSUME_ITS_EQUAL_I32(0, create_result);

    FILE *file = fopen(old_name, "w");
    ASSUME_NOT_CNULL(file);
    fprintf(file, "Rename test content");
    fclose(file);

    // Call the function to test (force = 1)
    handle_rename(old_name, new_name, 1);

    // Check if the new file exists and contains the expected content
    ASSUME_ITS_EQUAL_I32(1, FOSSIL_SANITY_SYS_FILE_EXISTS(new_name));
    ASSUME_ITS_EQUAL_I32(0, FOSSIL_SANITY_SYS_FILE_EXISTS(old_name));

    FILE *dest_file = fopen(new_name, "r");
    ASSUME_NOT_CNULL(dest_file);

    char buffer[256];
    fgets(buffer, sizeof(buffer), dest_file);
    ASSUME_ITS_EQUAL_CSTR("Rename test content", buffer);
    fclose(dest_file);

    // Cleanup
    remove(new_name);
}

FOSSIL_TEST_CASE(c_test_handle_rename_failure_no_source) {
    const char *old_name = "non_existent_rename_file.txt";
    const char *new_name = "test_destination_rename.txt";

    // Call the function to test (force = 1)
    handle_rename(old_name, new_name, 1);

    // Ensure the destination file does not exist
    ASSUME_ITS_EQUAL_I32(0, FOSSIL_SANITY_SYS_FILE_EXISTS(new_name));
}

FOSSIL_TEST_CASE(c_test_handle_rename_no_force_destination_exists) {
    const char *old_name = "test_source_rename2.txt";
    const char *new_name = "test_destination_rename2.txt";

    // Create source file
    int create_result = FOSSIL_SANITY_SYS_CREATE_FILE(old_name);
    ASSUME_ITS_EQUAL_I32(0, create_result);
    FILE *file = fopen(old_name, "w");
    ASSUME_NOT_CNULL(file);
    fprintf(file, "Rename test content 2");
    fclose(file);

    // Create destination file
    create_result = FOSSIL_SANITY_SYS_CREATE_FILE(new_name);
    ASSUME_ITS_EQUAL_I32(0, create_result);
    file = fopen(new_name, "w");
    ASSUME_NOT_CNULL(file);
    fprintf(file, "Existing destination content");
    fclose(file);

    // Call the function to test (force = 0)
    handle_rename(old_name, new_name, 0);

    // Source should still exist, destination should be unchanged
    ASSUME_ITS_EQUAL_I32(1, FOSSIL_SANITY_SYS_FILE_EXISTS(old_name));
    ASSUME_ITS_EQUAL_I32(1, FOSSIL_SANITY_SYS_FILE_EXISTS(new_name));

    file = fopen(new_name, "r");
    ASSUME_NOT_CNULL(file);
    char buffer[256];
    fgets(buffer, sizeof(buffer), file);
    ASSUME_ITS_EQUAL_CSTR("Existing destination content", buffer);
    fclose(file);

    // Cleanup
    remove(old_name);
    remove(new_name);
}

FOSSIL_TEST_CASE(c_test_handle_rename_force_destination_exists) {
    const char *old_name = "test_source_rename3.txt";
    const char *new_name = "test_destination_rename3.txt";

    // Create source file
    int create_result = FOSSIL_SANITY_SYS_CREATE_FILE(old_name);
    ASSUME_ITS_EQUAL_I32(0, create_result);
    FILE *file = fopen(old_name, "w");
    ASSUME_NOT_CNULL(file);
    fprintf(file, "Rename test content 3");
    fclose(file);

    // Create destination file
    create_result = FOSSIL_SANITY_SYS_CREATE_FILE(new_name);
    ASSUME_ITS_EQUAL_I32(0, create_result);
    file = fopen(new_name, "w");
    ASSUME_NOT_CNULL(file);
    fprintf(file, "Old destination content");
    fclose(file);

    // Call the function to test (force = 1)
    handle_rename(old_name, new_name, 1);

    // Source should not exist, destination should have new content
    ASSUME_ITS_EQUAL_I32(0, FOSSIL_SANITY_SYS_FILE_EXISTS(old_name));
    ASSUME_ITS_EQUAL_I32(1, FOSSIL_SANITY_SYS_FILE_EXISTS(new_name));

    file = fopen(new_name, "r");
    ASSUME_NOT_CNULL(file);
    char buffer[256];
    fgets(buffer, sizeof(buffer), file);
    ASSUME_ITS_EQUAL_CSTR("Rename test content 3", buffer);
    fclose(file);

    // Cleanup
    remove(new_name);
}

// * * * * * * * * * * * * * * * * * * * * * * * *
// * Fossil Logic Test Pool
// * * * * * * * * * * * * * * * * * * * * * * * *

FOSSIL_TEST_GROUP(c_rename_command_tests) {
    FOSSIL_TEST_ADD(c_rename_suite, c_test_handle_rename_success);
    FOSSIL_TEST_ADD(c_rename_suite, c_test_handle_rename_failure_no_source);
    FOSSIL_TEST_ADD(c_rename_suite, c_test_handle_rename_no_force_destination_exists);
    FOSSIL_TEST_ADD(c_rename_suite, c_test_handle_rename_force_destination_exists);

    FOSSIL_TEST_REGISTER(c_rename_suite);
}
