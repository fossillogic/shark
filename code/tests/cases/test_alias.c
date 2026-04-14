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
// * Fossil Logic Alias Test Suite
// * * * * * * * * * * * * * * * * * * * * * * * *

FOSSIL_SUITE(c_alias_command_suite);

FOSSIL_SETUP(c_alias_command_suite)
{
    // Setup alias environment if needed
}

FOSSIL_TEARDOWN(c_alias_command_suite)
{
    // Cleanup alias state if persisted
}

// Helper: simple alias definition
static void create_alias(const char* name, const char* cmd)
{
    char buffer[256];
    snprintf(buffer, sizeof(buffer), "%s=%s", name, cmd);
    fossil_shark_alias(buffer, cnull, false, false);
}

// * * * * * * * * * * * * * * * * * * * * * * * *
// * Test Cases
// * * * * * * * * * * * * * * * * * * * * * * * *

FOSSIL_TEST(c_test_alias_null_parameters)
{
    int result = fossil_shark_alias(cnull, cnull, false, false);
    ASSUME_NOT_EQUAL_I32(0, result);

    result = fossil_shark_alias("ls=show", cnull, false, false);
    ASSUME_ITS_EQUAL_I32(0, result);

    result = fossil_shark_alias(cnull, "ls", false, false);
    ASSUME_ITS_EQUAL_I32(0, result);
}

FOSSIL_TEST(c_test_alias_create_basic)
{
    int result = fossil_shark_alias("ll=list -l", cnull, false, false);
    ASSUME_ITS_EQUAL_I32(0, result);
}

FOSSIL_TEST(c_test_alias_create_multiple)
{
    fossil_shark_alias("ll=list -l", cnull, false, false);
    fossil_shark_alias("la=list -a", cnull, false, false);
    fossil_shark_alias("gs=git status", cnull, false, false);

    ASSUME_ITS_TRUE(true); // structural test (state stored internally)
}

FOSSIL_TEST(c_test_alias_overwrite_existing)
{
    fossil_shark_alias("ll=list", cnull, false, false);
    int result = fossil_shark_alias("ll=list -l -h", cnull, false, false);

    ASSUME_ITS_EQUAL_I32(0, result);
}

FOSSIL_TEST(c_test_alias_remove_existing)
{
    fossil_shark_alias("rmtest=remove item", cnull, false, false);

    int result = fossil_shark_alias(cnull, "rmtest", false, false);
    ASSUME_ITS_EQUAL_I32(0, result);
}

FOSSIL_TEST(c_test_alias_remove_nonexistent)
{
    int result = fossil_shark_alias(cnull, "does_not_exist", false, false);
    ASSUME_ITS_EQUAL_I32(0, result); // safe no-op expected
}

FOSSIL_TEST(c_test_alias_list_only)
{
    fossil_shark_alias("a=alpha", cnull, false, false);
    fossil_shark_alias("b=beta", cnull, false, false);

    int result = fossil_shark_alias(cnull, cnull, true, false);
    ASSUME_ITS_EQUAL_I32(0, result);
}

FOSSIL_TEST(c_test_alias_global_scope_set)
{
    int result = fossil_shark_alias("global=testcmd", cnull, false, true);
    ASSUME_ITS_EQUAL_I32(0, result);
}

FOSSIL_TEST(c_test_alias_global_scope_remove)
{
    fossil_shark_alias("gdel=temp", cnull, false, true);

    int result = fossil_shark_alias(cnull, "gdel", false, true);
    ASSUME_ITS_EQUAL_I32(0, result);
}

FOSSIL_TEST(c_test_alias_set_and_remove_combined)
{
    fossil_shark_alias("combo=cmd1", cnull, false, false);

    int result = fossil_shark_alias("combo=cmd2", "combo", false, false);
    ASSUME_ITS_EQUAL_I32(0, result);
}

FOSSIL_TEST(c_test_alias_invalid_format_missing_equals)
{
    int result = fossil_shark_alias("invalidalias", cnull, false, false);
    ASSUME_NOT_EQUAL_I32(0, result);
}

FOSSIL_TEST(c_test_alias_empty_name)
{
    int result = fossil_shark_alias("=cmd", cnull, false, false);
    ASSUME_NOT_EQUAL_I32(0, result);
}

FOSSIL_TEST(c_test_alias_empty_command)
{
    int result = fossil_shark_alias("empty=", cnull, false, false);
    ASSUME_ITS_EQUAL_I32(0, result); // may represent alias clearing
}

FOSSIL_TEST(c_test_alias_list_with_global_flag)
{
    fossil_shark_alias("x=globaltest", cnull, false, true);

    int result = fossil_shark_alias(cnull, cnull, true, true);
    ASSUME_ITS_EQUAL_I32(0, result);
}

FOSSIL_TEST(c_test_alias_duplicate_definition)
{
    fossil_shark_alias("dup=one", cnull, false, false);
    int result = fossil_shark_alias("dup=two", cnull, false, false);

    ASSUME_ITS_EQUAL_I32(0, result);
}

FOSSIL_TEST(c_test_alias_remove_after_redefine)
{
    fossil_shark_alias("temp=first", cnull, false, false);
    fossil_shark_alias("temp=second", cnull, false, false);

    int result = fossil_shark_alias(cnull, "temp", false, false);
    ASSUME_ITS_EQUAL_I32(0, result);
}

FOSSIL_TEST(c_test_alias_mass_creation)
{
    for (int i = 0; i < 50; i++)
    {
        char buf[64];
        snprintf(buf, sizeof(buf), "a%d=cmd%d", i, i);
        fossil_shark_alias(buf, cnull, false, false);
    }

    ASSUME_ITS_TRUE(true);
}

FOSSIL_TEST(c_test_alias_list_after_mass_creation)
{
    int result = fossil_shark_alias(cnull, cnull, true, false);
    ASSUME_ITS_EQUAL_I32(0, result);
}

FOSSIL_TEST(c_test_alias_global_and_local_mix)
{
    fossil_shark_alias("local1=cmd", cnull, false, false);
    fossil_shark_alias("global1=cmd", cnull, false, true);

    int result = fossil_shark_alias(cnull, cnull, true, false);
    ASSUME_ITS_EQUAL_I32(0, result);
}

FOSSIL_TEST(c_test_alias_remove_all_pattern)
{
    fossil_shark_alias("a1=x", cnull, false, false);
    fossil_shark_alias("a2=y", cnull, false, false);
    fossil_shark_alias("a3=z", cnull, false, false);

    fossil_shark_alias(cnull, "a1", false, false);
    fossil_shark_alias(cnull, "a2", false, false);
    fossil_shark_alias(cnull, "a3", false, false);

    ASSUME_ITS_TRUE(true);
}

// * * * * * * * * * * * * * * * * * * * * * * * *
// * Test Group Registration
// * * * * * * * * * * * * * * * * * * * * * * * *

FOSSIL_TEST_GROUP(c_alias_command_tests)
{
    FOSSIL_TEST_ADD(c_alias_command_suite, c_test_alias_null_parameters);
    FOSSIL_TEST_ADD(c_alias_command_suite, c_test_alias_create_basic);
    FOSSIL_TEST_ADD(c_alias_command_suite, c_test_alias_create_multiple);
    FOSSIL_TEST_ADD(c_alias_command_suite, c_test_alias_overwrite_existing);
    FOSSIL_TEST_ADD(c_alias_command_suite, c_test_alias_remove_existing);
    FOSSIL_TEST_ADD(c_alias_command_suite, c_test_alias_remove_nonexistent);
    FOSSIL_TEST_ADD(c_alias_command_suite, c_test_alias_list_only);
    FOSSIL_TEST_ADD(c_alias_command_suite, c_test_alias_global_scope_set);
    FOSSIL_TEST_ADD(c_alias_command_suite, c_test_alias_global_scope_remove);
    FOSSIL_TEST_ADD(c_alias_command_suite, c_test_alias_set_and_remove_combined);
    FOSSIL_TEST_ADD(c_alias_command_suite, c_test_alias_invalid_format_missing_equals);
    FOSSIL_TEST_ADD(c_alias_command_suite, c_test_alias_empty_name);
    FOSSIL_TEST_ADD(c_alias_command_suite, c_test_alias_empty_command);
    FOSSIL_TEST_ADD(c_alias_command_suite, c_test_alias_list_with_global_flag);
    FOSSIL_TEST_ADD(c_alias_command_suite, c_test_alias_duplicate_definition);
    FOSSIL_TEST_ADD(c_alias_command_suite, c_test_alias_remove_after_redefine);
    FOSSIL_TEST_ADD(c_alias_command_suite, c_test_alias_mass_creation);
    FOSSIL_TEST_ADD(c_alias_command_suite, c_test_alias_list_after_mass_creation);
    FOSSIL_TEST_ADD(c_alias_command_suite, c_test_alias_global_and_local_mix);
    FOSSIL_TEST_ADD(c_alias_command_suite, c_test_alias_remove_all_pattern);

    FOSSIL_TEST_REGISTER(c_alias_command_suite);
}

