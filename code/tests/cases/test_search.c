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
FOSSIL_SUITE(c_search_command_suite);

// Setup function for the test suite
FOSSIL_SETUP(c_search_command_suite)
{
    // Setup code here
}

// Teardown function for the test suite
FOSSIL_TEARDOWN(c_search_command_suite)
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

// Test cases for fossil_shark_search function

FOSSIL_TEST(c_test_search_null_path)
{
    // Should default to current directory when path is null
    int result = fossil_shark_search(cnull, false, cnull, cnull, false);
    ASSUME_ITS_EQUAL_I32(0, result);
}

FOSSIL_TEST(c_test_search_invalid_path)
{
    // Should handle invalid path gracefully
    int result = fossil_shark_search("/nonexistent/path", false, cnull, cnull, false);
    ASSUME_NOT_EQUAL_I32(0, result);
}

FOSSIL_TEST(c_test_search_by_name_pattern)
{
    // Create test files using fossil_shark_create
    int res1 = fossil_shark_create("test_file.txt", false, "file");
    ASSUME_ITS_EQUAL_I32(0, res1);

    FOSSIL_SANITY_SYS_WRITE_FILE("test_file.txt", "Test content\n");

    int res2 = fossil_shark_create("test_file.c", false, "file");
    ASSUME_ITS_EQUAL_I32(0, res2);

    FOSSIL_SANITY_SYS_WRITE_FILE("test_file.c", "C code\n");

    // Search for .txt files
    int result = fossil_shark_search(".", false, ".txt", cnull, false);
    ASSUME_ITS_EQUAL_I32(0, result);

    // Clean up
    FOSSIL_SANITY_SYS_DELETE_FILE("test_file.txt");
    FOSSIL_SANITY_SYS_DELETE_FILE("test_file.c");
}

FOSSIL_TEST(c_test_search_by_content_pattern)
{
    // Create test file
    int res = fossil_shark_create("search_content.txt", false, "file");
    ASSUME_ITS_EQUAL_I32(0, res);

    FOSSIL_SANITY_SYS_WRITE_FILE("search_content.txt", "This is a test file\nwith specific content\n");

    // Search for content pattern
    int result = fossil_shark_search(".", false, cnull, "specific", false);
    ASSUME_ITS_EQUAL_I32(0, result);

    // Clean up
    FOSSIL_SANITY_SYS_DELETE_FILE("search_content.txt");
}

FOSSIL_TEST(c_test_search_case_insensitive_name)
{
    // Create test file
    int res = fossil_shark_create("TestFile.TXT", false, "file");
    ASSUME_ITS_EQUAL_I32(0, res);

    FOSSIL_SANITY_SYS_WRITE_FILE("TestFile.TXT", "Test content\n");

    // Search with case insensitive pattern
    int result = fossil_shark_search(".", false, "testfile", cnull, true);
    ASSUME_ITS_EQUAL_I32(0, result);

    // Clean up
    FOSSIL_SANITY_SYS_DELETE_FILE("TestFile.TXT");
}

FOSSIL_TEST(c_test_search_case_insensitive_content)
{
    // Create test file
    int res = fossil_shark_create("case_test.txt", false, "file");
    ASSUME_ITS_EQUAL_I32(0, res);

    FOSSIL_SANITY_SYS_WRITE_FILE("case_test.txt", "This Contains UPPERCASE Text\n");

    // Search with case insensitive content
    int result = fossil_shark_search(".", false, cnull, "uppercase", true);
    ASSUME_ITS_EQUAL_I32(0, result);

    // Clean up
    FOSSIL_SANITY_SYS_DELETE_FILE("case_test.txt");
}

FOSSIL_TEST(c_test_search_combined_patterns)
{
    // Create test files
    int res1 = fossil_shark_create("match_both.txt", false, "file");
    ASSUME_ITS_EQUAL_I32(0, res1);

    FOSSIL_SANITY_SYS_WRITE_FILE("match_both.txt", "This file matches both patterns\n");

    int res2 = fossil_shark_create("match_name.txt", false, "file");
    ASSUME_ITS_EQUAL_I32(0, res2);

    FOSSIL_SANITY_SYS_WRITE_FILE("match_name.txt", "This file only matches name\n");

    // Search with both name and content patterns
    int result = fossil_shark_search(".", false, ".txt", "both patterns", false);
    ASSUME_ITS_EQUAL_I32(0, result);

    // Clean up
    FOSSIL_SANITY_SYS_DELETE_FILE("match_both.txt");
    FOSSIL_SANITY_SYS_DELETE_FILE("match_name.txt");
}

FOSSIL_TEST(c_test_search_no_matches)
{
    // Create test file
    int res = fossil_shark_create("no_match.txt", false, "file");
    ASSUME_ITS_EQUAL_I32(0, res);

    FOSSIL_SANITY_SYS_WRITE_FILE("no_match.txt", "This content does not match\n");

    // Search for non-existent pattern
    int result = fossil_shark_search(".", false, cnull, "nonexistent_pattern", false);
    ASSUME_ITS_EQUAL_I32(0, result);

    // Clean up
    FOSSIL_SANITY_SYS_DELETE_FILE("no_match.txt");
}

FOSSIL_TEST(c_test_search_empty_file)
{
    // Create empty test file
    int res = fossil_shark_create("empty_search.txt", false, "file");
    ASSUME_ITS_EQUAL_I32(0, res);

    // Search in empty file
    int result = fossil_shark_search(".", false, cnull, "anything", false);
    ASSUME_ITS_EQUAL_I32(0, result);

    // Clean up
    FOSSIL_SANITY_SYS_DELETE_FILE("empty_search.txt");
}

FOSSIL_TEST(c_test_search_no_patterns)
{
    // Create test file
    int res = fossil_shark_create("any_file.txt", false, "file");
    ASSUME_ITS_EQUAL_I32(0, res);

    FOSSIL_SANITY_SYS_WRITE_FILE("any_file.txt", "Any content\n");

    // Search without any patterns (should match all)
    int result = fossil_shark_search(".", false, cnull, cnull, false);
    ASSUME_ITS_EQUAL_I32(0, result);

    // Clean up
    FOSSIL_SANITY_SYS_DELETE_FILE("any_file.txt");
}

FOSSIL_TEST(c_test_search_recursive_basic)
{
    // Create nested directory structure
    int res = fossil_shark_create("test_dir/nested", true, "dir");
    ASSUME_ITS_EQUAL_I32(0, res);

    // Recursive search
    int result = fossil_shark_search(".", true, cnull, cnull, false);
    ASSUME_ITS_EQUAL_I32(0, result);

    // Clean up
    FOSSIL_SANITY_SYS_EXECUTE("rm -rf test_dir");
}

FOSSIL_TEST(c_test_search_non_recursive)
{
    // Search only in current directory
    int result = fossil_shark_search(".", false, cnull, cnull, false);
    ASSUME_ITS_EQUAL_I32(0, result);
}

FOSSIL_TEST(c_test_search_unreadable_file)
{
    // Create test file
    int res = fossil_shark_create("readable_file.txt", false, "file");
    ASSUME_ITS_EQUAL_I32(0, res);

    FOSSIL_SANITY_SYS_WRITE_FILE("readable_file.txt", "Content that can be read\n");

    // Search should handle files gracefully
    int result = fossil_shark_search(".", false, cnull, "content", false);
    ASSUME_ITS_EQUAL_I32(0, result);

    // Clean up
    FOSSIL_SANITY_SYS_DELETE_FILE("readable_file.txt");
}

// regex

FOSSIL_TEST(c_test_search_regex_extension_match)
{
    int res = fossil_shark_create("regex_test.c", false, "file");
    ASSUME_ITS_EQUAL_I32(0, res);

    FOSSIL_SANITY_SYS_WRITE_FILE("regex_test.c", "int main() {}\n");

    int result = fossil_shark_search(".", false, ".*\\.c$", cnull, false);

    ASSUME_ITS_EQUAL_I32(0, result);

    FOSSIL_SANITY_SYS_DELETE_FILE("regex_test.c");
}

FOSSIL_TEST(c_test_search_regex_character_class)
{
    int res = fossil_shark_create("abc123.txt", false, "file");
    ASSUME_ITS_EQUAL_I32(0, res);

    FOSSIL_SANITY_SYS_WRITE_FILE("abc123.txt", "content\n");

    int result = fossil_shark_search(".", false, "[a-z]+[0-9]+\\.txt", cnull, false);

    ASSUME_ITS_EQUAL_I32(0, result);

    FOSSIL_SANITY_SYS_DELETE_FILE("abc123.txt");
}

FOSSIL_TEST(c_test_search_regex_alternation)
{
    int res = fossil_shark_create("main.cpp", false, "file");
    ASSUME_ITS_EQUAL_I32(0, res);

    FOSSIL_SANITY_SYS_WRITE_FILE("main.cpp", "cpp file\n");

    int result = fossil_shark_search(".", false, ".*\\.(c|cpp)$", cnull, false);

    ASSUME_ITS_EQUAL_I32(0, result);

    FOSSIL_SANITY_SYS_DELETE_FILE("main.cpp");
}

FOSSIL_TEST(c_test_search_regex_line_anchor_start)
{
    int res = fossil_shark_create("anchor_test.txt", false, "file");
    ASSUME_ITS_EQUAL_I32(0, res);

    FOSSIL_SANITY_SYS_WRITE_FILE(
        "anchor_test.txt",
        "hello world\nsecond line\n");

    int result = fossil_shark_search(".", false, cnull, "^hello", false);

    ASSUME_ITS_EQUAL_I32(0, result);

    FOSSIL_SANITY_SYS_DELETE_FILE("anchor_test.txt");
}

FOSSIL_TEST(c_test_search_regex_line_anchor_end)
{
    int res = fossil_shark_create("end_anchor.txt", false, "file");
    ASSUME_ITS_EQUAL_I32(0, res);

    FOSSIL_SANITY_SYS_WRITE_FILE(
        "end_anchor.txt",
        "line ending\n");

    int result = fossil_shark_search(".", false, cnull, "ending$", false);

    ASSUME_ITS_EQUAL_I32(0, result);

    FOSSIL_SANITY_SYS_DELETE_FILE("end_anchor.txt");
}

FOSSIL_TEST(c_test_search_regex_wildcard)
{
    int res = fossil_shark_create("wildcard.txt", false, "file");
    ASSUME_ITS_EQUAL_I32(0, res);

    FOSSIL_SANITY_SYS_WRITE_FILE(
        "wildcard.txt",
        "abcdefg\n");

    int result = fossil_shark_search(".", false, cnull, "abc.*fg", false);

    ASSUME_ITS_EQUAL_I32(0, result);

    FOSSIL_SANITY_SYS_DELETE_FILE("wildcard.txt");
}

FOSSIL_TEST(c_test_search_regex_optional_quantifier)
{
    int res = fossil_shark_create("color.txt", false, "file");
    ASSUME_ITS_EQUAL_I32(0, res);

    FOSSIL_SANITY_SYS_WRITE_FILE(
        "color.txt",
        "colour\n");

    int result = fossil_shark_search(".", false, cnull, "colou?r", false);

    ASSUME_ITS_EQUAL_I32(0, result);

    FOSSIL_SANITY_SYS_DELETE_FILE("color.txt");
}

FOSSIL_TEST(c_test_search_regex_escape_literals)
{
    int res = fossil_shark_create("literal.txt", false, "file");
    ASSUME_ITS_EQUAL_I32(0, res);

    FOSSIL_SANITY_SYS_WRITE_FILE(
        "literal.txt",
        "price = 10.50\n");

    int result = fossil_shark_search(".", false, cnull, "10\\.50", false);

    ASSUME_ITS_EQUAL_I32(0, result);

    FOSSIL_SANITY_SYS_DELETE_FILE("literal.txt");
}

FOSSIL_TEST(c_test_search_invalid_regex)
{
    int result = fossil_shark_search(".", false, "[unclosed", cnull, false);

    ASSUME_ITS_EQUAL_I32(0, result);
}

FOSSIL_TEST(c_test_search_binary_file_skip)
{
    FILE *fp = fopen("binary.dat", "wb");

    unsigned char data[] = {0x00, 0x01, 0x02, 0x03};

    fwrite(data, 1, sizeof(data), fp);
    fclose(fp);

    int result = fossil_shark_search(".", false, cnull, "anything", false);

    ASSUME_ITS_EQUAL_I32(0, result);

    FOSSIL_SANITY_SYS_DELETE_FILE("binary.dat");
}

FOSSIL_TEST(c_test_search_plain_string_bug)
{
    int res = fossil_shark_create("plain.txt", false, "file");
    ASSUME_ITS_EQUAL_I32(0, res);

    FOSSIL_SANITY_SYS_WRITE_FILE(
        "plain.txt",
        "hello world\n");

    int result = fossil_shark_search(".", false, cnull, "hello", false);

    ASSUME_ITS_EQUAL_I32(0, result);

    FOSSIL_SANITY_SYS_DELETE_FILE("plain.txt");
}

// * * * * * * * * * * * * * * * * * * * * * * * *
// * Fossil Logic Test Pool
// * * * * * * * * * * * * * * * * * * * * * * * *

FOSSIL_TEST_GROUP(c_search_command_tests)
{
    FOSSIL_TEST_ADD(c_search_command_suite, c_test_search_null_path);
    FOSSIL_TEST_ADD(c_search_command_suite, c_test_search_invalid_path);
    FOSSIL_TEST_ADD(c_search_command_suite, c_test_search_by_name_pattern);
    FOSSIL_TEST_ADD(c_search_command_suite, c_test_search_by_content_pattern);
    FOSSIL_TEST_ADD(c_search_command_suite, c_test_search_case_insensitive_name);
    FOSSIL_TEST_ADD(c_search_command_suite, c_test_search_case_insensitive_content);
    FOSSIL_TEST_ADD(c_search_command_suite, c_test_search_combined_patterns);
    FOSSIL_TEST_ADD(c_search_command_suite, c_test_search_no_matches);
    FOSSIL_TEST_ADD(c_search_command_suite, c_test_search_empty_file);
    FOSSIL_TEST_ADD(c_search_command_suite, c_test_search_no_patterns);
    FOSSIL_TEST_ADD(c_search_command_suite, c_test_search_recursive_basic);
    FOSSIL_TEST_ADD(c_search_command_suite, c_test_search_non_recursive);
    FOSSIL_TEST_ADD(c_search_command_suite, c_test_search_unreadable_file);
    FOSSIL_TEST_ADD(c_search_command_suite, c_test_search_regex_extension_match);
    FOSSIL_TEST_ADD(c_search_command_suite, c_test_search_regex_character_class);
    FOSSIL_TEST_ADD(c_search_command_suite, c_test_search_regex_alternation);
    FOSSIL_TEST_ADD(c_search_command_suite, c_test_search_regex_line_anchor_start);
    FOSSIL_TEST_ADD(c_search_command_suite, c_test_search_regex_line_anchor_end);
    FOSSIL_TEST_ADD(c_search_command_suite, c_test_search_regex_wildcard);
    FOSSIL_TEST_ADD(c_search_command_suite, c_test_search_regex_optional_quantifier);
    FOSSIL_TEST_ADD(c_search_command_suite, c_test_search_regex_escape_literals);
    FOSSIL_TEST_ADD(c_search_command_suite, c_test_search_invalid_regex);
    FOSSIL_TEST_ADD(c_search_command_suite, c_test_search_binary_file_skip);
    FOSSIL_TEST_ADD(c_search_command_suite, c_test_search_plain_string_bug);

    FOSSIL_TEST_REGISTER(c_search_command_suite);
}
