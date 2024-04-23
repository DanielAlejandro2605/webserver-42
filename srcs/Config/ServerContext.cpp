#include "Config/ServerContext.hpp"

/*
** ------------------------------- CONSTRUCTOR --------------------------------
*/

ServerContext::ServerContext() : ContextBase()
{
	this->_auto_index = false;
    this->_upload = false;
	this->_cgi = NULL;
}

ServerContext::ServerContext(std::string absolute_route, std::string relative_route, int level)
	: ContextBase(absolute_route, relative_route, level)
{
	this->_auto_index = false;
    this->_upload = false;
	this->_cgi = NULL;
}

/*
** -------------------------------- DESTRUCTOR --------------------------------
*/

ServerContext::~ServerContext()
{
}

/*
** --------------------------------- METHODS ----------------------------------
*/

void	ServerContext::check_auto_index(string_vec_map::iterator it)
{
	std::string	auto_index_value = (*it).second[0];

	if (auto_index_value.compare("on") == 0)
		this->_auto_index = true;
	else if (auto_index_value.compare("off") == 0)
		this->_auto_index = false;
	else
		throw ServerContext::InvalidAutoIndexValue();
}

void	ServerContext::check_upload_location(string_vec_map::iterator it)
{
	/*Getting the value of upload location from the element (key, value) of the map*/
	std::string	upload_location = (*it).second[0];
	/*We check this stupid case*/
	if (upload_location.compare("yes") == 0)
    {
        this->_upload = true;
    }
    else if (upload_location.compare("not") == 0)
    {
        this->_upload = false;
    }
    else
    {
        throw ServerContext::InvalidUploadDirective();
    }
}

void	ServerContext::check_upload_script(string_vec_map::iterator it, std::string config_root)
{
	/*Getting the value of upload location from the element (key, value) of the map*/
	std::string	upload_script = (*it).second[0];
    std::string root_location;
    std::string full_path_upload_script;

    if (this->get_root().empty())
    {
        if (config_root.empty())
            throw ServerContext::PleaseSetARootDirective();
        else
            root_location.assign(config_root);
    }
    else
        root_location.assign(this->get_root());

    string_vec_map::iterator it_upload_location = this->_map.find("upload_location");
    if (it_upload_location == this->_map.end())
        throw ServerContext::ContextMustBeUploadable();
    else if (!(it_upload_location->second[0].compare("yes") == 0))
        throw ServerContext::ContextMustBeUploadable();

    root_location.append(this->get_absolute_route());
    full_path_upload_script.assign(secures_the_path(root_location, upload_script));
    if (file_stats(full_path_upload_script) != 2)
        throw ServerContext::CantUseUploadScript();
    this->_upload_script.assign(full_path_upload_script);
}


/*
** --------------------------------- ACCESSOR ---------------------------------
*/

std::vector<std::string> ServerContext::get_index() const {
    return this->_index;
}

bool ServerContext::get_auto_index() const {
    return this->_auto_index;
}

CGI* ServerContext::get_cgi() const
{
	return (this->_cgi);
}

bool ServerContext::get_upload() const
{
    return (this->_upload);
}

const std::string&	ServerContext::get_upload_script() const
{
    return (this->_upload_script);
}

/*
** --------------------------------- SETTERS ---------------------------------
*/

void ServerContext::set_cgi(CGI &ptr)
{
	this->_cgi = &ptr;
}

/*
** ------------------------------- EXCEPTIONS ---------------------------------
*/

const char* ServerContext::InvalidAutoIndexValue::what() const throw() {
    return ("Error: Invalid auto index value.");
}

const char* ServerContext::InvalidUploadDirective::what() const throw() {
    return ("Error: Invalid upload context value.");
}

const char* ServerContext::PleaseSetARootDirective::what() const throw() {
    return ("Please set a root directive, at least for the config context to be able to check upload script.");
}

const char* ServerContext::ContextMustBeUploadable::what() const throw() {
    return ("Error: Context must contains 'upload_location' set to 'yes' for specify upload script.");
}

const char* ServerContext::CantUseUploadScript::what() const throw() {
    return ("Error: Can't use upload script. Maybe it doesn't exist :)");
}

/* ************************************************************************** */