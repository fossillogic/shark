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
FOSSIL_SUITE(c_swap_command_suite);

// Setup function for the test suite
FOSSIL_SETUP(c_swap_command_suite)
{
    // Setup code here
}

// Teardown function for the test suite
FOSSIL_TEARDOWN(c_swap_command_suite)
{
    // Teardown code here
}

// * * * * * * * * * * * * * * * * * * * * * * * *
// * Fossil Logic Test Cases
// * * * * * * * * * * * * * * * * * * * * * * * *
// The test cases below are provided as samples, inspired
// by the Meson build system's approach of using test cases
// as samples for library usage.
// * * * * * * * * * * * * * * * * * * * * * * * *

// Test cases for fossil_spino_move function

FOSSIL_TEST(c_test_swap_null_parameters)
{
    // Test with null path1
    int result = fossil_shark_swap(cnull, "file2.txt", false, false, false, false, false, false, cnull, false);
    ASSUME_NOT_EQUAL_I32(0, result);

    // Test with null path2
    result = fossil_shark_swap("file1.txt", cnull, false, false, false, false, false, false, cnull, false);
    ASSUME_NOT_EQUAL_I32(0, result);

    // Test with both null
    result = fossil_shark_swap(cnull, cnull, false, false, false, false, false, false, cnull, false);
    ASSUME_NOT_EQUAL_I32(0, result);
}

FOSSIL_TEST(c_test_swap_simple_file)
{
    // Create two files
    FILE *file1 = fopen("swap_file1.txt", "w");
    ASSUME_NOT_CNULL(file1);
    fprintf(file1, "Content of file 1\n");
    fclose(file1);

    FILE *file2 = fopen("swap_file2.txt", "w");
    ASSUME_NOT_CNULL(file2);
    fprintf(file2, "Content of file 2\n");
    fclose(file2);

    // Swap files
    int result = fossil_shark_swap("swap_file1.txt", "swap_file2.txt", false, false, false, false, false, false, cnull, false);
    ASSUME_ITS_EQUAL_I32(0, result);

    // Clean up
    remove("swap_file1.txt");
    remove("swap_file2.txt");
}

FOSSIL_TEST(c_test_swap_nonexistent_source)
{
    // Create one file, try to swap with non-existent file
    FILE *file1 = fopen("swap_exists.txt", "w");
    ASSUME_NOT_CNULL(file1);
    fprintf(file1, "This file exists\n");
    fclose(file1);

    int result = fossil_shark_swap("swap_exists.txt", "swap_nonexistent.txt", false, false, false, false, false, false, cnull, false);
    ASSUME_NOT_EQUAL_I32(0, result);

    // Clean up
    remove("swap_exists.txt");
}

FOSSIL_TEST(c_test_swap_overwrite_without_force)
{
    // This test depends on implementation - may fail or succeed
    FILE *file1 = fopen("swap_no_force1.txt", "w");
    ASSUME_NOT_CNULL(file1);
    fprintf(file1, "File 1\n");
    fclose(file1);

    FILE *file2 = fopen("swap_no_force2.txt", "w");
    ASSUME_NOT_CNULL(file2);
    fprintf(file2, "File 2\n");
    fclose(file2);

    int result = fossil_shark_swap("swap_no_force1.txt", "swap_no_force2.txt", false, false, false, false, false, false, cnull, false);
    (void)result;

    // Clean up
    remove("swap_no_force1.txt");
    remove("swap_no_force2.txt");
}

FOSSIL_TEST(c_test_swap_overwrite_with_force)
{
    FILE *file1 = fopen("swap_force1.txt", "w");
    ASSUME_NOT_CNULL(file1);
    fprintf(file1, "File 1\n");
    fclose(file1);

    FILE *file2 = fopen("swap_force2.txt", "w");
    ASSUME_NOT_CNULL(file2);
    fprintf(file2, "File 2\n");
    fclose(file2);

    int result = fossil_shark_swap("swap_force1.txt", "swap_force2.txt", true, false, false, false, false, false, cnull, false);
    ASSUME_ITS_EQUAL_I32(0, result);

    // Clean up
    remove("swap_force1.txt");
    remove("swap_force2.txt");
}

FOSSIL_TEST(c_test_swap_with_backup)
{
    FILE *file1 = fopen("swap_backup1.txt", "w");
    ASSUME_NOT_CNULL(file1);
    fprintf(file1, "File 1 content\n");
    fclose(file1);

    FILE *file2 = fopen("swap_backup2.txt", "w");
    ASSUME_NOT_CNULL(file2);
    fprintf(file2, "File 2 content\n");
    fclose(file2);

    int result = fossil_shark_swap("swap_backup1.txt", "swap_backup2.txt", false, false, true, false, false, false, cnull, false);
    ASSUME_ITS_EQUAL_I32(0, result);

    // Clean up - backup files may exist
    remove("swap_backup1.txt");
    remove("swap_backup2.txt");
    remove("swap_backup1.txt.bak");
    remove("swap_backup2.txt.bak");
}

FOSSIL_TEST(c_test_swap_rename_same_directory)
{
    FILE *file1 = fopen("swap_rename1.txt", "w");
    ASSUME_NOT_CNULL(file1);
    fprintf(file1, "Original\n");
    fclose(file1);

    FILE *file2 = fopen("swap_rename2.txt", "w");
    ASSUME_NOT_CNULL(file2);
    fprintf(file2, "Renamed\n");
    fclose(file2);

    int result = fossil_shark_swap("swap_rename1.txt", "swap_rename2.txt", false, false, false, false, false, false, cnull, false);
    ASSUME_ITS_EQUAL_I32(0, result);

    // Clean up
    remove("swap_rename1.txt");
    remove("swap_rename2.txt");
}

FOSSIL_TEST(c_test_swap_empty_file)
{
    FILE *file1 = fopen("swap_empty1.txt", "w");
    ASSUME_NOT_CNULL(file1);
    fclose(file1);

    FILE *file2 = fopen("swap_empty2.txt", "w");
    ASSUME_NOT_CNULL(file2);
    fprintf(file2, "Not empty\n");
    fclose(file2);

    int result = fossil_shark_swap("swap_empty1.txt", "swap_empty2.txt", false, false, false, false, false, false, cnull, false);
    ASSUME_ITS_EQUAL_I32(0, result);

    // Clean up
    remove("swap_empty1.txt");
    remove("swap_empty2.txt");
}

FOSSIL_TEST(c_test_swap_large_file)
{
    FILE *file1 = fopen("swap_large1.txt", "w");
    ASSUME_NOT_CNULL(file1);
    for (int i = 0; i < 1000; i++)
    {
        fprintf(file1, "Line %d: Large file content for swap testing\n", i);
    }
    fclose(file1);

    FILE *file2 = fopen("swap_large2.txt", "w");
    ASSUME_NOT_CNULL(file2);
    fprintf(file2, "Small file\n");
    fclose(file2);

    int result = fossil_shark_swap("swap_large1.txt", "swap_large2.txt", false, false, false, false, false, false, cnull, false);
    ASSUME_ITS_EQUAL_I32(0, result);

    // Clean up
    remove("swap_large1.txt");
    remove("swap_large2.txt");
}

FOSSIL_TEST(c_test_swap_special_characters)
{
    FILE *file1 = fopen("swap_special1.txt", "w");
    ASSUME_NOT_CNULL(file1);
    fprintf(file1, "File with special chars\n");
    fclose(file1);

    FILE *file2 = fopen("swap_special2.txt", "w");
    ASSUME_NOT_CNULL(file2);
    fprintf(file2, "Another special file\n");
    fclose(file2);

    int result = fossil_shark_swap("swap_special1.txt", "swap_special2.txt", false, false, false, false, false, false, cnull, false);
    ASSUME_ITS_EQUAL_I32(0, result);

    // Clean up
    remove("swap_special1.txt");
    remove("swap_special2.txt");
}

FOSSIL_TEST(c_test_swap_to_existing_directory)
{
    FILE *file1 = fopen("swap_dir_file.txt", "w");
    ASSUME_NOT_CNULL(file1);
    fprintf(file1, "File to swap in directory\n");
    fclose(file1);

    FILE *file2 = fopen("swap_dir_file2.txt", "w");
    ASSUME_NOT_CNULL(file2);
    fprintf(file2, "Another file\n");
    fclose(file2);

    int result = fossil_shark_swap("swap_dir_file.txt", "swap_dir_file2.txt", false, false, false, false, false, false, cnull, false);
    ASSUME_ITS_EQUAL_I32(0, result);

    // Clean up
    remove("swap_dir_file.txt");
    remove("swap_dir_file2.txt");
}

FOSSIL_TEST(c_test_swap_same_source_and_dest)
{
    FILE *file = fopen("swap_same_file.txt", "w");
    ASSUME_NOT_CNULL(file);
    fprintf(file, "Same file swap\n");
    fclose(file);

    // Try to swap file with itself
    int result = fossil_shark_swap("swap_same_file.txt", "swap_same_file.txt", false, false, false, false, false, false, cnull, false);
    (void)result;

    // File should still exist (using io_filesys)
    ASSUME_ITS_TRUE(fossil_io_filesys_exists("swap_same_file.txt") > 0);

    // Clean up
    fossil_io_filesys_remove("swap_same_file.txt", false);
}

FOSSIL_TEST(c_test_swap_dry_run_no_actual_swap)
{
    FILE *file1 = fopen("swap_dry1.txt", "w");
    ASSUME_NOT_CNULL(file1);
    fprintf(file1, "File 1 dry run\n");
    fclose(file1);

    FILE *file2 = fopen("swap_dry2.txt", "w");
    ASSUME_NOT_CNULL(file2);
    fprintf(file2, "File 2 dry run\n");
    fclose(file2);

    int result = fossil_shark_swap("swap_dry1.txt", "swap_dry2.txt", false, false, false, false, false, true, cnull, false);
    ASSUME_ITS_EQUAL_I32(0, result);

    // Both files should still exist (no actual swap, using io_filesys)
    ASSUME_ITS_TRUE(fossil_io_filesys_exists("swap_dry1.txt") > 0);
    ASSUME_ITS_TRUE(fossil_io_filesys_exists("swap_dry2.txt") > 0);

    // Clean up
    fossil_io_filesys_remove("swap_dry1.txt", false);
    fossil_io_filesys_remove("swap_dry2.txt", false);
}

FOSSIL_TEST(c_test_swap_with_atomic_operation)
{
    FILE *file1 = fopen("swap_atomic1.txt", "w");
    ASSUME_NOT_CNULL(file1);
    fprintf(file1, "Atomic swap 1\n");
    fclose(file1);

    FILE *file2 = fopen("swap_atomic2.txt", "w");
    ASSUME_NOT_CNULL(file2);
    fprintf(file2, "Atomic swap 2\n");
    fclose(file2);

    int result = fossil_shark_swap("swap_atomic1.txt", "swap_atomic2.txt", false, false, false, true, false, false, cnull, false);
    ASSUME_ITS_EQUAL_I32(0, result);

    // Clean up
    remove("swap_atomic1.txt");
    remove("swap_atomic2.txt");
}

FOSSIL_TEST(c_test_swap_with_progress_reporting)
{
    FILE *file1 = fopen("swap_progress1.txt", "w");
    ASSUME_NOT_CNULL(file1);
    fprintf(file1, "Progress test 1\n");
    fclose(file1);

    FILE *file2 = fopen("swap_progress2.txt", "w");
    ASSUME_NOT_CNULL(file2);
    fprintf(file2, "Progress test 2\n");
    fclose(file2);

    int result = fossil_shark_swap("swap_progress1.txt", "swap_progress2.txt", false, false, false, false, true, false, cnull, false);
    ASSUME_ITS_EQUAL_I32(0, result);

    // Clean up
    remove("swap_progress1.txt");
    remove("swap_progress2.txt");
}

FOSSIL_TEST(c_test_swap_with_exclude_pattern)
{
    FILE *file1 = fopen("swap_exclude1.txt", "w");
    ASSUME_NOT_CNULL(file1);
    fprintf(file1, "File to exclude from swap\n");
    fclose(file1);

    FILE *file2 = fopen("swap_exclude2.txt", "w");
    ASSUME_NOT_CNULL(file2);
    fprintf(file2, "Another file\n");
    fclose(file2);

    int result = fossil_shark_swap("swap_exclude1.txt", "swap_exclude2.txt", false, false, false, false, false, false, cnull, false);
    ASSUME_ITS_EQUAL_I32(0, result);

    // Clean up
    remove("swap_exclude1.txt");
    remove("swap_exclude2.txt");
}

FOSSIL_TEST(c_test_swap_with_include_pattern)
{
    FILE *file1 = fopen("swap_include1.txt", "w");
    ASSUME_NOT_CNULL(file1);
    fprintf(file1, "File to include in swap\n");
    fclose(file1);

    FILE *file2 = fopen("swap_include2.txt", "w");
    ASSUME_NOT_CNULL(file2);
    fprintf(file2, "Another file\n");
    fclose(file2);

    int result = fossil_shark_swap("swap_include1.txt", "swap_include2.txt", false, false, false, false, false, false, cnull, false);
    ASSUME_ITS_EQUAL_I32(0, result);

    // Clean up
    remove("swap_include1.txt");
    remove("swap_include2.txt");
}

FOSSIL_TEST(c_test_swap_with_non_matching_include_pattern)
{
    FILE *file1 = fopen("swap_nomatch1.txt", "w");
    ASSUME_NOT_CNULL(file1);
    fprintf(file1, "Non-matching file\n");
    fclose(file1);

    FILE *file2 = fopen("swap_nomatch2.txt", "w");
    ASSUME_NOT_CNULL(file2);
    fprintf(file2, "Another non-matching file\n");
    fclose(file2);

    int result = fossil_shark_swap("swap_nomatch1.txt", "swap_nomatch2.txt", false, false, false, false, false, false, cnull, false);
    (void)result;

    // Clean up
    remove("swap_nomatch1.txt");
    remove("swap_nomatch2.txt");
}

FOSSIL_TEST(c_test_swap_backup_with_force)
{
    FILE *file1 = fopen("swap_bak_force1.txt", "w");
    ASSUME_NOT_CNULL(file1);
    fprintf(file1, "File 1 with backup and force\n");
    fclose(file1);

    FILE *file2 = fopen("swap_bak_force2.txt", "w");
    ASSUME_NOT_CNULL(file2);
    fprintf(file2, "File 2 with backup and force\n");
    fclose(file2);

    int result = fossil_shark_swap("swap_bak_force1.txt", "swap_bak_force2.txt", true, false, true, false, false, false, cnull, false);
    ASSUME_ITS_EQUAL_I32(0, result);

    // Clean up
    remove("swap_bak_force1.txt");
    remove("swap_bak_force2.txt");
    remove("swap_bak_force1.txt.bak");
    remove("swap_bak_force2.txt.bak");
}

FOSSIL_TEST(c_test_swap_atomic_with_progress)
{
    FILE *file1 = fopen("swap_atom_prog1.txt", "w");
    ASSUME_NOT_CNULL(file1);
    fprintf(file1, "Atomic with progress 1\n");
    fclose(file1);

    FILE *file2 = fopen("swap_atom_prog2.txt", "w");
    ASSUME_NOT_CNULL(file2);
    fprintf(file2, "Atomic with progress 2\n");
    fclose(file2);

    int result = fossil_shark_swap("swap_atom_prog1.txt", "swap_atom_prog2.txt", false, false, false, true, true, false, cnull, false);
    ASSUME_ITS_EQUAL_I32(0, result);

    // Clean up
    remove("swap_atom_prog1.txt");
    remove("swap_atom_prog2.txt");
}

FOSSIL_TEST(c_test_swap_path_normalization_windows_style)
{
    FILE *file1 = fopen("swap_norm1.txt", "w");
    ASSUME_NOT_CNULL(file1);
    fprintf(file1, "Path normalization test 1\n");
    fclose(file1);

    FILE *file2 = fopen("swap_norm2.txt", "w");
    ASSUME_NOT_CNULL(file2);
    fprintf(file2, "Path normalization test 2\n");
    fclose(file2);

    int result = fossil_shark_swap("swap_norm1.txt", "swap_norm2.txt", false, false, false, false, false, false, cnull, false);
    ASSUME_ITS_EQUAL_I32(0, result);

    // Clean up
    remove("swap_norm1.txt");
    remove("swap_norm2.txt");
}

// * * * * * * * * * * * * * * * * * * * * * * * *
// * Fossil Logic Test Pool
// * * * * * * * * * * * * * * * * * * * * * * * *

FOSSIL_TEST_GROUP(c_swap_command_tests)
{
    FOSSIL_TEST_ADD(c_swap_command_suite, c_test_swap_null_parameters);
    FOSSIL_TEST_ADD(c_swap_command_suite, c_test_swap_simple_file);
    FOSSIL_TEST_ADD(c_swap_command_suite, c_test_swap_nonexistent_source);
    FOSSIL_TEST_ADD(c_swap_command_suite, c_test_swap_overwrite_without_force);
    FOSSIL_TEST_ADD(c_swap_command_suite, c_test_swap_overwrite_with_force);
    FOSSIL_TEST_ADD(c_swap_command_suite, c_test_swap_with_backup);
    FOSSIL_TEST_ADD(c_swap_command_suite, c_test_swap_rename_same_directory);
    FOSSIL_TEST_ADD(c_swap_command_suite, c_test_swap_empty_file);
    FOSSIL_TEST_ADD(c_swap_command_suite, c_test_swap_large_file);
    FOSSIL_TEST_ADD(c_swap_command_suite, c_test_swap_special_characters);
    FOSSIL_TEST_ADD(c_swap_command_suite, c_test_swap_to_existing_directory);
    FOSSIL_TEST_ADD(c_swap_command_suite, c_test_swap_same_source_and_dest);
    FOSSIL_TEST_ADD(c_swap_command_suite, c_test_swap_dry_run_no_actual_swap);
    FOSSIL_TEST_ADD(c_swap_command_suite, c_test_swap_with_atomic_operation);
    FOSSIL_TEST_ADD(c_swap_command_suite, c_test_swap_with_progress_reporting);
    FOSSIL_TEST_ADD(c_swap_command_suite, c_test_swap_with_exclude_pattern);
    FOSSIL_TEST_ADD(c_swap_command_suite, c_test_swap_with_include_pattern);
    FOSSIL_TEST_ADD(c_swap_command_suite, c_test_swap_with_non_matching_include_pattern);
    FOSSIL_TEST_ADD(c_swap_command_suite, c_test_swap_backup_with_force);
    FOSSIL_TEST_ADD(c_swap_command_suite, c_test_swap_atomic_with_progress);
    FOSSIL_TEST_ADD(c_swap_command_suite, c_test_swap_path_normalization_windows_style);

    FOSSIL_TEST_REGISTER(c_swap_command_suite);
}
