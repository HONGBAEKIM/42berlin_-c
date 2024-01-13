/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp	       		                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: agrun <agrun@student.42berlin.de>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/15 12:23:01 by agrun             #+#    #+#             */
/*   Updated: 2023/12/15 12:23:01 by agrun            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

//////////////////<hb1>////////
void Server::initServer() 
{
	// Create socket file descriptor
	if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
		perror("socket failed");
		exit(EXIT_FAILURE);
	}

	// Set socket options
	int opt = 1;
	if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
		perror("setsockopt");
		exit(EXIT_FAILURE);
	}

	// Set the socket to non-blocking mode
	fcntl(server_fd, F_SETFL, O_NONBLOCK);

	// Assigning values to the server _host structure
	_host.sin_family = AF_INET;
	_host.sin_addr.s_addr = INADDR_ANY; // Bind to any available interface
	_host.sin_port = htons(_port); // Port number

	// Bind the socket to the network _host and port
	if (bind(server_fd, (struct sockaddr *)&_host, sizeof(_host)) < 0) {
		perror("bind failed");
		exit(EXIT_FAILURE);
	}

	// Set the socket to listen for incoming connections
	if (listen(server_fd, 3) < 0) { // Backlog set to 3
		perror("listen");
		exit(EXIT_FAILURE);
	}

	std::cout << "Server initialized on port " << _port << std::endl;
}
//////////////////<hb2>////////

//////////////////<hb1>////////

void Server::startIOmultiplexing() 
{
    std::vector<struct pollfd> fds;
    struct pollfd server_fd_struct;
    server_fd_struct.fd = server_fd;
    server_fd_struct.events = POLLIN;
    server_fd_struct.revents = 0;
    fds.push_back(server_fd_struct);

    while (true) 
	{
        if (poll(&fds[0], fds.size(), -1) < 0) 
		{
            perror("poll error");
            break;
        }

        for (size_t i = 0; i < fds.size(); ++i) 
		{
            if (fds[i].revents & POLLIN) 
			{
                if (fds[i].fd == server_fd) 
				{
                    struct sockaddr_in client_addr;
                    socklen_t client_len = sizeof(client_addr);
                    int new_socket = accept(server_fd, (struct sockaddr *)&client_addr, &client_len);

                    if (new_socket < 0) {
                        perror("accept error");
                        continue;
                    }

                    fcntl(new_socket, F_SETFL, O_NONBLOCK);

                    struct pollfd new_fd_struct;
                    new_fd_struct.fd = new_socket;
                    new_fd_struct.events = POLLIN;
                    new_fd_struct.revents = 0;
                    fds.push_back(new_fd_struct);
                } 
				else 
				{
                    char buffer[1024] = {0};
                    ssize_t count = read(fds[i].fd, buffer, 1024);

                    if (count == -1 && errno != EAGAIN) 
					{
                        perror("read error");
                        close(fds[i].fd);
                    } 
					else if (count == 0) 
					{
                        close(fds[i].fd);
                    } 
					else 
					{
                        std::cout << "Message from client: " << buffer << std::endl;
                    }
                }
            }
        }
    }
}


/* 
In this code:
- The `startIOmultiplexing()` method first sets up a `pollfd` structure for the server socket and adds it to a vector.
- It then enters an infinite loop where it calls `poll()` to monitor for events on the server socket and any connected client sockets.
- When `poll()` indicates a read event (`POLLIN`) on the server socket, it means a new connection request is incoming. The server accepts this request and adds the new socket to the list of file descriptors being monitored.
- If a read event is detected on a client socket, the server reads data from the socket. If the read returns `-1` and the error is not `EAGAIN`, it indicates a read error. If the read returns `0`, it means the client has closed the connection. Otherwise, the server processes the received data (in this example, it simply prints the message).

### Step 3: Integrate and Test
Now that you have implemented the `startIOmultiplexing()` method 
*/

//////////////////<hb2>////////











void	Server::embedLocation()
{
	Location n;
	this->_locations.push_back(n);
}

///////////////////////////////////////////////////////////////////////////////	SETTERS

void	Server::setPort(int p)
{
	if (p)
		this->_port = p;
}

void	Server::setHost(struct sockaddr_in host)
{
	this->_host = host;
}

void	Server::setServerName(std::string name)
{
	if (name.empty() == false)
		this->_servername = name;
}

void	Server::setRoot(std::string root)
{
	if (root.empty() == false)
		this->_root = root;
}

void	Server::setClientMax(unsigned long c_max)
{
	if (c_max > 0)
		this->_clientMAX = c_max;
}

void	Server::setIndex(std::string index)
{
	if (index.empty() == false)
		this->_index = index;
}

void	Server::setAutoIndex(bool a_index)
{
	this->_autoindex = a_index;
}

void	Server::setErrorPages(int n, std::string path)
{
	if (path.empty() == true && n <= 0)
		return ;
	this->_errorpages.insert(std::pair<int, std::string>(n, path));
}

void	Server::setLocations(std::string path)
{
	std::vector<Location>::iterator v;
	Location n;

	this->_locations.push_back(n);
	v = this->_locations.end(); --v;
	v->setPath(path);
}

void	Server::setServerIP(struct sockaddr_in ip)
{
	this->_serverIP = ip;
}

void	Server::setListenFD(int fd)
{
	if (fd > -1)
		this->_listenFD = fd;
}

///////////////////////////////////////////////////////////////////////////////	GETTERS

int	Server::getPort() const
{
	return this->_port;
}

struct sockaddr_in	Server::getHost() const
{
	return this->_host;
}

std::string	Server::getServerName() const
{
	return this->_servername;
}

std::string	Server::getRoot() const
{
	return this->_root;
}

unsigned long	Server::getClientMax() const
{
	return this->_clientMAX;
}

std::string	Server::getIndex() const
{
	return this->_index;
}

bool	Server::getAutoIndex() const
{
	return this->_autoindex;
}

std::map<int, std::string>::iterator	Server::getErrorPages()
{
	return this->_errorpages.begin();
}

std::map<int, std::string>::iterator	Server::getErrorPagesEnd()
{
	return this->_errorpages.end();
}

std::vector<Location>::iterator	Server::getLocations()
{
	return this->_locations.begin();
}

std::vector<Location>::iterator	Server::getLocationsEnd()
{
	return this->_locations.end();
}

struct sockaddr_in	Server::getServerIP() const
{
	return this->_serverIP;
}

int	Server::getListenFD() const
{
	return this->_listenFD;
}

///////////////////////////////////////////////////////////////////////////////	(DE)CON

Server::~Server()				//only containers require clearing out
{
	std::map<int, std::string>::iterator s;
	std::map<int, std::string>::iterator e;

	size_t n(0);

	s = this->_errorpages.begin();
	e = this->_errorpages.end();
	while (s != e)
	{
		s->second.clear();
		s++;
	}

	std::vector<Location>::iterator v;
	if (this->_locations.empty() == false)
		n = this->_locations.size();

	v = this->_locations.begin();
	for (size_t i = 0; i < n; i++)
		this->_locations.erase(v++);
	return ;
}

Server	&Server::operator=(const Server &copy)
{
	this->_port = copy._port;
	this->_host = copy._host;
	this->_servername = copy._servername;
	this->_root = copy._root;
	this->_clientMAX = copy._clientMAX;
	this->_index = copy._index;
	this->_autoindex = copy._autoindex;
	this->_errorpages = copy._errorpages;
	this->_locations = copy._locations;
	this->_serverIP = copy._serverIP;
	this->_listenFD = copy._listenFD;

	return *this;
}

Server::Server(const Server &copy)
{
	*this = copy;
}

Server::Server()
{
	inet_pton(AF_INET, "0.0.0.0", &(this->_serverIP.sin_addr));	//use [...].sin_addr.s_addr
	inet_pton(AF_INET, "0.0.0.0", &(this->_host.sin_addr));		//to access the actual ip
	this->_port = 0;
	this->_servername = "";
	this->_root = "";
	this->_clientMAX = 0;
	this->_index = "";
	this->_autoindex = false;
	this->_listenFD = -1;
	return ;
}