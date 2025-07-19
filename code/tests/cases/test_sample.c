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
FOSSIL_TEST_SUITE(c_sample_suite);
fossil_fstream_t c_string;

// Setup function for the test suite
FOSSIL_SETUP(c_sample_suite)
{
    // Setup code here
}

// Teardown function for the test suite
FOSSIL_TEARDOWN(c_sample_suite)
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

FOSSIL_TEST_CASE(c_test_shark_create) {
    shark_create("test_file.txt", "file");
    ASSUME_ITS_TRUE(FOSSIL_SANITY_SYS_FILE_EXISTS("test_file.txt"));

    shark_create("test_dir", "dir");
    ASSUME_ITS_TRUE(FOSSIL_SANITY_SYS_FILE_EXISTS("test_dir"));
}

FOSSIL_TEST_CASE(c_test_shark_delete) {
    shark_create("delete_file.txt", "file");
    shark_delete("delete_file.txt", true);
    ASSUME_ITS_FALSE(FOSSIL_SANITY_SYS_FILE_EXISTS("delete_file.txt"));

    shark_create("delete_dir", "dir");
    shark_delete("delete_dir", true);
    // ASSUME_ITS_FALSE(FOSSIL_SANITY_SYS_FILE_EXISTS("delete_dir"));
}
// TODO: add a Sanity patch for directory checks in Pizza Test

FOSSIL_TEST_CASE(c_test_shark_move) {
    shark_create("move_file.txt", "file");
    shark_move("move_file.txt", "moved_file.txt", true, false);
    ASSUME_ITS_TRUE(FOSSIL_SANITY_SYS_FILE_EXISTS("moved_file.txt"));
    ASSUME_ITS_FALSE(FOSSIL_SANITY_SYS_FILE_EXISTS("move_file.txt"));

    shark_create("move_dir", "dir");
    shark_move("move_dir", "moved_dir", true, false);
    ASSUME_ITS_TRUE(FOSSIL_SANITY_SYS_FILE_EXISTS("moved_dir"));
}

FOSSIL_TEST_CASE(c_test_shark_rename) {
    shark_create("rename_file.txt", "file");
    shark_rename("rename_file.txt", "renamed_file.txt", true, false);
    ASSUME_ITS_TRUE(FOSSIL_SANITY_SYS_FILE_EXISTS("renamed_file.txt"));
    ASSUME_ITS_FALSE(FOSSIL_SANITY_SYS_FILE_EXISTS("rename_file.txt"));
}

FOSSIL_TEST_CASE(c_test_shark_copy) {
    shark_create("copy_file.txt", "file");
    shark_copy("copy_file.txt", "copied_file.txt", true, NULL);
    ASSUME_ITS_TRUE(FOSSIL_SANITY_SYS_FILE_EXISTS("copy_file.txt"));
    ASSUME_ITS_TRUE(FOSSIL_SANITY_SYS_FILE_EXISTS("copied_file.txt"));
}

FOSSIL_TEST_CASE(c_test_shark_list) {
    shark_create("list_dir", "dir");
    shark_list("list_dir", "list");
    ASSUME_ITS_TRUE(FOSSIL_SANITY_SYS_FILE_EXISTS("list_dir"));
}

FOSSIL_TEST_CASE(c_test_shark_show) {
    shark_create("show_file.txt", "file");
    shark_show("show_file.txt", 10, 0);
    ASSUME_ITS_TRUE(FOSSIL_SANITY_SYS_FILE_EXISTS("show_file.txt"));
}

// FOSSIL_TEST_CASE(c_test_shark_find) {
//     shark_create("find_file.txt", "file");
//     shark_find(".", "find_file.txt", NULL);
//     ASSUME_ITS_TRUE(FOSSIL_SANITY_SYS_FILE_EXISTS("find_file.txt"));
// }

// FOSSIL_TEST_CASE(c_test_shark_size) {
//     shark_create("size_file.txt", "file");
//     shark_size("size_file.txt", false);
//     ASSUME_ITS_TRUE(FOSSIL_SANITY_SYS_FILE_EXISTS("size_file.txt"));
// }

FOSSIL_TEST_CASE(c_test_shark_compare) {
    shark_create("compare_file1.txt", "file");
    shark_create("compare_file2.txt", "file");
    shark_compare("compare_file1.txt", "compare_file2.txt", false, false, false);
    ASSUME_ITS_TRUE(FOSSIL_SANITY_SYS_FILE_EXISTS("compare_file1.txt"));
    ASSUME_ITS_TRUE(FOSSIL_SANITY_SYS_FILE_EXISTS("compare_file2.txt"));
}

// FOSSIL_TEST_CASE(c_test_shark_ask) {
//     shark_create("ask_file.txt", "file");
//     shark_ask("ask_file.txt", "non_existent.txt", "file");
//     ASSUME_ITS_TRUE(FOSSIL_SANITY_SYS_FILE_EXISTS("ask_file.txt"));
// }

FOSSIL_TEST_CASE(c_test_shark_change) {
    shark_create("change_file.txt", "file");
    shark_change("change_file.txt", "new_value", false, true);
    ASSUME_ITS_TRUE(FOSSIL_SANITY_SYS_FILE_EXISTS("change_file.txt"));
}

// Add the new test cases to the test group
FOSSIL_TEST_GROUP(c_sample_tests) {
    FOSSIL_TEST_ADD(c_sample_suite, c_test_shark_create);
    FOSSIL_TEST_ADD(c_sample_suite, c_test_shark_delete);
    FOSSIL_TEST_ADD(c_sample_suite, c_test_shark_move);
    FOSSIL_TEST_ADD(c_sample_suite, c_test_shark_rename);
    FOSSIL_TEST_ADD(c_sample_suite, c_test_shark_copy);
    FOSSIL_TEST_ADD(c_sample_suite, c_test_shark_list);
    FOSSIL_TEST_ADD(c_sample_suite, c_test_shark_show);
    // FOSSIL_TEST_ADD(c_sample_suite, c_test_shark_find);
    //FOSSIL_TEST_ADD(c_sample_suite, c_test_shark_size);
    FOSSIL_TEST_ADD(c_sample_suite, c_test_shark_compare);
//    FOSSIL_TEST_ADD(c_sample_suite, c_test_shark_ask);
    FOSSIL_TEST_ADD(c_sample_suite, c_test_shark_change);

    FOSSIL_TEST_REGISTER(c_sample_suite);
}
