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
FOSSIL_TEST_SUITE(c_compare_command_suite);

// Setup function for the test suite
FOSSIL_SETUP(c_compare_command_suite) {
    // Setup code here
}

// Teardown function for the test suite
FOSSIL_TEARDOWN(c_compare_command_suite) {
    // Teardown code here
}

// * * * * * * * * * * * * * * * * * * * * * * * *
// * Fossil Logic Test Cases
// * * * * * * * * * * * * * * * * * * * * * * * *
// The test cases below are provided as samples, inspired
// by the Meson build system's approach of using test cases
// as samples for library usage.
// * * * * * * * * * * * * * * * * * * * * * * * *

// Test cases for fossil_shark_compare function

FOSSIL_TEST_CASE(c_test_compare_null_parameters) {
    // Test with null path1
    int result = fossil_shark_compare(cnull, "test.txt", true, false, 0, false);
    ASSUME_NOT_EQUAL_I32(0, result);
    
    // Test with null path2
    result = fossil_shark_compare("test.txt", cnull, true, false, 0, false);
    ASSUME_NOT_EQUAL_I32(0, result);
    
    // Test with both null
    result = fossil_shark_compare(cnull, cnull, true, false, 0, false);
    ASSUME_NOT_EQUAL_I32(0, result);
}

FOSSIL_TEST_CASE(c_test_compare_identical_text_files) {
    // Create two identical files
    FILE *file1 = fopen("identical1.txt", "w");
    ASSUME_NOT_CNULL(file1);
    fprintf(file1, "Line 1\nLine 2\nLine 3\n");
    fclose(file1);
    
    FILE *file2 = fopen("identical2.txt", "w");
    ASSUME_NOT_CNULL(file2);
    fprintf(file2, "Line 1\nLine 2\nLine 3\n");
    fclose(file2);
    
    // Compare identical files
    int result = fossil_shark_compare("identical1.txt", "identical2.txt", true, false, 0, false);
    ASSUME_ITS_EQUAL_I32(0, result);
    
    // Clean up
    remove("identical1.txt");
    remove("identical2.txt");
}

FOSSIL_TEST_CASE(c_test_compare_different_text_files) {
    // Create two different files
    FILE *file1 = fopen("different1.txt", "w");
    ASSUME_NOT_CNULL(file1);
    fprintf(file1, "Line 1\nLine 2\nLine 3\n");
    fclose(file1);
    
    FILE *file2 = fopen("different2.txt", "w");
    ASSUME_NOT_CNULL(file2);
    fprintf(file2, "Line 1\nDifferent Line\nLine 3\n");
    fclose(file2);
    
    // Compare different files
    int result = fossil_shark_compare("different1.txt", "different2.txt", true, false, 0, false);
    ASSUME_NOT_EQUAL_I32(0, result);
    
    // Clean up
    remove("different1.txt");
    remove("different2.txt");
}

FOSSIL_TEST_CASE(c_test_compare_identical_binary_files) {
    // Create two identical binary files
    FILE *file1 = fopen("binary1.bin", "wb");
    ASSUME_NOT_CNULL(file1);
    unsigned char data[] = {0x00, 0xFF, 0xAB, 0xCD, 0x12, 0x34};
    fwrite(data, 1, sizeof(data), file1);
    fclose(file1);
    
    FILE *file2 = fopen("binary2.bin", "wb");
    ASSUME_NOT_CNULL(file2);
    fwrite(data, 1, sizeof(data), file2);
    fclose(file2);
    
    // Compare identical binary files
    int result = fossil_shark_compare("binary1.bin", "binary2.bin", false, true, 0, false);
    ASSUME_ITS_EQUAL_I32(0, result);
    
    // Clean up
    remove("binary1.bin");
    remove("binary2.bin");
}

FOSSIL_TEST_CASE(c_test_compare_different_binary_files) {
    // Create two different binary files
    FILE *file1 = fopen("binary_diff1.bin", "wb");
    ASSUME_NOT_CNULL(file1);
    unsigned char data1[] = {0x00, 0xFF, 0xAB, 0xCD, 0x12, 0x34};
    fwrite(data1, 1, sizeof(data1), file1);
    fclose(file1);
    
    FILE *file2 = fopen("binary_diff2.bin", "wb");
    ASSUME_NOT_CNULL(file2);
    unsigned char data2[] = {0x00, 0xFF, 0xAB, 0xCD, 0x56, 0x78};
    fwrite(data2, 1, sizeof(data2), file2);
    fclose(file2);
    
    // Compare different binary files
    int result = fossil_shark_compare("binary_diff1.bin", "binary_diff2.bin", false, true, 0, false);
    ASSUME_NOT_EQUAL_I32(0, result);
    
    // Clean up
    remove("binary_diff1.bin");
    remove("binary_diff2.bin");
}

FOSSIL_TEST_CASE(c_test_compare_case_sensitive) {
    // Create files with case differences
    FILE *file1 = fopen("case1.txt", "w");
    ASSUME_NOT_CNULL(file1);
    fprintf(file1, "Hello World\n");
    fclose(file1);
    
    FILE *file2 = fopen("case2.txt", "w");
    ASSUME_NOT_CNULL(file2);
    fprintf(file2, "hello world\n");
    fclose(file2);
    
    // Compare with case sensitivity (should find differences)
    int result = fossil_shark_compare("case1.txt", "case2.txt", true, false, 0, false);
    ASSUME_NOT_EQUAL_I32(0, result);
    
    // Clean up
    remove("case1.txt");
    remove("case2.txt");
}

FOSSIL_TEST_CASE(c_test_compare_case_insensitive) {
    // Create files with case differences
    FILE *file1 = fopen("case_ignore1.txt", "w");
    ASSUME_NOT_CNULL(file1);
    fprintf(file1, "Hello World\n");
    fclose(file1);
    
    FILE *file2 = fopen("case_ignore2.txt", "w");
    ASSUME_NOT_CNULL(file2);
    fprintf(file2, "hello world\n");
    fclose(file2);
    
    // Compare with case insensitivity (should be identical)
    int result = fossil_shark_compare("case_ignore1.txt", "case_ignore2.txt", true, false, 0, true);
    ASSUME_ITS_EQUAL_I32(0, result);
    
    // Clean up
    remove("case_ignore1.txt");
    remove("case_ignore2.txt");
}

FOSSIL_TEST_CASE(c_test_compare_with_context_lines) {
    // Create files with multiple lines and differences
    FILE *file1 = fopen("context1.txt", "w");
    ASSUME_NOT_CNULL(file1);
    fprintf(file1, "Line 1\nLine 2\nLine 3\nOriginal Line\nLine 5\nLine 6\nLine 7\n");
    fclose(file1);
    
    FILE *file2 = fopen("context2.txt", "w");
    ASSUME_NOT_CNULL(file2);
    fprintf(file2, "Line 1\nLine 2\nLine 3\nChanged Line\nLine 5\nLine 6\nLine 7\n");
    fclose(file2);
    
    // Compare with context lines
    int result = fossil_shark_compare("context1.txt", "context2.txt", true, false, 2, false);
    ASSUME_NOT_EQUAL_I32(0, result);
    
    // Clean up
    remove("context1.txt");
    remove("context2.txt");
}

FOSSIL_TEST_CASE(c_test_compare_empty_files) {
    // Create two empty files
    FILE *file1 = fopen("empty1.txt", "w");
    ASSUME_NOT_CNULL(file1);
    fclose(file1);
    
    FILE *file2 = fopen("empty2.txt", "w");
    ASSUME_NOT_CNULL(file2);
    fclose(file2);
    
    // Compare empty files
    int result = fossil_shark_compare("empty1.txt", "empty2.txt", true, false, 0, false);
    ASSUME_ITS_EQUAL_I32(0, result);
    
    // Clean up
    remove("empty1.txt");
    remove("empty2.txt");
}

FOSSIL_TEST_CASE(c_test_compare_different_length_files) {
    // Create files with different lengths
    FILE *file1 = fopen("short.txt", "w");
    ASSUME_NOT_CNULL(file1);
    fprintf(file1, "Short file\n");
    fclose(file1);
    
    FILE *file2 = fopen("long.txt", "w");
    ASSUME_NOT_CNULL(file2);
    fprintf(file2, "Short file\nAdditional line\nAnother line\n");
    fclose(file2);
    
    // Compare files with different lengths
    int result = fossil_shark_compare("short.txt", "long.txt", true, false, 0, false);
    ASSUME_NOT_EQUAL_I32(0, result);
    
    // Clean up
    remove("short.txt");
    remove("long.txt");
}

FOSSIL_TEST_CASE(c_test_compare_nonexistent_files) {
    // Try to compare non-existent files
    int result = fossil_shark_compare("nonexistent1.txt", "nonexistent2.txt", true, false, 0, false);
    ASSUME_NOT_EQUAL_I32(0, result);
}

FOSSIL_TEST_CASE(c_test_compare_one_nonexistent_file) {
    // Create one file
    FILE *file1 = fopen("exists.txt", "w");
    ASSUME_NOT_CNULL(file1);
    fprintf(file1, "This file exists\n");
    fclose(file1);
    
    // Try to compare existing file with non-existent file
    int result = fossil_shark_compare("exists.txt", "nonexistent.txt", true, false, 0, false);
    ASSUME_NOT_EQUAL_I32(0, result);
    
    // Clean up
    remove("exists.txt");
}

FOSSIL_TEST_CASE(c_test_compare_neither_text_nor_binary) {
    // Create two files
    FILE *file1 = fopen("neither1.txt", "w");
    ASSUME_NOT_CNULL(file1);
    fprintf(file1, "Test content\n");
    fclose(file1);
    
    FILE *file2 = fopen("neither2.txt", "w");
    ASSUME_NOT_CNULL(file2);
    fprintf(file2, "Test content\n");
    fclose(file2);
    
    // Try to compare without specifying text or binary mode
    int result = fossil_shark_compare("neither1.txt", "neither2.txt", false, false, 0, false);
    ASSUME_NOT_EQUAL_I32(0, result);
    
    // Clean up
    remove("neither1.txt");
    remove("neither2.txt");
}

FOSSIL_TEST_CASE(c_test_compare_large_files) {
    // Create large files for performance testing
    FILE *file1 = fopen("large1.txt", "w");
    ASSUME_NOT_CNULL(file1);
    for (int i = 0; i < 1000; i++) {
        fprintf(file1, "Line %d in first file\n", i);
    }
    fclose(file1);
    
    FILE *file2 = fopen("large2.txt", "w");
    ASSUME_NOT_CNULL(file2);
    for (int i = 0; i < 1000; i++) {
        fprintf(file2, "Line %d in first file\n", i);
    }
    fclose(file2);
    
    // Compare large identical files
    int result = fossil_shark_compare("large1.txt", "large2.txt", true, false, 0, false);
    ASSUME_ITS_EQUAL_I32(0, result);
    
    // Clean up
    remove("large1.txt");
    remove("large2.txt");
}

// * * * * * * * * * * * * * * * * * * * * * * * *
// * Fossil Logic Test Pool
// * * * * * * * * * * * * * * * * * * * * * * * *

FOSSIL_TEST_GROUP(c_compare_command_tests) {
    FOSSIL_TEST_ADD(c_compare_command_suite, c_test_compare_null_parameters);
    FOSSIL_TEST_ADD(c_compare_command_suite, c_test_compare_identical_text_files);
    FOSSIL_TEST_ADD(c_compare_command_suite, c_test_compare_different_text_files);
    FOSSIL_TEST_ADD(c_compare_command_suite, c_test_compare_identical_binary_files);
    FOSSIL_TEST_ADD(c_compare_command_suite, c_test_compare_different_binary_files);
    FOSSIL_TEST_ADD(c_compare_command_suite, c_test_compare_case_sensitive);
    FOSSIL_TEST_ADD(c_compare_command_suite, c_test_compare_case_insensitive);
    FOSSIL_TEST_ADD(c_compare_command_suite, c_test_compare_with_context_lines);
    FOSSIL_TEST_ADD(c_compare_command_suite, c_test_compare_empty_files);
    FOSSIL_TEST_ADD(c_compare_command_suite, c_test_compare_different_length_files);
    FOSSIL_TEST_ADD(c_compare_command_suite, c_test_compare_nonexistent_files);
    FOSSIL_TEST_ADD(c_compare_command_suite, c_test_compare_one_nonexistent_file);
    FOSSIL_TEST_ADD(c_compare_command_suite, c_test_compare_neither_text_nor_binary);
    FOSSIL_TEST_ADD(c_compare_command_suite, c_test_compare_large_files);

    FOSSIL_TEST_REGISTER(c_compare_command_suite);
}
