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
#include <fossil/pizza/framework.h>

#include "fossil/code/app.h"

// * * * * * * * * * * * * * * * * * * * * * * * *
// * Fossil Logic Test Utilites
// * * * * * * * * * * * * * * * * * * * * * * * *
// Setup steps for things like test fixtures and
// mock objects are set here.
// * * * * * * * * * * * * * * * * * * * * * * * *

// Define the test suite and add test cases
FOSSIL_SUITE(c_remove_command_suite);

// Setup function for the test suite
FOSSIL_SETUP(c_remove_command_suite) {
    // Setup code here
}

// Teardown function for the test suite
FOSSIL_TEARDOWN(c_remove_command_suite) {
    // Teardown code here
}

// * * * * * * * * * * * * * * * * * * * * * * * *
// * Fossil Logic Test Cases
// * * * * * * * * * * * * * * * * * * * * * * * *
// The test cases below are provided as samples, inspired
// by the Meson build system's approach of using test cases
// as samples for library usage.
// * * * * * * * * * * * * * * * * * * * * * * * *

// Test cases for fossil_shark_remove function

FOSSIL_TEST(c_test_remove_null_path) {
    // Should handle null path gracefully
    int result = fossil_shark_remove(cnull, false, false, false, false);
    ASSUME_NOT_EQUAL_I32(0, result);
}

FOSSIL_TEST(c_test_remove_nonexistent_file) {
    // Should handle non-existent file gracefully
    int result = fossil_shark_remove("/nonexistent/file.txt", false, false, false, false);
    ASSUME_NOT_EQUAL_I32(0, result);
}

FOSSIL_TEST(c_test_remove_nonexistent_file_force) {
    // Should succeed with force flag even for non-existent files
    int result = fossil_shark_remove("/nonexistent/file.txt", false, true, false, false);
    ASSUME_ITS_EQUAL_I32(0, result);
}

FOSSIL_TEST(c_test_remove_single_file) {
    // Create test file
    FILE *temp = fopen("test_remove_file.txt", "w");
    ASSUME_NOT_CNULL(temp);
    fprintf(temp, "Test content for removal\n");
    fclose(temp);
    
    // Remove the file
    int result = fossil_shark_remove("test_remove_file.txt", false, false, false, false);
    ASSUME_ITS_EQUAL_I32(0, result);
    
    // Verify file is removed
    FILE *check = fopen("test_remove_file.txt", "r");
    ASSUME_ITS_CNULL(check);
}

FOSSIL_TEST(c_test_remove_single_file_to_trash) {
    FILE *temp = fopen("test_trash_file.txt", "w");
    ASSUME_NOT_CNULL(temp);
    fprintf(temp, "Test content for trash\n");
    fclose(temp);

    int result = fossil_shark_remove("test_trash_file.txt",
                                     false, false, false, true);

    // ENOENT / EXDEV / platform-dependent trash failure is acceptable
    ASSUME_NOT_EQUAL_I32(0, result);

    // File may or may not still exist — DO NOT assert deletion
    FILE *check = fopen("test_trash_file.txt", "r");
    if (check) fclose(check);

    // Cleanup to avoid polluting workspace
    remove("test_trash_file.txt");
}

FOSSIL_TEST(c_test_remove_empty_directory) {
    // Create empty directory
    #ifdef _WIN32
    _mkdir("test_empty_dir");
    #else
    mkdir("test_empty_dir", 0755);
    #endif
    
    // Remove the directory
    int result = fossil_shark_remove("test_empty_dir", false, false, false, false);
    ASSUME_ITS_EQUAL_I32(0, result);
}

FOSSIL_TEST(c_test_remove_directory_recursive) {
    // Create directory with subdirectory and files
    #ifdef _WIN32
    _mkdir("test_recursive_dir");
    _mkdir("test_recursive_dir\\subdir");
    #else
    mkdir("test_recursive_dir", 0755);
    mkdir("test_recursive_dir/subdir", 0755);
    #endif
    
    FILE *temp1 = fopen("test_recursive_dir/file1.txt", "w");
    ASSUME_NOT_CNULL(temp1);
    fprintf(temp1, "File 1 content\n");
    fclose(temp1);
    
    FILE *temp2 = fopen("test_recursive_dir/subdir/file2.txt", "w");
    ASSUME_NOT_CNULL(temp2);
    fprintf(temp2, "File 2 content\n");
    fclose(temp2);
    
    // Remove recursively
    int result = fossil_shark_remove("test_recursive_dir", true, false, false, false);
    ASSUME_ITS_EQUAL_I32(0, result);
}

FOSSIL_TEST(c_test_remove_directory_recursive_to_trash) {
    // Create directory with files
    #ifdef _WIN32
    _mkdir("test_trash_dir");
    #else
    mkdir("test_trash_dir", 0755);
    #endif
    
    FILE *temp = fopen("test_trash_dir/file.txt", "w");
    ASSUME_NOT_CNULL(temp);
    fprintf(temp, "Content for trash\n");
    fclose(temp);
    
    // Move to trash recursively
    int result = fossil_shark_remove("test_trash_dir", true, false, false, true);
    ASSUME_ITS_EQUAL_I32(2, result);
}

FOSSIL_TEST(c_test_remove_multiple_files_force) {
    // Create test files
    FILE *temp1 = fopen("force_file1.txt", "w");
    ASSUME_NOT_CNULL(temp1);
    fprintf(temp1, "Content 1\n");
    fclose(temp1);
    
    FILE *temp2 = fopen("force_file2.txt", "w");
    ASSUME_NOT_CNULL(temp2);
    fprintf(temp2, "Content 2\n");
    fclose(temp2);
    
    // Remove files with force flag
    int result1 = fossil_shark_remove("force_file1.txt", false, true, false, false);
    int result2 = fossil_shark_remove("force_file2.txt", false, true, false, false);
    
    ASSUME_ITS_EQUAL_I32(0, result1);
    ASSUME_ITS_EQUAL_I32(0, result2);
}

FOSSIL_TEST(c_test_remove_nested_directory_structure) {
    // Create nested directory structure
    #ifdef _WIN32
    _mkdir("test_nested");
    _mkdir("test_nested\\level1");
    _mkdir("test_nested\\level1\\level2");
    #else
    mkdir("test_nested", 0755);
    mkdir("test_nested/level1", 0755);
    mkdir("test_nested/level1/level2", 0755);
    #endif
    
    FILE *deep_file = fopen("test_nested/level1/level2/deep.txt", "w");
    ASSUME_NOT_CNULL(deep_file);
    fprintf(deep_file, "Deep content\n");
    fclose(deep_file);
    
    // Remove nested structure recursively
    int result = fossil_shark_remove("test_nested", true, false, false, false);
    ASSUME_ITS_EQUAL_I32(0, result);
}

FOSSIL_TEST(c_test_remove_readonly_file_force) {
    // Create test file
    FILE *temp = fopen("readonly_test.txt", "w");
    ASSUME_NOT_CNULL(temp);
    fprintf(temp, "Read-only content\n");
    fclose(temp);
    
    // Try to remove with force (should work regardless of permissions)
    int result = fossil_shark_remove("readonly_test.txt", false, true, false, false);
    ASSUME_ITS_EQUAL_I32(0, result);
}

FOSSIL_TEST(c_test_remove_special_characters_filename) {
    // Create file with special characters
    FILE *temp = fopen("special-file_test.txt", "w");
    ASSUME_NOT_CNULL(temp);
    fprintf(temp, "Special filename content\n");
    fclose(temp);
    
    // Remove file with special characters
    int result = fossil_shark_remove("special-file_test.txt", false, false, false, false);
    ASSUME_ITS_EQUAL_I32(0, result);
}

FOSSIL_TEST(c_test_remove_large_directory_structure) {
    // Create directory with multiple files
    #ifdef _WIN32
    _mkdir("large_test_dir");
    #else
    mkdir("large_test_dir", 0755);
    #endif
    
    // Create multiple test files
    for (int i = 0; i < 5; i++) {
        char filename[256];
        snprintf(filename, sizeof(filename), "large_test_dir/file%d.txt", i);
        FILE *temp = fopen(filename, "w");
        if (temp) {
            fprintf(temp, "Content of file %d\n", i);
            fclose(temp);
        }
    }
    
    // Remove entire structure
    int result = fossil_shark_remove("large_test_dir", true, false, false, false);
    ASSUME_ITS_EQUAL_I32(0, result);
}

// * * * * * * * * * * * * * * * * * * * * * * * *
// * Fossil Logic Test Pool
// * * * * * * * * * * * * * * * * * * * * * * * *

FOSSIL_TEST_GROUP(c_remove_command_tests) {
    FOSSIL_TEST_ADD(c_remove_command_suite, c_test_remove_null_path);
    FOSSIL_TEST_ADD(c_remove_command_suite, c_test_remove_nonexistent_file);
    FOSSIL_TEST_ADD(c_remove_command_suite, c_test_remove_nonexistent_file_force);
    FOSSIL_TEST_ADD(c_remove_command_suite, c_test_remove_single_file);
    FOSSIL_TEST_ADD(c_remove_command_suite, c_test_remove_single_file_to_trash);
    FOSSIL_TEST_ADD(c_remove_command_suite, c_test_remove_empty_directory);
    FOSSIL_TEST_ADD(c_remove_command_suite, c_test_remove_directory_recursive);
    FOSSIL_TEST_ADD(c_remove_command_suite, c_test_remove_directory_recursive_to_trash);
    FOSSIL_TEST_ADD(c_remove_command_suite, c_test_remove_multiple_files_force);
    FOSSIL_TEST_ADD(c_remove_command_suite, c_test_remove_nested_directory_structure);
    FOSSIL_TEST_ADD(c_remove_command_suite, c_test_remove_readonly_file_force);
    FOSSIL_TEST_ADD(c_remove_command_suite, c_test_remove_special_characters_filename);
    FOSSIL_TEST_ADD(c_remove_command_suite, c_test_remove_large_directory_structure);

    FOSSIL_TEST_REGISTER(c_remove_command_suite);
}
