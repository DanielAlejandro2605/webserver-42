# ifndef UTILS_HPP
# define UTILS_HPP

# include "webserv.hpp"
typedef std::vector<std::string> 							string_vec;

std::string     int_to_string(int value);

void            log(std::string color, std::string msg);

void            time_stamp( std::string color );

long int        string_in_range(std::string str, int range);

/*Diff to std::vector<std::string>*/
string_vec		get_string_vec_diff(const string_vec& vec1, const string_vec& vec2);

size_t          get_last_idx_of(const std::string& str, std::string character);

int             file_stats(std::string file);

std::string     secures_the_path(std::string path, std::string file);

bool            ends_with(const std::string& str, const std::string& ending);

std::string     httpCodeToString(int code);
////////// CHELO UTILS
void    printAsciiValue( std::string str );
std::string intToString( int value );
int     stringToInt( std::string str );
void    timeStamp( std::string color );
void    newClient( std::string color, int fd );
void    newServer( std::string color, int fd );
void    logMsg( std::string color, std::string msg, int fd );
void	printServers( void );

# endif