#include "Config/ContextBase.hpp"

/*
** ------------------------------- CONSTRUCTOR --------------------------------
*/

ContextBase::ContextBase(std::string absolute_route, std::string relative_route, int level)
	: _absolute_route(absolute_route) , _relative_route(relative_route), _level(level)
{
	this->_client_max_body_size = 0;
	this->_http_errors.push_back("400");
	this->_http_errors.push_back("403");
	this->_http_errors.push_back("404");
	this->_http_errors.push_back("405");
	this->_http_errors.push_back("408");
	this->_http_errors.push_back("413");
	this->_http_errors.push_back("500");
	this->_http_errors.push_back("502");
	this->_http_errors.push_back("504");
	this->_http_errors.push_back("505");
}

ContextBase::ContextBase()
{
	this->_client_max_body_size = 0;
	this->_http_errors.push_back("400");
	this->_http_errors.push_back("403");
	this->_http_errors.push_back("404");
	this->_http_errors.push_back("405");
	this->_http_errors.push_back("408");
	this->_http_errors.push_back("413");
	this->_http_errors.push_back("500");
	this->_http_errors.push_back("502");
	this->_http_errors.push_back("504");
	this->_http_errors.push_back("505");
}

/*
** -------------------------------- DESTRUCTOR --------------------------------
*/

ContextBase::~ContextBase()
{
}

/*
** --------------------------------- METHODS ----------------------------------
*/

void	ContextBase::check_root(string_vec_map::iterator it)
{
	/*Getting the `root` directory from the element (key, value) of the map*/
	std::string	root_directory = (*it).second[0];
	/*For open the directory*/
	DIR *dir;

	if ((dir = opendir(root_directory.c_str())) != NULL)
	{
		this->_root.assign(root_directory);
		closedir(dir);
	}
	else
	{
		std::cerr << "\tError to open the dir" << std::endl;
	}
}

/*Checking if the methods value are in : GET, POST, DELETE*/
void	ContextBase::check_methods(string_vec_map::iterator it)
{
	string_vec::iterator not_doubles;
	string_vec methods = (*it).second;
	std::string	method;
	for (string_vec::iterator it_vec = methods.begin(); it_vec !=  methods.end(); it_vec++)
	{
		method = (*it_vec);
		not_doubles = std::find(this->_allow_methods.begin(), this->_allow_methods.end(), method);
		if ((method.compare("GET") != 0) && (method.compare("POST") != 0) && (method.compare("DELETE") != 0))
			throw ContextBase::NotSupportedMethod();
		else
		{
			if (not_doubles == this->_allow_methods.end())
				this->_allow_methods.push_back(method);
		}
	}	
}

void	ContextBase::check_client_size_body(string_vec_map::iterator it)
{
	std::string	client_max_body_size = (*it).second[0];

	if (client_max_body_size.compare("0") == 0)
		this->_client_max_body_size = 0;

	/*We check if the number presented by the value of directive 'listen'
	is in the range of UNSIGNED SHORT (max port in a machine)*/
	this->_client_max_body_size = string_in_range(client_max_body_size, MAX_BYTES);
	if (this->_client_max_body_size == -1)
		throw ContextBase::InvalidBodySizeValue();
}

/*Check if the error pages values is in the HTTP ERRORS set in _http_errors*/
void	ContextBase::check_error_pages(string_vec_map::iterator it)
{
	string_vec error_pages = (*it).second; 
	string_vec::iterator it_values = error_pages.begin();
	string_vec::iterator supported;

	/*Aux variables for insert*/
	std::string	error_code;
	std::string page;
	while (it_values != error_pages.end())
	{
		error_code = (*it_values);
		supported = std::find(this->_http_errors.begin(), this->_http_errors.end(), error_code);
		it_values++;
		page = (*it_values);
		it_values++;
		if (supported == this->_http_errors.end())
			throw ContextBase::UnknownErrorCode();
		this->_error_pages.insert(std::make_pair(error_code, page));
	}
}

/*
The purpose of this function is check if the error pages configured by the user in
the config file are correct (exist, permission). In the case that this check doesn't
work, the info set by the user will be ignored
*/
void	ContextBase::set_error_pages(void)
{
	string_map::iterator 	it_error_page;
	string_map::iterator 	it_delete;
	std::string				error_code;
	std::string				path_error_page;
	std::string				root_set;

	/*There's root in the config*/
	if (this->_root.empty() == 0)
	{
		root_set.assign(this->get_root());
	}
	else if (this->_root_config.empty() == 0)
	{
		root_set.assign(this->get_root_config());
	}

	it_error_page = this->_error_pages.begin();
	while (it_error_page != this->_error_pages.end())
	{
		error_code = (*it_error_page).first;
		path_error_page = (*it_error_page).second;
		if (path_error_page.at(0) != '/' && root_set.at(0) != '/')
				root_set = root_set + '/';
		std::string path_error_file_set(root_set + path_error_page);
		if (file_stats(path_error_file_set) != 2)
		{
			it_delete = it_error_page;
			it_error_page++;
			this->_error_pages.erase(it_delete);
		}
		else
			it_error_page++;
	}
}

void	ContextBase::inherits_allow_methods_from_config(string_vec allow_methods_config)
{
	if (this->_allow_methods.size() == 0)
	{
		this->_allow_methods.assign(allow_methods_config.begin(), allow_methods_config.end()); 
	}
}

void	ContextBase::inherits_client_size_body_from_config(int client_max_body_size_config)
{
	if (this->_client_max_body_size == 0)
	{
		this->_client_max_body_size = client_max_body_size_config;
	}
}

void	ContextBase::save_root_config(std::string root_config)
{
	this->_root_config = root_config;
}

/*
** ----------------------------------- UTILS ----------------------------------
*/

void	ContextBase::copy_string_vector(string_vec &attribute_vec, string_vec values_map)
{
	for (string_vec::iterator it_vec = values_map.begin(); it_vec !=  values_map.end(); it_vec++)
	{
		attribute_vec.push_back((*it_vec));
	}
}

bool	ContextBase::check_method_allowed(std::string request_method) const
{
	string_vec::const_iterator it = std::find(this->_allow_methods.begin(), this->_allow_methods.end(), request_method);
	
	if (it != this->_allow_methods.end())
		return (true);
	return (false);
}

/*
** --------------------------------- ACCESSOR ---------------------------------
*/

/*Architecture*/
const std::string&	ContextBase::get_absolute_route() const
{
    return (this->_absolute_route);
}

const std::string&	ContextBase::get_relative_route() const
{
    return (this->_relative_route);
}

int	ContextBase::get_level() const
{
    return (this->_level);
}

string_vec_map& ContextBase::get_map()
{
    return (this->_map);
}

const std::string& ContextBase::get_root() const {
    return (this->_root);
}

const std::vector<std::string>& ContextBase::get_allow_methods() const {
    return (this->_allow_methods);
}

const std::map<std::string, std::string>& ContextBase::get_error_pages() const {
    return (this->_error_pages);
}

int ContextBase::get_client_max_body_size() const {
	return (this->_client_max_body_size);
}

const std::string&	ContextBase::get_root_config() const
{
	return (this->_root_config);
}

/*
** ------------------------------- EXCEPTIONS ---------------------------------
*/

const char* ContextBase::NotSupportedMethod::what() const throw() {
    return ("Error: Not supported method.");
}

const char* ContextBase::InvalidBodySizeValue::what() const throw() {
    return ("Error: Invalid client body size value.");
}

const char* ContextBase::UnknownErrorCode::what() const throw() {
    return ("Error: Unknown HTTP Code Error.");
}

const char* ContextBase::ErrorToOpenDefaultFile::what() const throw() {
    return ("Error: Failed to open default file, it may be removed -_-");
}
/* ************************************************************************** */