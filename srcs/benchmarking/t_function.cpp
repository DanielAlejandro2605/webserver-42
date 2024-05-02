#include <criterion/criterion.h>
#include "Config/Parser.hpp"
#include "Cluster.hpp"


// Test(ParserConstructor, InitializeMembers) {
//     // cr_log_info("This is an informational message. They are not displayed "
//     //     "by default.");
//     // cr_log_warn("This is a warning. They indicate some possible malfunction "
//     //     "or misconfiguration in the test.");
//     // cr_log_error("This is an error. They indicate serious problems and "
//     //     "are usually shown before the test is aborted.");

//     char path[] = "test_file.txt";
//     Parser parser(path);

//     cr_assert_str_eq(parser.get_path_file().c_str(), path);
//     cr_assert_eq(parser.get_brackets_closed(), 0);
// }

// Test(ParserConstructor2, InitializeMembers) {
//     // cr_log_info("This is an informational message. They are not displayed "
//     //     "by default.");
//     // cr_log_warn("This is a warning. They indicate some possible malfunction "
//     //     "or misconfiguration in the test.");
//     // cr_log_error("This is an error. They indicate serious problems and "
//     //     "are usually shown before the test is aborted.");

//     char path[] = "test_file.txt";
//     Parser parser(path);

//     cr_assert_str_eq(parser.get_path_file().c_str(), path);
//     cr_assert_eq(parser.get_brackets_closed(), 0);
// }

#include <criterion/criterion.h>
#include "Config/Parser.hpp"

// Declarar una variable global del tipo Config
Config globalConfig;

// Se ejecuta antes de cada prueba
void setup(void) {
    // Inicializar la variable global Config
    // Puedes establecer cualquier configuración necesaria aquí
    globalConfig = Config();
    puts("Setup: Runs before the test\n");
}

// Se ejecuta después de cada prueba
void teardown(void) {
    // No se requiere limpieza adicional en este caso
    puts("Teardown: Runs after the test\n");
}

// Prueba
Test(HandlerContextAddKeyValuesToMap, InvalidDirectiveConfig, .init = setup, .fini = teardown) {
    // Configuración inicial del entorno de prueba
    HandlerContext handlerContext(globalConfig);

    // Llamada a la función bajo prueba con una directiva inválida para el contexto de configuración
    cr_expect_throw(handlerContext.add_key_values_to_map("invalid_directive", {"value"}),
                     HandlerContext::InvalidDirectiveConfig);
}

