/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp          	                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: agrun <agrun@student.42berlin.de>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/15 13:08:49 by agrun             #+#    #+#             */
/*   Updated: 2023/12/15 14:10:52 by agrun            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
# define SERVER_HPP

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

# include <unistd.h>
# include <sys/types.h>
# include <sys/stat.h>
# include <fcntl.h>

# include "Location.hpp"

class Location;

class Server
{
	private:
		struct sockaddr_in			_host;

		std::string				_servername;
		std::string				_root;
		unsigned long				_clientMAX;
		std::string				_index;
		bool					_autoindex;
		std::map<int, std::string>		_errorpages;
		std::vector<Location> 			_locations;

        	struct sockaddr_in	 		_serverIP;
		int					_listenFD;	//The actual server socket file descriptor

	public:
		Server();
		Server(const Server &copy);
		Server	&operator=(const Server &copy);
		~Server();

		std::vector<size_t>			_port;		//port number(s) for the server's listening socket(s)

		void	setPort(int p);
		void	setHost(struct sockaddr_in host);
		void	setServerName(std::string name);
		void	setRoot(std::string root);
		void	setClientMax(unsigned long c_max);
		void	setIndex(std::string index);
		void	setAutoIndex(bool a_index);
		void	setErrorPages(int n, std::string path);
		void	setLocations(std::string path);
		void	setServerIP(struct sockaddr_in ip);
		void	setListenFD(int fd);

		void					embedLocation();	//triggered by ConfigFile

		std::vector<size_t>::iterator		getPort();
		std::vector<size_t>::iterator		getPortEnd();
		//std::vector<int>::iterator		getListenFD();
		//std::vector<int>::iterator		getListenFDend();

		struct sockaddr_in			getHost() const;
		struct sockaddr_in			*getHostSRC();

		std::string				getServerName() const;
		std::string				getRoot() const;
		unsigned long				getClientMax() const;
		std::string				getIndex() const;
		bool					getAutoIndex() const;

		std::map<int, std::string>::iterator	getErrorPages();
		std::map<int, std::string>::iterator	getErrorPagesEnd();
		std::vector<Location>::iterator		getLocations();
		std::vector<Location>::iterator		getLocationsEnd();
		struct sockaddr_in			getServerIP() const;
		int					getListenFD() const;
};

		int					checkPathType(std::string const path);

#endif
