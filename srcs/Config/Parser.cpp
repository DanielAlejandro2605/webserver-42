#include "Config/Parser.hpp"

/*
** ------------------------------- CONSTRUCTOR --------------------------------
*/

Parser::Parser(char *path_file) : _path_file(path_file), _brackets_closed(0)
{
	this->_count_context = 0;
	try {
		this->_default_config = new Config(1);
	} catch(const std::exception& e) {
		std::cout << e.what() << std::endl;
	}
}


/*
** -------------------------------- DESTRUCTOR --------------------------------
*/

Parser::~Parser()
{
	if (this->_default_config)
		delete (this->_default_config);

	for (std::vector<Config *>::iterator it = this->_configs.begin(); it != this->_configs.end(); it++)
	{
		delete (*it);
	}
}


/*
** --------------------------------- METHODS ----------------------------------
*/

void	Parser::add_config_to_vector(Config *new_config)
{
	bool			can_be_added = true;
	int				similar;

	/*Variables for the Config already in the vector*/
	std::string		port;
	std::string		host;

	/*Variables for the incoming*/
	std::string		new_config_port = new_config->get_port_str();
	std::string		new_config_host = new_config->get_host();
	
	for (this->_it_configs = this->_configs.begin(); this->_it_configs != this->_configs.end(); this->_it_configs++)
	{
		similar = 0;
		port = (*this->_it_configs)->get_port_str();
		host = (*this->_it_configs)->get_host();

		if (port.compare(new_config_port) == 0)
			similar++;
		if (host.compare(new_config_host) == 0)
			similar++;
		if (similar == 2)
			can_be_added = false;
	}
	if (can_be_added)
		this->_configs.push_back(new_config);
	else
		throw Parser::NotDifferContext();
}

void	Parser::parse_server_context()
{
	/*Instance of 'HandlerContext for parse the std::string (`_buffer` attribute of `Parser` Class):
	
	(1). Get the tokens
	(2). Check the tokens
	(3). Set info in a `Config` Class
	(4). Using the `+=` overload for complete the configs with mandatory attributes for lauch a server
	*/
	Config				*config = NULL;

	try {
		/*Reference to the default config*/
		Config& ref_default_config = *this->_default_config;
		/*Instance of the Config*/
		config = new Config();
		/*Reference to the instance of the Config*/
		Config& ref_config = *config;
		(void)ref_config;
		(void)ref_default_config;
		/*Create the `Handler Config` by the reference of the `Config*/
		HandlerContext 	handler(ref_config);

		/*Getting the block of characters for this virtual server in a single std::string*/
		this->get_server_context();
		/*Getting the tokens*/
		handler.get_tokens(this->_buffer);
		/*Set the map from tokens*/
		handler.set_map_from_tokens(ref_config);
		/*Set attributes from map*/
		config->set_info_from_map();
		/*Complete the config*/
		ref_config += ref_default_config;
		/*Checking if the configuration is not similar to one that is already saved */
		this->add_config_to_vector(config);
		std::cout << BGREEN << "\t{Server block # " << this->_configs.size() << "} Syntax and values OK" << RESET << std::endl;
	} catch(const std::exception& e)
	{ 
		std::cout << BRED << "\t" << e.what() << RESET << std::endl;
		if (config)
		{
			delete config;
			config = NULL;
		}
	}
	this->_buffer.clear();
}

/*Concatenate all the character until all the curly braces are properly closed*/
void	Parser::get_server_context()
{
	this->_file.get(this->_c);
	while (this->_brackets_closed != 0)
	{
		if (this->_file.eof())
			break;
		if (this->_c == CURLY_BRACE_CLOSE)
		{
			this->_brackets_closed--;
		}
		else if (this->_c == CURLY_BRACE_OPEN)
		{
			this->_brackets_closed++;
		}

		if (this->_c == HASHTAG)
		{
			while (this->_file.eof() == 0 && this->_c != '\n')
			{
				this->_file.get(this->_c);
			}
		}
		this->_buffer += this->_c;
		this->_file.get(this->_c);
	}
	if (this->_brackets_closed != 0)
	{
		throw Parser::ExpectedBracket();
	}
}

bool	Parser::hasConfigExtension( const std::string & filename )
{
    size_t dotPos = filename.find_last_of('.');

    if (dotPos != std::string::npos && filename.substr(dotPos + 1).compare(FILE_EXTENSION) == 0) {
        return (true);
    }
    return (false);
}

/*Read file for searching the `server` directive*/
void	Parser::run()
{
	int		context = -1;
	size_t	comment;

	if (!hasConfigExtension(this->_path_file))
		throw std::runtime_error("Error: wrong file extension, use '." + std::string(FILE_EXTENSION) + "'");

	this->_file.open(this->_path_file.c_str(), std::fstream::in);
	if (this->_file.fail())
		throw Parser::ErrorOpenConfigFile();

	/*Reading the file searching the `server` directives.*/
	this->_file.get(this->_c);
	while (this->_file.eof() == 0)
	{
		/*We advance the spaces characters.*/
		while (isspace(this->_c) && this->_file.eof() == 0)
		{
			this->_file.get(this->_c);
		}
		/*
		We concatenate the non spaces characters until the current
		byte ready is a space character or a curly brance open `{`.
		*/
		while (!isspace(this->_c) && this->_file.eof() == 0)
		{
			if (this->_c == HASHTAG)
			{
				while (this->_file.eof() == 0 && this->_c != '\n')
				{
					this->_file.get(this->_c);
				}
			}
			if (this->_c != '\n')
			{
				this->_concatenation += this->_c;
			}
			this->_file.get(this->_c);
			if (this->_c == CURLY_BRACE_OPEN)
				break;
		}
		/*
		At this point, we have the first `word` expecting that is `server.
		We advance all the space characters for reach the first curly brance open `{`
		*/
		while (isspace(this->_c) && this->_file.eof() == 0)
		{
			this->_file.get(this->_c);
		}

		/*At the moment of the character readed is equal*/
		if (this->_c == CURLY_BRACE_OPEN)
		{
			/*
			We check if it is the word `server` for lauch the `parse_server_context`
			for get all the informations of virtual server
			*/
			comment = this->_concatenation.find("#");
			if (comment != std::string::npos)
			{
				this->_concatenation = this->_concatenation.substr(this->_concatenation.find("#"));
			}

			if (this->_concatenation.empty() == 0)
			{
				context = this->_concatenation.compare("server");
				this->_concatenation.clear();
				this->_buffer += this->_c;
				this->_brackets_closed++;
				if (context == 0)
				{
					this->print_parser_timestamp(this->_count_context + 1);
					this->parse_server_context();
					this->_count_context++;
				}
			}
		}
	}
}

/*
** --------------------------------- ACCESSOR ---------------------------------
*/

const std::vector<Config *>&	Parser::get_configs()
{
	return (this->_configs);
}

/*
** ---------------------------------  UTILS  ----------------------------------
*/

void	Parser::print_parser_timestamp(int context_number)
{
	int maxDots = 3;
	for (int i = 0; i < maxDots; i++)
	{
		std::cout << BWHITE << "\t[CONTEXT #" << context_number << "]\t" << "Checking context syntax and values " RESET;
		for (int j = 0; j < i + 1; j++)
			std::cout << ".";
		std::cout << "\r";
		std::cout.flush();
		usleep(350000);
	}
	std::cout << std::endl;
}

/*
** ------------------------------- EXCEPTIONS ---------------------------------
*/
const char* Parser::ExpectedBracket::what() const throw() {
    return ("Error: Expected bracket for open/close directive.");
}

const char* Parser::InvalidMainDirective::what() const throw() {
    return ("Error: Invalid main directive, expected 'server'.");
}

const char* Parser::ErrorOpenConfigFile::what() const throw() {
    return ("Error: Cannot open the configuration file.");
}

const char* Parser::NotDifferContext::what() const throw() {
	std::string warning;

	warning.assign(BPURPLE);
	warning.append("Warning: Same main directives ('listen', 'host') in other server block; it won't be used.");
    warning.append(RESET);
	
	std::cout << warning << std::endl;
	return ("");
}

/* ************************************************************************** */