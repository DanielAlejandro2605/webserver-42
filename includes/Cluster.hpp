#ifndef CLUSTER_HPP
# define CLUSTER_HPP

# include "Server.hpp"
# include "Request.hpp"
# include "HandlerRequest.hpp"
# include "HandlerResponse.hpp"



struct Client
{
    int             	socket;
	bool				firstLoop;
	time_t				lastActivity;
	struct sockaddr_in	client_addr;
    Request         	requestInfo;
    std::string     	request;
    std::string     	response;
};

class Cluster
{
	private:
		std::vector<Config *>					_configs;
		std::vector<Config *>::iterator			_it_configs;

		/*Vector of server sockets*/
		std::vector<int>						_server_sockets;
		/*Vector of Server pointers*/
		std::vector<Server *>					_servers;
		/*Iterator for the vector of Servers*/
		std::vector<Server *>::iterator			_it_servers;
		/*Vector of struct pollfd*/
		std::vector<struct pollfd>				_poll_fds;
		/*Iterator of vector of struct pollfd*/
		std::vector<struct pollfd>::iterator	_it_poll_fds;

		/*Iterator for the vector of Servers*/
		std::vector<Client *>::iterator			_it_clients;
		/*Vector of client sockets*/
		std::vector<int>						_client_sockets;
		/* Vector of Clients */
		std::vector<Client *>					_clients;

	public:
		/*Attribute for stop the server*/
		static bool								_end_server;
		/*Constructor and Destructor*/
		Cluster(std::vector <Config *> configs);
		~Cluster();
		/*Methods*/
		void		run();
		void		handleNewConnection( int serverSocket );
		void		handle_existing_client( struct pollfd & fd );
		bool		set_nonBlock_socket( int & socket );
		void		add_server_to_poll_vec(int server_socket);
		void		add_client_to_poll_vec( int client_socket );
		void		removeClient( Client & client );
		void		resetClient( Client & client, struct pollfd & fd );
		/*Utils*/
		void		log_servers(void);
		void		print_cluster_timestamp(void);
		void		print_server_info_from_config(Config *server);
		std::string	getRevent( short revent ) const;

		/*Getters*/
		const std::vector<Server *>&	get_servers() const;
		const std::vector<int>&			get_server_sockets() const;
		std::vector<Client *>&			get_clients( void );
		const std::vector<int>&			get_client_sockets( void ) const;
		std::vector<struct pollfd> &	get_poll_fds( void );	
};



#endif /* ********************************************************* CLUSTER_H */