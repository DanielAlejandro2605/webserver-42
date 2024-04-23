#include "utils/utils.hpp"

std::string     int_to_string(int value)
{
    std::stringstream ss;

    ss << value;
    return (ss.str());
}

void    log(std::string color, std::string msg)
{
	time_stamp(BGREY);
    std::cout << color << msg << RESET << std::endl;
}

void    time_stamp( std::string color )
{
	time_t now = time(0);

	tm *ltm = localtime(&now);

    // Set fill character to '0' and width to 2 for single-digit values
    std::cout << "\t" << std::setfill('0') << color
              << "[" << ltm->tm_year + 1900 << "-"
              << std::setw(2) << ltm->tm_mon + 1 << "-"  // tm_mon is 0-based (ex. january will be represented as 0 if we dont do + 1)
              << std::setw(2) << ltm->tm_mday << "  "
              << std::setw(2) << ltm->tm_hour << ":"
              << std::setw(2) << ltm->tm_min << ":"
              << std::setw(2) << ltm->tm_sec << "]" << std::setfill(' ') << RESET;
}

long int string_in_range(std::string str, int range)
{
	long int	num_max;
	char		*endptr_long;
	long int 	num_long = std::strtol(str.c_str(), &endptr_long, 10);

	if (range == UNSIGNED_SHORT)
	{
		num_max = std::numeric_limits<unsigned short>::max();
	}
	else if (range == CHAR)
	{
		num_max = std::numeric_limits<unsigned char>::max();
	}
	else if (range == MAX_BYTES)
	{
		num_max = 1573741824;
	}

	if ((*endptr_long == '\0') && (num_long <= num_max) && (num_long >= 0))
	{
		return (num_long);
	}
	else
	{
		return (-1);
	}
}

string_vec get_string_vec_diff(const string_vec& vec1, const string_vec& vec2)
{
    string_vec diff;

    for (string_vec::const_iterator it = vec1.begin(); it != vec1.end(); ++it) {
        if (std::find(vec2.begin(), vec2.end(), *it) == vec2.end()) {
            diff.push_back(*it);
        }
    }

    for (string_vec::const_iterator it = vec2.begin(); it != vec2.end(); ++it) {
        if (std::find(vec1.begin(), vec1.end(), *it) == vec1.end()) {
            diff.push_back(*it);
        }
    }

    return diff;
}

size_t get_last_idx_of(const std::string& str, std::string character)
{
    size_t found = str.find_last_of(character);
    if (found != std::string::npos) {
        return (found);
    }
	return (std::string::npos);
}

int file_stats(std::string file)
{
    struct stat file_info;
    int         stats;

    stats = 0;
    /*The file exists*/
    if (access(file.c_str(), F_OK) == 0)
        stats++;
    /*We have the permissions*/
    if (access(file.c_str(), R_OK) == 0)
        stats++;
    
    /*If is a directory*/
    if (stat(file.c_str(), &file_info) == 0)
    {
		if (S_ISDIR(file_info.st_mode) == 1)
            stats++;
	}
    return (stats);
}

std::string secures_the_path(std::string path, std::string file)
{
    std::string path_file;

    if (path.at(path.size() - 1) != '/' && file.at(0) != '/')
	{
        path_file.assign(path + '/' + file);
	}
    else if (path.at(path.size() - 1) == '/' && file.at(0) == '/')
    {
        path_file.assign(path + file.substr(1));
    }
    else
        path_file.assign(path + file);
    return (path_file);
}

bool ends_with(const std::string& str, const std::string& ending)
{
    if (str.length() >= ending.length()) {
        return (str.compare(str.length() - ending.length(), ending.length(), ending) == 0);
    } else {
        return false;
    }
}

std::string httpCodeToString(int code)
{
    switch (code)
    {
        case _OK:
            return "OK";
        case _NO_CONTENT:
            return "No Content";
        case _BAD_REQUEST:
            return "Bad Request";
        case _FORBIDDEN:
            return "Forbidden";
        case _NOT_FOUND:
            return "Not Found";
        case _METHOD_NOT_ALLOWED:
            return "Method Not Allowed";
        case _REQUEST_TIMEOUT:
            return "Request Timeout";
        case _PAYLOAD_TOO_LARGE:
            return "Payload Too Large";
        case _INTERNAL_SERVER_ERROR:
            return "Internal Server Error";
        case _BAD_GATEWAY:
            return "Bad Gateway";
        case _GATEWAY_TIMEOUT:
            return "Gateway Timeout";
        case _HTTP_VERSION_NOT_SUPPORTED:
            return "HTTP Version Not Supported";
        default:
            return "Unhandled HTTP code";
        }
}

/////////////////// CHELO UTILS

void            printAsciiValue( std::string str )
{
    std::cout << YELLOW "=> printing ASCII values" RESET << std::endl;
    for (std::string::iterator it = str.begin(); it != str.end(); ++it) {
        char currentChar = *it;
        int asciiValue = static_cast<int>(currentChar);
        if (asciiValue < 33 || asciiValue > 126)
            std::cout << " " << asciiValue << " ";
        else
            std::cout << currentChar;
    }
    std::cout << std::endl;
}

std::string     intToString( int value )
{
    std::stringstream ss;

    ss << value;
    return (ss.str());
}

int     stringToInt( std::string str )
{
    std::stringstream   ss;
    int                 nb(0);

    ss << str;
    ss >> nb;
    return (nb);
}

void    timeStamp( std::string color )
{
	time_t now = time(0);

	tm *ltm = localtime(&now);

    // Set fill character to '0' and width to 2 for single-digit values
    std::cout << std::setfill('0') << color
              << "[" << ltm->tm_year + 1900 << "-"
              << std::setw(2) << ltm->tm_mon + 1 << "-"  // tm_mon is 0-based (ex. january will be represented as 0 if we dont do + 1)
              << std::setw(2) << ltm->tm_mday << "  "
              << std::setw(2) << ltm->tm_hour << ":"
              << std::setw(2) << ltm->tm_min << ":"
              << std::setw(2) << ltm->tm_sec << "]" << std::setfill(' ') << RESET;
}


void    newClient( std::string color, int fd )
{
	timeStamp(color);
	std::cout << "\tnew CLIENT[" << fd << "]" << std::endl;
}

void    newServer( std::string color, int fd )
{
    timeStamp(color);
    std::cout << "\tnew SERVER[" << fd << "]" << std::endl;
}

void    logMsg( std::string color, std::string msg, int fd)
{
    timeStamp(BGREY);
    std::cout << color << msg;
    (fd == -1) ? std::cout << RESET << std::endl : std::cout << "[" << fd << "]" RESET << std::endl;
}

void	printServers( void )
{
	int maxDots = 3;
	for (int i = 0; i < maxDots; i++)
	{
		std::cout << BWHITE << "\t" << "Initializing server(s) ";
		for (int j = 0; j < i + 1; j++)
			std::cout << ".";
		std::cout << "\r";
		std::cout.flush();
		usleep(350000);
	}
	std::cout << "\033[2K\r"; // for controlling the appearance of text in a terminal
	timeStamp("");
	std::cout << BWHITE << "\t" << "Server(s) list :\n\n" << RESET;
}
