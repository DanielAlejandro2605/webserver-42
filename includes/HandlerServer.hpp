#ifndef HANDLERSERVER_HPP
# define HANDLERSERVER_HPP

# include "utils/utils.hpp"
# include "webserv.hpp"
# include "Server.hpp"
# include "Request.hpp"
# include "./Config/ServerContext.hpp"

class HandlerServer
{
	private:
		std::vector<Server *>::iterator			_it_servers;
	public:
		HandlerServer();
		~HandlerServer();

		/*Choose server by host and port*/
		Server*					choose_server(std::vector<Server *> servers, const Request &request);
		/*Utils for choose server*/
		bool					diff_host(std::string host_string);
		std::vector<Server *>	get_candidates_by_host(std::vector<Server *> servers, std::string host, std::string port);
		std::vector<Server *>	get_candidates_by_server_name(std::vector<Server *> servers, std::string host, std::string port);
		Server*					assign_first_from_servers_vector(std::vector<Server *> servers);
};

#endif /* *************************************************** HANDLERSERVER_H */