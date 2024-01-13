/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp          	                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: agrun <agrun@student.42berlin.de>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/31 11:08:49 by agrun             #+#    #+#             */
/*   Updated: 2023/12/31 12:10:52 by agrun            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
# define CLIENT_HPP

# include <iostream>
# include <cstdlib>
# include <sstream>
# include <fstream>
# include <cstring>
# include <string>
# include <map>
# include <vector>

# include <sys/socket.h>
# include <netinet/in.h>
# include <sys/select.h>
# include <arpa/inet.h>

# include "Server.hpp"
# include "Request.hpp"
# include "Response.hpp"

class Server;
class Request;
class Response;

class Client
{
	private:
		int				_clientSocket;
		struct sockaddr_in		_clientAddress;
		time_t				_lastMessage;

		Client();					//no need
		Client(const Client &copy);			//no need
		Client	&operator=(const Client &copy);		//no need

	public:
		Client(std::map<size_t, Server*>::iterator server);
		~Client();

		std::map<size_t, Server*>::iterator	_server;
		Request					_request;
		Response				_response;

		void				setSocket(int socket);
		void				setAddress(sockaddr_in &addr);
		void				setServer(std::map<size_t, Server*>::iterator server);
		void				createResponse();
		void				updateTime();

		const int			&getSocket() const;
		const struct sockaddr_in	&getAddress() const;
		const Request			&getRequest() const;
		const time_t			&getTime() const;
};

#endif
