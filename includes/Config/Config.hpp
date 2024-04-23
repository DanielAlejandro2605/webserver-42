/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mflores- <mflores-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/29 17:05:17 by dnieto-c          #+#    #+#             */
/*   Updated: 2024/01/31 16:30:40 by mflores-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONFIG_HPP
# define CONFIG_HPP

# include "ServerContext.hpp"
# include "Location.hpp"


class Config : public ServerContext
{
	private:
		unsigned short							_port;
		std::string								_port_str;
		std::string								_host;
		string_vec								_server_name;
		/*Locations*/
		std::vector<ContextBase *>				_context;
	public:
		Config();
		/*Default Config constructor*/
		Config(int);
		/*Operator overload*/
		Config &operator+=(Config const &rhs);
		/*Destructor*/
		~Config();

		
		/*Methods*/
		bool								cmp(std::string url, std::string path_loc);
		/*Get the location of this Config Server
		Can't be const because we add context in the parsing*/
		std::vector<ContextBase *>&			get_context();
		/*Getters*/
		unsigned short						get_port() const;
		const std::string&					get_port_str() const;
    	const std::string&					get_host() const;
    	const string_vec&					get_server_name() const;
		/*Set info from map*/
		void								set_info_from_map();
		/*Checkers*/
		void								check_listen(string_vec_map::iterator it);
		void								check_host(string_vec_map::iterator it);
		/*Debug*/
		void								print_config();
		class MissingListenDirective : public std::exception {
		public:
			const char* what() const throw();
		};

		class InvalideListenPort : public std::exception {
		public:
			const char* what() const throw();
		};

		class InvalideHost : public std::exception {
		public:
			const char* what() const throw();
		};
};

#endif /* ********************************************************** CONFIG_H */