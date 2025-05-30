#include "Socket.hpp"

void	Socket::createSocket(int ai_family, int ai_socktype, int ai_protocol) {
	int enable_me = 1;

	if ((socket_fd = socket(ai_family, ai_socktype, ai_protocol)) < 0)
		throw Socket::SocketFailureException();
	if (setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &enable_me, sizeof(int)) < 0) // sets the options for the already created socket
		throw Socket::SocketSetOptException();
	std::cout << "    listening_Socket set_up is successful!" << std::endl;
}


void	Socket::bindSocket(struct sockaddr* ai_addr, socklen_t ai_addrlen) {
	if (bind(socket_fd, ai_addr, ai_addrlen) < 0)
		throw Socket::SocketBindException();
	std::cout << "    bindSocket worked successfully" << std::endl;
}


void	Socket::listenToSocket(void) {
	if (listen(socket_fd, 20) < 0) // 20 is the backlog. Put the socket into a listening state, ready to accept incoming connections.
		throw Socket::SocketListenException();
	std::cout << "    Now listening on socket #" << socket_fd << " for incoming connections" << std::endl;
}

void Socket::initListenSocket(const char* port) {
    struct addrinfo criteria_template;
    struct addrinfo* ai = NULL;
    struct addrinfo* temp = NULL;

    // Clear the hints structure
    memset(&criteria_template, 0, sizeof(criteria_template));
    criteria_template.ai_family = AF_INET;        // Use IPv4
    criteria_template.ai_socktype = SOCK_STREAM;  // TCP socket. Specifies TCP socket (as opposed to SOCK_DGRAM for UDP)
    criteria_template.ai_flags = AI_PASSIVE;      // Use local IP to bind (0.0.0.0) == listen on all net interface. Am using NULL in getaddrinfo

    // Get a list of address structures and the job of "criteria_template" ends here
    if (getaddrinfo(NULL, port, &criteria_template, &ai) != 0) {
        throw SocketInitException();
    }

    // Try to create and bind a socket from the list using a while loop
    temp = ai;
    while (temp != NULL) {
        try {
            createSocket(temp->ai_family, temp->ai_socktype, temp->ai_protocol);
            bindSocket(temp->ai_addr, temp->ai_addrlen);
            break;
        } catch (const std::exception& e) {
            std::cerr << e.what() << std::endl;
            if (socket_fd >= 0)
                close(socket_fd);
        }
        temp = temp->ai_next;
    }

    // Free the address info list
    freeaddrinfo(ai);

    // If none of the addresses worked, throw
    if(temp == NULL) {
        throw SocketBindException();
    }

    // Start listening for incoming connections
    listenToSocket();
}



void    Socket::setServerBlock(ServerBlock* sb) {
    this->_server_block = sb;
}


void    Socket::setPortFD(std::string port) {
    this->_port = port;
}


int		Socket::getSocketFD(void) {
	return (this->socket_fd);
}


void    Socket::closeSocket(void) {
    int n;

    n = close(this->getSocketFD());
    if (n < 0)
        throw SocketCloseException();
    else
        std::cout << "---SOCKET: " << this->getSocketFD() << " has been successfully closed!---" << std::endl;
}


ServerBlock*    Socket::getServerBlock(void) {
    return (this->_server_block);
}

void    Socket::assignAcceptedFD(int fd) {
    if (fd < 0)
        throw SocketAcceptException();
    this->socket_fd = fd;
}



const char * Socket::SocketCloseException::what() const throw ()
{
    return ("---Socket failed to close! :(---");
}
