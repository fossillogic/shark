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
FOSSIL_SUITE(c_sync_command_suite);

// Setup function for the test suite
FOSSIL_SETUP(c_sync_command_suite) {
    // Setup code here
}

// Teardown function for the test suite
FOSSIL_TEARDOWN(c_sync_command_suite) {
    // Teardown code here
}

// * * * * * * * * * * * * * * * * * * * * * * * *
// * Fossil Logic Test Cases
// * * * * * * * * * * * * * * * * * * * * * * * *
// The test cases below are provided as samples, inspired
// by the Meson build system's approach of using test cases
// as samples for library usage.
// * * * * * * * * * * * * * * * * * * * * * * * *

FOSSIL_TEST(c_test_sync_null_source) {
    int result = fossil_shark_sync(cnull, "dest", false, false, false);
    ASSUME_NOT_EQUAL_I32(result, 0);
}

FOSSIL_TEST(c_test_sync_null_destination) {
    FOSSIL_SANITY_SYS_CREATE_FILE("test_sync_src.txt");
    int result = fossil_shark_sync("test_sync_src.txt", cnull, false, false, false);
    ASSUME_NOT_EQUAL_I32(result, 0);
    FOSSIL_SANITY_SYS_DELETE_FILE("test_sync_src.txt");
}

FOSSIL_TEST(c_test_sync_nonexistent_source) {
    int result = fossil_shark_sync("nonexistent_sync_src.txt", "sync_dest.txt", false, false, false);
    ASSUME_NOT_EQUAL_I32(result, 0);
}

FOSSIL_TEST(c_test_sync_single_file) {
    FOSSIL_SANITY_SYS_CREATE_FILE("test_sync_file_src.txt");
    int result = fossil_shark_sync("test_sync_file_src.txt", "test_sync_file_dest.txt", false, false, false);
    ASSUME_ITS_EQUAL_I32(result, 0);
    FOSSIL_SANITY_SYS_DELETE_FILE("test_sync_file_src.txt");
    if (FOSSIL_SANITY_SYS_FILE_EXISTS("test_sync_file_dest.txt")) {
        FOSSIL_SANITY_SYS_DELETE_FILE("test_sync_file_dest.txt");
    }
}

FOSSIL_TEST(c_test_sync_directory_non_recursive) {
    FOSSIL_SANITY_SYS_CREATE_DIR("test_sync_src_dir");
    FOSSIL_SANITY_SYS_CREATE_FILE("test_sync_src_dir/file1.txt");
    int result = fossil_shark_sync("test_sync_src_dir", "test_sync_dest_dir", false, false, false);
    ASSUME_ITS_EQUAL_I32(result, 0);
    FOSSIL_SANITY_SYS_DELETE_FILE("test_sync_src_dir/file1.txt");
    FOSSIL_SANITY_SYS_DELETE_FILE("test_sync_src_dir");
    if (FOSSIL_SANITY_SYS_DIR_EXISTS("test_sync_dest_dir")) {
        FOSSIL_SANITY_SYS_DELETE_FILE("test_sync_dest_dir");
    }
}

FOSSIL_TEST(c_test_sync_directory_recursive) {
    FOSSIL_SANITY_SYS_CREATE_DIR("test_sync_rec_src");
    FOSSIL_SANITY_SYS_CREATE_FILE("test_sync_rec_src/file1.txt");
    int result = fossil_shark_sync("test_sync_rec_src", "test_sync_rec_dest", true, false, false);
    ASSUME_ITS_EQUAL_I32(result, 0);
    FOSSIL_SANITY_SYS_DELETE_FILE("test_sync_rec_src/file1.txt");
    FOSSIL_SANITY_SYS_DELETE_FILE("test_sync_rec_src");
    if (FOSSIL_SANITY_SYS_DIR_EXISTS("test_sync_rec_dest")) {
        FOSSIL_SANITY_SYS_DELETE_FILE("test_sync_rec_dest");
    }
}

FOSSIL_TEST(c_test_sync_update_flag) {
    FOSSIL_SANITY_SYS_CREATE_FILE("test_sync_update_src.txt");
    int result = fossil_shark_sync("test_sync_update_src.txt", "test_sync_update_dest.txt", false, true, false);
    ASSUME_ITS_EQUAL_I32(result, 0);
    FOSSIL_SANITY_SYS_DELETE_FILE("test_sync_update_src.txt");
    if (FOSSIL_SANITY_SYS_FILE_EXISTS("test_sync_update_dest.txt")) {
        FOSSIL_SANITY_SYS_DELETE_FILE("test_sync_update_dest.txt");
    }
}

FOSSIL_TEST(c_test_sync_delete_flag) {
    FOSSIL_SANITY_SYS_CREATE_DIR("test_sync_del_src");
    FOSSIL_SANITY_SYS_CREATE_FILE("test_sync_del_src/file1.txt");
    int result = fossil_shark_sync("test_sync_del_src", "test_sync_del_dest", true, false, true);
    ASSUME_ITS_EQUAL_I32(result, 0);
    FOSSIL_SANITY_SYS_DELETE_FILE("test_sync_del_src/file1.txt");
    FOSSIL_SANITY_SYS_DELETE_FILE("test_sync_del_src");
    if (FOSSIL_SANITY_SYS_DIR_EXISTS("test_sync_del_dest")) {
        FOSSIL_SANITY_SYS_DELETE_FILE("test_sync_del_dest");
    }
}

FOSSIL_TEST(c_test_sync_identical_files) {
    FOSSIL_SANITY_SYS_CREATE_FILE("test_sync_identical_src.txt");
    FOSSIL_SANITY_SYS_CREATE_FILE("test_sync_identical_dest.txt");
    int result = fossil_shark_sync("test_sync_identical_src.txt", "test_sync_identical_dest.txt", false, false, false);
    ASSUME_ITS_EQUAL_I32(result, 0);
    FOSSIL_SANITY_SYS_DELETE_FILE("test_sync_identical_src.txt");
    FOSSIL_SANITY_SYS_DELETE_FILE("test_sync_identical_dest.txt");
}

// * * * * * * * * * * * * * * * * * * * * * * * *
// * Fossil Logic Test Pool
// * * * * * * * * * * * * * * * * * * * * * * * *

FOSSIL_TEST_GROUP(c_sync_command_tests) {
    FOSSIL_TEST_ADD(c_sync_command_suite, c_test_sync_null_source);
    FOSSIL_TEST_ADD(c_sync_command_suite, c_test_sync_null_destination);
    FOSSIL_TEST_ADD(c_sync_command_suite, c_test_sync_nonexistent_source);
    FOSSIL_TEST_ADD(c_sync_command_suite, c_test_sync_single_file);
    FOSSIL_TEST_ADD(c_sync_command_suite, c_test_sync_directory_non_recursive);
    FOSSIL_TEST_ADD(c_sync_command_suite, c_test_sync_directory_recursive);
    FOSSIL_TEST_ADD(c_sync_command_suite, c_test_sync_update_flag);
    FOSSIL_TEST_ADD(c_sync_command_suite, c_test_sync_delete_flag);
    FOSSIL_TEST_ADD(c_sync_command_suite, c_test_sync_identical_files);

    FOSSIL_TEST_REGISTER(c_sync_command_suite);
}
