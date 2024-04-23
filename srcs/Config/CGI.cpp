#include "Config/CGI.hpp"

/*
** ------------------------------- CONSTRUCTOR --------------------------------
*/

CGI::CGI(std::string absolute_route, std::string relative_route, int level)
    : ContextBase(absolute_route, relative_route, level)
{
}

/*
** -------------------------------- DESTRUCTOR --------------------------------
*/

CGI::~CGI()
{
}

/*
** --------------------------------- METHODS ----------------------------------
*/

/*Execution*/
std::string		CGI::exec(const Request &request, std::string root_context, std::string path_resource)
{
    std::string cgi_response;
    std::string extension_path_resource;

    if (file_stats(path_resource) == 0)
        return ("Not Found");

    extension_path_resource = path_resource.substr(get_last_idx_of(path_resource, "."));

    if ((extension_path_resource.compare(".php") == 0 && this->_executable.compare("/usr/bin/php-cgi") != 0)
        || (extension_path_resource.compare(".py") == 0 && this->_executable.compare("/usr/bin/python3") != 0))
            return (cgi_response);

    if (request.getMethod().compare("GET") == 0)
    {
        cgi_response = this->exec_get(request, root_context, path_resource);
        return (cgi_response);
    }
    else if (request.getMethod().compare("POST") == 0)
    {
        cgi_response = this->exec_post(request, root_context, path_resource);
        return (cgi_response);
    }
    return ("");
}


std::string		CGI::exec_get(const Request &request, std::string root_context, std::string path_resource)
{
    std::string response_body;

    int pipe_fd[2];

    if (pipe(pipe_fd) == -1) {
        perror("pipe");
        return ("Error");
    }

    pid_t pid = fork();

    if (pid == -1) {
        perror("fork");
        return ("Error");
    }

    if (pid == 0)
    {
        close(pipe_fd[0]);
        dup2(pipe_fd[1], STDOUT_FILENO);


        const char* executable = this->_executable.c_str();
        char* 	script = const_cast<char*>(path_resource.c_str());
        char* 	argv[] = { const_cast<char*>(executable), script, NULL };
        char**	envp = get_env_from_request(request, root_context, path_resource);

        execve(argv[0], argv, envp);
        perror("execve");
        exit(1);
    }
    else 
    {
        close(pipe_fd[1]);

        int     status;
        char    buffer[BUFSIZ] = {0};
        size_t  bytes_read = 0;
        int     timeout = 10;
        pid_t 	result = waitpid(pid, &status, WNOHANG);
        
        while ((bytes_read = read(pipe_fd[0], buffer, sizeof(buffer))) > 0 && (result == 0 && timeout > 0))
        {
            response_body.append(buffer, bytes_read);
			sleep(1);
            result = waitpid(pid, &status, WNOHANG);
            timeout--;
        }


        if (result == 0)
        {
            ::timeStamp(BGREY), std::cerr << RED " Timeout: CGI script took too long to execute." RESET << std::endl;
            kill(pid, SIGKILL);
            waitpid(pid, &status, 0);
            response_body.assign("Timeout");
        }
		else
        	response_body.push_back('\0');

        close(pipe_fd[0]);
    }
    return (response_body);
}

std::string		CGI::exec_post(const Request &request, std::string root_context, std::string path_resource)
{
	std::string response_body;

    int out_pipe[2];
    int in_pipe[2];

    if (pipe(out_pipe) == -1) {
        perror("out pipe");
        return ("Error");
    }

    if (pipe(in_pipe) == -1) {
        close(out_pipe[0]);
        close(out_pipe[1]);
        perror("in pipe");
        return ("Error");
    }

    pid_t pid = fork();

    if (pid == -1) {
        perror("fork");
        return ("Error");
    }

    if (pid == 0)
    {
        /*Closing write fd of in_pipe*/
        close(in_pipe[1]);
        /*Dup of in_pipe for STDIN*/
        dup2(in_pipe[0], STDIN_FILENO);
        /*Close read fd in_pipe*/
        close(in_pipe[0]);

        /*Close read fd of out_pipe*/
        close(out_pipe[0]);
        /*Dup of out_pipe for STDOUT*/
        dup2(out_pipe[1], STDOUT_FILENO);
        /*Close write fd in out_pipe*/
        close(out_pipe[1]);

        const char* executable = this->_executable.c_str();
        char* 	script = const_cast<char*>(path_resource.c_str());
        char* 	argv[] = { const_cast<char*>(executable), script, NULL };
        char**	envp = get_env_from_request(request, root_context, path_resource);

        execve(argv[0], argv, envp);
        perror("execve");
        exit(1);
    }
    else 
    {
        close(out_pipe[1]);
        close(in_pipe[0]);

        const int chunk_size = BUFSIZ;
        const int buffer_length = request.getBody().length();
        // int total = 0;
        for (int i = 0; i < buffer_length; i += chunk_size) {
            std::string chunk = request.getBody().substr(i, chunk_size);
            // total += chunk.length();
            write(in_pipe[1], chunk.c_str(), chunk.length());
        }


        std::string chunk = request.getBody().substr(0, chunk_size);
        /*Sending only BUFSIZ bytes to the child process*/
        write(in_pipe[1], chunk.c_str(), chunk.length());
        close(in_pipe[1]);

        int status;
        pid_t result = waitpid(pid, &status, WNOHANG);
        int timeout = 10;
        while (result == 0 && timeout > 0)
        {
            sleep(1);
            result = waitpid(pid, &status, WNOHANG);
            timeout--;
        }

        if (result == 0)
        {
            ::timeStamp(BGREY), std::cerr << RED " Timeout: CGI script took too long to execute." RESET << std::endl;
            kill(pid, SIGKILL);
            waitpid(pid, &status, 0);
            response_body.assign("Timeout");
        }
        else
        {
            char buffer[4096] = {0};
            size_t bytes_read = 0;
            while ((bytes_read = read(out_pipe[0], buffer, sizeof(buffer))) > 0)
            {
                response_body.append(buffer, bytes_read);
            }
            response_body.push_back('\0');
        }
        close(out_pipe[0]);
    }
    return (response_body);
}

/*Get the env from request*/
char**	CGI::get_env_from_request(const Request &request, std::string root_context, std::string path_script)
{
    std::vector<std::string> env_vars;

    env_vars.push_back("QUERY_STRING=" + request.getQuery());
    env_vars.push_back("REQUEST_METHOD=" + request.getMethod());
    env_vars.push_back("SCRIPT_FILENAME=" + path_script);
	env_vars.push_back("REDIRECT_STATUS=200");
	env_vars.push_back("UPLOAD_STORE=" + root_context + request.getUri());
    env_vars.push_back("CONTENT_LENGTH=" + request.getTokenValue("content-length"));
    if (request.getTokenValue("content-type").empty() == 0)
    {
        env_vars.push_back("CONTENT_TYPE=" + request.getTokenValue("content-type"));
    }
	
	char **env = static_cast<char**>(malloc(sizeof(char *) * (env_vars.size() + 1)));
    for (size_t j = 0; j < env_vars.size(); j++)
	{
		env[j] = strdup(env_vars[j].c_str());
	}
	env[env_vars.size()] = 0;
	return env;
}


/*Checking all possible attributes on a CGI*/
void	CGI::set_info_from_map()
{
    for (string_vec_map::iterator it = this->_map.begin(); it != this->_map.end();it++)
    {
        if ((*it).first.compare("root") == 0)
            this->check_root(it);
        else if ((*it).first.compare("allow_methods") == 0)
            this->check_methods(it);
        else if ((*it).first.compare("error_page") == 0)
            this->check_error_pages(it);
        else if ((*it).first.compare("client_max_body_size") == 0)
            this->check_client_size_body(it);
        else if ((*it).first.compare("cgi_pass") == 0)
            this->check_cgi_pass(it);
    }
}

/*Checking if the executable passed in the `cgi_pass` directive exists and we can execute it*/
void	CGI::check_cgi_pass(string_vec_map::iterator it)
{
    std::string cgi_pass = (*it).second[0];

    if (!(access(cgi_pass.c_str(), F_OK) == 0 && access(cgi_pass.c_str(), X_OK) == 0))
        throw CGI::CantUseCGI();
    this->_executable.assign(cgi_pass);
}

/*
** --------------------------------- EXCEPTIIONS ---------------------------------
*/

const char* CGI::CantUseCGI::what() const throw() {
    return ("Error: Can't use CGI executable.");
}


/* ************************************************************************** */

void	CGI::print_cgi()
{
    int level;

    level = this->get_level();

    std::cout << std::setw(level * 4) << BGCYAN << "CGI" << RESET << std::endl;

    std::cout << std::setw(level * 4) << CYAN << this->get_absolute_route() << RESET << std::endl;
    
    std::cout << std::setw(level * 4) << " " << "|-[root-config]: " << BLUE << "{" << _root_config << "}" << RESET << std::endl;

    std::cout << std::setw(level * 4) << " " << "|-[cgi_pass]: " << CYAN << "{" << _executable << "}" << RESET << std::endl;

    std::cout << std::setw(level * 4) << " " << "|-[root]: " << CYAN << "{" << _root << "}" << RESET << std::endl;

    std::cout << std::setw(level * 4) << " " << "|-[allow_methods]: ";
    for (string_vec::iterator it = _allow_methods.begin(); it != _allow_methods.end(); ++it) {
        std::cout << CYAN << "{" << *it << "}";
    }
    std::cout << RESET << std::endl;

    std::cout << std::setw(level * 4) << " " << "|-[error_pages]: ";
    for (std::map<std::string, std::string>::iterator it = _error_pages.begin(); it != _error_pages.end(); ++it) {
        std::cout << CYAN << "{" << it->first << ": " << it->second << "}";
    }
    std::cout << RESET << std::endl;

    std::cout << std::setw(level * 4) << " " << "|-[client_max_body_size]: " << CYAN << "{" << _client_max_body_size << "}" << RESET << std::endl;
}