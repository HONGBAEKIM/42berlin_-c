/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerManager.hpp          	                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: agrun <agrun@student.42berlin.de>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/01/24 14:08:49 by agrun             #+#    #+#             */
/*   Updated: 2023/01/24 14:10:52 by agrun            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVERMANAGER_HPP
# define SERVERMANAGER_HPP

# include <iostream>
# include <cstdlib>
# include <csignal>
# include <sstream>
# include <fstream>
# include <cstring>
# include <string>
# include <map>
# include <vector>

# include <sys/wait.h>
# include <sys/socket.h>
# include <netinet/in.h>
# include <sys/select.h>
# include <arpa/inet.h>

# include <fcntl.h>
# include <unistd.h>
# include <poll.h>

# include "Server.hpp"
# include "Client.hpp"

class Server;
class Client;

# define CONNECTION_TIMEOUT 60

# define ALLOW_DUPES 0							// Change this value to 1 if you want to enable duplicate logins
									// from the same source simultaneously. Rather unstable.

# define TELNET 1							// Change this value to 1 to be more forgiving with incoming request
									// formats, for instance from Telnet. Optimal for testing!

# define LOCK_PORT 1							// Change this value to 1 if you want to deny ports from being reused.

class ServerManager
{
	private:
		size_t					_i;		// indexing attribute used for multiplexing
		size_t					_o;		// offset for Client indexing at serverloop
		size_t					_s;		// socket to be served currently

		ServerManager();

	public:
		ServerManager(std::map<size_t, Server*>::iterator a,
			std::map<size_t, Server*>::iterator b);
		~ServerManager();

		std::vector< std::map<size_t, Server*>::iterator >	_servers;		// access to the serverconfigs

		std::map<size_t, std::vector<struct pollfd> >		fd_map;
		std::map<size_t, std::vector<Client> >			cl_map;

		std::vector<struct sockaddr_in>				_hosts;			// for storing ports
		std::vector<int>					_sockets;		// for storing sockets

		std::map<size_t, std::vector<struct sockaddr_in> >	_hlist;			// helps with keeping track of servers
		std::map<size_t, std::vector<int> >			_slist;			// helps with keeping track of servers

		void				configureExtraHost(size_t x, int i,
			std::map<size_t, Server*>::iterator it);				// support for extra ports/sockets
		void				bindExtraSocket(int socket, int i);		// support for extra ports/sockets

		int				setupClient(int socket, struct sockaddr_in client_addr);

		int	createSocket();
		void	configureSocketOptions(int socketFD);
		void	setSocketNonBlocking(int socketFD);
		void	checkClosedPorts();

		int	initServer();
		void	startIOmultiplexing();
		void	displayTime();

		void	configureHost(std::map<size_t, Server*>::iterator it);
		void	bindSocket(int socketFD, std::map<size_t, Server*>::iterator it);
		void	listenOnSocket(int socketFD);

		void	checkTimeout(std::vector<struct pollfd> &fds);
		bool	compareAddress(sockaddr_in new_client_addr);
		void	closingClientPipes(size_t i);

		void	handleNewConnection(std::vector<struct pollfd> &fds, struct pollfd &_listenFD_struct);
		void	handleClientRead(std::vector<struct pollfd> &fds, size_t &idx);
		void	sendResponse(std::vector<struct pollfd> &fds, size_t &i);

		void	sendCGImessage(Client &client, CGIhandler &cgi, long unsigned BUFFER);
		void	readCGIresponse(Client &client, CGIhandler &cgi, long unsigned BUFFER);
};

#endif
