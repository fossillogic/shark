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
FOSSIL_TEST_SUITE(c_view_command_suite);

// Setup function for the test suite
FOSSIL_SETUP(c_view_command_suite) {
    // Setup code here
}

// Teardown function for the test suite
FOSSIL_TEARDOWN(c_view_command_suite) {
    // Teardown code here
}

// * * * * * * * * * * * * * * * * * * * * * * * *
// * Fossil Logic Test Cases
// * * * * * * * * * * * * * * * * * * * * * * * *
// The test cases below are provided as samples, inspired
// by the Meson build system's approach of using test cases
// as samples for library usage.
// * * * * * * * * * * * * * * * * * * * * * * * *

FOSSIL_TEST_CASE(c_test_view_media_file_jpg) {
    FILE *temp = fopen("test_image.jpg", "wb");
    ASSUME_NOT_CNULL(temp);
    // Write minimal JPEG header (binary)
    unsigned char jpg_header[] = {0xFF, 0xD8, 0xFF, 0xE0, 0x00, 0x10, 0x4A, 0x46, 0x49, 0x46, 0x00, 0x01};
    fwrite(jpg_header, 1, sizeof(jpg_header), temp);
    fclose(temp);

    int result = fossil_shark_view("test_image.jpg", false, false, false, 0, 0, false);
    ASSUME_ITS_EQUAL_I32(0, result);

    remove("test_image.jpg");
}

FOSSIL_TEST_CASE(c_test_view_media_file_mp3) {
    FILE *temp = fopen("test_audio.mp3", "wb");
    ASSUME_NOT_CNULL(temp);
    unsigned char mp3_header[] = {0x49, 0x44, 0x33, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x21};
    fwrite(mp3_header, 1, sizeof(mp3_header), temp);
    fclose(temp);

    int result = fossil_shark_view("test_audio.mp3", false, false, false, 0, 0, false);
    ASSUME_ITS_EQUAL_I32(0, result);

    remove("test_audio.mp3");
}

FOSSIL_TEST_CASE(c_test_view_binary_file) {
    FILE *temp = fopen("test_binary.bin", "wb");
    ASSUME_NOT_CNULL(temp);
    unsigned char bin_data[] = {0x00, 0x01, 0x02, 0x03, 0x04};
    fwrite(bin_data, 1, sizeof(bin_data), temp);
    fclose(temp);

    int result = fossil_shark_view("test_binary.bin", false, false, false, 0, 0, false);
    ASSUME_ITS_EQUAL_I32(0, result);

    remove("test_binary.bin");
}

FOSSIL_TEST_CASE(c_test_view_code_file_c) {
    FILE *temp = fopen("test_code.c", "w");
    ASSUME_NOT_CNULL(temp);
    fprintf(temp, "#include <stdio.h>\nint main() { return 0; }\n");
    fclose(temp);

    int result = fossil_shark_view("test_code.c", false, false, false, 0, 0, false);
    ASSUME_ITS_EQUAL_I32(0, result);

    remove("test_code.c");
}

FOSSIL_TEST_CASE(c_test_view_structured_file_json) {
    FILE *temp = fopen("test_structured.json", "w");
    ASSUME_NOT_CNULL(temp);
    fprintf(temp, "{ \"key\": true, \"value\": null }\n");
    fclose(temp);

    int result = fossil_shark_view("test_structured.json", false, false, false, 0, 0, false);
    ASSUME_ITS_EQUAL_I32(0, result);

    remove("test_structured.json");
}

FOSSIL_TEST_CASE(c_test_view_meson_file) {
    FILE *temp = fopen("meson.build", "w");
    ASSUME_NOT_CNULL(temp);
    fprintf(temp, "project('demo', 'c')\nexecutable('app', 'main.c')\n");
    fclose(temp);

    int result = fossil_shark_view("meson.build", false, false, false, 0, 0, false);
    ASSUME_ITS_EQUAL_I32(0, result);

    remove("meson.build");
}

FOSSIL_TEST_CASE(c_test_view_structured_and_code_file) {
    FILE *temp = fopen("test_structured_code.json", "w");
    ASSUME_NOT_CNULL(temp);
    fprintf(temp, "{ \"main\": \"int main() { return 0; }\" }\n");
    fclose(temp);

    int result = fossil_shark_view("test_structured_code.json", false, false, false, 0, 0, false);
    ASSUME_ITS_EQUAL_I32(0, result);

    remove("test_structured_code.json");
}

FOSSIL_TEST_CASE(c_test_view_structured_and_meson_file) {
    FILE *temp = fopen("meson.options", "w");
    ASSUME_NOT_CNULL(temp);
    fprintf(temp, "option('feature', type: 'boolean', value: true)\n");
    fclose(temp);

    int result = fossil_shark_view("meson.options", false, false, false, 0, 0, false);
    ASSUME_ITS_EQUAL_I32(0, result);

    remove("meson.options");
}

FOSSIL_TEST_CASE(c_test_view_plain_text_no_extension) {
    FILE *temp = fopen("plainfile", "w");
    ASSUME_NOT_CNULL(temp);
    fprintf(temp, "Just some plain text.\nAnother line.\n");
    fclose(temp);

    int result = fossil_shark_view("plainfile", false, false, false, 0, 0, false);
    ASSUME_ITS_EQUAL_I32(0, result);

    remove("plainfile");
}

FOSSIL_TEST_CASE(c_test_view_csv_file) {
    FILE *temp = fopen("test.csv", "w");
    ASSUME_NOT_CNULL(temp);
    fprintf(temp, "name,age\nAlice,30\nBob,25\n");
    fclose(temp);

    int result = fossil_shark_view("test.csv", false, false, false, 0, 0, false);
    ASSUME_ITS_EQUAL_I32(0, result);

    remove("test.csv");
}

FOSSIL_TEST_CASE(c_test_view_html_file) {
    FILE *temp = fopen("test.html", "w");
    ASSUME_NOT_CNULL(temp);
    fprintf(temp, "<html><body><h1>Hello</h1></body></html>\n");
    fclose(temp);

    int result = fossil_shark_view("test.html", false, false, false, 0, 0, false);
    ASSUME_ITS_EQUAL_I32(0, result);

    remove("test.html");
}

FOSSIL_TEST_CASE(c_test_view_yaml_file) {
    FILE *temp = fopen("test.yaml", "w");
    ASSUME_NOT_CNULL(temp);
    fprintf(temp, "key: value\nflag: true\n");
    fclose(temp);

    int result = fossil_shark_view("test.yaml", false, false, false, 0, 0, false);
    ASSUME_ITS_EQUAL_I32(0, result);

    remove("test.yaml");
}

FOSSIL_TEST_CASE(c_test_view_ini_file) {
    FILE *temp = fopen("test.ini", "w");
    ASSUME_NOT_CNULL(temp);
    fprintf(temp, "[section]\nkey=value\n");
    fclose(temp);

    int result = fossil_shark_view("test.ini", false, false, false, 0, 0, false);
    ASSUME_ITS_EQUAL_I32(0, result);

    remove("test.ini");
}

FOSSIL_TEST_CASE(c_test_view_markdown_file) {
    FILE *temp = fopen("test.md", "w");
    ASSUME_NOT_CNULL(temp);
    fprintf(temp, "# Heading\n- List item\n`inline code`\n");
    fclose(temp);

    int result = fossil_shark_view("test.md", false, false, false, 0, 0, false);
    ASSUME_ITS_EQUAL_I32(0, result);

    remove("test.md");
}

// * * * * * * * * * * * * * * * * * * * * * * * *
// * Fossil Logic Test Pool
// * * * * * * * * * * * * * * * * * * * * * * * *

FOSSIL_TEST_GROUP(c_view_command_tests) {
    FOSSIL_TEST_ADD(c_view_command_suite, c_test_view_media_file_jpg);
    FOSSIL_TEST_ADD(c_view_command_suite, c_test_view_media_file_mp3);
    FOSSIL_TEST_ADD(c_view_command_suite, c_test_view_binary_file);
    FOSSIL_TEST_ADD(c_view_command_suite, c_test_view_code_file_c);
    FOSSIL_TEST_ADD(c_view_command_suite, c_test_view_structured_file_json);
    FOSSIL_TEST_ADD(c_view_command_suite, c_test_view_meson_file);
    FOSSIL_TEST_ADD(c_view_command_suite, c_test_view_structured_and_code_file);
    FOSSIL_TEST_ADD(c_view_command_suite, c_test_view_structured_and_meson_file);
    FOSSIL_TEST_ADD(c_view_command_suite, c_test_view_plain_text_no_extension);
    FOSSIL_TEST_ADD(c_view_command_suite, c_test_view_csv_file);
    FOSSIL_TEST_ADD(c_view_command_suite, c_test_view_html_file);
    FOSSIL_TEST_ADD(c_view_command_suite, c_test_view_yaml_file);
    FOSSIL_TEST_ADD(c_view_command_suite, c_test_view_ini_file);
    FOSSIL_TEST_ADD(c_view_command_suite, c_test_view_markdown_file);

    FOSSIL_TEST_REGISTER(c_view_command_suite);
}
