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
FOSSIL_SUITE(c_merge_command_suite);

// Setup function for the test suite
FOSSIL_SETUP(c_merge_command_suite) {
    // Setup code here
}

// Teardown function for the test suite
FOSSIL_TEARDOWN(c_merge_command_suite) {
    // Teardown code here
}

// * * * * * * * * * * * * * * * * * * * * * * * *
// * Fossil Logic Test Cases
// * * * * * * * * * * * * * * * * * * * * * * * *
// The test cases below are provided as samples, inspired
// by the Meson build system's approach of using test cases
// as samples for library usage.
// * * * * * * * * * * * * * * * * * * * * * * * *

FOSSIL_TEST(c_test_merge_null_parameters) {
    // Test with null paths array
    int result = fossil_shark_merge(cnull, 1, "dest", false, false, false, "overwrite", false, false, cnull, cnull);
    ASSUME_NOT_EQUAL_I32(0, result);
    
    // Test with null destination
    const char *paths[] = {"file1.txt"};
    result = fossil_shark_merge(paths, 1, cnull, false, false, false, "overwrite", false, false, cnull, cnull);
    ASSUME_NOT_EQUAL_I32(0, result);
    
    // Test with zero paths
    result = fossil_shark_merge(paths, 0, "dest", false, false, false, "overwrite", false, false, cnull, cnull);
    ASSUME_NOT_EQUAL_I32(0, result);
}

FOSSIL_TEST(c_test_merge_single_file) {
    // Create source file
    FOSSIL_SANITY_SYS_CREATE_FILE("merge_single_src.txt");
    FOSSIL_SANITY_SYS_WRITE_FILE("merge_single_src.txt", "Single file content\n");
    
    // Merge single file
    const char *paths[] = {"merge_single_src.txt"};
    int result = fossil_shark_merge(paths, 1, "merge_single_dest.txt", false, false, false, "overwrite", false, false, cnull, cnull);
    ASSUME_ITS_EQUAL_I32(0, result);
    
    // Clean up
    FOSSIL_SANITY_SYS_DELETE_FILE("merge_single_src.txt");
    FOSSIL_SANITY_SYS_DELETE_FILE("merge_single_dest.txt");
}

FOSSIL_TEST(c_test_merge_multiple_files_overwrite) {
    // Create source files
    FOSSIL_SANITY_SYS_CREATE_FILE("merge_file1.txt");
    FOSSIL_SANITY_SYS_WRITE_FILE("merge_file1.txt", "Content from file 1\n");
    
    FOSSIL_SANITY_SYS_CREATE_FILE("merge_file2.txt");
    FOSSIL_SANITY_SYS_WRITE_FILE("merge_file2.txt", "Content from file 2\n");
    
    // Merge with overwrite strategy
    const char *paths[] = {"merge_file1.txt", "merge_file2.txt"};
    int result = fossil_shark_merge(paths, 2, "merge_output.txt", false, false, false, "overwrite", false, false, cnull, cnull);
    ASSUME_ITS_EQUAL_I32(0, result);
    
    // Verify output exists
    ASSUME_ITS_TRUE(FOSSIL_SANITY_SYS_FILE_EXISTS("merge_output.txt"));
    
    // Clean up
    FOSSIL_SANITY_SYS_DELETE_FILE("merge_file1.txt");
    FOSSIL_SANITY_SYS_DELETE_FILE("merge_file2.txt");
    FOSSIL_SANITY_SYS_DELETE_FILE("merge_output.txt");
}

FOSSIL_TEST(c_test_merge_directories_overwrite) {
    // Create source directories
    FOSSIL_SANITY_SYS_CREATE_DIR("merge_dir1");
    FOSSIL_SANITY_SYS_CREATE_DIR("merge_dir2");
    
    // Create files in directories
    FOSSIL_SANITY_SYS_CREATE_FILE("merge_dir1/file.txt");
    FOSSIL_SANITY_SYS_WRITE_FILE("merge_dir1/file.txt", "From dir1\n");
    
    FOSSIL_SANITY_SYS_CREATE_FILE("merge_dir2/file.txt");
    FOSSIL_SANITY_SYS_WRITE_FILE("merge_dir2/file.txt", "From dir2\n");
    
    // Merge directories
    const char *paths[] = {"merge_dir1", "merge_dir2"};
    int result = fossil_shark_merge(paths, 2, "merge_dir_dest", false, false, false, "overwrite", false, false, cnull, cnull);
    ASSUME_ITS_EQUAL_I32(0, result);
    
    // Clean up
    FOSSIL_SANITY_SYS_DELETE_FILE("merge_dir1/file.txt");
    FOSSIL_SANITY_SYS_DELETE_FILE("merge_dir2/file.txt");
    FOSSIL_SANITY_SYS_DELETE_FILE("merge_dir_dest/file.txt");
}

FOSSIL_TEST(c_test_merge_keep_both_strategy) {
    // Create source files with same name
    FOSSIL_SANITY_SYS_CREATE_FILE("keep_both_1.txt");
    FOSSIL_SANITY_SYS_WRITE_FILE("keep_both_1.txt", "Content 1\n");
    
    FOSSIL_SANITY_SYS_CREATE_FILE("keep_both_2.txt");
    FOSSIL_SANITY_SYS_WRITE_FILE("keep_both_2.txt", "Content 2\n");
    
    // Merge with keep-both strategy
    const char *paths[] = {"keep_both_1.txt", "keep_both_2.txt"};
    int result = fossil_shark_merge(paths, 2, "keep_both_dest", false, false, false, "keep-both", false, false, cnull, cnull);
    ASSUME_ITS_EQUAL_I32(0, result);
    
    // Clean up
    FOSSIL_SANITY_SYS_DELETE_FILE("keep_both_1.txt");
    FOSSIL_SANITY_SYS_DELETE_FILE("keep_both_2.txt");
}

FOSSIL_TEST(c_test_merge_skip_strategy) {
    // Create source file
    FOSSIL_SANITY_SYS_CREATE_FILE("skip_merge_src.txt");
    FOSSIL_SANITY_SYS_WRITE_FILE("skip_merge_src.txt", "Source content\n");
    
    // Create existing destination
    FOSSIL_SANITY_SYS_CREATE_FILE("skip_merge_dest.txt");
    FOSSIL_SANITY_SYS_WRITE_FILE("skip_merge_dest.txt", "Existing content\n");
    
    // Merge with skip strategy
    const char *paths[] = {"skip_merge_src.txt"};
    int result = fossil_shark_merge(paths, 1, "skip_merge_dest.txt", false, false, false, "skip", false, false, cnull, cnull);
    ASSUME_ITS_EQUAL_I32(0, result);
    
    // Clean up
    FOSSIL_SANITY_SYS_DELETE_FILE("skip_merge_src.txt");
    FOSSIL_SANITY_SYS_DELETE_FILE("skip_merge_dest.txt");
}

FOSSIL_TEST(c_test_merge_with_backup) {
    // Create source file
    FOSSIL_SANITY_SYS_CREATE_FILE("backup_merge_src.txt");
    FOSSIL_SANITY_SYS_WRITE_FILE("backup_merge_src.txt", "Source for backup\n");
    
    // Create existing destination
    FOSSIL_SANITY_SYS_CREATE_FILE("backup_merge_dest.txt");
    FOSSIL_SANITY_SYS_WRITE_FILE("backup_merge_dest.txt", "Original destination\n");
    
    // Merge with backup flag
    const char *paths[] = {"backup_merge_src.txt"};
    int result = fossil_shark_merge(paths, 1, "backup_merge_dest.txt", false, false, true, "overwrite", false, false, cnull, cnull);
    ASSUME_ITS_EQUAL_I32(0, result);
    
    // Clean up
    FOSSIL_SANITY_SYS_DELETE_FILE("backup_merge_src.txt");
    FOSSIL_SANITY_SYS_DELETE_FILE("backup_merge_dest.txt");
}

FOSSIL_TEST(c_test_merge_dry_run) {
    // Create source file
    FOSSIL_SANITY_SYS_CREATE_FILE("dry_run_merge_src.txt");
    FOSSIL_SANITY_SYS_WRITE_FILE("dry_run_merge_src.txt", "Dry run content\n");
    
    // Merge with dry_run flag
    const char *paths[] = {"dry_run_merge_src.txt"};
    int result = fossil_shark_merge(paths, 1, "dry_run_merge_dest.txt", false, false, false, "overwrite", false, true, cnull, cnull);
    ASSUME_ITS_EQUAL_I32(0, result);
    
    // Verify destination was not created (dry run should not create files)
    bool file_exists = FOSSIL_SANITY_SYS_FILE_EXISTS("dry_run_merge_dest.txt");
    ASSUME_ITS_FALSE(file_exists);
    
    // Clean up
    FOSSIL_SANITY_SYS_DELETE_FILE("dry_run_merge_src.txt");
    if (file_exists) {
        FOSSIL_SANITY_SYS_DELETE_FILE("dry_run_merge_dest.txt");
    }
}

FOSSIL_TEST(c_test_merge_with_exclude_pattern) {
    // Create source directory with multiple files
    FOSSIL_SANITY_SYS_CREATE_DIR("merge_exclude_src");
    
    FOSSIL_SANITY_SYS_CREATE_FILE("merge_exclude_src/include.txt");
    FOSSIL_SANITY_SYS_WRITE_FILE("merge_exclude_src/include.txt", "Include this\n");
    
    FOSSIL_SANITY_SYS_CREATE_FILE("merge_exclude_src/exclude.log");
    FOSSIL_SANITY_SYS_WRITE_FILE("merge_exclude_src/exclude.log", "Exclude this\n");
    
    // Merge with exclude pattern
    const char *paths[] = {"merge_exclude_src"};
    int result = fossil_shark_merge(paths, 1, "merge_exclude_dest", false, false, false, "overwrite", false, false, "*.log", cnull);
    ASSUME_ITS_EQUAL_I32(0, result);
    
    // Clean up
    FOSSIL_SANITY_SYS_DELETE_FILE("merge_exclude_src/include.txt");
    FOSSIL_SANITY_SYS_DELETE_FILE("merge_exclude_src/exclude.log");
    FOSSIL_SANITY_SYS_DELETE_FILE("merge_exclude_dest/include.txt");
}

FOSSIL_TEST(c_test_merge_with_include_pattern) {
    // Create source directory
    FOSSIL_SANITY_SYS_CREATE_DIR("merge_include_src");
    
    FOSSIL_SANITY_SYS_CREATE_FILE("merge_include_src/data.txt");
    FOSSIL_SANITY_SYS_WRITE_FILE("merge_include_src/data.txt", "Include text\n");
    
    FOSSIL_SANITY_SYS_CREATE_FILE("merge_include_src/data.bin");
    FOSSIL_SANITY_SYS_WRITE_FILE("merge_include_src/data.bin", "Binary content\n");
    
    // Merge with include pattern
    const char *paths[] = {"merge_include_src"};
    int result = fossil_shark_merge(paths, 1, "merge_include_dest", false, false, false, "overwrite", false, false, cnull, "*.txt");
    ASSUME_ITS_EQUAL_I32(0, result);
    
    // Clean up
    FOSSIL_SANITY_SYS_DELETE_FILE("merge_include_src/data.txt");
    FOSSIL_SANITY_SYS_DELETE_FILE("merge_include_src/data.bin");
    FOSSIL_SANITY_SYS_DELETE_FILE("merge_include_dest/data.txt");
}

FOSSIL_TEST(c_test_merge_nonexistent_source) {
    // Try to merge non-existent file - should fail
    const char *paths[] = {"nonexistent_merge_file_that_does_not_exist_12345.txt"};
    int result = fossil_shark_merge(paths, 1, "dest", false, false, false, "overwrite", false, false, cnull, cnull);
    ASSUME_NOT_EQUAL_I32(0, result);
}

FOSSIL_TEST(c_test_merge_invalid_strategy) {
    // Create source file
    FOSSIL_SANITY_SYS_CREATE_FILE("invalid_strategy_src.txt");
    FOSSIL_SANITY_SYS_WRITE_FILE("invalid_strategy_src.txt", "Content\n");
    
    // Merge with invalid strategy - should fail
    const char *paths[] = {"invalid_strategy_src.txt"};
    int result = fossil_shark_merge(paths, 1, "dest", false, false, false, "invalid_strategy_xyz", false, false, cnull, cnull);
    ASSUME_NOT_EQUAL_I32(0, result);
    
    // Clean up
    FOSSIL_SANITY_SYS_DELETE_FILE("invalid_strategy_src.txt");
}

FOSSIL_TEST(c_test_merge_three_files) {
    // Create three source files
    FOSSIL_SANITY_SYS_CREATE_FILE("merge_three_1.txt");
    FOSSIL_SANITY_SYS_WRITE_FILE("merge_three_1.txt", "File 1 content\n");
    
    FOSSIL_SANITY_SYS_CREATE_FILE("merge_three_2.txt");
    FOSSIL_SANITY_SYS_WRITE_FILE("merge_three_2.txt", "File 2 content\n");
    
    FOSSIL_SANITY_SYS_CREATE_FILE("merge_three_3.txt");
    FOSSIL_SANITY_SYS_WRITE_FILE("merge_three_3.txt", "File 3 content\n");
    
    // Merge three files
    const char *paths[] = {"merge_three_1.txt", "merge_three_2.txt", "merge_three_3.txt"};
    int result = fossil_shark_merge(paths, 3, "merge_three_dest.txt", false, false, false, "overwrite", false, false, cnull, cnull);
    ASSUME_ITS_EQUAL_I32(0, result);
    
    // Clean up
    FOSSIL_SANITY_SYS_DELETE_FILE("merge_three_1.txt");
    FOSSIL_SANITY_SYS_DELETE_FILE("merge_three_2.txt");
    FOSSIL_SANITY_SYS_DELETE_FILE("merge_three_3.txt");
    FOSSIL_SANITY_SYS_DELETE_FILE("merge_three_dest.txt");
}

FOSSIL_TEST(c_test_merge_empty_files) {
    // Create empty source files
    FOSSIL_SANITY_SYS_CREATE_FILE("merge_empty_1.txt");
    FOSSIL_SANITY_SYS_CREATE_FILE("merge_empty_2.txt");
    
    // Merge empty files
    const char *paths[] = {"merge_empty_1.txt", "merge_empty_2.txt"};
    int result = fossil_shark_merge(paths, 2, "merge_empty_dest.txt", false, false, false, "overwrite", false, false, cnull, cnull);
    ASSUME_ITS_EQUAL_I32(0, result);
    
    // Clean up
    FOSSIL_SANITY_SYS_DELETE_FILE("merge_empty_1.txt");
    FOSSIL_SANITY_SYS_DELETE_FILE("merge_empty_2.txt");
    FOSSIL_SANITY_SYS_DELETE_FILE("merge_empty_dest.txt");
}

FOSSIL_TEST(c_test_merge_force_overwrite) {
    // Create source file
    FOSSIL_SANITY_SYS_CREATE_FILE("force_merge_src.txt");
    FOSSIL_SANITY_SYS_WRITE_FILE("force_merge_src.txt", "New content\n");
    
    // Create existing destination
    FOSSIL_SANITY_SYS_CREATE_FILE("force_merge_dest.txt");
    FOSSIL_SANITY_SYS_WRITE_FILE("force_merge_dest.txt", "Old content\n");
    
    // Merge with force flag
    const char *paths[] = {"force_merge_src.txt"};
    int result = fossil_shark_merge(paths, 1, "force_merge_dest.txt", true, false, false, "overwrite", false, false, cnull, cnull);
    ASSUME_ITS_EQUAL_I32(0, result);
    
    // Clean up
    FOSSIL_SANITY_SYS_DELETE_FILE("force_merge_src.txt");
    FOSSIL_SANITY_SYS_DELETE_FILE("force_merge_dest.txt");
}

// * * * * * * * * * * * * * * * * * * * * * * * *
// * Fossil Logic Test Pool
// * * * * * * * * * * * * * * * * * * * * * * * *

FOSSIL_TEST_GROUP(c_merge_command_tests) {
    FOSSIL_TEST_ADD(c_merge_command_suite, c_test_merge_null_parameters);
    FOSSIL_TEST_ADD(c_merge_command_suite, c_test_merge_single_file);
    FOSSIL_TEST_ADD(c_merge_command_suite, c_test_merge_multiple_files_overwrite);
    FOSSIL_TEST_ADD(c_merge_command_suite, c_test_merge_directories_overwrite);
    FOSSIL_TEST_ADD(c_merge_command_suite, c_test_merge_keep_both_strategy);
    FOSSIL_TEST_ADD(c_merge_command_suite, c_test_merge_skip_strategy);
    FOSSIL_TEST_ADD(c_merge_command_suite, c_test_merge_with_backup);
    FOSSIL_TEST_ADD(c_merge_command_suite, c_test_merge_dry_run);
    FOSSIL_TEST_ADD(c_merge_command_suite, c_test_merge_with_exclude_pattern);
    FOSSIL_TEST_ADD(c_merge_command_suite, c_test_merge_with_include_pattern);
    FOSSIL_TEST_ADD(c_merge_command_suite, c_test_merge_nonexistent_source);
    FOSSIL_TEST_ADD(c_merge_command_suite, c_test_merge_invalid_strategy);
    FOSSIL_TEST_ADD(c_merge_command_suite, c_test_merge_three_files);
    FOSSIL_TEST_ADD(c_merge_command_suite, c_test_merge_empty_files);
    FOSSIL_TEST_ADD(c_merge_command_suite, c_test_merge_force_overwrite);

    FOSSIL_TEST_REGISTER(c_merge_command_suite);
}
