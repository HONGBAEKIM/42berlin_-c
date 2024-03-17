/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp          	                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: agrun <agrun@student.42berlin.de>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/01 11:08:49 by agrun             #+#    #+#             */
/*   Created: 2024/01/01 12:10:52 by agrun            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef REQUEST_HPP
# define REQUEST_HPP

# include <iostream>
# include <cstdlib>
# include <cctype>
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

# define MAX_URI_LENGTH 4096

# define ERROR_LOG 0			// Change this value to 1 if you want error-printouts on feed()!
# define INDEX_KILLER 1			// Change this value to 1 if you want to get rid of the browser
					// adding "index.html" and the like on its own. Mostly Chrome.
enum Method
{
	GET,
	POST,
	DELETE,
	PUT,
	HEAD,
	NONE
};

enum Parsing
{
	Request_Line,
	Request_Line_Post_Put,
	Request_Line_Method,
	Request_Line_First_Space,
	Request_Line_URI_Path_Slash,
	Request_Line_URI_Path,
	Request_Line_URI_Query,
	Request_Line_URI_Fragment,
	Request_Line_Ver,
	Request_Line_HT,
	Request_Line_HTT,
	Request_Line_HTTP,
	Request_Line_HTTP_Slash,
	Request_Line_Major,
	Request_Line_Dot,
	Request_Line_Minor,
	Request_Line_CR,
	Request_Line_LF,
	Field_Name_Start,
	Fields_End,
	Field_Name,
	Field_Value,
	Field_Value_End,
	Chunked_Length_Begin,
	Chunked_Length,
	Chunked_Ignore,
	Chunked_Length_CR,
	Chunked_Length_LF,
	Chunked_Data,
	Chunked_Data_CR,
	Chunked_Data_LF,
	Chunked_End_CR,
	Chunked_End_LF,
	Message_Body,
	Parsing_Done
};

class Request
{
	private:
		std::string				_serverName;
		std::string				_path;
		std::string				_query;
		std::string				_fragment;
		std::map<std::string, std::string>	_headers;
		std::vector<u_int8_t>			_body;
		std::string				_bodyStr;
		std::string				_boundary;

		Parsing					_state;
		Method					_method;
		std::map<u_int8_t, std::string>		_methodStr;
		short					_methodIndex;

		short					_errorcode;
		size_t					_maxBodySize;
		size_t					_bodyLength;
		size_t					_chunkLength;
		std::string				_storage;
		std::string				_keyStorage;
		u_int8_t				_verMajor;
		u_int8_t				_verMinor;

		bool					_fieldsDoneFlag;
		bool					_bodyFlag;
		bool					_bodyDoneFlag;
		bool					_completeFlag;
		bool					_chunkedFlag;
		bool					_multiformFlag;

	public:
		Request();
		~Request();

		Method						&getMethod();
		std::string					getMethodStr();
		std::string					&getPath();
		std::string					&getQuery();
		std::string					&getFragment();
		std::string					getHeader(std::string const &name);
		const std::map<std::string, std::string>	&getHeaders() const;

		std::string					&getBody();
		std::string					getServerName();
		std::string					&getBoundary();
		bool						getMultiformFlag();
		short						getErrorCode();

		void	handleHeaders();	//originally set to private

		void				setMethod(Method &method);
		void				setHeader(std::string &name, std::string &value);
		void				setMaxBodySize(size_t max);
		void				setBody(std::string body);
		void				setPath(std::string path);

		void				feed(char *data, size_t size, std::string index);
		void				cutBody(int bytes);
		bool				parsingComplete();
		bool				keepAlive();
		void				printMessage();
		void				clear();
};

#endif
