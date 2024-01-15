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
	
	// AF_INET : the address family (IPv4 Internet protocols)
    // SOCK_STREAM : the socket type for TCP (as opposed to SOCK_DGRAM for UDP)
    // 0 : the default protocol (TCP for SOCK_STREAM).
	if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) 
	{
		perror("socket failed");
		exit(EXIT_FAILURE);
	}

	// Set socket options

	//server_fd : the file descriptor for the server socket.
    //SOL_SOCKET : the options are at the socket level.
    //SO_REUSEADDR and SO_REUSEPORT are used to allow the socket to bind to an address/port that is already in use
    //opt : variable used with the setsockopt function to enable the SO_REUSEADDR and SO_REUSEPORT options on the socket.
	
	//When you pass &opt and sizeof(opt) to setsockopt, 
    //you're telling the function to use the value of opt (which is 1) 
    //to set the state of SO_REUSEADDR and SO_REUSEPORT. 
    //This enables the socket to bind to a port 
    //even if it is already used by a socket in TIME_WAIT state, enhancing the robustness of binding, 
    //especially in development and testing scenarios where servers are restarted frequently.
	int opt = 1;
	if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) 
	{
		perror("setsockopt");
		exit(EXIT_FAILURE);
	}

	// Set the socket to non-blocking mode

	//fcntl is a versatile function used to manipulate file descriptors.
    //server_fd is the file descriptor for the server socket.
    //F_SETFL is the command used to set the file status flags.
    //O_NONBLOCK is the flag set on the socket, which enables non-blocking mode.
	fcntl(server_fd, F_SETFL, O_NONBLOCK);

	// Assigning values to the server _host structure

	//Sets the address family to AF_INET, which means IPv4 addresses will be used.
	_host.sin_family = AF_INET;
	//Assigns the IP address for the socket. 
	_host.sin_addr.s_addr = INADDR_ANY; // Bind to any available interface
	 //Sets the port number for the socket. 
	_host.sin_port = htons(_port); // Port number

	// Bind the socket to the _host which is address of network and port
	if (bind(server_fd, (struct sockaddr *)&_host, sizeof(_host)) < 0) 
	{
		perror("bind failed");
		exit(EXIT_FAILURE);
	}

	// Set the socket to listen for incoming connections
	if (listen(server_fd, 3) < 0) 
	{
		perror("listen");
		exit(EXIT_FAILURE);
	}

	std::cout << "Server initialized on port " << _port << std::endl;
}
//////////////////<hb2>////////

//////////////////<hb1>////////
//poll() for handling mutiple client connections


/* 
In this code:
- The `startIOmultiplexing()` method first sets up a `pollfd` structure 
for the server socket and adds it to a vector.

- It then enters an infinite loop where it calls `poll()` to monitor 
for events on the server socket and any connected client sockets.

- When `poll()` indicates a read event (`POLLIN`) on the server socket, 
it means a new connection request is incoming. 
The server accepts this request and adds the new socket 
to the list of file descriptors being monitored.

- If a read event is detected on a client socket, the server reads data from the socket. 
If the read returns `-1` and the error is not `EAGAIN`, it indicates a read error. 
If the read returns `0`, it means the client has closed the connection. 
Otherwise, the server processes the received data (in this example, it simply prints the message).

*/

void Server::startIOmultiplexing() 
{
    //Each pollfd represents a file descriptor (socket) along with its events and revents (returned events).
	std::vector<struct pollfd> fds;
    
	//This structure is prepared for the server's main socket (referred to as server_fd).
	//It's set to monitor for POLLIN events, indicating incoming connections or data.
	struct pollfd server_fd_struct;
    server_fd_struct.fd = server_fd;
    server_fd_struct.events = POLLIN;
    server_fd_struct.revents = 0;
	
	//same like
	//fds.push_back({server_fd, POLLIN, 0});
	fds.push_back(server_fd_struct);

	//the server's continuous operation.
    while (true) 
	{
		//for monitoring multiple file descriptors to see if any of them have an event or change of state
        
        //&fds[0]: This provides a pointer to the underlying array of pollfd structures in the fds vector. 
        //It represents the file descriptors that poll will monitor.
        //fds.size(): This is the number of file descriptors that poll will monitor.
        int ret = poll(&fds[0], fds.size(), -1); // Timeout set to -1 for indefinite waiting
		if (ret < 0) 
		{
            perror("poll error");
            break;
        }
        for (size_t i = 0; i < fds.size(); ++i) 
		{
            //fds[i].revents: This is the field in the pollfd structure that poll() sets to indicate what event occurred.
            //POLLIN: This is a flag that indicates there is data to read (i.e., a read event). 
            //It could mean a new connection request on a listening socket or available data on a connected socket.
			if (fds[i].revents & POLLIN) 
			{
                //create new_socket
				if (fds[i].fd == server_fd) 
				{
                    struct sockaddr_in client_addr;
                    socklen_t client_len = sizeof(client_addr);
                    // Accept a new connection
					int new_socket = accept(server_fd, (struct sockaddr *)&client_addr, &client_len);
                    if (new_socket < 0) 
					{
                        perror("accept error");
                        continue;
                    }
					//to set the newly created socket (represented by new_socket) to operate in non-blocking mode:
                    fcntl(new_socket, F_SETFL, O_NONBLOCK);

                    //This structure is used to manage the new client socket.
					struct pollfd new_fd_struct;
                    new_fd_struct.fd = new_socket;
                    new_fd_struct.events = POLLIN;
                    new_fd_struct.revents = 0;
					//fds.push_back({new_socket, POLLIN, 0});

                    //the new client socket is included in the set of file descriptors 
					//that thepoll` system call will monitor in future iterations of the server's main loop.
					fds.push_back(new_fd_struct);
                } 
				//current file descriptor is not the server's main socket,
                //it means data is ready to be read from the client.
				else 
				{
                    // Handle read from a client socket
                    //initialized to store incoming data from the client.
					char buffer[1024] = {0};
					//read up to 1024 bytes from the client socket (fds[i].fd) into buffer
                    ssize_t count = read(fds[i].fd, buffer, 1024);

                    if (count == -1 && errno != EAGAIN) 
					{
                        perror("read failed");
                        close(fds[i].fd);
                    } 
					else if (count == 0) 
					{
                        // Client has closed the connection
						close(fds[i].fd);
                    } 
					else 
					{
                        // Process the read data
						std::cout << "Message from client: " << buffer << std::endl;
						
						//iterating over the vector fds, which contains structures of type pollfd. 
						//Each pollfd represents a client connection (or the server's listening socket).
						for (size_t j = 0; j < fds.size(); ++j) 
						{
							//if the current file descriptor is not the server's main listening socket
							if (fds[j].fd != server_fd) 
							{ 
								//send data over a socket
								
								//fds[j].fd : the file descriptor of the j-th client connected to the server
								//buffer : the message data to be sent
								//count : length of the message in bytes
								//0 : no special flags are used during the sending operation
								ssize_t sent = send(fds[j].fd, buffer, count, 0);
								if (sent < 0) 
								{
									perror("send failed");
								}
							}
						}
                    }
                }
            }
        }
    }
}




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