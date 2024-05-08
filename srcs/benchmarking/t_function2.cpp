// #include <criterion/criterion.h>
// #include "Config/Parser.hpp"
// #include "Cluster.hpp"

// char config_file[] = "./configs/handler_context.config";
// Parser globalParser(config_file);

// void setup(void)
// {
// 	/*Running parser*/
// 	globalParser.run();
//     puts("Setting WebServer\n");
// }

// void teardown(void) {
//     puts("\n\nTeardown: This code runs after the test\n");
// }

// Test(HandlerContextAddKeyValuesToMap, InvalidDirectiveConfig, .init = setup, .fini = teardown) {
    
//    	const std::vector<Config*>& configs = globalParser.get_configs();
// 	Config *config_ptr;
// 	for (auto it = configs.begin(); it != configs.end(); ++it)
// 	{
// 		config_ptr = *it;
// 		Config& ref_config = *config_ptr;
// 		HandlerContext handlerContext(ref_config);
//     	/*Testing some function and throw an exception member of the class*/
// 		cr_expect_throw(handlerContext.add_key_values_to_map("root", {"default_root", "dir"}), HandlerContext::InvalidDirectiveConfig);
// 	}
// }

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

struct ConfigParams {
    Config* config;
};

ParameterizedTestParameters(ParserConfigurations, HandlerContextAddKeyValuesToMap) {
	setup();
    const std::vector<Config*>& configs = globalParser->get_configs();
    size_t param_count = configs.size();

    struct ConfigParams* test_params = (struct ConfigParams*)malloc(param_count * sizeof(struct ConfigParams));

    for (size_t i = 0; i < param_count; ++i) {
        test_params[i].config = configs[i];
    }

    return cr_make_param_array(struct ConfigParams, test_params, param_count);
}

ParameterizedTest(struct ConfigParams* param, ParserConfigurations, HandlerContextAddKeyValuesToMap,
                  .init = setup, .fini = teardown) {
    Config& ref_config = *(param->config);
    HandlerContext handlerContext(ref_config);

    cr_expect_throw(handlerContext.add_key_values_to_map("root", {"default_root", "dir"}), HandlerContext::InvalidDirectiveConfig);
}
