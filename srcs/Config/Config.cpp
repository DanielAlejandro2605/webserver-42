#include "Config/Config.hpp"

/*
** ------------------------------- CONSTRUCTOR --------------------------------
*/

Config::Config()
	: ServerContext()
{
}

/*Constructor of default Config*/

Config::Config(int default_config)
{
	if (default_config)
	{   /* Estilos para el mensaje de error */
		/*Port by default*/
		this->_port = 8080;
		this->_port_str = "8080";
		/*Host by default*/
		this->_host.assign("0.0.0.0");
		/*Server name by default*/
		this->_server_name.push_back("webserv");
		/*Root by default*/
		this->_root.assign("./default_root");
		/*Index by default*/
		this->_index.push_back("default.html");
		/*Allows methods by default*/
		this->_allow_methods.push_back("GET");
		/*Client size body by default*/
		this->_client_max_body_size = 100;
	}
}

/*
** -------------------------------- DESTRUCTOR --------------------------------
*/

Config::~Config()
{
	for (std::vector<ContextBase *>::iterator it = this->_context.begin(); it != this->_context.end(); it++)
	{
		if (*it)
			delete (*it);
	}
}


/*
** --------------------------------- OVERLOAD ---------------------------------
*/

Config &				Config::operator+=(Config const &rhs)
{
	if (this != &rhs)
	{
		if (this->_port_str.empty())
		{
			this->_port = rhs.get_port();
			this->_port_str = rhs.get_port_str();
			std::cout << BYELLOW << "\tSetting default port..." << RESET << std::endl;
		}
		if (this->_host.empty())
			this->_host = rhs.get_host();
		if (this->_server_name.size() == 0)
			this->_server_name = rhs.get_server_name();
		if (this->_root.empty())
			this->_root = rhs.get_root();
		if (this->_index.size() == 0)
			this->_index = rhs.get_index();
		if (this->_allow_methods.size() == 0)
			this->_allow_methods = rhs.get_allow_methods();
		if (this->_client_max_body_size == 0)
			this->_client_max_body_size = rhs.get_client_max_body_size();
		if (this->_root_config.empty())
			this->_root_config = this->_root;
		/*Check error page configured in the config file*/
		this->set_error_pages();
		/*We set the attributes from the map fot all the `Location` Class and `CGI` Class*/
		for (std::vector<ContextBase *>::iterator it = this->_context.begin(); it != this->_context.end();it++)
		{
			(*it)->inherits_allow_methods_from_config(this->get_allow_methods());
			(*it)->inherits_client_size_body_from_config(this->get_client_max_body_size());
			(*it)->save_root_config(this->get_root());
			(*it)->set_error_pages();
		}
	}
	return (*this);
}

/*
** --------------------------------- METHODS ----------------------------------
*/

bool	Config::cmp(std::string url, std::string path_loc)
{
	if (path_loc.compare(url) == 0)
		return (true);
	else if (path_loc.at(path_loc.length() - 1) == '/')
	{
		std::string sub = path_loc.substr(0, path_loc.length() - 1);
		if (url.compare(sub) == 0)
			return (true);
	}
	else if (path_loc.at(path_loc.length() - 1) != '/' && url.at(url.length() - 1) == '/')
	{
		std::string sub(path_loc);
		sub.append("/");
		if (url.compare(sub) == 0)
			return (true);
	}
	return (false);
}


/*The purpose of this function is the check and set information retrieved on the map
in the `Config` class attributes*/
void		Config::set_info_from_map()
{
	Location	*loc;
	CGI			*cgi;

	for (string_vec_map::iterator it = this->_map.begin(); it != this->_map.end();it++)
	{
		if ((*it).first.compare("listen") == 0)
			this->check_listen(it);
		else if ((*it).first.compare("host") == 0)
			this->check_host(it);
		else if ((*it).first.compare("server_name") == 0)
			this->copy_string_vector(this->_server_name, (*it).second);
		else if ((*it).first.compare("root") == 0)
			this->check_root(it);
		else if ((*it).first.compare("autoindex") == 0)
			this->check_auto_index(it);
		else if ((*it).first.compare("index") == 0)
			this->copy_string_vector(this->_index, (*it).second);
		else if ((*it).first.compare("allow_methods") == 0)
			this->check_methods(it);
		else if ((*it).first.compare("error_page") == 0)
			this->check_error_pages(it);
		else if ((*it).first.compare("client_max_body_size") == 0)
			this->check_client_size_body(it);
		else if ((*it).first.compare("upload_location") == 0)
            this->check_upload_location(it);
        else if ((*it).first.compare("upload_script") == 0)
            this->check_upload_script(it, this->get_root());
	}

	/*We set the attributes from the map fot all the `Location` Class and `CGI` Class*/
	for (std::vector<ContextBase *>::iterator it = this->_context.begin(); it != this->_context.end();it++)
	{
		loc  = dynamic_cast<Location *>(*it);
		if (loc)
			loc->set_info_from_map(this->get_root());
		else
		{
			cgi  = dynamic_cast<CGI *>(*it);
			if (cgi)
			{
				cgi->set_info_from_map();
			}
		}
	}
}

void						Config::print_config()
{
	Location *loc;


	std::cout << BGMAGENTA << "Config" << RESET << std::endl;
    
    std::cout << std::setw(1 * 4) << " " << "|-[host]: " << GREEN << "{" << _host << "}" << RESET << std::endl;

    std::cout << std::setw(1 * 4) << " " << "|-[port]: " << GREEN << "{" << _port << "}" << RESET << std::endl;

    std::cout << std::setw(1 * 4) << " " << "|-[server_name]: ";
    for (string_vec::iterator it = _server_name.begin(); it != _server_name.end(); ++it) {
        std::cout << GREEN << "{" << *it << "}";
    }
    std::cout << RESET << std::endl;

    std::cout << std::setw(1 * 4) << " " << "|-[root]: " << GREEN << "{" << _root << "}" << RESET << std::endl;

	if (_auto_index == true)
		std::cout << std::setw(1 * 4) << " " << "|-[autoindex]: " << GREEN << "{" << "on" << "}" << RESET << std::endl;
	else
		std::cout << std::setw(1 * 4) << " " << "|-[autoindex]: " << GREEN << "{" << "off" << "}" << RESET << std::endl;

    std::cout << std::setw(1 * 4) << " " << "|-[index]: ";
    for (string_vec::iterator it = _index.begin(); it != _index.end(); ++it) {
        std::cout << GREEN << "{" << *it << "}";
    }
    std::cout << RESET << std::endl;

    std::cout << std::setw(1 * 4) << " " << "|-[allow_methods]: ";
    for (string_vec::iterator it = _allow_methods.begin(); it != _allow_methods.end(); ++it) {
        std::cout << GREEN << "{" << *it << "}";
    }
    std::cout << RESET << std::endl;

    std::cout << std::setw(1 * 4) << " " << "|-[error_pages]: ";
    for (std::map<std::string, std::string>::iterator it = _error_pages.begin(); it != _error_pages.end(); ++it) {
        std::cout << GREEN << "{" << it->first << ": " << it->second << "}";
    }
    std::cout << RESET << std::endl;

    std::cout << std::setw(1 * 4) << " " << "|-[client_max_body_size]: " << GREEN << "{" << _client_max_body_size << "}" << RESET << std::endl;
	if (_upload == true)
    {
		std::cout << std::setw(1 * 4) << " " << "|-[upload]: " << BLUE << "{" << "yes" << "}" << RESET << std::endl;
        std::cout << std::setw(1 * 4) << " " << "|-[upload_script]: " << BLUE << "{" << this->_upload_script << "}" << RESET << std::endl;
    }
	else
		std::cout << std::setw(1 * 4) << " " << "|-[upload]: " << BLUE << "{" << "not" << "}" << RESET << std::endl;

	
	
	std::cout << "***************** Locations *****************" << std::endl; 
	for (std::vector<ContextBase *>::iterator it = this->_context.begin(); it != this->_context.end(); it++)
	{
		loc  = dynamic_cast<Location *>(*it);
		if (loc)
			loc->print_location();
	}

	if (this->_cgi)
    {
        this->_cgi->print_cgi();
    }
	
	std::cout << "**********************************************" << RESET << std::endl;
}


/*
** --------------------------------- CHECKERS ---------------------------------
*/

void	Config::check_listen(string_vec_map::iterator it)
{
	if (it->second.size() > 1)
		std::cout << PURPLE << "\t(Warning) There is a listen port already, the first one found will be prioritized" << RESET <<  std::endl;
	/*Getting the number of port from the element (key, value) of the map*/
	std::string	port_string = (*it).second[0];
	/*We check this stupid case*/
	if (port_string.compare("0") == 0)
		throw Config::InvalideListenPort();

	/*We check if the number presented by the value of directive 'listen'
	is in the range of UNSIGNED SHORT (max port in a machine)*/
	this->_port = string_in_range(port_string, UNSIGNED_SHORT);
	if (this->_port == 0)
		throw Config::InvalideListenPort();
	this->_port_str.assign(port_string);
}

/*Split the host value and check if each `byte` is within 0 and 255. */
void	Config::check_host(string_vec_map::iterator it)
{
	std::string	host_string;
	string_vec 	sub_strs;
	std::string sub_string;

	host_string = (*it).second[0];
	size_t i = 0;
	size_t j = 0;
	while (i < host_string.size())
	{
		if (host_string[i] == '.')
		{
			sub_string = host_string.substr(j, i - j);
			sub_strs.push_back(sub_string);
			j = i + 1;
		}
		else if (host_string[i + 1] == '\0')
		{
			sub_string = host_string.substr(j, i - j + 1);
			sub_strs.push_back(sub_string);
		}
		i++;
	}

	if (sub_strs.size() != 4)
		throw Config::InvalideHost();
	for (string_vec::iterator it_vec = sub_strs.begin(); it_vec !=  sub_strs.end(); it_vec++)
	{
		if(string_in_range((*it_vec), CHAR) == -1)
		{
			throw Config::InvalideHost();
		}
	}
	this->_host.assign(host_string);
}

/*
** --------------------------------- ACCESSOR ---------------------------------
*/

/*
Function which return the reference to the vector of Context pointers
*/
std::vector<ContextBase *>&	Config::get_context()
{
	return (this->_context);
}

const std::string&	Config::get_port_str() const
{
	return (this->_port_str);
}

unsigned short	Config::get_port() const
{
	return (this->_port);
}

const std::string& Config::get_host() const {
    return (this->_host);
}

const string_vec& Config::get_server_name() const {
    return (this->_server_name);
}

/*
** ------------------------------- EXCEPTIONS ---------------------------------
*/

const char* Config::MissingListenDirective::what() const throw() {
    return ("Error: Missing `listen` directive in server's context.");
}

const char* Config::InvalideListenPort::what() const throw() {
    return ("Error: Invalid port.");
}

const char* Config::InvalideHost::what() const throw() {
    return ("Error: Invalid host.");
}


/* ************************************************************************** */