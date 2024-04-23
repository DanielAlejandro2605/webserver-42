#include "Config/Location.hpp"

/*
** ------------------------------- CONSTRUCTOR --------------------------------
*/

Location::Location(std::string absolute_route, std::string relative_route, int level)
    : ServerContext(absolute_route, relative_route, level)
{

}

/*
** -------------------------------- DESTRUCTOR --------------------------------
*/

Location::~Location()
{
}

/*
** --------------------------------- METHODS ----------------------------------
*/

/*Checking and setting all possible attributes on a Location Class*/
void    Location::set_info_from_map(std::string root_config)
{
    for (string_vec_map::iterator it = this->_map.begin(); it != this->_map.end();it++)
    {
        if ((*it).first.compare("root") == 0)
            this->check_root(it);
        if ((*it).first.compare("autoindex") == 0)
            this->check_auto_index(it);
        else if ((*it).first.compare("index") == 0)
            this->copy_string_vector(this->_index, (*it).second);
        else if ((*it).first.compare("allow_methods") == 0)
            this->check_methods(it);
        else if ((*it).first.compare("error_page") == 0)
            this->check_error_pages(it);
        else if ((*it).first.compare("client_max_body_size") == 0)
            this->check_client_size_body(it);
        else if ((*it).first.compare("return") == 0)
            this->_dir_return.assign((*it).second[0]);
        else if ((*it).first.compare("upload_location") == 0)
            this->check_upload_location(it);
        else if ((*it).first.compare("upload_script") == 0)
            this->check_upload_script(it, root_config);
    }
}


void    Location::print_location()
{
    int level;

    level = this->get_level();

    std::cout << std::setw(level * 4) << BGBLUE << "Location" << RESET << std::endl;

    std::cout << std::setw(level * 4) << BLUE << this->get_absolute_route() << RESET << std::endl;
    
    std::cout << std::setw(level * 4) << " " << "|-[root-config]: " << BLUE << "{" << _root_config << "}" << RESET << std::endl;

    std::cout << std::setw(level * 4) << " " << "|-[root]: " << BLUE << "{" << _root << "}" << RESET << std::endl;

    std::cout << std::setw(level * 4) << " " << "|-[return]: " << BLUE << "{" << _dir_return << "}" << RESET << std::endl;

	if (_auto_index == true)
		std::cout << std::setw(level * 4) << " " << "|-[autoindex]: " << BLUE << "{" << "on" << "}" << RESET << std::endl;
	else
		std::cout << std::setw(level * 4) << " " << "|-[autoindex]: " << BLUE << "{" << "off" << "}" << RESET << std::endl;

    std::cout << std::setw(level * 4) << " " << "|-[index]: ";
    for (string_vec::iterator it = _index.begin(); it != _index.end(); ++it) {
        std::cout << BLUE << "{" << *it << "}";
    }
    std::cout << RESET << std::endl;

    std::cout << std::setw(level * 4) << " " << "|-[allow_methods]: ";
    for (string_vec::iterator it = _allow_methods.begin(); it != _allow_methods.end(); ++it) {
        std::cout << BLUE << "{" << *it << "}";
    }
    std::cout << RESET << std::endl;

    std::cout << std::setw(level * 4) << " " << "|-[error_pages]: ";
    for (std::map<std::string, std::string>::iterator it = _error_pages.begin(); it != _error_pages.end(); ++it) {
        std::cout << BLUE << "{" << it->first << ": " << it->second << "}";
    }
    std::cout << RESET << std::endl;

    std::cout << std::setw(level * 4) << " " << "|-[client_max_body_size]: " << BLUE << "{" << _client_max_body_size << "}" << RESET << std::endl;

    if (_upload == true)
    {
		std::cout << std::setw(level * 4) << " " << "|-[upload]: " << BLUE << "{" << "yes" << "}" << RESET << std::endl;
        std::cout << std::setw(level * 4) << " " << "|-[upload_script]: " << BLUE << "{" << this->_upload_script << "}" << RESET << std::endl;
    }
	else
		std::cout << std::setw(level * 4) << " " << "|-[upload]: " << BLUE << "{" << "not" << "}" << RESET << std::endl;

    if (this->_cgi)
    {
        this->_cgi->print_cgi();
    }
}

/*
** --------------------------------- GETTERS ----------------------------------
*/

const std::string&  Location::get_redir() const
{
    return (this->_dir_return);
}

/* ************************************************************************** */
