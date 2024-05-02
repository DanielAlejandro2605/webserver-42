#include "Cluster.hpp"

/*
** ------------------------------- CONSTRUCTOR --------------------------------
*/

bool Cluster::_end_server = false;

Cluster::Cluster(std::vector <Config *> configs) : _configs(configs)
{
	this->_end_server = 0;
}

/*
** -------------------------------- DESTRUCTOR --------------------------------
*/

Cluster::~Cluster()
{
	for (std::vector<Server *>::iterator it = this->_servers.begin(); it != this->_servers.end(); it++)
	{
		if (*it)
		{
			delete (*it);
			*it = NULL;
		}
	}
	for (std::vector<Client *>::iterator it = this->_clients.begin(); it != this->_clients.end(); it++)
	{
		if (*it)
		{
			delete (*it);
			*it = NULL;
		}
	}
}

/*
** --------------------------------- METHODS ----------------------------------
*/
void	Cluster::run()
{
	Server			*server;
	Config			*config_ptr;
	int				i_output;

	server = NULL;
	i_output = 1;
	this->print_cluster_timestamp();
	for (this->_it_configs = this->_configs.begin(); this->_it_configs != this->_configs.end(); _it_configs++)
	{
		config_ptr = (*_it_configs);
		std::cout << BWHITE << "\t[SERVER #" << i_output << "]\t" RESET << std::endl;
		try {
			server = new Server(config_ptr);
			this->print_server_info_from_config(config_ptr);
			this->_server_sockets.push_back(server->get_socket());
			this->_servers.push_back(server);
			this->add_server_to_poll_vec(server->get_socket());
		} catch(const std::exception& e)
		{
			std::cout << "\t", ::timeStamp(BGREY);
			std::cout << RED << "\t" << e.what() << RESET << std::endl;
		}
		i_output++;
	}

	if (this->_servers.size() == 0)
	{
		std::cout << BGYELLOW << "There's not servers, there's not fun!" << RESET << std::endl;
		return ;
	}

	std::cout << BGREEN "\t" << this->_servers.size() << " server(s) running!" RESET << std::endl; 
	while (!Cluster::_end_server)
	{
		int	nfds = poll(&this->_poll_fds[0], this->_poll_fds.size(), -1);
		if (nfds == -1)
		{
			if (Cluster::_end_server)
                break;
            ::timeStamp(BGREY), std::cerr << RED "Error: poll() failed" RESET << std::endl;
			break;
		}
		for (size_t i = 0; i < this->_poll_fds.size(); ++i) // Iterate over all poll fds to handle events
		{
			if (this->_poll_fds[i].revents == 0)
				continue;
			if (i < this->_server_sockets.size())
			{
				if (this->_poll_fds[i].revents == POLLIN)
				{
					handleNewConnection(this->_poll_fds[i].fd);
				}
			}
			else
				handle_existing_client(this->_poll_fds[i]);
		}
	}
}

void	Cluster::handle_existing_client( struct pollfd & fd )
{
	for (_it_clients = _clients.begin(); _it_clients != _clients.end(); ++_it_clients)
	{
		if ((*_it_clients)->socket == fd.fd)
		{
			Client * client = *_it_clients;

			if (fd.revents & (POLLERR | POLLHUP | POLLNVAL))
			{
				removeClient(*client);
				return;
			}

			if (fd.revents & POLLIN)
			{
				HandlerRequest 	handle_request;
				handle_request.read(*client, fd, *this);
				return;
			}

			if (fd.revents & POLLOUT)
			{
				if (client->request.empty() && !client->requestInfo.isReqAlreadyShitty())
				{
					fd.events = POLLIN;
					return ;
				}
				
				if (client->response.empty())
				{
					HandlerResponse handler_response;
					::timeStamp(BGREY), std::cout << GREEN  << " [" << client->requestInfo.getMethod() << "]" PURPLE " [" << client->requestInfo.getUri() << "]" RESET << std::endl;
					client->response = handler_response.run(this->get_servers(), client->requestInfo);
				}

				ssize_t bytes_sent = send(client->socket, client->response.c_str(), client->response.size(), 0);
				if (bytes_sent < 0)
				{
					removeClient(*client);
					return ;
				}
				else if (bytes_sent == 0)
					return;

				if (client->requestInfo.keepConnectionAlive())
					resetClient(*client, fd);
				else
				{
					::timeStamp(BGREY), std::cout << BLUE "\t[CLIENT " << client->socket << "] REMOVED" RESET << std::endl;
					removeClient(*client);
				}
				return;
			}
		}
	}
	return;
}

bool	Cluster::set_nonBlock_socket( int & socket )
{
	int flags = fcntl(socket, F_GETFL, 0);
	if (flags == -1)
	{
		::timeStamp(BGREY), std::cerr << RED "Error: fcntl() failed to set client flags" RESET << std::endl;
		close(socket);
		return (false);
	}
	if ((fcntl(socket, F_SETFL, flags | O_NONBLOCK)) == -1)
	{
		::timeStamp(BGREY), std::cerr << RED "Error: fcntl() failed to set client socket to NON blocking" RESET << std::endl;
		close(socket);
		return (false);
	}
	return (true);
}

void	Cluster::handleNewConnection( int serverSocket )
{
	Client	*newClient = new Client;
	socklen_t client_len = sizeof(newClient->client_addr);
	int clientSocket = accept(serverSocket, (struct sockaddr*)&newClient->client_addr, &client_len);
	if (clientSocket == -1)
	{
		::timeStamp(BGREY), perror("Error accept() failed: ");
		return ;
	}

	if ((set_nonBlock_socket(clientSocket)) == false)
		return ;

	newClient->socket = clientSocket;
	newClient->firstLoop = true;
	newClient->lastActivity = time(NULL);
	newClient->request = "";
	newClient->response = "";
	this->_clients.push_back(newClient);
	this->_client_sockets.push_back(newClient->socket);
	this->add_client_to_poll_vec(newClient->socket);
}

void	Cluster::add_server_to_poll_vec(int server_socket)
{
	struct pollfd	new_server;

	new_server.fd = server_socket;
	new_server.events = POLLIN;
	new_server.revents = 0;
	this->_poll_fds.push_back(new_server);
}

void	Cluster::add_client_to_poll_vec( int client_socket )
{
	struct pollfd	new_client;

	new_client.fd = client_socket;
	new_client.events = POLLIN | POLLOUT;
	new_client.revents = 0;
	this->_poll_fds.push_back(new_client);
}

void    Cluster::removeClient( Client & client )
{
	for (this->_it_poll_fds = this->_poll_fds.begin(); this->_it_poll_fds != this->_poll_fds.end(); ++this->_it_poll_fds)
	{
		if (this->_it_poll_fds->fd == client.socket)
		{
			this->_poll_fds.erase(this->_it_poll_fds);
			break;
		}
	}
	close(client.socket);

	for (std::vector<Client *>::iterator it = this->_clients.begin(); it != this->_clients.end(); ++it)
	{
		if ((*this->_it_clients)->socket == (*it)->socket)
		{
			delete *it;
			this->_clients.erase(it);
			break;
		}
	}
    return ;
}

void	Cluster::resetClient( Client & client, struct pollfd & fd )
{
	client.firstLoop = true;
	client.requestInfo.resetClassMembers();
	client.request.clear();
	client.response.clear();
	fd.events = POLLIN;
}

/*
** --------------------------------- ACCESSOR ---------------------------------
*/

const std::vector<Server *>&	Cluster::get_servers() const
{
	return (this->_servers);
}

const std::vector<int>&		Cluster::get_server_sockets( void ) const
{
	return (this->_server_sockets);
}

std::vector<Client *>&	Cluster::get_clients( void )
{
	return (this->_clients);
}

const std::vector<int>&		Cluster::get_client_sockets( void ) const
{
	return (this->_client_sockets);
}

std::vector<struct pollfd> &	Cluster::get_poll_fds( void )
{
	return (this->_poll_fds);
}

/*
** ---------------------------------   UTILS  ----------------------------------
*/

void	Cluster::log_servers(void)
{
	std::string	msg; 
	int			server_socket;
	std::string	port;

	for (this->_it_servers = this->_servers.begin(); this->_it_servers != this->_servers.end(); this->_it_servers++)
	{
		server_socket = (*_it_servers)->get_socket();
		port = (*_it_servers)->get_port_str();
		msg.assign("\tSERVER[" + int_to_string(server_socket) + "] listening in " + "PORT " + port);
		log(MAGENTA, msg);
	}
}


void	Cluster::print_cluster_timestamp( void )
{
	int maxDots = 3;
	std::cout << std::endl;
	for (int i = 0; i < maxDots; i++)
	{
		std::cout << BCYAN << "\t" << "Initializing server(s) ";
		for (int j = 0; j < i + 1; j++)
			std::cout << ".";
		std::cout << "\r";
		std::cout.flush();
		usleep(350000);
	}
	std::cout << std::endl << BCYAN << "\t" << "Server(s) list :\n\n" << RESET;
}

void	Cluster::print_server_info_from_config(Config *config)
{
	string_vec	servers_name = config->get_server_name();

	std::cout << "\033[2K\r"; // for controlling the appearance of text in a terminal
	time_stamp(BGREY);
	std::cout << std::setw(4) << "\t[HOST]: " << config->get_host();
	std::cout << std::setw(4) << "\t" << "[PORT]: " << config->get_port() << std::endl;
	std::cout << std::setw(20) << "\t" << "\t[SERVER NAME(S)]: ";

	for (string_vec::iterator it = servers_name.begin(); it != servers_name.end(); it++)
	{
		std::cout << "{" << (*it) << "} ";
	}
	std::cout << std::endl << std::endl;
}

std::string	Cluster::getRevent( short revent ) const
{
    switch (revent) {
        case 0:
            return("NO EVENT");
        case POLLIN:
            return("POLLIN");
        case POLLPRI:
            return("POLLPRI");
        case POLLOUT:
            return("POLLOUT");
        case POLLERR:
            return("POLLERR");
        case POLLHUP:
            return("POLLHUP");
        case POLLNVAL:
            return("POLLNVAL");
        default:
            return("Unknown Event");
    }
}

/* ************************************************************************** */