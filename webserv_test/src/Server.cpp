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