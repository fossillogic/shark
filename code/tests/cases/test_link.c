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
FOSSIL_TEST_SUITE(c_link_command_suite);

// Setup function for the test suite
FOSSIL_SETUP(c_link_command_suite) {
    // Setup code here
}

// Teardown function for the test suite
FOSSIL_TEARDOWN(c_link_command_suite) {
    // Teardown code here
}

// * * * * * * * * * * * * * * * * * * * * * * * *
// * Fossil Logic Test Cases
// * * * * * * * * * * * * * * * * * * * * * * * *
// The test cases below are provided as samples, inspired
// by the Meson build system's approach of using test cases
// as samples for library usage.
// * * * * * * * * * * * * * * * * * * * * * * * *

FOSSIL_TEST_CASE(c_test_link_null_target) {
    // Null target path should fail
    int result = fossil_shark_link(cnull, "link.txt", false, false, false);
    ASSUME_NOT_EQUAL_I32(0, result);
}

FOSSIL_TEST_CASE(c_test_link_null_link_name) {
    // Null link name should fail
    FILE *file = fopen("link_target.txt", "w");
    ASSUME_NOT_CNULL(file);
    fprintf(file, "Target file\n");
    fclose(file);

    int result = fossil_shark_link("link_target.txt", cnull, false, false, false);
    ASSUME_NOT_EQUAL_I32(0, result);

    remove("link_target.txt");
}

FOSSIL_TEST_CASE(c_test_link_hard_link_success) {
    // Create a file and hard link to it
    FILE *file = fopen("hard_target.txt", "w");
    ASSUME_NOT_CNULL(file);
    fprintf(file, "Hard link target\n");
    fclose(file);

    int result = fossil_shark_link("hard_target.txt", "hard_link.txt", false, false, false);
    ASSUME_EQUAL_I32(0, result);

    // Check both files exist
    FILE *link_file = fopen("hard_link.txt", "r");
    ASSUME_NOT_CNULL(link_file);
    fclose(link_file);

    remove("hard_target.txt");
    remove("hard_link.txt");
}

FOSSIL_TEST_CASE(c_test_link_symbolic_link_success) {
    // Create a file and symlink to it
    FILE *file = fopen("sym_target.txt", "w");
    ASSUME_NOT_CNULL(file);
    fprintf(file, "Symlink target\n");
    fclose(file);

    int result = fossil_shark_link("sym_target.txt", "sym_link.txt", true, false, false);
    ASSUME_EQUAL_I32(0, result);

    // Check symlink exists (lstat)
#ifndef _WIN32
    struct stat st;
    ASSUME_EQUAL_I32(0, lstat("sym_link.txt", &st));
    ASSUME_TRUE(S_ISLNK(st.st_mode));
#endif

    remove("sym_target.txt");
    remove("sym_link.txt");
}

FOSSIL_TEST_CASE(c_test_link_target_does_not_exist) {
    // Target does not exist
    int result = fossil_shark_link("no_such_file.txt", "link.txt", false, false, false);
    ASSUME_NOT_EQUAL_I32(0, result);
}

FOSSIL_TEST_CASE(c_test_link_destination_exists_no_force) {
    // Destination exists, no force
    FILE *file = fopen("dest_exists.txt", "w");
    ASSUME_NOT_CNULL(file);
    fprintf(file, "Existing destination\n");
    fclose(file);

    FILE *target = fopen("target_exists.txt", "w");
    ASSUME_NOT_CNULL(target);
    fprintf(target, "Target file\n");
    fclose(target);

    int result = fossil_shark_link("target_exists.txt", "dest_exists.txt", false, false, false);
    ASSUME_EQUAL_I32(EEXIST, result);

    remove("dest_exists.txt");
    remove("target_exists.txt");
}

FOSSIL_TEST_CASE(c_test_link_destination_exists_with_force) {
    // Destination exists, force overwrite
    FILE *file = fopen("force_dest.txt", "w");
    ASSUME_NOT_CNULL(file);
    fprintf(file, "Existing destination\n");
    fclose(file);

    FILE *target = fopen("force_target.txt", "w");
    ASSUME_NOT_CNULL(target);
    fprintf(target, "Target file\n");
    fclose(target);

    int result = fossil_shark_link("force_target.txt", "force_dest.txt", false, true, false);
    ASSUME_EQUAL_I32(0, result);

    remove("force_dest.txt");
    remove("force_target.txt");
}

FOSSIL_TEST_CASE(c_test_link_hard_link_to_directory) {
    // Create a directory and try to hard link to it
#ifdef _WIN32
    _mkdir("dir_target");
#else
    mkdir("dir_target", 0700);
#endif
    int result = fossil_shark_link("dir_target", "dir_hard_link", false, false, false);
    ASSUME_EQUAL_I32(EPERM, result);

#ifdef _WIN32
    rmdir("dir_target");
#else
    rmdir("dir_target");
#endif
}

FOSSIL_TEST_CASE(c_test_link_symbolic_link_to_directory) {
    // Create a directory and symlink to it
#ifdef _WIN32
    _mkdir("dir_sym_target");
#else
    mkdir("dir_sym_target", 0700);
#endif
    int result = fossil_shark_link("dir_sym_target", "dir_sym_link", true, false, false);
    ASSUME_EQUAL_I32(0, result);

#ifdef _WIN32
    rmdir("dir_sym_target");
    remove("dir_sym_link");
#else
    rmdir("dir_sym_target");
    unlink("dir_sym_link");
#endif
}

FOSSIL_TEST_CASE(c_test_link_no_dereference_symlink) {
    // Create a file and symlink with no_dereference
    FILE *file = fopen("no_deref_target.txt", "w");
    ASSUME_NOT_CNULL(file);
    fprintf(file, "No deref target\n");
    fclose(file);

    int result = fossil_shark_link("no_deref_target.txt", "no_deref_link.txt", true, false, true);
    ASSUME_EQUAL_I32(0, result);

    remove("no_deref_target.txt");
    remove("no_deref_link.txt");
}

// * * * * * * * * * * * * * * * * * * * * * * * *
// * Fossil Logic Test Pool
// * * * * * * * * * * * * * * * * * * * * * * * *

FOSSIL_TEST_GROUP(c_link_command_tests) {
    FOSSIL_TEST_ADD(c_link_command_suite, c_test_link_null_target);
    FOSSIL_TEST_ADD(c_link_command_suite, c_test_link_null_link_name);
    FOSSIL_TEST_ADD(c_link_command_suite, c_test_link_hard_link_success);
    FOSSIL_TEST_ADD(c_link_command_suite, c_test_link_symbolic_link_success);
    FOSSIL_TEST_ADD(c_link_command_suite, c_test_link_target_does_not_exist);
    FOSSIL_TEST_ADD(c_link_command_suite, c_test_link_destination_exists_no_force);
    FOSSIL_TEST_ADD(c_link_command_suite, c_test_link_destination_exists_with_force);
    FOSSIL_TEST_ADD(c_link_command_suite, c_test_link_hard_link_to_directory);
    FOSSIL_TEST_ADD(c_link_command_suite, c_test_link_symbolic_link_to_directory);
    FOSSIL_TEST_ADD(c_link_command_suite, c_test_link_no_dereference_symlink);

    FOSSIL_TEST_REGISTER(c_link_command_suite);
}
