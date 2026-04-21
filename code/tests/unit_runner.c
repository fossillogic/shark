
// Generated Fossil Logic Test Runner
#include <fossil/pizza/framework.h>

// * * * * * * * * * * * * * * * * * * * * * * * *
// * Fossil Logic Test List
// * * * * * * * * * * * * * * * * * * * * * * * *
FOSSIL_TEST_EXPORT(c_grammer_command_tests);
FOSSIL_TEST_EXPORT(c_split_command_tests);
FOSSIL_TEST_EXPORT(c_search_command_tests);
FOSSIL_TEST_EXPORT(c_move_command_tests);
FOSSIL_TEST_EXPORT(c_create_command_tests);
FOSSIL_TEST_EXPORT(c_rewrite_command_tests);
FOSSIL_TEST_EXPORT(c_swap_command_tests);
FOSSIL_TEST_EXPORT(c_merge_command_tests);
FOSSIL_TEST_EXPORT(c_sync_command_tests);
FOSSIL_TEST_EXPORT(c_compare_command_tests);
FOSSIL_TEST_EXPORT(c_cryptic_command_tests);
FOSSIL_TEST_EXPORT(c_rename_command_tests);
FOSSIL_TEST_EXPORT(c_dedupe_command_tests);
FOSSIL_TEST_EXPORT(c_remove_command_tests);

// * * * * * * * * * * * * * * * * * * * * * * * *
// * Fossil Logic Test Runner
// * * * * * * * * * * * * * * * * * * * * * * * *
int main(int argc, char **argv) {
    FOSSIL_TEST_START(argc, argv);
    FOSSIL_TEST_IMPORT(c_grammer_command_tests);
    FOSSIL_TEST_IMPORT(c_split_command_tests);
    FOSSIL_TEST_IMPORT(c_search_command_tests);
    FOSSIL_TEST_IMPORT(c_move_command_tests);
    FOSSIL_TEST_IMPORT(c_create_command_tests);
    FOSSIL_TEST_IMPORT(c_rewrite_command_tests);
    FOSSIL_TEST_IMPORT(c_swap_command_tests);
    FOSSIL_TEST_IMPORT(c_merge_command_tests);
    FOSSIL_TEST_IMPORT(c_sync_command_tests);
    FOSSIL_TEST_IMPORT(c_compare_command_tests);
    FOSSIL_TEST_IMPORT(c_cryptic_command_tests);
    FOSSIL_TEST_IMPORT(c_rename_command_tests);
    FOSSIL_TEST_IMPORT(c_dedupe_command_tests);
    FOSSIL_TEST_IMPORT(c_remove_command_tests);

    FOSSIL_RUN_ALL();
    FOSSIL_SUMMARY();
    return FOSSIL_END();
} // end of main
