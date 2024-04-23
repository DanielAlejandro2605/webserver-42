#ifndef TOKENS_HPP
# define TOKENS_HPP

# include <iostream>
# include <cstdlib>
# include <string>
# include <vector>
# include <map>
# include <algorithm>
# include <iomanip>
# include <sstream>
# include <limits>
# include <sys/types.h>
# include <dirent.h>
# include <fstream>
# include <unistd.h>
# include <sys/socket.h>
# include <sys/select.h>
# include <poll.h>
# include <netdb.h>
# include <cstring>
# include <fcntl.h>
# include <signal.h>
# include <errno.h>
# include <sys/wait.h>
# include <sys/stat.h>
# include <ctime>
# include <typeinfo>


# include "utils/colors.hpp"
# include "utils/httpCodes.hpp"
# include "utils/utils.hpp"

# define BUFFER_SIZE        BUFSIZ
# define TIMEOUT            5
# define TIMEOUT_CGI        10
# define FILE_EXTENSION     "config"
/*Tokens*/
enum Tokens
{
    CURLY_BRACE_OPEN = '{',
    CURLY_BRACE_CLOSE = '}',
    SEMICOLON = ';',
    HASHTAG = '#',
    STRING = 1,
    COLOR = 2,
};

enum limits
{
    UNSIGNED_SHORT,
    CHAR,
    MAX_BYTES
};

enum context_type
{
    LOCATION_CONTEXT,
    CGI_CONTEXT
};


/*Typedefs for the project*/

/*Context Parsing*/
typedef std::vector<std::string> 							string_vec;
typedef std::map<std::string, string_vec> 					string_vec_map;
typedef string_vec_map::iterator							string_vec_map_it;

typedef std::vector<std::pair<int, std::string> >			tokens_pairs;
typedef std::vector<std::pair<int, std::string> >::iterator it_tokens;

/*Request parsing*/
typedef std::map<std::string, std::string>::const_iterator  const_str_map_it;
typedef const std::map<std::string, std::string> &          const_string_map;
typedef std::map<std::string, std::string>::iterator        str_map_it; // map iterator
typedef std::map<std::string, std::string> 	                string_map; // [token, string]

#endif