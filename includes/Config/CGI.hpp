/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CGI.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mflores- <mflores-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/07 16:41:02 by dnieto-c          #+#    #+#             */
/*   Updated: 2024/02/02 18:12:06 by mflores-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CGI_HPP
# define CGI_HPP

# include "ContextBase.hpp"
# include "../Request.hpp"

class CGI : public ContextBase
{
	private:
		std::string		_executable;
	public:
		CGI(std::string absolute_route, std::string relative_route, int level);
		~CGI();

		/*Execution*/
		std::string		exec(const Request &request, std::string root_context, std::string path_resource);
		std::string		exec_get(const Request &request, std::string root_context, std::string path_resource);
		std::string		exec_post(const Request &request, std::string root_context, std::string path_resource);
		/*Methods*/
		void			set_info_from_map();
		char			**get_env_from_request(const Request &request, std::string root_context, std::string path_script);
		/*Checkets*/
		void			check_cgi_pass(string_vec_map::iterator it);
		void			print_cgi();

		class CantUseCGI: public std::exception {
		public:
			const char* what() const throw();
		};
};

std::ostream &			operator<<( std::ostream & o, CGI const & i );

#endif /* ************************************************************* CGI_H */