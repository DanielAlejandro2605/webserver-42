#include "HandlerResponse.hpp"

/*
** ------------------------------- CONSTRUCTOR --------------------------------
*/

HandlerResponse::HandlerResponse()
{
}

/*
** -------------------------------- DESTRUCTOR --------------------------------
*/

HandlerResponse::~HandlerResponse()
{
}

/*
** --------------------------------- METHODS ----------------------------------
*/

std::string	HandlerResponse::run(std::vector<Server *> servers, const Request &request)
{
	/*Server choosed for serve the response*/
	Server			*server;
	/*Specify context in the server for serve the response*/
	ServerContext 	*context;
	/*Handle server to search for the right server*/
	HandlerServer	handler_server;
	/*Response*/
	std::string		response;

	server = handler_server.choose_server(servers, request);
	if (server != NULL)
	{
		Config &config = server->get_config();

		context = this->search_correct_context(request.getUri(), config);
		// REDIRECTION CASE -> SEND HTTP RESPONSE 301 
		if (this->_redir_response.empty() == 0)
		{
			return (this->_redir_response);
		}

		/*We did not find a specific context, so it will be the config context that will serve the response*/
		if (!context)
		{
			context = &config;
			this->set_uri_for_config_request();
			this->dispatcher.set_env(&config);
		}
		else
		{
			this->dispatcher.set_env(context);
		}
		response = this->create_response(context, request);
	}
	return (response);
}

ServerContext*	HandlerResponse::search_correct_context(std::string url, Config &config)
{
	Location *context_location = NULL;

	if (url.compare("/") == 0)
	{
		this->_path_uri.assign(url);
		return (&config);
	}
	context_location = this->search_context(url, config);
	if (!context_location)
	{
		size_t idx = get_last_idx_of(url, "/");
		if (idx != std::string::npos)
		{
			if (idx == 0)
			{
				this->_path_uri.assign("/");
				this->_resource_uri.assign(url.substr(idx + 1));
				return (&config);
			}
			else
			{
				this->_path_uri.assign(url.substr(0, idx + 1));
				this->_resource_uri.assign(url.substr(idx + 1));
				context_location = search_context(this->_path_uri, config);
			}
		}
	}
	else
		this->_path_uri.assign(url);
	return (context_location);
}

/*Search the correct context by `url` and handling redirections*/
Location*	HandlerResponse::search_context(std::string url, Config &config)
{
	std::vector<ContextBase *>& context_vec = config.get_context();
	std::vector<ContextBase *>::const_iterator it = context_vec.begin();
	std::string path_loc;

	Location *context_location = NULL;
	while (it != context_vec.end())
	{
		context_location = dynamic_cast<Location *>(*it);
		if (context_location)
		{
			path_loc = context_location->get_absolute_route();
			if (config.cmp(url, path_loc))
			{
				/*Config redirecton*/
				if (context_location->get_redir().empty() == 0)
				{
					this->set_redir_response(context_location->get_redir());
					return (this->search_context(context_location->get_redir(), config));
				}
				/*Manually redirection*/
				else if (ends_with(path_loc, "/") == true && ends_with(url, "/") == false)
				{
					this->set_redir_response(path_loc);
				}
				break;
			}
		}
		context_location = NULL;
		it++;
	}
	return (context_location);
}

void	HandlerResponse::set_redir_response(std::string redir_location)
{
	this->_redir_response = this->dispatcher.dispatch_http_redir_response(301, "Moved Permanently", redir_location);
}

void				HandlerResponse::set_uri_for_config_request(void)
{
	if (!this->_path_uri.empty())
	{
		this->_resource_uri = this->_path_uri.substr(1);
		this->_path_uri = this->_path_uri.substr(0, 1);
	}
}

std::string HandlerResponse::create_response( ServerContext *context, const Request &request)
{
	HttpMethod	request_method;

	/*Simple responses from errors or conflicts from the request*/
	if (request.getCode() == _HTTP_VERSION_NOT_SUPPORTED)
		return (this->dispatcher.dispatch_error_page(_HTTP_VERSION_NOT_SUPPORTED, "HTTP Version Not Supported"));
	
	if (request.getCode() == _BAD_REQUEST)
		return (this->dispatcher.dispatch_error_page(_BAD_REQUEST, "Bad request"));

	if (request.getCode() == _REQUEST_TIMEOUT)
		return (this->dispatcher.dispatch_error_page(_REQUEST_TIMEOUT, "Request Timeout"));
	if ((int)request.getBody().length() > context->get_client_max_body_size())
		return (this->dispatcher.dispatch_error_page(_PAYLOAD_TOO_LARGE, "Request Entity Too Large"));

	if (context->check_method_allowed(request.getMethod()) == false)
		return (this->dispatcher.dispatch_error_page(_METHOD_NOT_ALLOWED, "Method Not Allowed"));
	
	request_method = request.getHttpMethod();
	switch (request_method)
	{
		case _GET:
			return (this->response_get(context, request));
		case _POST:
			return (this->response_post(context, request));
		case _DELETE:
			return (this->response_delete());
		case _METHOD_NOT_SET:
			return (this->dispatcher.dispatch_error_page(_METHOD_NOT_ALLOWED, "Method Not Allowed"));
		default:
			std::cout << request_method << std::endl;
	}
	return (this->dispatcher.dispatch_error_page(_NOT_FOUND, "Not Found"));
}

std::string		HandlerResponse::response_delete( void )
{
	int		last_stat = 0;

	if (this->_resource_uri.empty() || this->_resource_uri.find("..") != std::string::npos)
		return (this->dispatcher.dispatch_error_page(_BAD_REQUEST, "Bad Request"));
	std::string path = this->dispatcher.get_root_context() + this->_path_uri + this->_resource_uri;
	last_stat = file_stats(path);
	if (last_stat == 0)
		return (this->dispatcher.dispatch_error_page(_NOT_FOUND, "Not Found"));
	else if (last_stat == 1)
		return (this->dispatcher.dispatch_error_page(_FORBIDDEN, "Forbidden"));
	else if (last_stat == 2)
	{
		if (std::remove(const_cast<char*>(path.c_str())) == -1)
			return (this->dispatcher.dispatch_error_page(_INTERNAL_SERVER_ERROR, "Internal Server Error"));
		else
			return (this->dispatcher.dispatch_error_page(_NO_CONTENT, "No Content"));
	}
	return (this->dispatcher.dispatch_error_page(_NOT_FOUND, "Not Found"));
}

std::string		HandlerResponse::response_post(ServerContext *context, const Request &request)
{
	std::string body_message;

	if (this->_resource_uri.empty() == 0)
		return (this->dispatcher.dispatch_error_page(_BAD_REQUEST, "Bad Request: POST to a non location"));

	if (context->get_upload())
	{
		this->dispatcher.set_full_path_resource(context->get_upload_script());
		return (this->dispatcher.dispatch_resource(request));
	}
	else
	{
		if (context->get_absolute_route().empty())
			body_message.assign("<h1>Handling POST request to /</h1>");
		else
			body_message.assign("<h1>Handling POST request to " + context->get_absolute_route() + "</h1>");
		return (this->dispatcher.dispatch_http_response(200, "Content-Type: text/html", "OK", body_message.length(), body_message));
	}
}

std::string		HandlerResponse::response_get( ServerContext *context, const Request &request)
{
	/*There's not a resource requested*/
	if (this->_resource_uri.empty())
		return (this->request_to_location(context, request));

	this->dispatcher.set_full_path_resource(this->dispatcher.get_root_context() + this->_path_uri + this->_resource_uri);
	return (this->dispatcher.dispatch_resource(request));
}


/*This function will set the correct directory for search a `index` (directive) candidate*/
std::string		HandlerResponse::request_to_location( ServerContext *context, const Request &request)
{
	/*Final root directory for the dispatched page*/
	std::string				root_directory(this->dispatcher.get_root_context() + context->get_absolute_route());
	string_vec				index_candidates;
	std::string				resource;
	int						last_stat;
	string_vec::iterator 	it;

	index_candidates = context->get_index();
	last_stat = 0;
	it = index_candidates.begin();
	while (it != index_candidates.end())
	{
		resource.assign(secures_the_path(root_directory,(*it)));
		last_stat = file_stats(resource);
		if (last_stat != 0)
			break ;
		it++;
	}

	/*Any index has been found*/
	if (last_stat == 0)
	{
		if (context->get_auto_index())
			return (this->dispatcher.dispatch_auto_index_page(root_directory));
		return (this->dispatcher.dispatch_error_page(_OK, "It's a directory"));		
	}
	/*Index resource was found but it can be readed*/
	else if (last_stat == 1)
		return (this->dispatcher.dispatch_error_page(_FORBIDDEN, "Forbidden"));
	/*Index resource was found and it can be dispatched*/
	else if (last_stat == 2)
	{
		this->dispatcher.set_full_path_resource(resource);
		return (this->dispatcher.dispatch_resource(request));
	}
	return (this->dispatcher.dispatch_error_page(_NOT_FOUND, "Not Found"));
}

/*
** --------------------------------- ACCESSOR ---------------------------------
*/


/* ************************************************************************** */