#include "HandlerRequest.hpp"

/*
** ------------------------------- CONSTRUCTOR --------------------------------
*/

HandlerRequest::HandlerRequest( void ) {return;}

/*
** -------------------------------- DESTRUCTOR --------------------------------
*/

HandlerRequest::~HandlerRequest( void ) {return;}


/*
** --------------------------------- METHODS ----------------------------------
*/

void    HandlerRequest::read( Client & client, struct pollfd & poll_fd, Cluster & cluster )
{
    char        buffer[BUFFER_SIZE] = {0};
    ssize_t     bytesRead;

	bytesRead = recv(client.socket, buffer, BUFFER_SIZE - 1, 0);
	
	if (bytesRead == 0 || bytesRead == -1)
    {
		if (bytesRead == -1)
			::timeStamp(BGREY), std::cerr << RED "Error: recv() failed" RESET << std::endl;
        cluster.removeClient(client);
	}
    else
    {
        client.request.append(buffer, bytesRead);

        if (client.request.compare("\r\n") == 0)
            client.request.clear();

        checkTimeout(client);
        
        size_t  idx = client.request.find("\r\n\r\n");
        if (idx != std::string::npos)
            client.requestInfo.parseRequest(client.request, idx);

        if (client.requestInfo.isReqAlreadyShitty()
            || client.requestInfo.getParseState() == Request::PARSE_COMPLETE)
        {
            poll_fd.events = POLLOUT;
        }
    }
    return;
}

void    HandlerRequest::checkTimeout( Client & client )
{
    if (client.firstLoop)
    {
        client.firstLoop = false;
        client.lastActivity = time(NULL);
    }
    else
    {
        time_t checkTime = time(NULL);
        if ((checkTime - client.lastActivity) >= TIMEOUT)
        {
            ::timeStamp(BGREY), std::cerr << RED "\t[CLIENT " << client.socket << "] REQUEST TIMEOUT\n" RESET;
            client.requestInfo.setCode(_REQUEST_TIMEOUT);
            client.requestInfo.setConnection(false);
        }
    }
    return ;
}

/*
** --------------------------------- ACCESSOR ---------------------------------
*/


/* ************************************************************************** */