#include "Config/Parser.hpp"
#include "Cluster.hpp"

// bool Cluster::_end_server = false;

void	sighandler(int signum)
{
	(void)signum;
	int max_dots = 3;
    std::cout << std::endl;
	for (int i = 0; i < max_dots; i++)
	{
		std::cout << BWHITE << "\t" << "Stopping Web Server ";
		for (int j = 0; j < i + 1; j++)
			std::cout << ".";
		std::cout << "\r";
		std::cout.flush();
		usleep(350000);
	}
	Cluster::_end_server = true;
}

void    close_everything( Cluster & cluster )
{
	std::vector<int>::iterator it;
    std::vector<int> sockets = cluster.get_server_sockets();
	for (it = sockets.begin(); it != sockets.end(); ++it)
		if (*it)
			close((*it));

    sockets = cluster.get_client_sockets();
	for (it = sockets.begin(); it != sockets.end(); ++it)
		if (*it)
			close((*it));

    return ;
}

int     main( int argc, char *argv[] )
{
    if (argc == 2)
    {
        signal(SIGINT, sighandler);
        signal(SIGPIPE, SIG_IGN);

        try {
            Parser parser(argv[1]);
            parser.run();
            Cluster cluster(parser.get_configs());
            cluster.run();
            close_everything(cluster);
        } catch(const std::exception& e)
        {
            std::cout << e.what() << std::endl;
        }
        return (EXIT_SUCCESS);
    }
    std::cerr << "Usage: " << argv[0] << " <config_file>" << std::endl;
    return (EXIT_FAILURE);
}