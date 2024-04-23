#ifndef SERVER_HPP
# define SERVER_HPP

# include "Config/Config.hpp"

class Server
{
	private:
		int					_server_socket;
		struct sockaddr_in	_server_address;

		unsigned short		_port;
		std::string			_host_address;
		std::string			_port_str;
		string_vec			_server_name;
		/*Config*/
		Config &			_config;
	public:
		Server(Config *config);
		~Server();

		/*Getters*/
		Config&				get_config() const;
		int					get_socket() const;
		unsigned short		get_port() const;
		const std::string&	get_host_address() const;
		const std::string&	get_port_str() const;
		const string_vec&	get_server_name() const;

		/*Utils*/
		void	print_server();
		/*Exceptions*/
		class CantCreateServerSocket : public std::exception {
		public:
			const char* what() const throw();
		};

		class GetAddrInfoFail : public std::exception {
		public:
			const char* what() const throw();
		};

		class SetSocketOptFail : public std::exception {
		public:
			const char* what() const throw();
		};

		class FcntlFail : public std::exception {
		public:
			const char* what() const throw();
		};

		class ErrorBinding : public std::exception {
		public:
			const char* what() const throw();
		};

		class ErrorListening : public std::exception {
		public:
			const char* what() const throw();
		};
};

#endif /* ********************************************************** SERVER_H */