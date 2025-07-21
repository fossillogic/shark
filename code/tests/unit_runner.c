
// Generated Fossil Logic Test Runner
#include <fossil/pizza/framework.h>

// * * * * * * * * * * * * * * * * * * * * * * * *
// * Fossil Logic Test List
// * * * * * * * * * * * * * * * * * * * * * * * *
FOSSIL_TEST_EXPORT(c_copy_command_tests);
FOSSIL_TEST_EXPORT(c_delete_command_tests);
FOSSIL_TEST_EXPORT(c_list_command_tests);
FOSSIL_TEST_EXPORT(c_search_command_tests);
FOSSIL_TEST_EXPORT(c_move_command_tests);
FOSSIL_TEST_EXPORT(c_sample_tests);

// * * * * * * * * * * * * * * * * * * * * * * * *
// * Fossil Logic Test Runner
// * * * * * * * * * * * * * * * * * * * * * * * *
int main(int argc, char **argv) {
    FOSSIL_TEST_START(argc, argv);
    FOSSIL_TEST_IMPORT(c_copy_command_tests);
    FOSSIL_TEST_IMPORT(c_delete_command_tests);
    FOSSIL_TEST_IMPORT(c_list_command_tests);
    FOSSIL_TEST_IMPORT(c_search_command_tests);
    FOSSIL_TEST_IMPORT(c_move_command_tests);
    FOSSIL_TEST_IMPORT(c_sample_tests);

    FOSSIL_RUN_ALL();
    FOSSIL_SUMMARY();
    return FOSSIL_END();
} // end of main
