#include "Server.hpp"
#include <iterator>
#include <ostream>
#include <sys/poll.h>



Server::Server(Config* conf)
    : _fd_count(0), _fd_size(50), _config(conf)
{
    // Allocate memory for pollfd array
    _pfds = new pollfd[_fd_size];

    int i = 0;
    std::vector<ServerBlock*>::iterator it = _config->_server_blocks.begin();

    while (it != _config->_server_blocks.end()) {
        ServerBlock* serverBlock = *it;
        std::string port = serverBlock->getListeningPort(); // change function name based on my partner

        std::cout << "- Launching a server on port " << port << " at poll_fds[" << i << "]" << std::endl;

        // Create and configure listening socket
        Socket* listenSocket = new Socket;
        listenSocket->setPortFD(port);
        listenSocket->setServerBlock(serverBlock);
        listenSocket->initListenSocket(port.c_str());

        // Store the socket
        listenSock_vector.push_back(listenSocket);

        // Register socket FD with poll
        _pfds[i].fd = listenSocket->getSocketFD();
        _pfds[i].events = POLLIN | POLLOUT;

        _fd_count++;
        ++i;
        ++it;
    }
}


Server::~Server() {
    delete[] _pfds;

    std::map<int, Connection*>::iterator it = _connections.begin();
    while (it != _connections.end()) {
        delete it->second;
        ++it;
    }

    std::cout << "server: Shuts down" << std::endl;
}


 Server&	Server::operator=(const Server& original_copy) {
	if (this != &original_copy){
        this->_listenSocket = original_copy._listenSocket;
	    this->_hints = original_copy._hints;
	   this->_servinfo = original_copy._servinfo;
    }
    
	return (*this);
}

Socket* Server::findListeningSocketByFD(int fd) {
    std::vector<Socket*>::iterator it = listenSock_vector.begin();
    
    while (it != listenSock_vector.end()) {
        if (fd == (*it)->getSocketFD())
            return (*it);
        ++it;
    }
    return NULL;
}



void	Server::launchServer(void) {
	int poll_count;

	std::cout << "Launching server..." << std::endl;
    
    for (int i = 0; i < _fd_count; ++i) // Clear revents for all fds before calling poll(), avoids gabbage values
    {
            _pfds[i].revents = 0;
    }

    while (true) 
    {
		poll_count = poll(_pfds, _fd_count, -1); //  If the value of timeout is -1, the poll blocks indefinitely; until somehing happens
		if (poll_count < 0)
			throw PollException();

		for (int i = 0; i < _fd_count; ++i) {
			short revents = _pfds[i].revents;

			if (revents & POLLHUP) {
				disconnectClient(i); // start here Thursday 21 May 2025...
			}
			else if (revents & POLLIN) {
				Socket* listenSock = findListeningSocketByFD(_pfds[i].fd);
				if (listenSock) {
					processNewConnection(listenSock); // 22nd May
					break; // Only handle one new connection per loop iteration
				} 
                else 
                {
					readClientRequest(i); // start here 30th May
				}
			}
			else if (revents & POLLOUT) {
				respondToExistingConnection(i);
			}
		}
	}
}


const char * Server::PollException::what() const throw ()
{
    return ("Failure: Poll() returned negative value");
}



void Server::disconnectClient(int i) {
    int fd = _pfds[i].fd;

    // Close and safely delete the connection if it exists
    std::map<int, Connection*>::iterator it = _connections.find(fd);
    if (it != _connections.end()) {
        it->second->closeSocket(); 
        delete it->second; 
        _connections.erase(it);
    }

    // Compact the pollfd array by replacing the current entry with the last one
    if (i != _fd_count - 1) {
        _pfds[i] = _pfds[_fd_count - 1];
    }

    // Decrease the count of active fd.
    _fd_count--;

    std::cout << "--- Connection destroyed! (fd: " << fd << ") ---" << std::endl;
}




void Server::processNewConnection(Socket* listenSock) {
	socklen_t				addrlen = sizeof(sockaddr_storage);
	struct sockaddr_storage	remote_addr;
	char					remoteIP[INET_ADDRSTRLEN]; // d size of this array is a macro for IPv4 length

	Connection* new_connection = new ClientSession(); // implement later Thursday 29th May. Change n

	try {
		// Accept the incoming connection
		int client_fd = accept(listenSock->getSocketFD(), (struct sockaddr*)&remote_addr, &addrlen); // i casted the second parameter
		new_connection->assignAcceptedFD(client_fd);
		new_connection->setServerBlock(listenSock->getServerBlock());
	}
	catch (const std::exception& e) {
		std::cerr << "Failed to establish new connection: " << e.what() << std::endl;
		delete new_connection;
		return;
	}

	// Add new connection to the server’s tracking structures
	addConnection(new_connection->getSocketFD(), new_connection);

	// Log connection details
	const char* ip_str = inet_ntop(remote_addr.ss_family,
								   extractIPAddressPtr((struct sockaddr*)&remote_addr),
								   remoteIP,
								   INET_ADDRSTRLEN);

	std::cout << "---NEW CONNECTION: " << ip_str  // add colors later
			  << " on socket " << new_connection->getSocketFD()
			  << " over port " << new_connection->getServerBlock()->getListeningPort()
			  << "---" << std::endl; // 29th May
}


void Server::addConnection(int newfd, ClientSession* new_connect) {
    // Expand the _pfds array if needed
    if (_fd_count == _fd_size) {
        _fd_size *= 2;
        struct pollfd* new_pfds = new pollfd[_fd_size];
        std::copy(_pfds, _pfds + _fd_count, new_pfds);
        delete[] _pfds;
        _pfds = new_pfds;
    }

    // Register the new file descriptor for reading and writing
    // OS initiliazes revents but i decided to do it to avoid garbage values
    _pfds[_fd_count].fd = newfd;
    _pfds[_fd_count].events = POLLIN | POLLOUT;
    _pfds[_fd_count].revents = 0; 
    _fd_count++;


    std::pair<std::map<int, ClientSession*>::iterator, bool> result =
        _connections.insert(std::make_pair(newfd, new_connect));

    if (!result.second) {
        std::cerr << "Warning: Attempted to insert duplicate connection for fd " << newfd << std::endl;
        // Handle potential leak or reject duplicate (optional cleanup)
        delete new_connect;
    }
}


void* Server::extractIPAddressPtr(struct sockaddr *sa) {
    if (sa->sa_family == AF_INET)
        return &(((struct sockaddr_in*)sa)->sin_addr);
    else
        return &(((struct sockaddr_in6*)sa)->sin6_addr);
}


void Server::readClientRequest(int i) 
{
    int fd;
    int nbytes;
    char buf[2000];
    
    memset(buf, 0, 2000);

    fd = _pfds[i].fd;
    nbytes = recv(fd, buf, sizeof(buf), 0);

    if (nbytes <= 0) {
        if (nbytes == 0) {
            std::cout << "--SOCKET " << fd << "**The client ended it's side of the connection**" << std::endl;
        } 
        else {
            std::cerr << "Error reading from socket " << fd << std::endl;
        }
        disconnectClient(i);
    }

    else
	{
        _connections[fd]->handleRequest(buf); // 30th May
        memset(buf, 0, 2000); // just to be sure
	}
}

    
