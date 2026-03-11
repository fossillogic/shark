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
FOSSIL_SUITE(c_split_command_suite);

// Setup function for the test suite
FOSSIL_SETUP(c_split_command_suite)
{
    // Setup code here
}

// Teardown function for the test suite
FOSSIL_TEARDOWN(c_split_command_suite)
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

FOSSIL_TEST(c_test_split_null_parameters)
{
    int result = fossil_shark_split(cnull, 10, 0, 0, "output", 0, false, cnull, false);
    ASSUME_NOT_EQUAL_I32(0, result);

    result = fossil_shark_split("test.txt", 10, 0, 0, cnull, 0, false, cnull, false);
    ASSUME_NOT_EQUAL_I32(0, result);
}

FOSSIL_TEST(c_test_split_by_lines)
{
    FILE *file = fopen("split_lines.txt", "w");
    ASSUME_NOT_CNULL(file);
    for (int i = 0; i < 100; i++)
    {
        fprintf(file, "Line %d\n", i);
    }
    fclose(file);

    int result = fossil_shark_split("split_lines.txt", 25, 0, 0, "split_lines_", 0, false, cnull, false);
    ASSUME_ITS_EQUAL_I32(0, result);

    FOSSIL_SANITY_SYS_DELETE_FILE("split_lines.txt");
    FOSSIL_SANITY_SYS_DELETE_FILE("split_lines_a");
    FOSSIL_SANITY_SYS_DELETE_FILE("split_lines_b");
    FOSSIL_SANITY_SYS_DELETE_FILE("split_lines_c");
    FOSSIL_SANITY_SYS_DELETE_FILE("split_lines_d");
}

FOSSIL_TEST(c_test_split_by_bytes)
{
    FILE *file = fopen("split_bytes.txt", "w");
    ASSUME_NOT_CNULL(file);
    fprintf(file, "0123456789");
    fclose(file);

    int result = fossil_shark_split("split_bytes.txt", 0, 5, 0, "split_bytes_", 0, false, cnull, false);
    ASSUME_ITS_EQUAL_I32(0, result);

    FOSSIL_SANITY_SYS_DELETE_FILE("split_bytes.txt");
    FOSSIL_SANITY_SYS_DELETE_FILE("split_bytes_a");
    FOSSIL_SANITY_SYS_DELETE_FILE("split_bytes_b");
}

FOSSIL_TEST(c_test_split_by_segments)
{
    FILE *file = fopen("split_segs.txt", "w");
    ASSUME_NOT_CNULL(file);
    fprintf(file, "Content for splitting into segments");
    fclose(file);

    int result = fossil_shark_split("split_segs.txt", 0, 0, 3, "split_segs_", 0, false, cnull, false);
    ASSUME_ITS_EQUAL_I32(0, result);

    FOSSIL_SANITY_SYS_DELETE_FILE("split_segs.txt");
    FOSSIL_SANITY_SYS_DELETE_FILE("split_segs_a");
    FOSSIL_SANITY_SYS_DELETE_FILE("split_segs_b");
    FOSSIL_SANITY_SYS_DELETE_FILE("split_segs_c");
}

FOSSIL_TEST(c_test_split_numeric_suffix)
{
    FILE *file = fopen("split_num.txt", "w");
    ASSUME_NOT_CNULL(file);
    for (int i = 0; i < 50; i++)
    {
        fprintf(file, "Line %d\n", i);
    }
    fclose(file);

    int result = fossil_shark_split("split_num.txt", 25, 0, 0, "split_num_", 0, true, cnull, false);
    ASSUME_ITS_EQUAL_I32(0, result);

    FOSSIL_SANITY_SYS_DELETE_FILE("split_num.txt");
    FOSSIL_SANITY_SYS_DELETE_FILE("split_num_0");
    FOSSIL_SANITY_SYS_DELETE_FILE("split_num_1");
}

FOSSIL_TEST(c_test_split_with_suffix_digits)
{
    FILE *file = fopen("split_digits.txt", "w");
    ASSUME_NOT_CNULL(file);
    for (int i = 0; i < 75; i++)
    {
        fprintf(file, "Line %d\n", i);
    }
    fclose(file);

    int result = fossil_shark_split("split_digits.txt", 25, 0, 0, "split_digits_", 3, false, cnull, false);
    ASSUME_ITS_EQUAL_I32(0, result);

    FOSSIL_SANITY_SYS_DELETE_FILE("split_digits.txt");
    FOSSIL_SANITY_SYS_DELETE_FILE("split_digits_000");
    FOSSIL_SANITY_SYS_DELETE_FILE("split_digits_001");
    FOSSIL_SANITY_SYS_DELETE_FILE("split_digits_002");
}

FOSSIL_TEST(c_test_split_with_delimiter)
{
    FILE *file = fopen("split_delim.txt", "w");
    ASSUME_NOT_CNULL(file);
    fprintf(file, "data|data|data|data");
    fclose(file);

    int result = fossil_shark_split("split_delim.txt", 0, 0, 0, "split_delim_", 0, false, "|", false);
    ASSUME_ITS_EQUAL_I32(0, result);

    FOSSIL_SANITY_SYS_DELETE_FILE("split_delim.txt");
    FOSSIL_SANITY_SYS_DELETE_FILE("split_delim_a");
    FOSSIL_SANITY_SYS_DELETE_FILE("split_delim_b");
}

FOSSIL_TEST(c_test_split_dry_run)
{
    FILE *file = fopen("split_dry.txt", "w");
    ASSUME_NOT_CNULL(file);
    for (int i = 0; i < 50; i++)
    {
        fprintf(file, "Line %d\n", i);
    }
    fclose(file);

    int result = fossil_shark_split("split_dry.txt", 25, 0, 0, "split_dry_", 0, false, cnull, true);
    ASSUME_ITS_EQUAL_I32(0, result);

    ASSUME_ITS_FALSE(FOSSIL_SANITY_SYS_FILE_EXISTS("split_dry_a"));

    FOSSIL_SANITY_SYS_DELETE_FILE("split_dry.txt");
}

FOSSIL_TEST(c_test_split_nonexistent_file)
{
    int result = fossil_shark_split("nonexistent_split.txt", 10, 0, 0, "output_", 0, false, cnull, false);
    ASSUME_NOT_EQUAL_I32(0, result);
}

FOSSIL_TEST(c_test_split_empty_file)
{
    FILE *file = fopen("split_empty.txt", "w");
    ASSUME_NOT_CNULL(file);
    fclose(file);

    int result = fossil_shark_split("split_empty.txt", 10, 0, 0, "split_empty_", 0, false, cnull, false);
    ASSUME_ITS_EQUAL_I32(0, result);

    FOSSIL_SANITY_SYS_DELETE_FILE("split_empty.txt");
}

FOSSIL_TEST(c_test_split_large_file)
{
    FILE *file = fopen("split_large.txt", "w");
    ASSUME_NOT_CNULL(file);
    for (long i = 0; i < 10000; i++)
    {
        fprintf(file, "Line number %ld with some content\n", i);
    }
    fclose(file);

    int result = fossil_shark_split("split_large.txt", 2500, 0, 0, "split_large_", 0, false, cnull, false);
    ASSUME_ITS_EQUAL_I32(0, result);

    FOSSIL_SANITY_SYS_DELETE_FILE("split_large.txt");
    FOSSIL_SANITY_SYS_DELETE_FILE("split_large_a");
    FOSSIL_SANITY_SYS_DELETE_FILE("split_large_b");
    FOSSIL_SANITY_SYS_DELETE_FILE("split_large_c");
    FOSSIL_SANITY_SYS_DELETE_FILE("split_large_d");
}

FOSSIL_TEST(c_test_split_single_line)
{
    FILE *file = fopen("split_single.txt", "w");
    ASSUME_NOT_CNULL(file);
    fprintf(file, "Single line content");
    fclose(file);

    int result = fossil_shark_split("split_single.txt", 1, 0, 0, "split_single_", 0, false, cnull, false);
    ASSUME_ITS_EQUAL_I32(0, result);

    FOSSIL_SANITY_SYS_DELETE_FILE("split_single.txt");
    FOSSIL_SANITY_SYS_DELETE_FILE("split_single_a");
}

FOSSIL_TEST(c_test_split_zero_split_param)
{
    FILE *file = fopen("split_zero.txt", "w");
    ASSUME_NOT_CNULL(file);
    fprintf(file, "Content");
    fclose(file);

    int result = fossil_shark_split("split_zero.txt", 0, 0, 0, "split_zero_", 0, false, cnull, false);
    ASSUME_NOT_EQUAL_I32(0, result);

    FOSSIL_SANITY_SYS_DELETE_FILE("split_zero.txt");
}

FOSSIL_TEST(c_test_split_alphabetic_suffix)
{
    FILE *file = fopen("split_alpha.txt", "w");
    ASSUME_NOT_CNULL(file);
    for (int i = 0; i < 100; i++)
    {
        fprintf(file, "Line %d\n", i);
    }
    fclose(file);

    int result = fossil_shark_split("split_alpha.txt", 50, 0, 0, "split_alpha_", 0, false, cnull, false);
    ASSUME_ITS_EQUAL_I32(0, result);

    FOSSIL_SANITY_SYS_DELETE_FILE("split_alpha.txt");
    FOSSIL_SANITY_SYS_DELETE_FILE("split_alpha_a");
    FOSSIL_SANITY_SYS_DELETE_FILE("split_alpha_b");
}

// * * * * * * * * * * * * * * * * * * * * * * * *
// * Fossil Logic Test Pool
// * * * * * * * * * * * * * * * * * * * * * * * *

FOSSIL_TEST_GROUP(c_split_command_tests)
{
    FOSSIL_TEST_ADD(c_split_command_suite, c_test_split_null_parameters);
    FOSSIL_TEST_ADD(c_split_command_suite, c_test_split_by_lines);
    FOSSIL_TEST_ADD(c_split_command_suite, c_test_split_by_bytes);
    FOSSIL_TEST_ADD(c_split_command_suite, c_test_split_by_segments);
    FOSSIL_TEST_ADD(c_split_command_suite, c_test_split_numeric_suffix);
    FOSSIL_TEST_ADD(c_split_command_suite, c_test_split_with_suffix_digits);
    FOSSIL_TEST_ADD(c_split_command_suite, c_test_split_with_delimiter);
    FOSSIL_TEST_ADD(c_split_command_suite, c_test_split_dry_run);
    FOSSIL_TEST_ADD(c_split_command_suite, c_test_split_nonexistent_file);
    FOSSIL_TEST_ADD(c_split_command_suite, c_test_split_empty_file);
    FOSSIL_TEST_ADD(c_split_command_suite, c_test_split_large_file);
    FOSSIL_TEST_ADD(c_split_command_suite, c_test_split_single_line);
    FOSSIL_TEST_ADD(c_split_command_suite, c_test_split_zero_split_param);
    FOSSIL_TEST_ADD(c_split_command_suite, c_test_split_alphabetic_suffix);

    FOSSIL_TEST_REGISTER(c_split_command_suite);
}
