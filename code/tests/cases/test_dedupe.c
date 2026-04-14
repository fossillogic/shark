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
// * Fossil Logic Dedupe Test Suite
// * * * * * * * * * * * * * * * * * * * * * * * *

FOSSIL_SUITE(c_dedupe_command_suite);

FOSSIL_SETUP(c_dedupe_command_suite)
{
    // Setup test environment
}

FOSSIL_TEARDOWN(c_dedupe_command_suite)
{
    // Cleanup after tests
}

// Helper: create file with content
static void create_file(const char* path, const char* content)
{
    FILE* f = fopen(path, "w");
    ASSUME_NOT_CNULL(f);
    fprintf(f, "%s", content);
    fclose(f);
}

// * * * * * * * * * * * * * * * * * * * * * * * *
// * Test Cases
// * * * * * * * * * * * * * * * * * * * * * * * *

FOSSIL_TEST(c_test_dedupe_null_parameters)
{
    int result = fossil_shark_dedupe(cnull, true, false, false, false, "text");
    ASSUME_NOT_EQUAL_I32(0, result);

    result = fossil_shark_dedupe("testdir", true, false, false, false, cnull);
    ASSUME_NOT_EQUAL_I32(0, result);
}

FOSSIL_TEST(c_test_dedupe_empty_directory)
{
    mkdir("empty_dedupe_dir", 0700);

    int result = fossil_shark_dedupe("empty_dedupe_dir", true, false, false, false, "text");
    ASSUME_ITS_EQUAL_I32(0, result);

    rmdir("empty_dedupe_dir");
}

FOSSIL_TEST(c_test_dedupe_no_duplicates)
{
    mkdir("nodupe_dir", 0700);

    create_file("nodupe_dir/file1.txt", "alpha");
    create_file("nodupe_dir/file2.txt", "beta");
    create_file("nodupe_dir/file3.txt", "gamma");

    int result = fossil_shark_dedupe("nodupe_dir", true, false, false, false, "text");
    ASSUME_ITS_EQUAL_I32(0, result);

    remove("nodupe_dir/file1.txt");
    remove("nodupe_dir/file2.txt");
    remove("nodupe_dir/file3.txt");
    rmdir("nodupe_dir");
}

FOSSIL_TEST(c_test_dedupe_detect_duplicates_by_hash)
{
    mkdir("hash_dupe_dir", 0700);

    create_file("hash_dupe_dir/a.txt", "SAME_CONTENT");
    create_file("hash_dupe_dir/b.txt", "SAME_CONTENT"); // duplicate
    create_file("hash_dupe_dir/c.txt", "DIFFERENT");

    int result = fossil_shark_dedupe("hash_dupe_dir", true, false, false, false, "text");
    ASSUME_ITS_EQUAL_I32(0, result);

    remove("hash_dupe_dir/a.txt");
    remove("hash_dupe_dir/b.txt");
    remove("hash_dupe_dir/c.txt");
    rmdir("hash_dupe_dir");
}

FOSSIL_TEST(c_test_dedupe_detect_duplicates_by_size)
{
    mkdir("size_dupe_dir", 0700);

    create_file("size_dupe_dir/x.txt", "12345");
    create_file("size_dupe_dir/y.txt", "12345"); // same size
    create_file("size_dupe_dir/z.txt", "999");

    int result = fossil_shark_dedupe("size_dupe_dir", false, false, false, false, "text");
    ASSUME_ITS_EQUAL_I32(0, result);

    remove("size_dupe_dir/x.txt");
    remove("size_dupe_dir/y.txt");
    remove("size_dupe_dir/z.txt");
    rmdir("size_dupe_dir");
}

FOSSIL_TEST(c_test_dedupe_delete_duplicates)
{
    mkdir("delete_dupe_dir", 0700);

    create_file("delete_dupe_dir/a.txt", "DUPLICATE");
    create_file("delete_dupe_dir/b.txt", "DUPLICATE");

    int result = fossil_shark_dedupe("delete_dupe_dir", true, false, true, false, "text");
    ASSUME_ITS_EQUAL_I32(0, result);

    // One file should remain
    ASSUME_ITS_TRUE(
        fossil_io_filesys_exists("delete_dupe_dir/a.txt") ||
        fossil_io_filesys_exists("delete_dupe_dir/b.txt")
    );

    rmdir("delete_dupe_dir");
}

FOSSIL_TEST(c_test_dedupe_link_duplicates)
{
    mkdir("link_dupe_dir", 0700);

    create_file("link_dupe_dir/a.txt", "LINKME");
    create_file("link_dupe_dir/b.txt", "LINKME");

    int result = fossil_shark_dedupe("link_dupe_dir", true, false, false, true, "text");
    ASSUME_ITS_EQUAL_I32(0, result);

    // Expect link behavior (implementation dependent check)
    ASSUME_ITS_TRUE(fossil_io_filesys_exists("link_dupe_dir/a.txt"));

    rmdir("link_dupe_dir");
}

FOSSIL_TEST(c_test_dedupe_interactive_mode)
{
    mkdir("interactive_dupe_dir", 0700);

    create_file("interactive_dupe_dir/a.txt", "ASKME");
    create_file("interactive_dupe_dir/b.txt", "ASKME");

    int result = fossil_shark_dedupe("interactive_dupe_dir", true, true, true, false, "text");
    ASSUME_ITS_EQUAL_I32(0, result);

    remove("interactive_dupe_dir/a.txt");
    remove("interactive_dupe_dir/b.txt");
    rmdir("interactive_dupe_dir");
}

FOSSIL_TEST(c_test_dedupe_json_output)
{
    mkdir("json_dupe_dir", 0700);

    create_file("json_dupe_dir/a.txt", "JSONDATA");
    create_file("json_dupe_dir/b.txt", "JSONDATA");

    int result = fossil_shark_dedupe("json_dupe_dir", true, false, false, false, "json");
    ASSUME_ITS_EQUAL_I32(0, result);

    remove("json_dupe_dir/a.txt");
    remove("json_dupe_dir/b.txt");
    rmdir("json_dupe_dir");
}

FOSSIL_TEST(c_test_dedupe_fson_output)
{
    mkdir("fson_dupe_dir", 0700);

    create_file("fson_dupe_dir/a.txt", "FSONDATA");
    create_file("fson_dupe_dir/b.txt", "FSONDATA");

    int result = fossil_shark_dedupe("fson_dupe_dir", true, false, false, false, "fson");
    ASSUME_ITS_EQUAL_I32(0, result);

    remove("fson_dupe_dir/a.txt");
    remove("fson_dupe_dir/b.txt");
    rmdir("fson_dupe_dir");
}

FOSSIL_TEST(c_test_dedupe_invalid_directory)
{
    int result = fossil_shark_dedupe("nonexistent_dir", true, false, false, false, "text");
    ASSUME_NOT_EQUAL_I32(0, result);
}

FOSSIL_TEST(c_test_dedupe_mixed_files_and_duplicates)
{
    mkdir("mixed_dupe_dir", 0700);

    create_file("mixed_dupe_dir/a.txt", "DUP");
    create_file("mixed_dupe_dir/b.txt", "DUP");
    create_file("mixed_dupe_dir/c.txt", "UNIQUE");

    int result = fossil_shark_dedupe("mixed_dupe_dir", true, false, true, false, "text");
    ASSUME_ITS_EQUAL_I32(0, result);

    rmdir("mixed_dupe_dir");
}

// * * * * * * * * * * * * * * * * * * * * * * * *
// * Test Group Registration
// * * * * * * * * * * * * * * * * * * * * * * * *

FOSSIL_TEST_GROUP(c_dedupe_command_tests)
{
    FOSSIL_TEST_ADD(c_dedupe_command_suite, c_test_dedupe_null_parameters);
    FOSSIL_TEST_ADD(c_dedupe_command_suite, c_test_dedupe_empty_directory);
    FOSSIL_TEST_ADD(c_dedupe_command_suite, c_test_dedupe_no_duplicates);
    FOSSIL_TEST_ADD(c_dedupe_command_suite, c_test_dedupe_detect_duplicates_by_hash);
    FOSSIL_TEST_ADD(c_dedupe_command_suite, c_test_dedupe_detect_duplicates_by_size);
    FOSSIL_TEST_ADD(c_dedupe_command_suite, c_test_dedupe_delete_duplicates);
    FOSSIL_TEST_ADD(c_dedupe_command_suite, c_test_dedupe_link_duplicates);
    FOSSIL_TEST_ADD(c_dedupe_command_suite, c_test_dedupe_interactive_mode);
    FOSSIL_TEST_ADD(c_dedupe_command_suite, c_test_dedupe_json_output);
    FOSSIL_TEST_ADD(c_dedupe_command_suite, c_test_dedupe_fson_output);
    FOSSIL_TEST_ADD(c_dedupe_command_suite, c_test_dedupe_invalid_directory);
    FOSSIL_TEST_ADD(c_dedupe_command_suite, c_test_dedupe_mixed_files_and_duplicates);

    FOSSIL_TEST_REGISTER(c_dedupe_command_suite);
}
