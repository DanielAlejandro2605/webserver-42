#include <criterion/criterion.h>
#include <curl/curl.h>
#include "Config/Parser.hpp"
#include "Cluster.hpp"

/*According to https://criterion.readthedocs.io/en/master/internal.html*/

// Structure to store the response received from the server
struct MemoryStruct {
    char *memory;
    size_t size;
};

// Callback function to handle writing data for libcurl
static size_t write_memory_callback(void *contents, size_t size, size_t nmemb, void *userp) {
    size_t realsize = size * nmemb;
    struct MemoryStruct *mem = (struct MemoryStruct *)userp;

    char *ptr = (char *)realloc(mem->memory, mem->size + realsize + 1);
    if (ptr == NULL) {
        // Return 0 to abort the transfer in case of error
        return 0;
    }

    mem->memory = ptr;
    memcpy(&(mem->memory[mem->size]), contents, realsize);
    mem->size += realsize;
    mem->memory[mem->size] = 0;

    return realsize;
}

// Function to send a GET request to the server and return the response
char *send_get_request() {
    // Initialize libcurl
    curl_global_init(CURL_GLOBAL_ALL);

    // Initialize curl handle
    CURL *curl = curl_easy_init();
    if (curl) {
        struct MemoryStruct chunk;
        chunk.memory = (char *)malloc(1);  // Initialize memory buffer
        chunk.size = 0;

        // Set the URL for the GET request
        curl_easy_setopt(curl, CURLOPT_URL, "http://127.0.0.1:8080/my-loc/index.html");

        // Set the write data function to handle the response
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_memory_callback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&chunk);

        // Perform the HTTP GET request
        CURLcode res = curl_easy_perform(curl);
        if (res != CURLE_OK) {
            // Print an error message if there's an error in the request
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
            return NULL;
        }

        // Cleanup and close the curl handle
        curl_easy_cleanup(curl);

        // Finalize libcurl
        curl_global_cleanup();

        // Return the received response
        return chunk.memory;
    }

    // Finalize libcurl in case of error
    curl_global_cleanup();
    return NULL;
}

// Test case to verify the server's response to a GET request
Test(server_response, test_get_request) {
    // Send a GET request to the server and receive the response
    char *response = send_get_request();
    cr_assert(response != NULL, "Failed to receive server response");

    // Define the expected response
    const char *expected_response = "<!DOCTYPE html><html><head>    <meta charset=\"UTF-8\">    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">    <title>Home - Webserv</title>    <link type=\"image/png\" rel=\"icon\" href=\"./skull.png\">    <style>        * {            margin: 0;            padding: 0;            box-sizing: border-box;        }        body {            font-family: 'Arial', sans-serif;            line-height: 1.6;            margin: 0;            color: #333;            background-color: #f4f4f4;            display: flex;            flex-direction: column;            min-height: 100vh;        }        header {            background: #1E212A;            color: #fff;            padding: 1rem 0;            text-align: center;            width: 100%;        }        header h1 {            margin: 0;        }        nav ul {            list-style: none;            margin: 0;            padding: 0;            padding-top: 10px;        }        nav ul li {            display: inline;            margin-right: 20px;        }        nav ul li a {            color: #fff;            text-decoration: none;            font-weight: bold;        }        nav ul li a:hover {            color: #ddd;        }                section {            margin: 20px 0;            padding: 20px;            align-items: center;            justify-content: center;            background-color: #f0f0f0;            border-radius: 10px;            box-shadow: 0 4px 8px rgba(0, 0, 0, 0.1);            flex: 1;            display: flex;            flex-direction: column;        }        h2 {            margin-bottom: 10px;        }        p, a {            text-decoration: none;            margin: 15px 0;            font-size: 18px;        }        a:visited {            color: inherit;        }        p a:hover {            color: #1c52b4;            text-decoration: underline;        }        footer {            background: #1E212A;            color: #fff;            text-align: center;            padding: 20px;            bottom: 0;            width: 100%;        }    </style></head><body>    <header>        <h1>Webserv</h1>        <nav>            <ul>                <li><a href=\"./index.html\">Home</a></li>                <li><a href=\"./contact.html\">Upload</a></li>            </ul>        </nav>    </header>    <section>        <h2>Welcome to our Webserv</h2>        <p>This is our awesome and beautiful Webserv, please treat him nicely 🤟 </p>        <p>Coded by <a href=\"https://profile.intra.42.fr/users/dnieto-c\">dnieto-c</a> and <a href=\"https://profile.intra.42.fr/users/mflores-\">mflores-</a></p>    </section>    <footer>        <p>Copyright © 2024 Webserv wey</p>    </footer></body></html>";

    // Check if the received response matches the expected response
    cr_assert_str_eq(response, expected_response, "Received response does not match expected response");
    free(response);  // Free the memory allocated for the response
}

int main(int argc, char *argv[]) {
    // Initialize Criterion
    struct criterion_test_set *tests = criterion_initialize();

    // Parse command line arguments and run the tests
    int result = 0;
    if (criterion_handle_args(argc, argv, true))
        result = !criterion_run_all_tests(tests);

    // Finalize Criterion
    criterion_finalize(tests);
    return result;
}
