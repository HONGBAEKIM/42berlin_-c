/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerManager.cpp	       		                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: agrun <agrun@student.42berlin.de>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/01/24 14:23:01 by agrun             #+#    #+#             */
/*   Updated: 2023/01/24 14:23:01 by agrun            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ServerManager.hpp"

int	killer = 0;						// Will shut down the server if set to 1.

static void	disableServer(int sig)				// Signal functions must be static...
{
	std::signal(sig, SIG_IGN);
	if (sig == SIGINT)
	{
		char		buffer[100];
		tzset();
		time_t		now = time(0);
		struct tm	tm = *gmtime(&now);
		tm.tm_hour = tm.tm_hour + 1;
		strftime(buffer, 100, "[%Y-%m-%d  %H:%M:%S]", &tm);

		std::cout << std::endl;
		std::cout << "\x1B[94m" << buffer << "\x1B[0m" << "   ";
		std::cout << "Server ordered to shut down, quitting program." << std::endl;
		killer = 1;
	}
}

/*
	AF_INET 	: the address family (IPv4 Internet protocols)
	SOCK_STREAM 	: the socket type for TCP (as opposed to SOCK_DGRAM for UDP)
	0 		: the default protocol (TCP for SOCK_STREAM).
*/

int	ServerManager::createSocket()
{
	int socketFD = socket(AF_INET, SOCK_STREAM, 0);
	if (socketFD == 0)
	{
		perror("socket() failed");
		killer = 1;
		//exit(EXIT_FAILURE);
	}
	return socketFD;
}

/*	Set socket options:

	_listenFD : the file descriptor for the server socket.
    	SOL_SOCKET : the options are at the socket level.

    	SO_REUSEADDR & SO_REUSEPORT are used to allow the socket to bind to an address/port that is already in use
    	opt : variable used with the setsockopt function to enable the SO_REUSEADDR and SO_REUSEPORT options on the socket.
	
	When you pass &opt and sizeof(opt) to setsockopt, you're telling the function to use the value of opt (which is 1) 
   	to set the state of SO_REUSEADDR and SO_REUSEPORT.
 
    	This enables the socket to bind to a port even if it is already used by a socket in TIME_WAIT state, enhancing the 
	robustness of binding, especially in development and testing scenarios where servers are restarted frequently.
*/

void	ServerManager::configureSocketOptions(int socketFD)
{
	if (killer)
		return ;
	int opt = 1;
	if (LOCK_PORT)
	{
		if (setsockopt(socketFD, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)))
		{
			perror("setsockopt() failed");
			killer = 1;
		}
	}
	else
	{
		if (setsockopt(socketFD, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)))
		{
			perror("setsockopt() failed");
			killer = 1;
		}
	}
}

/*
     	this->_listenFD is the file descriptor for the server socket.
	F_SETFL is the command used to set the file status flags.
	O_NONBLOCK is the flag set on the socket, which enables non-blocking mode.
*/

void	ServerManager::setSocketNonBlocking(int socketFD)
{
	if (killer)
		return ;
	fcntl(socketFD, F_SETFL, O_NONBLOCK);				// Set the socket to non-blocking mode
}

void	ServerManager::configureHost(std::map<size_t, Server*>::iterator it)
{
	if (killer)
		return ;
	struct sockaddr_in	host;

	host.sin_family = AF_INET;					// Set the address protocol to IPv4
	host.sin_port = htons(it->second->_port[0]); 			// Set the port number for the socket

	host.sin_addr.s_addr = it->second->getHost().sin_addr.s_addr;
	//host.sin_addr.s_addr = INADDR_ANY; 				// Bind to any available interface... but why?
									// We have a pre-set host in the config-file!
	it->second->setHost(host);
}

void	ServerManager::bindSocket(int socketFD, std::map<size_t, Server*>::iterator it)		// Bind socket to host which is
{												// the address of network & port.
	if (killer)
		return ;
	if (bind(socketFD, (struct sockaddr *)it->second->getHostSRC(),
		sizeof(it->second->getHost())) < 0)
	{
		perror("bind() failed");
		killer = 1;
		//exit(EXIT_FAILURE);
	}
}

void	ServerManager::listenOnSocket(int socketFD)			// Set the socket to listen for incoming connections
{
	if (killer)
		return ;
	if (listen(socketFD, SOMAXCONN) < 0)				// SOMAXCONN : maximum number of pending connections
	{								// that can be queued up before connections are refused.
		perror("listen() failed");				// Is usually set to 1024 by default but can be altered.
		killer = 1;
		//exit(EXIT_FAILURE);
	}
}

void	ServerManager::configureExtraHost(size_t x, int i, std::map<size_t, Server*>::iterator it)
{
	if (killer)
		return ;
	struct sockaddr_in	host;

	int	backup(i);
	int	rest(0);
	int	n = x - 1;
	while (n > 0)
	{
		rest += _servers[n-1]->second->_port.size();
		n--;
	}
	i -= rest;

	host.sin_family = AF_INET;					// Set the address protocol to IPv4
	host.sin_port = htons(it->second->_port[i]);			// Set the port number for the socket

	host.sin_addr.s_addr = INADDR_ANY;
	i = backup;
	_hosts[i] = host;
}

void	ServerManager::bindExtraSocket(int socket, int i)
{
	if (killer)
		return ;
	if (bind(socket, (struct sockaddr *)&_hosts[i],
		sizeof(_hosts[i])) < 0)
	{
		perror("bind() failed");
		killer = 1;
		//exit(EXIT_FAILURE);
	}
}

int	ServerManager::initServer()
{
	std::map<size_t, Server*>::iterator it;

	if (_servers.size() == 0)
	{
		std::cerr << "\x1B[31mServer setup error; quitting program.\x1B[0m" << std::endl;
		return 1;
	}
	it = _servers[0];

	it->second->setListenFD(createSocket());
	configureSocketOptions(it->second->getListenFD());
	setSocketNonBlocking(it->second->getListenFD());
	configureHost(it);
	bindSocket(it->second->getListenFD(), it);
	listenOnSocket(it->second->getListenFD());

	struct pollfd		_listenFD_struct;
	_listenFD_struct.fd = it->second->getListenFD();
	_listenFD_struct.events = POLLIN;
	_listenFD_struct.revents = 0;

	/////////////////////////////////////////////////////////////////////

	std::vector<Client>		empty_c;
	std::vector<struct pollfd>	empty_s;

	cl_map.insert(std::pair<size_t, std::vector<Client> >(it->second->getListenFD(), empty_c));
	fd_map.insert(std::pair<size_t, std::vector<struct pollfd> >(it->second->getListenFD(), empty_s));

	fd_map[it->second->getListenFD()].push_back(_listenFD_struct);

	/////////////////////////////////////////////////////////////////////

	_sockets.push_back(it->second->getListenFD());			// Adding main-port to vector container.
	_hosts.push_back(it->second->getHost());

	std::vector<struct sockaddr_in>	vstr;
	std::vector<int>		vin;

	_hlist.insert(std::pair<size_t, std::vector<struct sockaddr_in> >(1, vstr));
	_slist.insert(std::pair<size_t, std::vector<int> >(1, vin));

	_hlist[1].push_back(it->second->getHost());
	_slist[1].push_back(it->second->getListenFD());

	/////////////////////////////////////////////////////////////////////

	size_t			y(0);
	size_t			x(1);
	int 			n(0);
	size_t			g(1);
	struct sockaddr_in	host;

	while (y < _servers.size() && !killer)
	{
		n += it->second->_port.size();					// Number of ports supported by server.
		for (int i = g; i < n && !killer; i++)				// Setting up alternate ports/sockets.
		{
			_hosts.push_back(host);
			_sockets.push_back(createSocket());
			configureSocketOptions(_sockets[i]);
			setSocketNonBlocking(_sockets[i]);
			configureExtraHost(x, i, it);
			bindExtraSocket(_sockets[i], i);
			listenOnSocket(_sockets[i]);

			_listenFD_struct.fd = _sockets[i];
			_listenFD_struct.events = POLLIN;
			_listenFD_struct.revents = 0;

			cl_map.insert(std::pair<size_t, std::vector<Client> >(_sockets[i], empty_c));
			fd_map.insert(std::pair<size_t, std::vector<struct pollfd> >(_sockets[i], empty_s));
			fd_map[_sockets[i]].push_back(_listenFD_struct);

			_hlist[x].push_back(_hosts.back());
			_slist[x].push_back(_sockets.back());
			g++;
		}
		it++;
		y++;
		if (y < _servers.size() && !killer)
		{
			x++;
			_hlist.insert(std::pair<size_t, std::vector<struct sockaddr_in> >(x, vstr));
			_slist.insert(std::pair<size_t, std::vector<int> >(x, vin));
		}
	}
	this->displayTime();
	struct sockaddr_in	ip_addr;
	char			str[INET_ADDRSTRLEN];
	ip_addr = _servers[0]->second->getHost();
	inet_ntop(AF_INET, &(ip_addr.sin_addr), str, INET_ADDRSTRLEN);

	if (killer)
		return 1;
	std::cout << "Server initialized   (IP: \x1b[32m" << str << "\x1B[0m)" << std::endl;
	
	/////////////////////////////////////////////////////////////////////

	it = _servers[0];
	y = 0;

	size_t	j;
	size_t	k = 1;
	while (y < _servers.size())
	{
		std::cout << "     Server [" << k << "] ports:   \x1b[32m";
		j = 0;
		while (j < it->second->_port.size())
		{
			std::cout << it->second->_port[j] << " ";
			j++;
		}
		std::cout << "\x1B[0m" << std::endl;
		k++;
		it++;
		y++;
	}
	std::cout << "\x1B[95m" << "           [ATTENTION]   Clients will be displayed as sockets!";
	std::cout << "\x1B[0m" << std::endl;
	std::cout << std::endl;
	std::signal(SIGINT, disableServer);				// Catching shutdown signal(s).
	return 0;
}

/* 	  In this code:

	- The `startIOmultiplexing()` method first sets up a `pollfd` structure 
	  for the server socket and adds it to a vector.

	- It then enters an infinite loop where it calls `poll()` to monitor 
	  for events on the server socket and any connected client sockets.

	- When poll() indicates a read event (`POLLIN`) on the server socket, 
	  it means a new connection request is incoming. The server accepts this 
	  request and adds the new socket to the list of FDs being monitored.

	- If a read event is detected on a client socket, the server reads data from the socket. 
	  If the read returns `-1` and the error is not `EAGAIN`, it indicates a read error. 
	  If the read returns `0`, it means the client has closed the connection. 
	  Otherwise, the server processes the received data.
*/

void	ServerManager::handleNewConnection(std::vector<struct pollfd> &fds, struct pollfd &_listenFD_struct)
{
	if (killer)
		return ;
	struct sockaddr_in	client_addr;
	socklen_t		client_addr_size = sizeof(client_addr);

	if (cl_map[_s].size() >= SOMAXCONN)
	{
		this->displayTime();
		std::cout << "\x1B[31m" << "Number of maximum clients (" << SOMAXCONN
			<< ") has been reached! Connection refused." << "\x1B[0m" << std::endl;
		return ;
	}
	int new_socket = accept(_listenFD_struct.fd,
		(struct sockaddr *)&client_addr, &client_addr_size);

	if (new_socket < 0)
	{
		perror("accept() error");
		return ;
	}
	fcntl(new_socket, F_SETFL, O_NONBLOCK);			// Set to non-blocking mode.

	struct pollfd	new_fd_struct;				// This structure is used to manage
	new_fd_struct.fd = new_socket;				// the new client socket.
	new_fd_struct.events = POLLIN;
	new_fd_struct.revents = 0;

	if (!ALLOW_DUPES)					// Use this to limit duplicate ip-logins.
		if (compareAddress(client_addr) == false)
			return ;
	for (size_t i = 0; i < fds.size(); ++i)			// Is the socket already taken? That's BAD.
		if (new_fd_struct.fd == fds[i].fd)
			return ;

	fds.push_back(new_fd_struct);
	int x = this->setupClient(new_fd_struct.fd, client_addr);
	if (x < 0)
	{
		displayTime();
		std::cout << "\x1B[31mCritical client error! Shutting down server.\x1B[0m" << std::endl;
		killer = 1;
		return ;
	}
	this->displayTime();
	std::cout << "\x1b[32m" << "Client " << new_fd_struct.fd << " has connected (IP: ";
	std::cout << cl_map[_s].back().getIP() << ":";

	int	rest(0);
	int	n = x;
	while (n > 0)
	{
		rest += _servers[n-1]->second->_port.size();
		n--;
	}
	std::cout << _servers[x]->second->_port[_i - rest] << ")\x1B[0m" << std::endl;
	cl_map[_s].back().updateTime();
}

void	ServerManager::closingClientPipes(size_t i)			// Making sure these are properly closed, better safe than sorry.
{
	if (cl_map[_s][i]._response._cgiHandler._pipeIN[1] != -1)
		close(cl_map[_s][i]._response._cgiHandler._pipeIN[1]);
	if (cl_map[_s][i]._response._cgiHandler._pipeOUT[1] != -1)
        	close(cl_map[_s][i]._response._cgiHandler._pipeOUT[1]);

	cl_map[_s][i]._response._cgiHandler._pipeIN[1] = -1;
	cl_map[_s][i]._response._cgiHandler._pipeOUT[1] = -1;
}

bool	ServerManager::compareAddress(sockaddr_in new_client_addr)	// Prevents multiple access attempts from the same source.
{
	char		str[INET_ADDRSTRLEN];
	std::string	ip;

	inet_ntop(AF_INET, &(new_client_addr.sin_addr), str, INET_ADDRSTRLEN);
	ip = (std::string)str;

	for (size_t i = 0; i < cl_map[_s].size(); i++)
		if (ip == cl_map[_s][i].getIP())
			return false;
	return true;
}

int	ServerManager::setupClient(int socket, struct sockaddr_in client_addr)
{	
	if (killer)
		return -1;
	size_t k = 0;
	size_t x = 0;
	int y = -1;

	std::map<size_t, Server*>::iterator	it;
	while (k < _servers.size())
	{
		x = 0;
		while (x < _slist[k+1].size())
		{
			if (_sockets[_i] == _slist[k+1][x])
			{
				it = _servers[k];
				y = k;
				break ;
			}
			x++;
		}
		k++;
	}
	if ((size_t)y >= _servers.size())
		return -1;
	Client client(it);

	client.setSocket(socket);
	client.setAddress(client_addr);

	this->cl_map[_s].push_back(client);
	return y;
}

/*
bool	ServerManager::spamFilter(size_t n)					// scrapped concept for a message-filter to reduce flooding on the terminal
{
	static time_t	last_msg(0);
	static size_t	last_user(0);

	if (n != last_user)
	{
		last_user = n;
		return false;
	}

	if (cl_map[_s][n].getTime() - last_msg < 2)
	{
		last_msg = time(NULL);
		return true;
	}
	last_msg = time(NULL);
	return false;
}
*/

void	ServerManager::checkTimeout(std::vector<struct pollfd> &fds)
{
	if (killer)
		return ;
	std::vector<Client>::iterator	s(cl_map[_s].begin());
	std::vector<Client>::iterator	e(cl_map[_s].end());

	size_t 	n(1);

	while (s != e)
	{
		if (time(NULL) - s->getTime() >= CONNECTION_TIMEOUT)
		{
			this->displayTime();
			std::cout << "\x1B[91m" << "Client " << fds[n].fd << " has timed out" << "\x1B[0m" << std::endl;
			close(s->_response._cgiHandler._pipeIN[1]);
       			close(s->_response._cgiHandler._pipeOUT[1]);
			close(fds[n].fd);
			fds.erase(fds.begin() + n);
			cl_map[_s].erase(cl_map[_s].begin() + n - 1);
		}
		s++;
		n++;
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////// /!\ CGI /!\ vvvvvv

void	ServerManager::sendCGImessage(Client &client, CGIhandler &cgi, long unsigned BUFFER)
{
	if (killer)
		return ;
	int		bytes;
	std::string	&reqBody = client._request.getBody();

	if (reqBody.length() == 0)
		bytes = 0;
	else if (reqBody.length() >= BUFFER)							// arbitrary write buffer
		bytes = write(cgi._pipeIN[1], reqBody.c_str(), BUFFER);
	else
		bytes = write(cgi._pipeIN[1], reqBody.c_str(), reqBody.length());

	if (bytes < 0)
	{
		perror("CGI write() error");
		close(cgi._pipeIN[1]);
		close(cgi._pipeOUT[1]);
		cgi._pipeIN[1] = -1;
		cgi._pipeOUT[1] = -1;
		client._response.setErrorResponse(500);
	}
	else if (bytes == 0 || (size_t)bytes == reqBody.length())
	{
		client.updateTime();
		reqBody = reqBody.substr(bytes);
		close(cgi._pipeIN[1]);
		close(cgi._pipeOUT[1]);
		cgi._pipeIN[1] = -1;
		cgi._pipeOUT[1] = -1;
	}
	else
	{
		client.updateTime();
		if (reqBody.size() < (size_t)bytes)
			bytes = reqBody.size();
		reqBody = reqBody.substr(bytes);
	}
}

void	ServerManager::readCGIresponse(Client &client, CGIhandler &cgi, long unsigned BUFFER)
{
	if (killer)
		return ;
	char	buffer[BUFFER];
	int	bytes_read = 0;

	bytes_read = read(cgi._pipeOUT[0], buffer, BUFFER);					// arbitrary write buffer

	if (bytes_read == 0)
	{
		close(cgi._pipeIN[0]);
		close(cgi._pipeOUT[0]);
		int	status;
		waitpid(cgi.getCGIpid(), &status, 0);
		if(WEXITSTATUS(status) != 0)
			client._response.setErrorResponse(502);
		client._response.setCGIstate(2);
		if (client._response._responseContent.find("HTTP/1.1") == std::string::npos)
			client._response._responseContent.insert(0, "HTTP/1.1 200 OK\r\n");
		return ;
	}
	else if (bytes_read < 0)
	{
		perror("CGI read() error");
		close(cgi._pipeIN[0]);
		close(cgi._pipeOUT[0]);
		cgi._pipeIN[0] = -1;
		cgi._pipeOUT[0] = -1;

		client._response.setCGIstate(2);
		client._response.setErrorResponse(500);
		return ;
	}
	else
	{
		client.updateTime();
		client._response._responseContent.append(buffer, bytes_read);
		memset(buffer, 0, sizeof(buffer));
		//client._response.setCGIstate(0);		// BEWARE!
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////// /!\ CGI /!\ ^^^^^^

/*
static bool	filterDupes(std::string buffer, int socket)		//blocking identical requests from same client(s)
{
	static int		n(0);
	static std::string	checker("");

	if (n == socket)
	{
		if (checker == buffer)
			return true;
		else
			checker = buffer;
	}
	else
	{
		checker = buffer;
		n = socket;
	}
	return false;
}
*/

static void	handleRequestBody(Client &c)
{
	if (killer)
		return ;
	if (c._request.getBody().length() == 0)
	{
		std::string 	buffer;
		std::fstream 	file(c._response._cgiHandler.getCGIpath().c_str());

		std::stringstream ss;
		ss << file.rdbuf();
		buffer = ss.str();

		c._request.setBody(buffer);
	}
}

void	ServerManager::handleClientRead(std::vector<struct pollfd> &fds, size_t &idx)
{
	if (killer)
		return ;

	long unsigned	BUFFER = cl_map[_s][idx-_o]._server->second->getClientMax() * 1024;
	char 		buffer[BUFFER];
	int		count = -1;

	if (idx <= 0)							// ID 0 is the server itself!
		return ;

	count = recv(fds[idx].fd, buffer, sizeof(buffer) - 1, 0);
	if (count == -1)
	{
		this->displayTime();
		std::cerr << "\x1B[31m" << "Recv() operation failed on socket " << fds[idx].fd << "\x1B[0m" << std::endl;
		close(fds[idx].fd);
		return ;
	}
	else if ((count == 0 || cl_map[_s][idx-_o]._request.keepAlive() == false)
		&& cl_map[_s][idx-_o]._response.getCGIstate() == 0)
	{
		this->displayTime();
		std::cout << "\x1B[91m" << "Client " << fds[idx].fd << " has disconnected" << "\x1B[0m" << std::endl;

		closingClientPipes(idx - 1);
		close(fds[idx].fd);
		fds.erase(fds.begin() + idx);
		cl_map[_s].erase(cl_map[_s].begin() + idx - 1);									// clearing container
		--idx;
		return ;
	}
	else
	{
		buffer[count] = '\0';
		// std::cout << "[" << buffer << "]" << std::endl;							// DEBUGGING | DEBUGGING | DEBUGGING
		std::string	index = '/' + cl_map[_s][idx-_o]._server->second->getIndex();

		cl_map[_s][idx-_o]._request.feed(buffer, count, index);
		memset(buffer, 0, sizeof(buffer));										// clearing out buffer
	}
	if (cl_map[_s][idx-_o]._request.parsingComplete() || cl_map[_s][idx-_o]._request.getErrorCode()
		|| (TELNET && cl_map[_s][idx-_o]._request.getPath().empty() == false
		&& cl_map[_s][idx-_o]._request.getMethod() == DELETE))
	{
		std::string	method =  cl_map[_s][idx-_o]._request.getMethodStr().c_str();
		if (method.empty() == true)
			method = "\x1B[91mN/A\x1B[33m";
		std::string	path = cl_map[_s][idx-_o]._request.getPath();
		if (path.empty() == true)
			path = "\x1B[91mN/A\x1B[33m";

		std::cout << "   \x1B[33mRequest\x1B[0m received from Client " << fds[idx].fd << "     ";
		std::cout << "Method [\x1B[33m" << method << "\x1B[0m] "<< "URI [\x1B[33m";
		std::cout << path << "\x1B[0m]" << std::endl;

		cl_map[_s][idx-_o].createResponse();
		if (cl_map[_s][idx-_o]._response.getCGIstate())	
			handleRequestBody(cl_map[_s][idx-_o]);
		cl_map[_s][idx-_o].updateTime();
	}
}

void	ServerManager::sendResponse(std::vector<struct pollfd> &fds, size_t &i)
{
	if (killer)
		return ;
	short		kickme(0);									// Dealing with DELETE requests since
	int		bytes_sent(0);									// they trigger CGI and require disconnects.
	long unsigned	BUFFER = cl_map[_s][i-_o]._server->second->getClientMax() * 1024;
	std::string	response = cl_map[_s][i-_o]._response.getResponse();

	if (response.length() >= BUFFER)								// arbitrary write buffer
		bytes_sent = write(cl_map[_s][i-_o].getSocket(), response.c_str(), BUFFER);
	else
		bytes_sent = write(cl_map[_s][i-_o].getSocket(), response.c_str(), response.length());

	if (bytes_sent || cl_map[_s][i-_o]._response.getResponse().empty() == false || cl_map[_s][i-_o]._response.getCGIstate() == 2)		// CRITICAL!
	{
		std::cout << "        \x1B[33mResponse\x1B[0m sent to Client " << cl_map[_s][i-_o].getSocket();
		if (cl_map[_s][i-_o]._response._lastCode > 0)
			std::cout << "       Code [\x1B[90m" << cl_map[_s][i-_o]._response._lastCode << "\x1B[0m] ";
		else
			std::cout << "       Code [\x1B[90m" << cl_map[_s][i-_o]._response.getCode() << "\x1B[0m] ";
		if (cl_map[_s][i-_o]._response._lastCode > 0)
			std::cout << "\x1B[90m" << statusCodeString(cl_map[_s][i-_o]._response._lastCode) << "\x1B[0m" << std::endl;
		else
			std::cout << "\x1B[90m" << statusCodeString(cl_map[_s][i-_o]._response.getCode()) << "\x1B[0m" << std::endl;
		cl_map[_s][i-_o]._response._lastCode = 0;
	}

	if (cl_map[_s][i-_o]._request.getMethod() == DELETE)
		kickme = 1;															// CRITICAL!

		//if (cl_map[_s][i-_o]._response.getResponse().empty() == false)						// debugging
		//	std::cout << cl_map[_s][i-_o]._response.getResponse() << std::endl;					// debugging

	if (bytes_sent == 0 || (size_t)bytes_sent == response.length() || kickme > 0 || bytes_sent == -1)
	{
		if (cl_map[_s][i-_o]._request.keepAlive() == false || cl_map[_s][i-_o]._request.getErrorCode()
			|| cl_map[_s][i-_o]._response.getCGIstate() || kickme > 0)								// CRITICAL!
		{
			this->displayTime();
			std::cout << "\x1B[91m" << "Client " << cl_map[_s][i-_o].getSocket() << " has disconnected" << "\x1B[0m" << std::endl;
			close(fds[i].fd);
			fds.erase(fds.begin() + i);
			closingClientPipes(i - _o);
			cl_map[_s].erase(cl_map[_s].begin() + i - _o);
			--i;
			return ;
		}
		else
		{
			cl_map[_s][i-_o].clearClient();
			return ;
		}
	}
	cl_map[_s][i-_o]._response.cutResponse(bytes_sent);
	cl_map[_s][i-_o].updateTime();
}

/*
        	&fds[0]: This provides a pointer to the underlying array of pollfd structures in the fds vector. 
        	It represents the file descriptors that poll will monitor.
        	fds.size(): This is the number of file descriptors that poll will monitor.
		
		Assuming 'fds' is a vector of pollfd structures
		std::vector<struct pollfd>::iterator it = std::remove_if(fds.begin(), fds.end(), shouldRemove);
		fds.erase(it, fds.end());

		fds[i].revents: This is the field in the pollfd structure that poll() sets to indicate what event occurred.
            	POLLIN: This is a flag that indicates there is data to read (i.e., a read event). 
            	It could mean a new connection request on a listening socket or available data on a connected socket.
*/

void	ServerManager::checkClosedPorts()					// checker/diagnosis tool for campus tests in case of port problems
{
/*	size_t x(1);
	size_t y(0);
	while (x < _slist.size() + 1)
	{
		while (y < _slist[x].size())
		{
			std::cout << _slist[x][y] << " ";
			y++;
		}
		y = 0;
		x++;
	}
	std::cout << std::endl;				*/

	//if (fd_map[_s][_i].events & POLLHUP)
	//if (fd_map[_s][_i].events & POLLIN)
	if (fd_map[_s][_i].events & POLLIN)
	{
		size_t k = 0;
		size_t x = 0;
		int 	y = -1;
		int	rest(0);

		while (k < _servers.size())
		{
			x = 0;
			while (x < _slist[k+1].size())
			{
				if (_sockets[_i] == _slist[k+1][x])
				{
					y = k;
					break ;
				}
				x++;
			}
			k++;
		}
		if (y < 0)
			return ;
		x = y;
		while (y > 0)
		{
			rest += _servers[y-1]->second->_port.size();
			y--;
		}
		std::cout << "[" << _servers[x]->second->_port[_i - rest] << "]" << std::endl;
	}
/*
	size_t a(0);
	size_t b(0);

	while (a < _servers.size())
	{
		while (b < _servers[a]->second->_port.size())
		{
			std::cout << "[" << _servers[a]->second->_port[b] << "]" << " ";
			b++;
		}
		a++;
		b = 0;
	}
	std::cout << std::endl;			*/
}

void	ServerManager::startIOmultiplexing()
{
	int	cgi_state;
	int	ret;

//	THIS IS NOW STORED INSIDE A MAP ON THE HEADERFILE
//	std::vector<struct pollfd> fds;			// Each pollfd represents a file descriptor (socket) 
							// along with its events and revents (returned events).

		// This structure is prepared for the server's main socket (referred to as _listenFD).
		// It's set to monitor for POLLIN events, indicating incoming connections or data.

				// The FIRST element in this container is the SERVER-SOCKET itself!
				// Consider the OFFSET for the Client container, add _o or -1 each time!			

	std::vector<int>::iterator	s = _sockets.begin();
	std::vector<int>::iterator	se = _sockets.end();
	_i = 0;
/*
	while (s != se)
	{
		checkClosedPorts();
		_i++;
		s++;
	}
*/
	while (true && !killer)
	{
		if (s == se)
		{
			s = _sockets.begin();
			_i = 0;
		}
		_s = *s;

		ret = poll(&fd_map[_s][0], fd_map[_s].size(), 5); 				// Set the third argument to -1 for indefinite waiting
		if (ret < 0 && !killer)
		{
			perror("poll() error");
			// exit(EXIT_FAILURE);
			killer = 1;
			break ;
		}

		for (size_t i = 0; i < fd_map[_s].size() && !killer; i++)				// ++i as the original value increment
		{
			if (fd_map[_s][i].revents & POLLIN)
			{
				if ((size_t)fd_map[_s][i].fd == _s)
					handleNewConnection(fd_map[_s], fd_map[_s][0]);
				else
					handleClientRead(fd_map[_s], i);
			}
			else if (fd_map[_s][i].revents & (POLLHUP | POLLERR | POLLNVAL)) 		// Handle POLLHUP and socket errors
			{
				displayTime();
				std::cerr << "\x1B[31m" << "Error or hangup on Client ";
				std::cerr << fd_map[_s][i].fd << ", disconnecting...\x1B[0m" << std::endl;
				close(fd_map[_s][i].fd);
				fd_map[_s].erase(fd_map[_s].begin() + i);
				closingClientPipes(i - _o);
				cl_map[_s].erase(cl_map[_s].begin() + 1 - _o);					// Removing from container
				--i; 										// Adjust the loop counter
			}
			else if (fd_map[_s][i].revents & POLLOUT)					// Data may be written without blocking
			{
				this->displayTime();
				std::cout << "\x1B[91m" << "Client " << cl_map[_s][i-_o].getSocket();
				std::cout << " has disconnected" << "\x1B[0m" << std::endl;
				close(fd_map[_s][i].fd);
				fd_map[_s].erase(fd_map[_s].begin() + i);
				closingClientPipes(i - _o);
				cl_map[_s].erase(cl_map[_s].begin() + 1 - _o);
				--i;										// Adjust the loop counter
			}
			else if (i > _o - 1)
			{
				cgi_state = cl_map[_s][i-_o]._response.getCGIstate();
				if (cgi_state == 1)
				{
					if (cl_map[_s][i-_o]._response._cgiHandler._pipeIN[1] > 0)
					{
						this->sendCGImessage(cl_map[_s][i-_o],
							cl_map[_s][i-_o]._response._cgiHandler,
							cl_map[_s][i-_o]._server->second->getClientMax() * 1024);
					}
					else if (cl_map[_s][i-_o]._response._cgiHandler._pipeOUT[0] > 0)
					{
						this->readCGIresponse(cl_map[_s][i-_o],
							cl_map[_s][i-_o]._response._cgiHandler,
							cl_map[_s][i-_o]._server->second->getClientMax() * 1024);
					}
				}
				else if (cgi_state == 0 || cgi_state == 2)
					sendResponse(fd_map[_s], i);
			}
		}
		checkTimeout(fd_map[_s]);
		s++;
		_i++;										// Used for indexing the stored server ports
	}
}

/////////////////////////////////////////////////////////////////////////////// GET TIME

void	ServerManager::displayTime()
{
	char		buffer[100];

	tzset();
	time_t		now = time(0);
	struct tm	tm = *gmtime(&now);

	tm.tm_hour = tm.tm_hour + 1;
	strftime(buffer, 100, "[%Y-%m-%d  %H:%M:%S]", &tm);

	std::cout << "\x1B[94m" << buffer << "\x1B[0m" << "   ";
}

///////////////////////////////////////////////////////////////////////////////	(DE)CON

ServerManager::~ServerManager()
{
	return ;
}

ServerManager::ServerManager(std::map<size_t, Server*>::iterator a, std::map<size_t, Server*>::iterator b)
{
	while (a != b)
	{
		_servers.push_back(a);
		a++;
	}

	_i = 0;
	_o = 1;
	_s = 3;		//on default the first socket/FD to be used by a server is "3" but this will be adjusted anyway
}
