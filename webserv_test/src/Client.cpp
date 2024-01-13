/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp	       		                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: agrun <agrun@student.42berlin.de>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/31 11:23:01 by agrun             #+#    #+#             */
/*   Updated: 2023/12/31 12:23:32 by agrun            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"

///////////////////////////////////////////////////////////////////////////////	SETTERS

void	Client::setSocket(int socket)
{
	this->_clientSocket = socket;
}

void	Client::setAddress(sockaddr_in &addr)
{
	this->_clientAddress = addr;
}

void	Client::setServer(std::map<size_t, Server*>::iterator server)
{
	this->_server = server;
	this->_response.setServer(server);
}

void	Client::createResponse()
{
	this->_response.setRequest(this->_request);
	this->_response.createResponse();
}

void	Client::updateTime()
{
	this->_lastMessage = time(NULL);
}

///////////////////////////////////////////////////////////////////////////////	GETTERS

const int	&Client::getSocket() const
{
	return this->_clientSocket;
}

const struct sockaddr_in	&Client::getAddress() const
{
	return this->_clientAddress;
}

const Request	&Client::getRequest() const
{
	return this->_request;
}

const time_t	&Client::getTime() const
{
	return this->_lastMessage;
}

///////////////////////////////////////////////////////////////////////////////	(DE)CON

Client::~Client()
{
	this->_response.clear();
	this->_request.clear();
}

Client::Client(std::map<size_t, Server*>::iterator server)
{
	this->setServer(server);

	size_t clients = (size_t)server->second->getClientMax();
	this->_request.setMaxBodySize(clients);

	this->setSocket(-1);							//placeholder
	inet_pton(AF_INET, "0.0.0.0", &(this->_clientAddress.sin_addr));	//placeholder

	this->_lastMessage = time(NULL);
}