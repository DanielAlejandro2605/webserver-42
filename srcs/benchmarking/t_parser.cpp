#include <criterion/criterion.h>
#include "Config/Parser.hpp"
#include "Cluster.hpp"


// Test(ParserConstructor, InitializeMembers) {
//     cr_log_info("This is an informational message. They are not displayed "
//         "by default.");
//     cr_log_warn("This is a warning. They indicate some possible malfunction "
//         "or misconfiguration in the test.");
//     cr_log_error("This is an error. They indicate serious problems and "
//         "are usually shown before the test is aborted.");

//     char path[] = "test_file.txt";
//     Parser parser(path);

//     cr_assert_str_eq(parser.get_path_file().c_str(), path);
//     cr_assert_eq(parser.get_brackets_closed(), 0);
//     hasConfigExtension(this->_path_file)
// }

// void setup(void) {
//     puts("Runs before the test");
// }

// void teardown(void) {
//     puts("Runs after the test");
// }

Test(ParserHasConfigExtension, ValidExtension) {
    char path[] = "file.config";
    Parser parser(path);
    bool result = parser.hasConfigExtension(parser.get_path_file());
    cr_assert(result, "Expected true for a valid extension.");
}

Test(ParserHasConfigExtension, InvalidExtension) {
    char path[] = "file.txt";
    Parser parser(path);
    bool result = parser.hasConfigExtension(parser.get_path_file());
    cr_assert_not(result, "Expected false for an invalid extension.");
}

Test(ParserHasConfigExtension, NoExtension) {
    char path[] = "file";
    Parser parser(path);
    bool result = parser.hasConfigExtension(parser.get_path_file());
    cr_assert_not(result, "Expected false for a filename without extension.");
}