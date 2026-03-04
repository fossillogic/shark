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
FOSSIL_SUITE(c_archive_command_suite);

// Setup function for the test suite
FOSSIL_SETUP(c_archive_command_suite) {
    // Setup code here
}

// Teardown function for the test suite
FOSSIL_TEARDOWN(c_archive_command_suite) {
    // Teardown code here
}

// * * * * * * * * * * * * * * * * * * * * * * * *
// * Fossil Logic Test Cases
// * * * * * * * * * * * * * * * * * * * * * * * *
// The test cases below are provided as samples, inspired
// by the Meson build system's approach of using test cases
// as samples for library usage.
// * * * * * * * * * * * * * * * * * * * * * * * *

FOSSIL_TEST(c_test_archive_create_zip) {
    FOSSIL_SANITY_SYS_CREATE_FILE("test_file.txt");
    FOSSIL_SANITY_SYS_WRITE_FILE("test_file.txt", "test content for archive\n");

    int result = fossil_shark_archive("test_file.txt", true, false, false, "zip", NULL, 6, false, NULL);
    ASSUME_ITS_EQUAL_I32(0, result);

    FOSSIL_SANITY_SYS_DELETE_FILE("test_file.txt");
    FOSSIL_SANITY_SYS_DELETE_FILE("test_file.txt.zip");
}

FOSSIL_TEST(c_test_archive_create_tar) {
    FOSSIL_SANITY_SYS_CREATE_FILE("test_doc.txt");
    FOSSIL_SANITY_SYS_WRITE_FILE("test_doc.txt", "archive test data\n");

    int result = fossil_shark_archive("test_doc.txt", true, false, false, "tar", NULL, 0, false, NULL);
    ASSUME_ITS_EQUAL_I32(0, result);

    FOSSIL_SANITY_SYS_DELETE_FILE("test_doc.txt");
    FOSSIL_SANITY_SYS_DELETE_FILE("test_doc.txt.tar");
}

FOSSIL_TEST(c_test_archive_create_tar_gz) {
    FOSSIL_SANITY_SYS_CREATE_FILE("test_compressed.txt");
    FOSSIL_SANITY_SYS_WRITE_FILE("test_compressed.txt", "compressed archive content\n");

    int result = fossil_shark_archive("test_compressed.txt", true, false, false, "gz", NULL, 9, false, NULL);
    ASSUME_ITS_EQUAL_I32(0, result);

    FOSSIL_SANITY_SYS_DELETE_FILE("test_compressed.txt");
    FOSSIL_SANITY_SYS_DELETE_FILE("test_compressed.txt.tar.gz");
}

FOSSIL_TEST(c_test_archive_list_zip) {
    FOSSIL_SANITY_SYS_CREATE_FILE("archive_test.txt");
    FOSSIL_SANITY_SYS_WRITE_FILE("archive_test.txt", "list test\n");

    fossil_shark_archive("archive_test.txt", true, false, false, "zip", NULL, 6, false, NULL);
    int result = fossil_shark_archive("archive_test.txt.zip", false, false, true, "zip", NULL, 0, false, NULL);
    ASSUME_ITS_EQUAL_I32(0, result);

    FOSSIL_SANITY_SYS_DELETE_FILE("archive_test.txt");
    FOSSIL_SANITY_SYS_DELETE_FILE("archive_test.txt.zip");
}

FOSSIL_TEST(c_test_archive_extract_zip) {
    FOSSIL_SANITY_SYS_CREATE_FILE("extract_test.txt");
    FOSSIL_SANITY_SYS_WRITE_FILE("extract_test.txt", "extract me\n");

    fossil_shark_archive("extract_test.txt", true, false, false, "zip", NULL, 6, false, NULL);
    FOSSIL_SANITY_SYS_DELETE_FILE("extract_test.txt");
    
    int result = fossil_shark_archive("extract_test.txt.zip", false, true, false, "zip", NULL, 0, false, NULL);
    ASSUME_ITS_EQUAL_I32(0, result);

    FOSSIL_SANITY_SYS_DELETE_FILE("extract_test.txt.zip");
    FOSSIL_SANITY_SYS_DELETE_FILE("extract_test.txt");
}

FOSSIL_TEST(c_test_archive_no_compression) {
    FOSSIL_SANITY_SYS_CREATE_FILE("no_compress.txt");
    FOSSIL_SANITY_SYS_WRITE_FILE("no_compress.txt", "stored without compression\n");

    int result = fossil_shark_archive("no_compress.txt", true, false, false, "zip", NULL, 0, false, NULL);
    ASSUME_ITS_EQUAL_I32(0, result);

    FOSSIL_SANITY_SYS_DELETE_FILE("no_compress.txt");
    FOSSIL_SANITY_SYS_DELETE_FILE("no_compress.txt.zip");
}

FOSSIL_TEST(c_test_archive_with_exclude_pattern) {
    FOSSIL_SANITY_SYS_CREATE_FILE("include_me.txt");
    FOSSIL_SANITY_SYS_WRITE_FILE("include_me.txt", "include this\n");

    int result = fossil_shark_archive("include_me.txt", true, false, false, "zip", NULL, 6, false, "*.log");
    ASSUME_ITS_EQUAL_I32(0, result);

    FOSSIL_SANITY_SYS_DELETE_FILE("include_me.txt");
    FOSSIL_SANITY_SYS_DELETE_FILE("include_me.txt.zip");
}

FOSSIL_TEST(c_test_archive_stdout_output) {
    FOSSIL_SANITY_SYS_CREATE_FILE("stdout_test.txt");
    FOSSIL_SANITY_SYS_WRITE_FILE("stdout_test.txt", "output to stdout\n");

    int result = fossil_shark_archive("stdout_test.txt", true, false, false, "zip", NULL, 6, true, NULL);
    ASSUME_ITS_EQUAL_I32(0, result);

    FOSSIL_SANITY_SYS_DELETE_FILE("stdout_test.txt");
}

FOSSIL_TEST(c_test_archive_encrypted_zip) {
    FOSSIL_SANITY_SYS_CREATE_FILE("secret.txt");
    FOSSIL_SANITY_SYS_WRITE_FILE("secret.txt", "encrypted content\n");

    int result = fossil_shark_archive("secret.txt", true, false, false, "zip", "mypassword", 6, false, NULL);
    ASSUME_ITS_EQUAL_I32(0, result);

    FOSSIL_SANITY_SYS_DELETE_FILE("secret.txt");
    FOSSIL_SANITY_SYS_DELETE_FILE("secret.txt.zip");
}

FOSSIL_TEST(c_test_archive_max_compression) {
    FOSSIL_SANITY_SYS_CREATE_FILE("max_compress.txt");
    FOSSIL_SANITY_SYS_WRITE_FILE("max_compress.txt", "highly repetitive data repetitive data repetitive\n");

    int result = fossil_shark_archive("max_compress.txt", true, false, false, "zip", NULL, 9, false, NULL);
    ASSUME_ITS_EQUAL_I32(0, result);

    FOSSIL_SANITY_SYS_DELETE_FILE("max_compress.txt");
    FOSSIL_SANITY_SYS_DELETE_FILE("max_compress.txt.zip");
}

FOSSIL_TEST(c_test_archive_extract_tar) {
    FOSSIL_SANITY_SYS_CREATE_FILE("tar_extract.txt");
    FOSSIL_SANITY_SYS_WRITE_FILE("tar_extract.txt", "tar extraction\n");

    fossil_shark_archive("tar_extract.txt", true, false, false, "tar", NULL, 0, false, NULL);
    FOSSIL_SANITY_SYS_DELETE_FILE("tar_extract.txt");
    
    int result = fossil_shark_archive("tar_extract.txt.tar", false, true, false, "tar", NULL, 0, false, NULL);
    ASSUME_ITS_EQUAL_I32(0, result);

    FOSSIL_SANITY_SYS_DELETE_FILE("tar_extract.txt.tar");
    FOSSIL_SANITY_SYS_DELETE_FILE("tar_extract.txt");
}

FOSSIL_TEST(c_test_archive_list_tar_gz) {
    FOSSIL_SANITY_SYS_CREATE_FILE("list_targz.txt");
    FOSSIL_SANITY_SYS_WRITE_FILE("list_targz.txt", "list tar.gz\n");

    fossil_shark_archive("list_targz.txt", true, false, false, "gz", NULL, 9, false, NULL);
    int result = fossil_shark_archive("list_targz.txt.tar.gz", false, false, true, "gz", NULL, 0, false, NULL);
    ASSUME_ITS_EQUAL_I32(0, result);

    FOSSIL_SANITY_SYS_DELETE_FILE("list_targz.txt");
    FOSSIL_SANITY_SYS_DELETE_FILE("list_targz.txt.tar.gz");
}

// * * * * * * * * * * * * * * * * * * * * * * * *
// * Fossil Logic Test Pool
// * * * * * * * * * * * * * * * * * * * * * * * *

FOSSIL_TEST_GROUP(c_archive_command_tests) {
    FOSSIL_TEST_ADD(c_archive_command_suite, c_test_archive_create_zip);
    FOSSIL_TEST_ADD(c_archive_command_suite, c_test_archive_create_tar);
    FOSSIL_TEST_ADD(c_archive_command_suite, c_test_archive_create_tar_gz);
    FOSSIL_TEST_ADD(c_archive_command_suite, c_test_archive_list_zip);
    FOSSIL_TEST_ADD(c_archive_command_suite, c_test_archive_extract_zip);
    FOSSIL_TEST_ADD(c_archive_command_suite, c_test_archive_no_compression);
    FOSSIL_TEST_ADD(c_archive_command_suite, c_test_archive_with_exclude_pattern);
    FOSSIL_TEST_ADD(c_archive_command_suite, c_test_archive_stdout_output);
    FOSSIL_TEST_ADD(c_archive_command_suite, c_test_archive_encrypted_zip);
    FOSSIL_TEST_ADD(c_archive_command_suite, c_test_archive_max_compression);
    FOSSIL_TEST_ADD(c_archive_command_suite, c_test_archive_extract_tar);
    FOSSIL_TEST_ADD(c_archive_command_suite, c_test_archive_list_tar_gz);

    FOSSIL_TEST_REGISTER(c_archive_command_suite);
}
