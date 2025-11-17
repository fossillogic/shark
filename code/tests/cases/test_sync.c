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
FOSSIL_TEST_SUITE(c_sync_command_suite);

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

FOSSIL_TEST_CASE(c_test_sync_null_paths) {
    // Null source and destination
    int result = fossil_shark_sync(cnull, cnull, false, false, false);
    ASSUME_NOT_EQUAL_I32(0, result);

    // Null source only
    result = fossil_shark_sync(cnull, "dest.txt", false, false, false);
    ASSUME_NOT_EQUAL_I32(0, result);

    // Null destination only
    result = fossil_shark_sync("src.txt", cnull, false, false, false);
    ASSUME_NOT_EQUAL_I32(0, result);
}

FOSSIL_TEST_CASE(c_test_sync_file_to_file) {
    // Create source file
    FILE *src = fopen("sync_src.txt", "w");
    ASSUME_NOT_CNULL(src);
    fprintf(src, "Sync test content\n");
    fclose(src);

    // Remove destination if exists
    remove("sync_dest.txt");

    // Sync file
    int result = fossil_shark_sync("sync_src.txt", "sync_dest.txt", false, false, false);
    ASSUME_EQUAL_I32(0, result);

    // Check destination exists
    FILE *dest = fopen("sync_dest.txt", "r");
    ASSUME_NOT_CNULL(dest);
    fclose(dest);

    // Clean up
    remove("sync_src.txt");
    remove("sync_dest.txt");
}

FOSSIL_TEST_CASE(c_test_sync_directory_recursive) {
    // Create source directory and files
    mkdir("sync_src_dir", 0755);
    FILE *f1 = fopen("sync_src_dir/file1.txt", "w");
    ASSUME_NOT_CNULL(f1);
    fprintf(f1, "File 1\n");
    fclose(f1);

    mkdir("sync_src_dir/subdir", 0755);
    FILE *f2 = fopen("sync_src_dir/subdir/file2.txt", "w");
    ASSUME_NOT_CNULL(f2);
    fprintf(f2, "File 2\n");
    fclose(f2);

    // Remove destination directory if exists
    remove("sync_dest_dir/file1.txt");
    remove("sync_dest_dir/subdir/file2.txt");
    rmdir("sync_dest_dir/subdir");
    rmdir("sync_dest_dir");

    // Sync recursively
    int result = fossil_shark_sync("sync_src_dir", "sync_dest_dir", true, false, false);
    ASSUME_EQUAL_I32(0, result);

    // Check files exist in destination
    FILE *df1 = fopen("sync_dest_dir/file1.txt", "r");
    ASSUME_NOT_CNULL(df1);
    fclose(df1);

    FILE *df2 = fopen("sync_dest_dir/subdir/file2.txt", "r");
    ASSUME_NOT_CNULL(df2);
    fclose(df2);

    // Clean up
    remove("sync_src_dir/file1.txt");
    remove("sync_src_dir/subdir/file2.txt");
    rmdir("sync_src_dir/subdir");
    rmdir("sync_src_dir");
    remove("sync_dest_dir/file1.txt");
    remove("sync_dest_dir/subdir/file2.txt");
    rmdir("sync_dest_dir/subdir");
    rmdir("sync_dest_dir");
}

FOSSIL_TEST_CASE(c_test_sync_update_flag) {
    // Create source and destination files
    FILE *src = fopen("sync_update_src.txt", "w");
    ASSUME_NOT_CNULL(src);
    fprintf(src, "New content\n");
    fclose(src);

    FILE *dest = fopen("sync_update_dest.txt", "w");
    ASSUME_NOT_CNULL(dest);
    fprintf(dest, "Old content\n");
    fclose(dest);

    // Set destination mtime newer than source
    struct utimbuf times;
    times.actime = times.modtime = time(NULL) + 60;
    utime("sync_update_dest.txt", &times);

    // Sync with update flag (should skip copy)
    int result = fossil_shark_sync("sync_update_src.txt", "sync_update_dest.txt", false, true, false);
    ASSUME_ITS_EQUAL_I32(0, result);

    // Clean up
    remove("sync_update_src.txt");
    remove("sync_update_dest.txt");
}

FOSSIL_TEST_CASE(c_test_sync_delete_extraneous_files) {
    // Create source directory and file
    mkdir("sync_del_src", 0755);
    FILE *srcf = fopen("sync_del_src/file.txt", "w");
    ASSUME_NOT_CNULL(srcf);
    fprintf(srcf, "Keep me\n");
    fclose(srcf);

    // Create destination directory with extra file
    mkdir("sync_del_dest", 0755);
    FILE *destf = fopen("sync_del_dest/file.txt", "w");
    ASSUME_NOT_CNULL(destf);
    fprintf(destf, "Keep me\n");
    fclose(destf);

    FILE *extraf = fopen("sync_del_dest/extra.txt", "w");
    ASSUME_NOT_CNULL(extraf);
    fprintf(extraf, "Delete me\n");
    fclose(extraf);

    // Sync with delete flag
    int result = fossil_shark_sync("sync_del_src", "sync_del_dest", false, false, true);
    ASSUME_ITS_EQUAL_I32(0, result);

    // Check extra file is deleted
    FILE *check = fopen("sync_del_dest/extra.txt", "r");
    ASSUME_ITS_CNULL(check);

    // Clean up
    remove("sync_del_src/file.txt");
    rmdir("sync_del_src");
    remove("sync_del_dest/file.txt");
    rmdir("sync_del_dest");
}

FOSSIL_TEST_CASE(c_test_sync_nonexistent_source) {
    // Sync with non-existent source
    int result = fossil_shark_sync("no_such_file.txt", "dest.txt", false, false, false);
    ASSUME_NOT_EQUAL_I32(0, result);
}

FOSSIL_TEST_CASE(c_test_sync_special_characters_in_filenames) {
    // Create source file with spaces
    FILE *src = fopen("sync src special.txt", "w");
    ASSUME_NOT_CNULL(src);
    fprintf(src, "Special chars\n");
    fclose(src);

    // Remove destination if exists
    remove("sync dest special.txt");

    // Sync file
    int result = fossil_shark_sync("sync src special.txt", "sync dest special.txt", false, false, false);
    ASSUME_ITS_EQUAL_I32(0, result);

    // Check destination exists
    FILE *dest = fopen("sync dest special.txt", "r");
    ASSUME_NOT_CNULL(dest);
    fclose(dest);

    // Clean up
    remove("sync src special.txt");
    remove("sync dest special.txt");
}

// * * * * * * * * * * * * * * * * * * * * * * * *
// * Fossil Logic Test Pool
// * * * * * * * * * * * * * * * * * * * * * * * *

FOSSIL_TEST_GROUP(c_sync_command_tests) {
    FOSSIL_TEST_ADD(c_sync_command_suite, c_test_sync_null_paths);
    FOSSIL_TEST_ADD(c_sync_command_suite, c_test_sync_file_to_file);
    FOSSIL_TEST_ADD(c_sync_command_suite, c_test_sync_directory_recursive);
    FOSSIL_TEST_ADD(c_sync_command_suite, c_test_sync_update_flag);
    FOSSIL_TEST_ADD(c_sync_command_suite, c_test_sync_delete_extraneous_files);
    FOSSIL_TEST_ADD(c_sync_command_suite, c_test_sync_nonexistent_source);
    FOSSIL_TEST_ADD(c_sync_command_suite, c_test_sync_special_characters_in_filenames);

    FOSSIL_TEST_REGISTER(c_sync_command_suite);
}
