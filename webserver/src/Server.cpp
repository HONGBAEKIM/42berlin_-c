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

int	checkPathType(std::string const path)
{
	struct stat	buffer;
	int		result;
	
	result = stat(path.c_str(), &buffer);
	if (!result)
	{
		if (buffer.st_mode & S_IFREG)		// regular file
			return (1);
		else if (buffer.st_mode & S_IFDIR)	// directory
			return (2);
		else
			return (3);
	}
	return 0;
}

void	Server::embedLocation()
{
	Location n;
	n.setClientMax(this->getClientMax());		// In case Client Max doesn't get
	this->_locations.push_back(n);			// defined by the Location itself.
}

///////////////////////////////////////////////////////////////////////////////	SETTERS

void	Server::setPort(int p)
{
	std::vector<size_t>::iterator it = _port.begin();
	std::vector<size_t>::iterator ite = _port.end();

	if (p < 1 || p > 9999)
		return ;
	while (it != ite)				// Making sure no dupes get added.
	{
		if (*it == (size_t)p)
			return ;
		it++;
	}
	this->_port.push_back(p);
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
	if (checkPathType(root) == 2)			// root is a directory
	{
		this->_root = root;
		return ;
	}
	char	dir[1024];
	getcwd(dir, 1024);
	std::string full_root = dir + root;

	this->_root = full_root;
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

std::vector<size_t>::iterator	Server::getPort()
{
	return this->_port.begin();
}

std::vector<size_t>::iterator	Server::getPortEnd()
{
	return this->_port.end();
}

struct sockaddr_in	Server::getHost() const
{
	return this->_host;
}

struct sockaddr_in	*Server::getHostSRC()
{
	return &this->_host;
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
	//inet_pton(AF_INET, "0.0.0.0", &(this->_serverIP.sin_addr));	//use [...].sin_addr.s_addr
	//inet_pton(AF_INET, "0.0.0.0", &(this->_host.sin_addr));	//to access the actual ip

	inet_pton(AF_INET, "127.0.0.1", &(this->_serverIP.sin_addr));
	inet_pton(AF_INET, "127.0.0.1", &(this->_host.sin_addr));

	this->_servername = "";
	this->_root = "";
	this->_clientMAX = 0;
	this->_index = "";
	this->_autoindex = false;					//FALSE might cause error 403
	return ;
}
