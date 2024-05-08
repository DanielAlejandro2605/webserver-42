#include <criterion/criterion.h>
#include "Config/Parser.hpp"
#include "Cluster.hpp"

/*According to https://criterion.readthedocs.io/en/master/internal.html*/

Test(server_response, test_get_request) {
    std::cout << "Test!" << std::endl;
}


int main(int argc, char *argv[]) {
    struct criterion_test_set *tests = criterion_initialize();

    int result = 0;
    if (criterion_handle_args(argc, argv, true))
        result = !criterion_run_all_tests(tests);

    criterion_finalize(tests);
    return result;
}