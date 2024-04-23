#include "HandlerServer.hpp"

/*
** ------------------------------- CONSTRUCTOR --------------------------------
*/

HandlerServer::HandlerServer()
{
}

/*
** -------------------------------- DESTRUCTOR --------------------------------
*/

HandlerServer::~HandlerServer()
{
}

/*
** --------------------------------- METHODS ----------------------------------
*/

Server*	HandlerServer::choose_server(std::vector<Server *> servers, const Request &request)
{
	/*Request's host and port*/
	std::string				host;
	std::string 			port;

	/*For save the candidate by host (IP address)*/
	std::vector<Server *>	candidates_by_host;
	/*For save the candidate by server name*/
	std::vector<Server *>	candidates_by_server_name;
	/*Server pointer that must be set*/
	Server					*ptr_server;
	bool					is_ip_request;

	host = request.getHostname();
	port = request.getPort();
	ptr_server = NULL;

	/*Determines if the request has server name in the host*/
	is_ip_request = this->diff_host(host);
	(void)is_ip_request;
	/*Get candidates by host*/
	candidates_by_host = this->get_candidates_by_host(servers, host, port);
	/*There's not candidate by host address, so we have to check for server name*/
	if (candidates_by_host.size() == 0)
	{
		candidates_by_server_name = this->get_candidates_by_server_name(servers, host, port);
		/*If there is one or more matches by name, we assign to the first server*/
		if (candidates_by_server_name.size() > 0)
			ptr_server = this->assign_first_from_servers_vector(candidates_by_server_name);
		else
			/*Default server*/
			ptr_server = this->assign_first_from_servers_vector(servers);
	}
	else
	{
		/*There's multiple candidate by host, so we check by server name*/
		if (candidates_by_host.size() > 1)
		{
			/*If there is one or more matches by name, we assign to the first server*/
			candidates_by_server_name = this->get_candidates_by_server_name(candidates_by_host, host, port);
			if (candidates_by_server_name.size() > 0)
				ptr_server = this->assign_first_from_servers_vector(candidates_by_server_name);
			else
				/*Default server*/
				ptr_server = this->assign_first_from_servers_vector(servers);
		}
		else
			/*Only one match by host, so we choosee it*/
			ptr_server = this->assign_first_from_servers_vector(candidates_by_host);
	}
	return (ptr_server);
}

std::vector<Server *>	HandlerServer::get_candidates_by_host(std::vector<Server *> servers, std::string host, std::string port)
{
	Server					*ptr_server;
	std::vector<Server *>	candidates;

	for (this->_it_servers = servers.begin(); this->_it_servers != servers.end(); this->_it_servers++)
	{
		ptr_server = (*this->_it_servers);
		if ((host.compare(ptr_server->get_host_address()) == 0) && (port.compare(ptr_server->get_port_str()) == 0))
		{
			candidates.push_back(ptr_server);
		}
	}
	return (candidates);
}

std::vector<Server *>	HandlerServer::get_candidates_by_server_name(std::vector<Server *> servers, std::string host, std::string port)
{
	std::vector<Server *>	candidates;
	Server					*ptr_server;
	string_vec				servers_name_candidate;
	string_vec::iterator	it_find;

	for (this->_it_servers = servers.begin(); this->_it_servers != servers.end(); this->_it_servers++)
	{
		ptr_server = (*this->_it_servers);
		servers_name_candidate = ptr_server->get_server_name();
		it_find = std::find(servers_name_candidate.begin(), servers_name_candidate.end(), host); 
		if (it_find != servers_name_candidate.end() && (port.compare(ptr_server->get_port_str()) == 0))
		{
			candidates.push_back(ptr_server);
		}
	}
	return (candidates);
}

/*Deprecated for the moment*/
bool	HandlerServer::diff_host(std::string host_string)
{
	bool		is_ip;
	string_vec 	sub_strs;
	std::string sub_string;

	is_ip = true;
	size_t i = 0;
	size_t j = 0;
	while (i < host_string.size())
	{
		if (host_string[i] == '.')
		{
			sub_string = host_string.substr(j, i - j);
			sub_strs.push_back(sub_string);
			j = i + 1;
		}
		else if (host_string[i + 1] == '\0')
		{
			sub_string = host_string.substr(j, i - j + 1);
			sub_strs.push_back(sub_string);
		}
		i++;
	}

	/*It's potential a IP address*/
	if (sub_strs.size() == 4)
	{
		for (string_vec::iterator it_vec = sub_strs.begin(); it_vec !=  sub_strs.end(); it_vec++)
		{
			if(string_in_range((*it_vec), CHAR) == -1)
			{
				is_ip = false;
			}
		}
	}
	else
		is_ip = false;
	return (is_ip);
}

Server *	HandlerServer::assign_first_from_servers_vector(std::vector<Server *> servers)
{
	Server	*ptr_server;

	this->_it_servers = servers.begin();
	ptr_server = (*this->_it_servers);
	return (ptr_server);
}

/* ************************************************************************** */