#ifndef HANDLERCONTEXT_HPP
# define HANDLERCONTEXT_HPP

# include "Location.hpp"
# include "Config.hpp"

class HandlerContext
{
	private:
		/*Tokens*/
		tokens_pairs					_tokens;
		/*Tokens iterators*/
		it_tokens						_it_start;
		it_tokens						_it_end;
		/*Context string*/
		std::string						_context;
		/*Iterator of context*/
		std::string::iterator 			_context_it;
		/*
		Variables to create the `string_vec_map`, 
		both for the `Config` class and its respective `Location`s vector Class.
		*/
		int									_is_key;
		int									_brackets_level;
		/*Directives accepted*/
		string_vec							_accepted_directives_config;
		string_vec							_accepted_directives_location;
		/*Pointer to the `Config*/
		Config &							_config;
		/*Pointer to `Config` map*/
		string_vec_map&						_map_config;
		/*Pointer to locations*/
		std::vector<ContextBase *>			*_ptr_context;
	public:
		HandlerContext(Config &config);
		~HandlerContext();

		/*Methods*/
		void			get_tokens(std::string context_server);
		void			set_map_from_tokens(Config &config);
		void			add_key_values_to_map(std::string key, string_vec values);
		void			add_context_to_vector(string_vec context);
		void			insert_context_to_vector(int context_type, std::string context_path, std::string context_route);
		std::string		get_route_for_context();
		ContextBase*	search_context_by_level(int level);
		void			insert_key_value_to_map(string_vec_map &map, std::string key, string_vec values);
		bool			valide_directive_config(std::string directive, string_vec values);
		bool			valide_directive_location(std::string directive, string_vec values);
		/*Print functions*/
		void	print_tokens(void);
		void	print_map(string_vec_map &map);
		void	print_single_token(int token);

		class InvalidDirectiveConfig : public std::exception {
		public:
			const char* what() const throw();
		};
		
		class InvalidDirectiveLocation : public std::exception {
		public:
			const char* what() const throw();
		};
};


#endif /* *************************************************** HANDLERCONFIG_H */