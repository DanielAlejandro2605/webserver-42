/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HandlerContext.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mflores- <mflores-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/07 16:57:41 by dnieto-c          #+#    #+#             */
/*   Updated: 2024/02/02 18:24:38 by mflores-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Config/HandlerContext.hpp"

/*
** ------------------------------- CONSTRUCTOR --------------------------------
*/

/*Getting the reference of the `Config` Class map*/	
HandlerContext::HandlerContext(Config &config) : _is_key(1), _brackets_level(0), _config(config), _map_config(config.get_map())
{
	/*Setting directives supported in this webserver by any context : Config or Location*/
    this->_accepted_directives_config.push_back("root");
	this->_accepted_directives_config.push_back("autoindex");
	this->_accepted_directives_config.push_back("index");
	this->_accepted_directives_config.push_back("allow_methods");
    this->_accepted_directives_config.push_back("error_page");
    this->_accepted_directives_config.push_back("client_max_body_size");
	this->_accepted_directives_config.push_back("upload_location");
	this->_accepted_directives_config.push_back("upload_script");
	/*Copy accepted directives in config to location*/
	this->_accepted_directives_location = this->_accepted_directives_config;

	/*Directives only supported by the Config context*/
	this->_accepted_directives_config.push_back("listen");
	this->_accepted_directives_config.push_back("host");
	this->_accepted_directives_config.push_back("server_name");

	/*Directives only supported by the Location context*/
	this->_accepted_directives_location.push_back("return");
	// this->_accepted_directives_location.push_back("alias");
	/*IMPORTANT*/
	this->_accepted_directives_location.push_back("cgi_pass");
    this->_accepted_directives_location.push_back("cgi_path");
}

/*
** -------------------------------- DESTRUCTOR --------------------------------
*/

HandlerContext::~HandlerContext()
{
}


/*
** --------------------------------- METHODS ----------------------------------
*/

/*
	Read the big line for adding the differents token:
		CURLY_BRACE_OPEN = '{',
		CURLY_BRACE_CLOSE = '}',
		SEMICOLON = ';',
		STRING = 1,
	
	The token STRING is a concatenation of the rest of characters that are not in the possible tokens,
	for example : `location`, `server_name`, etc.
	
	Ignoring comments in the file
*/
void	HandlerContext::get_tokens(std::string context_server)
{
	/*Aux variable for create the tokens*/
	std::string		str;

	this->_context_it = context_server.begin();

	while (this->_context_it != context_server.end())
	{
		/*If we encounter a `#`, we advance until the next `newline` character.*/
		if ((*this->_context_it) == HASHTAG)
		{
			while ((this->_context_it != context_server.end() - 1) && (*this->_context_it != '\n'))
			{ 
				this->_context_it++;
			}
		}
		/*If we encounter a space character, we check if there's something in the `context` variable which
		is the result of the concatenation.*/
		else if (isspace(*this->_context_it))
		{
			/*It's not empty*/
			if (this->_context.empty() == 0)
			{
				this->_tokens.push_back(std::make_pair(STRING, this->_context));
			}
			this->_context.clear();
		}
		else
		{
			/*
			 switch()
			 For all the tokens characters, we add a token.
			 For all the other character, we concatente for creating `words`
			*/
			switch((*this->_context_it))
			{
				case SEMICOLON:
					if (this->_context.empty() == 0)
					{
						this->_tokens.push_back(std::make_pair(STRING, this->_context));
						this->_context.clear();
					}
					str += ((*this->_context_it));
					this->_tokens.push_back(std::make_pair(SEMICOLON, str));
					str.clear();
					break;
				case CURLY_BRACE_OPEN:
					str += ((*this->_context_it));
					this->_tokens.push_back(std::make_pair(CURLY_BRACE_OPEN, str));
					str.clear();
					break;
				case CURLY_BRACE_CLOSE:
					str += ((*this->_context_it));
					this->_tokens.push_back(std::make_pair(CURLY_BRACE_CLOSE, str));
					str.clear();
					break;
				default:
					this->_context += ((*this->_context_it));
					break;
			}
		}
		this->_context_it++;
	}
}

void	HandlerContext::set_map_from_tokens(Config &config)
{
	/*Getting the reference of the `Config` Class std:::vector of pointers of Locations*/	
	this->_ptr_context = &config.get_context();
	this->_it_start = this->_tokens.begin();
	this->_it_end = this->_tokens.end();

	/*Variables for create the elements of a `string_vec_map`*/
	std::string	key;
	string_vec	value_vec;

	/*iterating through all tokens*/
	while (this->_it_start != this->_it_end)
	{
		/*	Every time we find an open bracket, it means that we are in another context, usually a 'location' directive. 
			Every time we encounter a closed bracket, we have left that context.
		*/
		if (this->_it_start->first == CURLY_BRACE_OPEN)
		{
			this->_brackets_level++;
		}
		else if (this->_it_start->first == CURLY_BRACE_CLOSE)
		{
			this->_brackets_level--;
		}

		/*If we find a semicolon or an open bracket that means that we have just read a directive.*/
		if (this->_it_start->first == SEMICOLON || this->_it_start->first == CURLY_BRACE_OPEN)
		{
			if (key.empty() == 0)
			{
				this->add_key_values_to_map(key, value_vec);
			}
			this->_is_key = 1;
			key.clear();
			value_vec.clear();
		}
		else if (this->_is_key && this->_it_start->first == STRING)
		{
			key.assign(this->_it_start->second);
			this->_is_key = 0;
		}
		else if (this->_it_start->first == STRING)
		{
			value_vec.push_back(this->_it_start->second);
		}
		this->_it_start++;
	}
}

void	HandlerContext::add_key_values_to_map(std::string key, string_vec values)
{
	ContextBase			*context;
	bool				valide_directive;

	/*If the key is the `location` directive*/
	if ((key.compare("location") == 0) && (values.size() == 1))
	{
		this->add_context_to_vector(values);
	}
	else
	{
		/*If the brackets_level is greather that 1, that means we are in other context (location)*/
		if (this->_brackets_level > 1)
		{
			/*If we check that the directive is handled by the webserv*/
			valide_directive = this->valide_directive_location(key, values);
			if (valide_directive)
			{
				/*We search the correct location and we add*/
				context = search_context_by_level(this->_brackets_level);
				std::string	relative_path = context->get_relative_route();
				insert_key_value_to_map(context->get_map(), key, values);
			}
			else
			{
				std::cout << "\t" << BRED << "(" << key << ") " << RESET;
				throw HandlerContext::InvalidDirectiveLocation();
			}
		}
		else
		{
			/*If the brackets_level is equals to 1, that means we are in the context of the 'server'*/
			valide_directive = this->valide_directive_config(key, values);
			if (valide_directive)
				insert_key_value_to_map(this->_map_config, key, values);
			else
			{
				std::cout << "\t" << BRED << "(" << key << ") " << RESET;
				throw HandlerContext::InvalidDirectiveConfig();	
			}
		}
	}
}

void			HandlerContext::add_context_to_vector(string_vec context)
{
	int	context_type;

	/*It's a location*/
	if ((context.size() == 1) && context[0][0] == '/')
		context_type = LOCATION_CONTEXT;
	/*It's a CGI*/
	else if ((context.size() == 1) && ((context[0].compare("*.php") == 0) || ((context[0].compare("*.py") == 0))))
		context_type = CGI_CONTEXT;

	if (this->_brackets_level == 2)
	{
		this->insert_context_to_vector(context_type, context[0], context[0]);
	}
	else
	{
		/*We search the absolute route for the context*/
		std::string	route_context = this->get_route_for_context();
		if (context_type == CGI_CONTEXT && route_context[route_context.size() - 1] != '/')
			route_context += '/';
		route_context.append(context[0]);
		this->insert_context_to_vector(context_type, context[0], route_context);
	}
}

void			HandlerContext::insert_context_to_vector(int context_type, std::string context_path, std::string context_route)
{
	ContextBase *context;
	ContextBase *location_context;
	Location	*location;
	CGI			*cgi_ptr;

	/*We add the new element to the std::vector<ContextBase *>*/
	try {
		if (context_type == LOCATION_CONTEXT)
		{
			context = new Location(context_route, context_path, this->_brackets_level);
			this->_ptr_context->push_back(context);
		}
		else if (context_type == CGI_CONTEXT)
		{
			/*In this case, we are talking about a CGI for the Config,
			so we attach the pointer to the attribute of the Config class*/
			if (this->_brackets_level == 2)
			{
				cgi_ptr = this->_config.get_cgi();
				if (cgi_ptr == NULL)
				{
					context = new CGI(context_route, context_path, this->_brackets_level);
					CGI& ref_cgi = dynamic_cast<CGI &>(*context);
					this->_config.set_cgi(ref_cgi);
					this->_ptr_context->push_back(context);
				}
			}
			/*In this case, we are talking about a CGI for one Location,
			so we search the location at a lower level brackets, and we
			attach the pointer to the attribute of the Location class*/
			else
			{
				location_context = search_context_by_level(this->_brackets_level - 1);
				location = dynamic_cast<Location *>(location_context);
				if (location)
				{
					cgi_ptr = location->get_cgi();
					if (cgi_ptr == NULL)
					{
						context = new CGI(context_route, context_path, this->_brackets_level);	
						CGI& ref_cgi = dynamic_cast<CGI &>(*context);
						location->set_cgi(ref_cgi);
						this->_ptr_context->push_back(context);
					}
				}				
			}
		}
	} catch(const std::exception& e) {
		std::cout << e.what() << std::endl;
	}
}

std::string			HandlerContext::get_route_for_context()
{
	ContextBase 							*node;
	std::vector<ContextBase *>::iterator	it_context;
	std::vector<std::string>				route_vec;
	std::string								abs_route;
	int										level_consumed;
	
	/*We obtain the last element of the std::vector `context`*/
	it_context = this->_ptr_context->end() - 1;
	node = (*it_context);

	/*We recover the level we are interested in*/
	level_consumed = this->_brackets_level - 1;

	/*
	We iterate until we find the first location `parent` node 
	while adding the `routes` of the nodes that we are only interested in consuming to the `route_vec` vector.
	*/
	while (node->get_level() > 2)
	{
		if (node->get_level() == level_consumed)
		{
			route_vec.push_back(node->get_relative_route());
			level_consumed--;
		}
		it_context--;
		node = (*it_context);
	}
	/*We add the `route` of the location `parent` node*/
	route_vec.push_back(node->get_relative_route());

	/*We concatenate all the routes to the absolute route varible*/
	for (std::vector<std::string>::reverse_iterator it = route_vec.rbegin(); it != route_vec.rend(); it++)
	{
		if ((*it).at((*it).length() - 1) == '/')
		{
			abs_route.append((*it).substr(0, (*it).length() - 1));
		}
		else
		{
			abs_route.append(*it);
		}
	}
	return (abs_route);
}


/*
Search the first node of the std::vector<ContextBase *> that match
on level from back to front
*/
ContextBase*	HandlerContext::search_context_by_level(int level)
{
	ContextBase										*context;
	std::vector<ContextBase *>::reverse_iterator 	it;

	it = this->_ptr_context->rbegin();
	while (it != this->_ptr_context->rend())
	{
		context = *it;
		if (context->get_level() == level)
			break;
		it++;
	}
	return (context);
}

/*
Function to add a new [key: {value1, value2}]
If the key is present in the map, we add only the value(s)
*/
void		HandlerContext::insert_key_value_to_map(string_vec_map &map, std::string key, string_vec values)
{
	string_vec_map::iterator it_map = map.find(key);

	if (it_map != map.end())
	{
		for (string_vec::iterator it_vec = values.begin(); it_vec != values.end(); it_vec++)
		{
			it_map->second.push_back((*it_vec));
		}	
	}
	else
		map.insert(std::make_pair(key, values));
}

bool	HandlerContext::valide_directive_config(std::string directive, string_vec values)
{
	std::vector<std::string>::iterator it = std::find(this->_accepted_directives_config.begin(), this->_accepted_directives_config.end(), directive);

	if (it != this->_accepted_directives_config.end())
	{
		if (((*it).compare("listen") == 0) && (values.size() == 1))
			return (true);
		/*Only one possible value to the directive 'host'*/
		if (((*it).compare("host") == 0) && (values.size() == 1))
			return (true);
		/*Only one possible value to the directive 'server_name'*/
		else if (((*it).compare("server_name") == 0) && (values.size() >= 1))
			return (true);
		/*Only one possible value to the directive 'root'*/
		else if (((*it).compare("root") == 0) && (values.size() == 1))
			return (true);
		/*Only one possible value to the directive 'auto_index'*/
		else if (((*it).compare("autoindex") == 0) && (values.size() == 1))
			return (true);
		/*Only one possible value to the directive 'index'*/
		else if (((*it).compare("index") == 0) && (values.size() >= 1))
			return (true);
		/*Minimun 1 values to the directive 'allow_methods'*/
		else if (((*it).compare("allow_methods") == 0) && (values.size() >= 1))
			return (true);
		/*Minimun one pair of ERROR_CODE PAGE to the directive 'error_page'*/
		else if (((*it).compare("error_page") == 0) && (values.size() >= 2))
			return (true);
		/*Only one possible value to the directive 'client_max_body_size' must be set  */
		else if (((*it).compare("client_max_body_size") == 0) && (values.size() == 1))
			return (true);
		/*Only one possible value to the directive 'upload_location' : yes or not*/
		else if (((*it).compare("upload_location") == 0) && (values.size() == 1))
			return (true);
		else if (((*it).compare("upload_script") == 0) && (values.size() == 1))
			return (true);
	}
	return (false);
}

bool	HandlerContext::valide_directive_location(std::string directive, string_vec values)
{
    std::vector<std::string>::iterator it;

    it = std::find(this->_accepted_directives_location.begin(), this->_accepted_directives_location.end(), directive);

	if (it != this->_accepted_directives_location.end())
	{
		/*Only one possible value to the directive 'root'*/
		if (((*it).compare("root") == 0) && (values.size() == 1))
			return (true);
		/*Only one possible value to the directive 'auto_index'*/
		else if (((*it).compare("autoindex") == 0) && (values.size() == 1))
			return (true);
		/*Only one possible value to the directive 'index'*/
		else if (((*it).compare("index") == 0) && (values.size() >= 1))
			return (true);
		/*Minimun 1 values to the directive 'allow_methods'*/
		else if (((*it).compare("allow_methods") == 0) && (values.size() >= 1))
			return (true);
		/*Minimun one pair of ERROR_CODE PAGE to the directive 'error_page'*/
		else if (((*it).compare("error_page") == 0) && (values.size() >= 2))
			return (true);	
		/*Only one possible value to the directive 'client_max_body_size' must be set  */
		else if (((*it).compare("client_max_body_size") == 0) && (values.size() == 1))
			return (true);
		/*Only one possible to the directive 'return' must be be set*/
		else if (((*it).compare("return") == 0) && (values.size() == 1))
			return (true);
		/*Only one possible to the directive 'cgi_pass' must be be set*/
		else if (((*it).compare("cgi_pass") == 0) && (values.size() == 1))
			return (true);
		/*Only one possible value to the directive 'upload_location' : yes or not*/
		else if (((*it).compare("upload_location") == 0) && (values.size() == 1))
			return (true);
		else if (((*it).compare("upload_script") == 0) && (values.size() == 1))
			return (true);
	}
	return (false);
}

void	HandlerContext::print_map(string_vec_map &map)
{
	for (string_vec_map::iterator it_map = map.begin(); it_map != map.end(); it_map++) {
        std::cout << "[" << it_map->first << "]: ";
		for (string_vec::iterator it_vec = it_map->second.begin(); it_vec != it_map->second.end(); it_vec++)
		{
			std::cout << "{" << *it_vec << "}";
		}
		std::cout << std::endl;
    }
}

void	HandlerContext::print_tokens()
{
	/*Printing tokens, the reason of the '+1' and '-1' is only because
	we already sure that block starts and ends by curly braces.
	*/
	for (this->_it_start = this->_tokens.begin() + 1; this->_it_start != this->_tokens.end() - 1; this->_it_start++)
	{
		std::cout << "Token : ";
		switch(this->_it_start->first)
		{
			case SEMICOLON:
				std::cout << "SEMICOLON";
				break;
			case CURLY_BRACE_OPEN:
				std::cout << "CURLY_BRACE_OPEN";
				break;
			case CURLY_BRACE_CLOSE:
				std::cout << "CURLY_BRACE_CLOSE";
				break;
			case STRING:
				std::cout << "STRING";
				break;
		}
		std::cout << " | Value : " << this->_it_start->second << std::endl;
	}
}

void	HandlerContext::print_single_token(int token)
{
	switch(token)
	{
		case SEMICOLON:
			std::cout << "SEMICOLON";
			break;
		case CURLY_BRACE_OPEN:
			std::cout << "CURLY_BRACE_OPEN";
			break;
		case CURLY_BRACE_CLOSE:
			std::cout << "CURLY_BRACE_CLOSE";
			break;
		case STRING:
			std::cout << "STRING";
			break;
	}
}
/*
** --------------------------------- ACCESSOR ---------------------------------
*/

const char* HandlerContext::InvalidDirectiveConfig::what() const throw() {
    return ("Error: Invalid directive in server context.");
}

const char* HandlerContext::InvalidDirectiveLocation::what() const throw() {
    return ("Error: Invalid directive in location context.");
}
/* ************************************************************************** */