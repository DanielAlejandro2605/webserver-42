#ifndef DISPATCHER_HPP
# define DISPATCHER_HPP

# include "webserv.hpp"
# include "Config/Config.hpp"
# include "Config/CGI.hpp"

class Dispatcher
{
	private:
		/*Attributes*/
		std::string				_full_path_resource;
		std::string				_root_context;
		string_map				_error_pages;

		/*CGI Agent*/
		CGI						*_cgi_agent;
		/*Resource extensions supported*/
		static std::string 	RESOURCE_SUPPORTED[];
		/*Function*/
		size_t	supported_extension(std::string &str);
	public:
		Dispatcher();
		~Dispatcher();


		/********Methods***************/
		/*Dispatch resource (cgi, html, etc...)*/
		std::string	dispatch_resource(const Request &request);
		/*Dispatch resource raw*/
		std::string dispatch_resource_page(int status);
		/*Dispatch cgi response*/
		std::string dispatch_cgi_response(std::string cgi_response);
		/*Create HTTP RESPONSE*/
		std::string	dispatch_http_response(int status, std::string content_type, std::string header_message, int content_length, std::string body);
		/*Create HTTP REDIRECTION RESPONSE*/
		std::string	dispatch_http_redir_response(int status, std::string header_message, std::string redir_location);
		/*Dispatch error page*/
		std::string	dispatch_error_page(int error_http_code, std::string message);
		/*Dispatch auto index page*/
		std::string dispatch_auto_index_page(std::string directory);
		/*Create error page*/
		std::string	create_error_page(int error_http_code, std::string message);
		/*Create auto index page*/
		std::string create_auto_index_page(std::string directory);
		/*Read resource*/
		std::string read_resource(size_t extension_open_mode);
		/*Get content type header*/
		std::string get_content_type_by_extension(size_t extension);

		/*Getters*/
		const std::string&	get_root_context() const;

		/*Setters*/
		void		set_env(const ServerContext *context);
		/*Set root from context*/
		void		set_root_context(const ServerContext *context);
		/*Set full path resource*/
		void		set_full_path_resource(std::string resource);
		/*Set dispatcher variable*/
		void		set_dispatch_env(CGI *cgi, std::string root, std::string path, std::string resource);

};


#endif /* ****************************************************** DISPATCHER_H */