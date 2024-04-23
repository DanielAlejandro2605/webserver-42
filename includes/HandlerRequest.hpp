#ifndef HANDLERREQUEST_HPP
# define HANDLERREQUEST_HPP

# include "utils/utils.hpp"
# include "webserv.hpp"
# include "Cluster.hpp"
# include "Server.hpp"
# include "Request.hpp"


struct Client;
class Cluster;

class HandlerRequest
{
	private:
		std::vector<Server *>::iterator			_it_servers;
		std::vector<Client *>::iterator			_it_clients;
		std::vector<struct pollfd>::iterator	_it_poll_fds;
	public:
		HandlerRequest();
		~HandlerRequest();

		void					read( Client & client, struct pollfd & poll_fd, Cluster & cluster );
		void					checkTimeout( Client & client );
};

#endif /* *************************************************** HANDLEREQUEST_H */