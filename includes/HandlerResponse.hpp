#ifndef HANDLERRESPONSE_HPP
# define HANDLERRESPONSE_HPP

# include "utils/utils.hpp"
# include "webserv.hpp"
# include "Server.hpp"
# include "Request.hpp"
# include "./Config/Location.hpp"
# include "HandlerServer.hpp"
# include "Dispatcher.hpp"

class HandlerResponse
{
	private:			
		Dispatcher				dispatcher;
		std::string				_path_uri;
		std::string				_resource_uri;
		std::string				_redir_response;
	public:
		HandlerResponse();
		~HandlerResponse();

		std::string				run(std::vector<Server *> servers, const Request &request);
		ServerContext*			search_correct_context(std::string url, Config &config);
		Location*				search_context(std::string url, Config &config);
		void					set_redir_response(std::string redir_location);
		std::string 			create_response(ServerContext *context, const Request &request);
		/*Response according to the method*/
		std::string				response_delete( void );
		std::string				response_get(ServerContext *context, const Request &request);			
		std::string				response_post(ServerContext *context, const Request &request);
		std::string				request_to_location(ServerContext *context, const Request &request);
		void					set_uri_for_config_request(void);
};

#endif /* ************************************************* HANDLERRESPONSE_H */