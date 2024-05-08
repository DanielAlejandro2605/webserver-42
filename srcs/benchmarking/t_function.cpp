#include <criterion/criterion.h>
#include <criterion/parameterized.h>
#include "Config/Parser.hpp"
#include "Cluster.hpp"

Parser* globalParser = nullptr;
char config_file[] = "./configs/handler_context.config";

void setup(void)
{
    globalParser = new Parser(config_file);
    globalParser->run();
    puts("Setting WebServer\n");
}

void teardown(void) {
    delete globalParser;
    puts("\n\nTeardown: This code runs after the test\n");
}
