/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CGIhandler.hpp          	                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: agrun <agrun@student.42berlin.de>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/01/14 11:08:49 by agrun             #+#    #+#             */
/*   Updated: 2023/01/14 12:10:52 by agrun            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CGIHANDLER_HPP
# define CGIHANDLER_HPP

# include <iostream>
# include <algorithm>
# include <ctime>
# include <cstdlib>
# include <sstream>
# include <fstream>
# include <cstring>
# include <string>
# include <map>

# include <sys/socket.h>
# include <netinet/in.h>
# include <sys/select.h>
# include <arpa/inet.h>

# include <unistd.h>
# include <stdlib.h>
# include <stdarg.h>
# include <fcntl.h>
# include <sys/time.h>
# include <sys/stat.h>

# include "Request.hpp"
# include "Location.hpp"

class Request;
class Location;

class CGIhandler
{
	private:
		std::map<std::string, std::string>	_env;
		std::string				_cgiPath;
		int					_exit;
		char					**_argv;	//REQUIRED FOR EXECVE
		char					**_chENV;	//REQUIRED FOR EXECVE
		pid_t					_cgiPID;

	public:
		int					_pipeIN[2];	//REQUIRED FOR PIPING
		int					_pipeOUT[2];	//REQUIRED FOR PIPING

		CGIhandler();
		CGIhandler(std::string path);
		CGIhandler(const CGIhandler &copy);
		CGIhandler	&operator=(const CGIhandler &copy);
		~CGIhandler();

		void 		initEnv(Request &request, std::vector<Location>::iterator loc);
		void 		initEnvCgi(Request &request, std::vector<Location>::iterator loc);
		void 		execute(short &errorcode);
		void 		clear();

		void						setCGIpid(pid_t pid);
		void						setCGIpath(const std::string &path);
		const std::map<std::string, std::string>	&getENV() const;
		const pid_t					&getCGIpid() const;
		const std::string				&getCGIpath() const;

		std::string	getPathInfo(std::string &path, std::string extension);		//REWORKED HEADER
		int		findStart(const std::string path, const std::string delimiter);
		std::string	decode(std::string &path);
};

#endif
