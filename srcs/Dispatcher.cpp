#include "Dispatcher.hpp"

/*
** ------------------------------- CONSTRUCTOR --------------------------------
*/

std::string Dispatcher::RESOURCE_SUPPORTED[] = {
	".php",
	".py",
	".html",
	".css",
	".svg",
	".jpg",
	".jpeg",
	".png",
	".mp4",
	".pdf",
	"undefined"
};


Dispatcher::Dispatcher()
{
	
}

/*
** -------------------------------- DESTRUCTOR --------------------------------
*/

Dispatcher::~Dispatcher()
{
}

/*
** --------------------------------- METHODS ----------------------------------
*/


std::string	Dispatcher::dispatch_resource(const Request &request)
{
	std::string body_cgi;

	if (this->_cgi_agent != NULL && (ends_with(this->_full_path_resource, ".php") || (ends_with(this->_full_path_resource, ".py"))))
	{
		body_cgi = this->_cgi_agent->exec(request, this->_root_context, this->_full_path_resource);
		if (body_cgi.compare("Error") == 0)
			return (this->dispatch_error_page(_INTERNAL_SERVER_ERROR, "Internal Server Error"));
		if (body_cgi.compare("Not Found") == 0)
			return (this->dispatch_error_page(_NOT_FOUND, body_cgi));
		if (body_cgi.compare("Timeout") == 0)
			return (this->dispatch_error_page(_GATEWAY_TIMEOUT, "Gateway Timeout"));
		if (body_cgi.find("\r\n") == std::string::npos)
			return (this->dispatch_error_page(_BAD_GATEWAY, "Bad Gateway"));

		return (this->dispatch_cgi_response(body_cgi));
	}
	return (this->dispatch_resource_page(200));
}

std::string Dispatcher::dispatch_cgi_response(std::string cgi_response)
{
	std::string response_body;
	std::string content_type;
	int			content_length;
	
	size_t code_cgi_idx = cgi_response.find("\r\n\r\n");
	
	/*If '/r/n/r/n' is not found, that means the CGI sent a correct HTTP Response*/
	if (code_cgi_idx == std::string::npos)
	{
		// CRLF (a carriage return and a line feed character
		size_t crlf = cgi_response.find("\r\n");
		if (code_cgi_idx == std::string::npos)
		{
			content_type = cgi_response.substr(0, crlf);
			response_body = cgi_response.substr(crlf);
			content_length = response_body.length();
			return (this->dispatch_http_response(200, content_type, "OK", content_length, response_body));
		}
		return (this->dispatch_error_page(_BAD_GATEWAY, "Bad Gatewayy"));
	}
	/*CGI has sent HTTP Code error, we handle this*/
	else
	{
		std::string code_cgi_str;
		int			code_cgi;

		if ((code_cgi_idx + 4) <= cgi_response.length())
		{
			code_cgi_str = cgi_response.substr(code_cgi_idx + 4);
			if (code_cgi_str.length() >= 3)
			{
				code_cgi = stringToInt(code_cgi_str.substr(0, 3));
				content_type = cgi_response.substr(0, cgi_response.find("\r\n"));
				code_cgi_idx = code_cgi_str.find("\r\n");
				if (code_cgi_idx != std::string::npos)
				{
					response_body = code_cgi_str.substr(code_cgi_str.find("\r\n"));
					content_length = response_body.length();
					return (this->dispatch_http_response(code_cgi, content_type, httpCodeToString(code_cgi), content_length, response_body));
				}
				return (this->dispatch_error_page(_BAD_GATEWAY, "Bad Gatewayy"));
			}
			return (this->dispatch_error_page(_BAD_GATEWAY, "Bad Gatewayy"));
		}
	}
	return (this->dispatch_error_page(_BAD_GATEWAY, "Bad Gatewayy"));
}

std::string Dispatcher::dispatch_resource_page(int status)
{
	std::string 	body;
	size_t 			extension;
	int				content_length;
	std::string		content_type;

	int stats = file_stats(this->_full_path_resource);
	if (stats == 0)
		return (this->dispatch_error_page(_NOT_FOUND, "Not Found"));
	else if (stats == 1)
		return (this->dispatch_error_page(_FORBIDDEN, "Forbidden"));
	else if (stats == 3)
		return this->dispatch_error_page(_OK, "It's a directory");

		
	extension = this->supported_extension(this->_full_path_resource);
	body = this->read_resource(extension);
	/*Header for HTTP Response*/
	content_length = body.length();
	content_type = this->get_content_type_by_extension(extension);
	return (this->dispatch_http_response(status, "Content-Type: " + content_type, "OK", content_length, body));
}

std::string	Dispatcher::dispatch_http_response(int status, std::string content_type, std::string header_message, int content_length, std::string body)
{
	std::string http_response = std::string(HTTP_VERSION_SUPPORTED) + " "
							+ intToString(static_cast<int>(status))
							+ " " + header_message + "\r\n"
							+  content_type + "\r\n"
							+ "Content-Length: " + intToString(static_cast<int>(content_length)) + "\r\n"
							+ "\r\n"
							+ body;

	return (http_response);
}

std::string	Dispatcher::dispatch_http_redir_response(int status, std::string header_message, std::string redir_location)
{
	std::string http_redir_response = std::string(HTTP_VERSION_SUPPORTED) + " "
							+ intToString(static_cast<int>(status))
							+ " " + header_message + "\r\n"
							+ "Location: " + redir_location + "\r\n"
							+ "Connection: close\r\n\r\n";

	return (http_redir_response);
}


std::string Dispatcher::get_content_type_by_extension(size_t extension)
{
	std::string extension_copy;
	std::string content_type;

	if (extension <= 3)
	{
		content_type.append("text");
	}
	else if (extension >= 4 && extension <= 7)
	{
		content_type.append("image");
	}
	else if (extension == 8)
	{
		content_type.append("video");
	}
	else if (extension == 9)
	{
		content_type.append("application");
	}
	else
	{
		return ("text/plain");
	}

	extension_copy.assign(this->RESOURCE_SUPPORTED[extension]);
	extension_copy.replace(extension_copy.find('.'), 1, "/");
	content_type.append(extension_copy);
    return content_type;
}

std::string Dispatcher::read_resource(size_t extension_open_mode)
{
	std::ifstream 	raw_file;
	std::string 	resource;
	std::string 	raw_line;

	if (extension_open_mode <= 2 || extension_open_mode == 11)
	{
		raw_file.open(this->_full_path_resource.c_str(), std::ifstream::in);
		while (std::getline(raw_file, raw_line))
		{
			resource.append(raw_line);
		}
	}
	else if (extension_open_mode >= 3 && extension_open_mode <= 10)
	{
		std::ostringstream buffer;

		raw_file.open(this->_full_path_resource.c_str(), std::ios::binary);
		buffer << raw_file.rdbuf();
		resource.assign(buffer.str());
	}
	raw_file.close();
	return (resource);
}

std::string	Dispatcher::dispatch_error_page(int error_http_code, std::string message)
{
	string_map::iterator it = this->_error_pages.find(intToString(error_http_code));
	
	if (it != this->_error_pages.end())
	{
		this->set_full_path_resource(this->_root_context + it->second);
		return (this->dispatch_resource_page(error_http_code));
	}

	std::string	body = create_error_page(error_http_code, message);
	int	content_length = body.length();

	std::string http_response = std::string(HTTP_VERSION_SUPPORTED) + " "
								+ intToString(static_cast<int>(error_http_code))
								+ " " + message + "\r\n"
								+ "Content-Type: text/html\r\n"
								+ "Content-Length: " + intToString(static_cast<int>(content_length)) + "\r\n"
								+ "\r\n"
								+ body;

	return (http_response);
}

std::string Dispatcher::create_error_page(int error_http_code, std::string message) {
    std::string page = "<!DOCTYPE html>\n"
                       	"<html lang='en'>"
                       		"<head>"
								"<meta charset='UTF-8'>"
								"<link type='image/png' rel='icon' href='./skull.png'>"
								"<title>Error " + intToString(static_cast<int>(error_http_code)) + "-" + message + "</title>"
								"<style>"
									"body {"
										"font-family: Arial, sans-serif;"
										"text-align: center;"
										"background-color: #f5f5f5;"
										"margin: 0;"
										"padding: 50px;"
									"}"
									"h1 {"
										"color: #333333;"
									"}"
									"p {"
										"color: #666666;"
									"}"

									"#background-video {"
										"visibility : hidden;"
									"}"
								"</style>"
								"<link rel='stylesheet' href='./webserv.css'>"
                       		"</head>"
                       		"<body>"
								"<h1>" + intToString(static_cast<int>(error_http_code)) + "</h1>"
								"<p>" + message + "</p>"
								"<p> Coded by mflores- and dnieto-c </p>"
								"<video id='background-video' autoplay loop muted>"
								"<source src='./webserv.mp4' type='video/mp4'>"
    							"</video>"
                       		"</body>"
                       "</html>";
    return page;
}

std::string Dispatcher::dispatch_auto_index_page(std::string directory)
{
	std::string	body = create_auto_index_page(directory);
	int	content_length = body.length();

	std::string http_response = std::string(HTTP_VERSION_SUPPORTED) + " "
								+ intToString(static_cast<int>(200))
								+ " OK\r\n"
								+ "Content-Type: text/html\r\n"
								+ "Content-Length: " + intToString(static_cast<int>(content_length)) + "\r\n"
								+ "\r\n"
								+ body;
	return (http_response);
}


std::string Dispatcher::create_auto_index_page(std::string directory)
{
	/*For open the directory*/
	string_vec	list_entities;
	DIR *dir;
	struct dirent *ent;
	std::stringstream result;

	if ((dir = opendir(directory.c_str())) == NULL)
		return (this->dispatch_error_page(_FORBIDDEN, "Forbidden"));
    while ((ent = readdir(dir)) != NULL)
	{
        std::string filename(ent->d_name);
        result << "<a href='" << filename << "'><li>" << filename << "</li></a>" << std::endl;
    }
    closedir(dir);

	std::string index_page = "<!DOCTYPE html>\n"
							"<html lang='en'>\n"
                       		"<head>\n"
								"<meta charset='UTF-8'>\n"
								"<title>" + directory + "</title>\n"
								"<style>\n"
									"body {\n"
										"font-family: Arial, sans-serif;\n"
										"text-align: center;\n"
										"background-color: #f5f5f5;\n"
										"margin: 0;\n"
										"padding: 50px;\n"
									"}\n"
									"h1 {\n"
										"color: #333333;\n"
										"text-align: left;\n"
									"}\n"
									"p {\n"
										"color: #666666;\n"
									"}\n"
									"ul {\n"
										"text-align: left;\n"
									"}\n"
								"</style>\n"
                       		"</head>\n"
                       		"<body>\n"
								"<h1>Index</h1>\n"
								"<ul>" + result.str() + "</ul>\n"
								"<p> Coded by mflores- and dnieto-c </p>\n"
                       		"</body>\n"
                       "</html>";
	return (index_page);
}

size_t Dispatcher::supported_extension(std::string& str) {
	size_t i;
	for (i = 0; i < sizeof(RESOURCE_SUPPORTED) / sizeof(RESOURCE_SUPPORTED[0]); i++) {
		if (ends_with(str, RESOURCE_SUPPORTED[i])) {
			return (i);
		}
	}
	return (i);
}

/*
** --------------------------------- SETTERS ---------------------------------
*/

void	Dispatcher::set_env(const ServerContext *context)
{
	if (context->get_root().empty() == 0)
		this->_root_context = context->get_root();
	else if (context->get_root_config().empty() == 0)
		this->_root_context = context->get_root_config();

	this->_cgi_agent = context->get_cgi();
	this->_error_pages = context->get_error_pages();
}


void	Dispatcher::set_root_context(const ServerContext *context)
{
	if (context->get_root().empty() == 0)
		this->_root_context = context->get_root();
	else if (context->get_root_config().empty() == 0)
		this->_root_context = context->get_root_config();
}

void	Dispatcher::set_full_path_resource(std::string resource)
{
	this->_full_path_resource = resource;
}

void	Dispatcher::set_dispatch_env(CGI *cgi, std::string root, std::string path, std::string resource)
{
	this->_cgi_agent = cgi;
	this->_full_path_resource.assign(root + path + resource);
}


/*
** --------------------------------- ACCESSOR ---------------------------------
*/

const std::string&	Dispatcher::get_root_context() const
{
	return (this->_root_context);
}

/* ************************************************************************** */