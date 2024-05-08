#include <criterion/criterion.h>
#include "Config/Parser.hpp"

/*Testing Parser constructor class*/

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