#ifndef CONTEXTBASE_HPP
# define CONTEXTBASE_HPP

# include "webserv.hpp"
# include "utils/utils.hpp"

class ContextBase
{
	protected:
		std::string							_absolute_route;
		std::string							_relative_route;
		int									_level;
		/*Map to save the information of the context*/
		string_vec_map						_map;
		/*Attributes*/
		std::string							_root;
		string_vec							_allow_methods;
		string_map							_error_pages;
		int									_client_max_body_size;
		/*Root of config*/
		std::string							_root_config;
		/*Error codes*/
		string_vec							_http_errors;
	public:
		ContextBase(std::string absolute_route, std::string relative_route, int level);
		ContextBase();
		virtual ~ContextBase();

		/*Getters for architecture*/
		const std::string&							get_absolute_route() const;
		const std::string&							get_relative_route() const;
		int											get_level() const;
		/*Getters*/
		string_vec_map& 							get_map(); // Can't be const because here we add values in parsing.
		const std::string& 							get_root() const;
    	const std::vector<std::string>&				get_allow_methods() const;
    	const std::map<std::string, std::string>& 	get_error_pages() const;		
		int											get_client_max_body_size() const;
		const std::string&							get_root_config() const;
		/*Setters*/
		void								set_error_pages(void);
		void								inherits_allow_methods_from_config(string_vec allow_methods_config);
		void								inherits_client_size_body_from_config(int client_max_body_size_config);
		void								save_root_config(std::string root_config);
		/*Basic checkers in context*/
		void		check_root(string_vec_map::iterator it);
		void		check_methods(string_vec_map::iterator it);
		void		check_client_size_body(string_vec_map::iterator it);
		void		check_error_pages(string_vec_map::iterator it);

		/*Utils of context*/
		void		copy_string_vector(string_vec &attribute_vec, string_vec values_map);
		bool		check_method_allowed(std::string request_method) const;
		/*Exceptions*/
		class NotSupportedMethod : public std::exception {
		public:
			const char* what() const throw();
		};

		class InvalidBodySizeValue : public std::exception {
		public:
			const char* what() const throw();
		};

		class UnknownErrorCode : public std::exception {
		public:
			const char* what() const throw();
		};

		class ErrorToOpenDefaultFile: public std::exception {
		public:
			const char* what() const throw();
		};
};

std::ostream &			operator<<( std::ostream & o, ContextBase const & i );

#endif /* ***************************************************** CONTEXTBASE_H */