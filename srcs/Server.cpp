#include "Server.hpp"

/*
** ------------------------------- CONSTRUCTOR --------------------------------
*/

Server::Server(Config *config) : _config((*config))
{
	/*Info address*/
	struct sockaddr_in *addr_in;
    struct addrinfo		hints, *res;
	/*Make server socket reusable for the kernel*/
	int					yes;
	/*For set the server socket as NON-BLOCKING*/
	int 				flags;

	/*Setting variables from Config*/
	this->_host_address = this->_config.get_host();
	this->_port_str = this->_config.get_port_str();
	this->_port = this->_config.get_port();
	this->_server_name = this->_config.get_server_name();

	if ((this->_server_socket = socket(AF_INET, SOCK_STREAM, 0)) == -1)
	{
		throw Server::CantCreateServerSocket();
	}

	/*Initializing server address struc*/
	std::memset(&this->_server_address, 0, sizeof(this->_server_address));
	/*Setting AF_INET for IPv4 address family.*/
	this->_server_address.sin_family = AF_INET;
	/*Initializing addrinfo for get the information about the address in the `Config` Class*/
    std::memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
	/*
	IMPORTANT
	Setting flags in passive mode. This means that the address will be used to bind a socket
	that will listen for incoming connections at that particular address.
	*/
    hints.ai_flags = AI_PASSIVE;

	if (getaddrinfo(this->_host_address.c_str(), this->_port_str.c_str(), &hints, &res) != 0)
    {
		throw Server::GetAddrInfoFail();
	}
    /*Copy the obtained IP address to the structure sockaddr_in of server socket*/
    addr_in = (struct sockaddr_in *)res->ai_addr;
    this->_server_address.sin_addr = addr_in->sin_addr;
    freeaddrinfo(res);
	
	/*Setting the address port for the server socket*/
	this->_server_address.sin_port = htons(this->_port);

	/*Init this for make clean*/
	std::memset(this->_server_address.sin_zero, '\0', sizeof this->_server_address.sin_zero);

	/*Make the server socket reusable after killing the server!*/
	yes = 1;
	if (setsockopt(this->_server_socket, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &yes, sizeof(yes)) < 0)
	{
		close(this->_server_socket);
		throw Server::SetSocketOptFail();
	}
	

	/*Setting the server socket as non-blocking*/
    flags = fcntl(this->_server_socket, F_GETFL, 0);
    if (flags == -1) {
		close(this->_server_socket);
		throw Server::FcntlFail();
    }
    if (fcntl(this->_server_socket, F_SETFL, flags | O_NONBLOCK) == -1) {
       	close(this->_server_socket);
		throw Server::FcntlFail();
    }

	if ((bind(this->_server_socket, (struct sockaddr*)&this->_server_address, sizeof(this->_server_address))) == -1)
    {
		close(this->_server_socket);
		throw Server::ErrorBinding();
    }

	if ((listen(this->_server_socket, 5)) == -1)
    {
		close(this->_server_socket);
		throw Server::ErrorListening();
    }
}

/*
** -------------------------------- DESTRUCTOR --------------------------------
*/

Server::~Server()
{
}

/*
** --------------------------------- METHODS ----------------------------------
*/

void	Server::print_server()
{
	std::cout << "Port: " << this->_port << std::endl;
	std::cout << "Host Address: " << this->_host_address << std::endl;
	std::cout << "Port String: " << this->_port_str << std::endl;
	
	std::cout << "Server Names:" << std::endl;
	for (string_vec::const_iterator it = this->_server_name.begin(); it != this->_server_name.end(); ++it) {
		std::cout << " - " << *it << std::endl;
	}
}

/*
** --------------------------------- ACCESSOR ---------------------------------
*/

Config&	Server::get_config() const
{
	return (this->_config);
}

int	Server::get_socket() const
{
	return (this->_server_socket);
}

unsigned short		Server::get_port() const
{
	return (this->_port);
}

const std::string&	Server::get_host_address() const
{
	return (this->_host_address);
}

const std::string&	Server::get_port_str() const
{
	return (this->_port_str);
}

const string_vec&	Server::get_server_name() const
{
	return (this->_server_name);
}

/*
** ------------------------------- EXCEPTIONS ---------------------------------
*/

const char* Server::CantCreateServerSocket::what() const throw() {
    return ("Error: Cannot create server socket.");
}

const char* Server::GetAddrInfoFail::what() const throw() {
    return ("Error: getaddrinfo() failed.");
}

const char* Server::SetSocketOptFail::what() const throw() {
    return ("Error: setting socket options.");
}

const char* Server::FcntlFail::what() const throw() {
    return ("Error: fcntl() failed.");
}

const char* Server::ErrorBinding::what() const throw() {
    return ("Error: bind() failed.");
}

const char* Server::ErrorListening::what() const throw() {
    return ("Error: listen() failed.");
}

/* ************************************************************************** */