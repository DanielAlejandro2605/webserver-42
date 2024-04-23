#ifndef SERVERCONTEXT_HPP
# define SERVERCONTEXT_HPP

# include "ContextBase.hpp"
# include "CGI.hpp"

class ServerContext : public ContextBase
{
	protected:		
		bool								_auto_index;
		string_vec							_index;
		bool								_upload;
		std::string							_upload_script;

		CGI									*_cgi;
	public:
		ServerContext();
		ServerContext(std::string absolute_route, std::string relative_route, int level);
		~ServerContext();

		/*Basic checkers*/
		void								check_auto_index(string_vec_map::iterator it);
		/*Checkers*/
		void								check_upload_location(string_vec_map::iterator it);
		void								check_upload_script(string_vec_map::iterator it, std::string config_root);
		/*Getters*/
		bool								get_auto_index() const;
    	std::vector<std::string> 			get_index() const;
		CGI*								get_cgi() const;
		bool								get_upload() const;
		const std::string&					get_upload_script() const;

		/*Setters*/
		void								set_cgi(CGI &ptr);
		/*Exceptions*/
		class InvalidAutoIndexValue : public std::exception {
		public:
			const char* what() const throw();
		};

		class InvalidUploadDirective: public std::exception {
		public:
			const char* what() const throw();
		};

		class PleaseSetARootDirective: public std::exception {
		public:
			const char* what() const throw();
		};

		class ContextMustBeUploadable: public std::exception {
		public:
			const char* what() const throw();
		};

		class CantUseUploadScript: public std::exception {
		public:
			const char* what() const throw();
		};
};

#endif /* *************************************************** SERVERCONTEXT_H */