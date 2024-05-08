#include <criterion/criterion.h>
#include "Config/Parser.hpp"
#include "Cluster.hpp"

char config_file[] = "./configs/default.config";
Parser globalParser(config_file);

void setup(void)
{
	/*Running parser*/
	globalParser.run();
    puts("Setting WebServer\n");
}

void teardown(void) {
    puts("\n\nTeardown: This code runs after the test\n");
}

Test(HandlerContextAddKeyValuesToMap, InvalidDirectiveConfig, .init = setup, .fini = teardown) {
    
   	const std::vector<Config*>& configs = globalParser.get_configs();
	Config *config_ptr;
	for (auto it = configs.begin(); it != configs.end(); ++it)
	{
		config_ptr = *it;
		config_ptr->print_config();
	}
}
