#ifndef PARSER_HPP
# define PARSER_HPP

/*Standard librarys*/
# include <iostream>
# include <fstream>
# include <cstdlib>
# include <ctype.h>

/*Librarys*/
# include "utils/utils.hpp"
# include "Config.hpp"
# include "HandlerContext.hpp"


class Parser
{
	private:
		/*Varibles for read the config file*/
		std::string							_path_file;
		std::ifstream 						_file;
		std::string							_buffer;
		std::string							_concatenation;
		std::string							_context;
		char								_c;
		int									_brackets_closed;

		/*Number of context*/
		int									_count_context;
		/*Default Config pointer*/
		Config*								_default_config;				
		/*Config vector to virtual server references*/
		std::vector<Config *>				_configs;
		/*Iterator for Config vector*/
		std::vector<Config *>::iterator		_it_configs;
	public:
		Parser(char *path_file);
		~Parser();
		/*Methods*/
		void						run();
		bool						hasConfigExtension( const std::string & filename );
		void						parse_server_context();
		void						get_server_context();
		void						add_config_to_vector(Config *conf);
		
		/*Getters*/
		const std::vector<Config *>&	get_configs();
		/*Utils*/
		void							print_parser_timestamp(int context_number);
		class ExpectedBracket: public std::exception {
		public:
			const char* what() const throw();
		};
		class InvalidMainDirective : public std::exception {
		public:
			const char* what() const throw();
		};

		class ErrorOpenConfigFile : public std::exception {
		public:
			const char* what() const throw();
		};

		class NotDifferContext : public std::exception {
		public:
			const char* what() const throw();
		};
};

#endif /* ********************************************************** PARSER_H */