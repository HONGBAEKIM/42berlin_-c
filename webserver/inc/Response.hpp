/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.hpp          	                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: agrun <agrun@student.42berlin.de>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/03 10:08:49 by agrun             #+#    #+#             */
/*   Created: 2024/01/03 11:10:52 by agrun            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef RESPONSE_HPP
# define RESPONSE_HPP

# include <iostream>
# include <cstdlib>
# include <cctype>
# include <sstream>
# include <fstream>
# include <cstring>
# include <string>
# include <map>
# include <vector>

# include <fcntl.h>
# include <sys/types.h>
# include <sys/time.h>
# include <signal.h>
# include <dirent.h>		//for "DIR"
# include <sys/stat.h>		//for "struct stat"
# include <unistd.h>		//for "access"

# include <sys/socket.h>
# include <netinet/in.h>
# include <sys/select.h>
# include <arpa/inet.h>

# include "Server.hpp"
# include "Request.hpp"
# include "CGIhandler.hpp"

class Server;
class Request;
class CGIhandler;

class Response
{
	private:
		std::map<size_t, Server*>::iterator	_server;
		std::string				_targetFile;
		std::vector<uint8_t>			_body;
		size_t					_bodyLen;
		std::string				_responseBody;
		std::string				_location;
		short					_code;

		int					_cgi;
		int					_cgiFD[2];
		size_t					_cgiLen;
		bool					_autoIndex;
		std::map<std::string, std::string>	_fileTypes;

	public:
		Response();
		Response(Request &request);
		Response(const Response &copy);
		Response	&operator=(const Response &copy);
		~Response();

		short				_lastCode;		// for logging purposes
		CGIhandler			_cgiHandler;		// IMPORTANT
		Request				_request;
		std::string			_responseContent;

		std::string			getResponse();
		size_t				getLen() const;
		short				getCode() const;
		
		void				setRequest(Request &request);
		void				setServer(std::map<size_t, Server*>::iterator server);

		void				clear();
		void				createResponse();
		void				cutResponse(size_t i);

		void				setErrorResponse(short errorcode);
		void				setCGIstate(int state);
		int				getCGIstate();

		std::string			removeBoundary(std::string &body, std::string &boundary);
		std::string			getFileType(std::string extension);

		int				buildBody();
		void				setStatus();
		void				setHeaders();
		void				setServerDefaultErrorPages();

		int				readFile();
		void				contentType();
		void				contentLen();
		void				connection();
		void				server();
		void				location();
		void				date();

		bool				requestError();
		int				handleTarget();
		void				buildError();
		int				handleCGI(std::string &key);
		int				handleCGItemp(std::string &key);
		int 				buildHtmlIndex(std::string &dir_name, std::vector<uint8_t> &body, size_t &body_len);

		bool				checkExtensions(std::string path, std::vector<Location>::iterator v);
		bool				checkFileValidity(std::string &file, size_t len);
		std::string			checkForIndex(std::string buffer, size_t len);
		std::string			extractIndex();
};
		std::string			statusCodeString(short statusCode);

#endif
