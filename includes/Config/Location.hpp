/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Location.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mflores- <mflores-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/07 09:50:08 by dnieto-c          #+#    #+#             */
/*   Updated: 2024/02/01 19:25:34 by mflores-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LOCATION_HPP
# define LOCATION_HPP

# include "ServerContext.hpp"

class Location : public ServerContext
{
	private:
		std::string							_dir_return;
		std::string 						_alias;

	public:

		Location(std::string absolute_route, std::string relative_route ,int level);
		~Location();
		
		/*Set info from map*/
		void							set_info_from_map(std::string root_config);
		/*Getters*/
		const std::string&				get_redir() const;
		/*Print*/
		void							print_location();
};

#endif /* ******************************************************** LOCATION_H */