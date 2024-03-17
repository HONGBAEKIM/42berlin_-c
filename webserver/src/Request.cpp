/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp	       		                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: agrun <agrun@student.42berlin.de>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/01 11:23:01 by agrun             #+#    #+#             */
/*   Updated: 2024/01/01 12:23:32 by agrun            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Request.hpp"

/////////////////////////////////////////////////////////////////////////////// EXTRA

static bool	checkUriPos(std::string path)
{
	std::string tmp(path);
	char *res = std::strtok((char*)tmp.c_str(), "/");	//finds the next token in a null terminated bytestring
	int pos = 0;
	while (res != NULL)
	{
		if (!strcmp(res, ".."))
			pos--;
		else
			pos++;
		if (pos < 0)
			return (1);
		res = std::strtok(NULL, "/");			//finds the next token in a null terminated bytestring
	}
	return (0);
}

static bool	allowedCharURI(uint8_t ch)
{
	if ((ch >= '#' && ch <= ';') || (ch >= '?' && ch <= '[') || (ch >= 'a' && ch <= 'z') ||
		ch == '!' || ch == '=' || ch == ']' || ch == '_' || ch == '~')
		return (true);
	return (false);
}

static bool	isToken(uint8_t ch)
{
	if (ch == '!' || (ch >= '#' && ch <= '\'') || ch == '*'|| ch == '+' || ch == '-'  || ch == '.' ||
		(ch >= '0' && ch <= '9') || (ch >= 'A' && ch <= 'Z') || (ch >= '^' && ch <= '`') ||
		(ch >= 'a' && ch <= 'z') || ch == '|')
		return (true);
	return (false);
}

static void	trim_str(std::string &str)			//disregards whitespaces before and after the word/token
{
/*
	std::string	new_str;
	size_t		i(0);
	size_t		j(0);

	while (isspace(str[i]) && str[i] != 13)
		i++;
	while (!isspace(str[i]) && str[i] == 13)
		new_str[j++] = str[i++];
	str.clear();
	str = new_str;
*/
	static const char* spaces = " \t";
	str.erase(0, str.find_first_not_of(spaces));
	str.erase(str.find_last_not_of(spaces) + 1);
}

/////////////////////////////////////////////////////////////////////////////// FUNCTIONS

void	Request::feed(char *data, size_t size, std::string index)
{
	u_int8_t			character;		//only a single unsigned character can fit in there
	static std::stringstream	s;
/*
	std::string	debug[] = {"Request_Line", "Request_Line_Post_Put", "Request_Line_Method", "Request_Line_First_Space",
					"Request_Line_URI_Path_Slash", "Request_Line_URI_Path", "Request_Line_URI_Query", "Request_Line_URI_Fragment",
					"Request_Line_Ver", "Request_Line_HT", "Request_Line_HTT", "Request_Line_HTTP", "Request_Line_HTTP_Slash",
					"Request_Line_Major", "Request_Line_Dot", "Request_Line_Minor", "Request_Line_CR", "Request_Line_LF",
					"Field_Name_Start", "Fields_End", "Field_Name", "Field_Value", "Field_Value_End", "Chunked_Length_Begin",
					"Chunked_Length", "Chunked_Ignore", "Chunked_Length_CR", "Chunked_Length_LF", "Chunked_Data", "Chunked_Data_CR",
					"Chunked_Data_LF", "Chunked_End_CR", "Chunked_End_LF", "Message_Body", "Parsing_Done"};
	int		state = _state;
*/
	for (size_t i = 0; i < size; ++i)
	{
		character = data[i];
		switch (this->_state)						//HTTP requests
		{
			case Request_Line:					//method-parsing
			{
				if (character == 'G')
					this->_method = GET;
				else if (character == 'P')
				{
					this->_state = Request_Line_Post_Put;
					break ;
				}
				else if (character == 'D')
					this->_method = DELETE;
				else if (character == 'H')
					this->_method = HEAD;
				else							//method is not implemented
				{
					this->_errorcode = 501;
					if (ERROR_LOG == 1)
						std::cout << "Method Error Request_Line and Character is = " << character << std::endl;
					return ;
				}
				this->_state = Request_Line_Method;
				break ;
			}
			case Request_Line_Post_Put:
			{
				if (character == 'O')
					this->_method = POST;
				else if (character == 'U')
					this->_method = PUT;
				else							//method is not implemented
				{
					this->_errorcode = 501;
					if (ERROR_LOG == 1)
						std::cout << "Method Error Request_Line and Character is = " << character << std::endl;
					return ;
				}
				this->_methodIndex++;
				this->_state = Request_Line_Method;
				break ;
			}
			case Request_Line_Method:
			{
				if (character == this->_methodStr[this->_method][this->_methodIndex])
					this->_methodIndex++;
				else							//method is not implemented
				{
					this->_errorcode = 501;
					if (ERROR_LOG == 1)
						std::cout << "Method Error Request_Line and Character is = " << character << std::endl;
					return ;
				}
				if ((size_t)this->_methodIndex == this->_methodStr[this->_method].length())
					this->_state = Request_Line_First_Space;
				break ;
			}
			case Request_Line_First_Space:
			{
				if (character != ' ')					//bad request, formatting is false
				{
					this->_errorcode = 400;
					if (ERROR_LOG == 1)
						std::cout << "Bad Character (Request_Line_First_Space)" << std::endl;
					return ;
				}
				this->_state = Request_Line_URI_Path_Slash;
				continue ;
			}
			case Request_Line_URI_Path_Slash:
			{
				if (character == '/')
				{
					this->_state = Request_Line_URI_Path;
					this->_storage.clear();
				}
				else							//bad request, formatting is false
				{
					this->_errorcode = 400;
					if (ERROR_LOG == 1)
						std::cout << "Bad Character (Request_Line_URI_Path_Slash)" << std::endl;
					return ;
				}
				break ;
			}
			case Request_Line_URI_Path:										// LOOK OUT FOR STORAGE AND PATH
			{													// Response::handleTarget()
				if (character == ' ')										// "GET / " will be read by then
				{
					this->_state = Request_Line_Ver;
					this->_path.append(_storage);
					this->_storage.clear();
					continue ;				//skip this iteration and continue the loop
				}
				else if (character == '?')
				{
					this->_state = Request_Line_URI_Query;
					this->_path.append(_storage);
					this->_storage.clear();
					continue ;				//skip this iteration and continue the loop
				}
				else if (character == '#')
				{
					this->_state = Request_Line_URI_Fragment;
					this->_path.append(_storage);
					this->_storage.clear();
					continue ;				//skip this iteration and continue the loop
				}
				else if (!allowedCharURI(character))			//bad request, formatting is false
				{
					this->_errorcode = 400;
					if (ERROR_LOG == 1)
						std::cout << "Bad Character (Request_Line_URI_Path)" << std::endl;
					return ;
				}
				else if (i > MAX_URI_LENGTH)				//request URI is far too long
				{
					this->_errorcode = 414;
					if (ERROR_LOG == 1)
						std::cout << "URI Too Long (Request_Line_URI_Path)" << std::endl;
					return ;
				}
				break ;
			}
			case Request_Line_URI_Query:
			{
				if (character == ' ')
				{
					this->_state = Request_Line_Ver;
					this->_query.append(_storage);
					this->_storage.clear();
					continue ;				//skip this iteration and continue the loop
				}
				else if (character == '#')
				{
					this->_state = Request_Line_URI_Fragment;
					this->_query.append(_storage);
					this->_storage.clear();
					continue ;				//skip this iteration and continue the loop
				}
				else if (!allowedCharURI(character))			//bad request, formatting is false
				{
					this->_errorcode = 400;
					if (ERROR_LOG == 1)
						std::cout << "Bad Character (Request_Line_URI_Query)" << std::endl;
					return ;
				}
				else if (i > MAX_URI_LENGTH)				//request URI is far too long
				{
					this->_errorcode = 414;
					if (ERROR_LOG == 1)
						std::cout << "URI Too Long (Request_Line_URI_Path)" << std::endl;
					return ;
				}
				break ;
			}
			case Request_Line_URI_Fragment:
			{
				if (character == ' ')
				{
					this->_state = Request_Line_Ver;
					this->_fragment.append(_storage);
					this->_storage.clear();
					continue ;				//skip this iteration and continue the loop
				}
				else if (!allowedCharURI(character))			//bad request, formatting is false
				{
					this->_errorcode = 400;
					if (ERROR_LOG == 1)
						std::cout << "Bad Character (Request_Line_URI_Fragment)" << std::endl;
					return ;
				}
				else if ( i > MAX_URI_LENGTH)				//request URI is far too long
				{
					this->_errorcode = 414;
					if (ERROR_LOG == 1)
						std::cout << "URI Too Long (Request_Line_URI_Path)" << std::endl;
					return ;
				}
				break ;
			}
			case Request_Line_Ver:
			{
				if (checkUriPos(_path))					//bad request, formatting is false
				{
					this->_errorcode = 400;
					if (ERROR_LOG == 1)
						std::cout << "Request URI ERROR: goes before root !!" << std::endl;
					return ;
				}
				if (character != 'H')					//bad request, formatting is false
				{
					this->_errorcode = 400;
					if (ERROR_LOG == 1)
						std::cout << "Bad Character (Request_Line_Ver)" << std::endl;
					return ;
				}
				this->_state = Request_Line_HT;
				break ;
			}
			case Request_Line_HT:
			{
				if (character != 'T')					//bad request, formatting is false
				{
					this->_errorcode = 400;
					if (ERROR_LOG == 1)
						std::cout << "Bad Character (Request_Line_HT)" << std::endl;
					return ;
				}
				this->_state = Request_Line_HTT;
				break ;
			}
			case Request_Line_HTT:
			{
				if (character != 'T')					//bad request, formatting is false
				{
					this->_errorcode = 400;
					if (ERROR_LOG == 1)
						std::cout << "Bad Character (Request_Line_HTT)" << std::endl;
					return ;
				}
				this->_state = Request_Line_HTTP;
				break ;
			}
			case Request_Line_HTTP:
			{
				if (character != 'P')					//bad request, formatting is false
				{
					this->_errorcode = 400;
					if (ERROR_LOG == 1)
						std::cout << "Bad Character (Request_Line_HTTP)" << std::endl;
					return ;
				}
				this->_state = Request_Line_HTTP_Slash;
				break ;
			}
			case Request_Line_HTTP_Slash:
			{
				if (character != '/')					//bad request, formatting is false
				{
					this->_errorcode = 400;
					if (ERROR_LOG == 1)
						std::cout << "Bad Character (Request_Line_HTTP_Slash)" << std::endl;
					return ;
				}
				this->_state = Request_Line_Major;
				break ;
			}
			case Request_Line_Major:
			{
				if (!isdigit(character))				//bad request, formatting is false
				{
					this->_errorcode = 400;
					if (ERROR_LOG == 1)
						std::cout << "Bad Character (Request_Line_Major)" << std::endl;
					return ;
				}
				this->_verMajor = character;
				this->_state = Request_Line_Dot;
				break ;
			}
			case Request_Line_Dot:
			{
				if (character != '.')					//bad request, formatting is false
				{
					this->_errorcode = 400;
					if (ERROR_LOG == 1)
						std::cout << "Bad Character (Request_Line_Dot)" << std::endl;
					return ;
				}
				this->_state = Request_Line_Minor;
				break ;
			}
			case Request_Line_Minor:
			{
				if (!isdigit(character))				//bad request, formatting is false
				{
					this->_errorcode = 400;
					if (ERROR_LOG == 1)
						std::cout << "Bad Character (Request_Line_Minor)" << std::endl;
					return ;
				}
				this->_verMinor = character;
				this->_state = Request_Line_CR;
				break ;
			}
			case Request_Line_CR:
			{
				if (character != '\r')					//bad request, formatting is false
				{
					this->_errorcode = 400;
					if (ERROR_LOG == 1)
						std::cout << "Bad Character (Request_Line_CR)" << std::endl;
					return ;
				}
				this->_state = Request_Line_LF;
				break ;
			}
			case Request_Line_LF:
			{
				if (character != '\n')					//bad request, formatting is false
				{
					this->_errorcode = 400;
					if (ERROR_LOG == 1)
						std::cout << "Bad Character (Request_Line_LF)" << std::endl;
					return ;
				}
				this->_state = Field_Name_Start;
				this->_storage.clear();
				continue ;					//skip this iteration and continue the loop
			}
			case Field_Name_Start:
			{
				if (character == '\r')
					this->_state = Fields_End;
				else if (isToken(character))
					this->_state = Field_Name;
				else							//bad request, formatting is false
				{
					this->_errorcode = 400;
					if (ERROR_LOG == 1)
						std::cout << "Bad Character (Field_Name_Start)" << std::endl;
					return ;
				}
				break ;
			}
			case Fields_End:
			{
				if (character == '\n')
				{
					this->_storage.clear();
					this->_fieldsDoneFlag = true;
					this->handleHeaders();
					if (this->_bodyFlag == 1)
					{
						if (this->_chunkedFlag == true)
							this->_state = Chunked_Length_Begin;
						else
							this->_state = Message_Body;
					}
					else
						this->_state = Parsing_Done;
					continue ;				//skip this iteration and continue the loop
				}
				else							//bad request, formatting is false
				{
					this->_errorcode = 400;
					if (ERROR_LOG == 1)
						std::cout << "Bad Character (Fields_End)" << std::endl;
					return ;
                		}
				break ;
			}
			case Field_Name:
			{
				if (character == ':')
				{
					this->_keyStorage = _storage;
					this->_storage.clear();
					this->_state = Field_Value;
					continue ;				//skip this iteration and continue the loop
				}
				else if (!isToken(character))				//bad request, formatting is false
				{
					this->_errorcode = 400;
					if (ERROR_LOG == 1)
						std::cout << "Bad Character (Field_Name)" << std::endl;
					return ;
				}
				break ;
			}
			case Field_Value:
			{
				if (character == '\r')
				{
					setHeader(_keyStorage, _storage);
					this->_keyStorage.clear();
					this->_storage.clear();
					this->_state = Field_Value_End;
					continue ;				//skip this iteration and continue the loop
				}
				break ;
			}
			case Field_Value_End:
			{
				if (character == '\n')
				{
					this->_state = Field_Name_Start;
					continue ;				//skip this iteration and continue the loop
				}
				else							//bad request, formatting is false
				{
					this->_errorcode = 400;
					if (ERROR_LOG == 1)
						std::cout << "Bad Character (Field_Value_End)" << std::endl;
					return ;
				}
				break ;
			}
			case Chunked_Length_Begin:
			{
				if (isxdigit(character) == 0)			//check for hexadecimal character...
				{
					this->_errorcode = 400;				//bad request, formatting is false
					if (ERROR_LOG == 1)
						std::cout << "Bad Character (Chunked_Length_Begin)" << std::endl;
					return ;
				}
				s.str("");
				s.clear();
				s << character;
				s >> std::hex >> _chunkLength;
				if (_chunkLength == 0)
					this->_state = Chunked_Length_CR;
				else
					this->_state = Chunked_Length;
				continue ;					//skip this iteration and continue the loop
			}
			case Chunked_Length:
			{
				if (isxdigit(character) != 0)			//check for hexadecimal character...
				{
					int temp_len = 0;
					s.str("");
					s.clear();
					s << character;
					s >> std::hex >> temp_len;
					this->_chunkLength *= 16;
					this->_chunkLength += temp_len;
				}
				else if (character == '\r')
					this->_state = Chunked_Length_LF;
				else
					this->_state = Chunked_Ignore;
				continue ;
			}
			case Chunked_Length_CR:
			{
				if (character == '\r')
					this->_state = Chunked_Length_LF;
				else							//bad request, formatting is false
				{
					this->_errorcode = 400;
					if (ERROR_LOG == 1)
						std::cout << "Bad Character (Chunked_Length_CR)" << std::endl;
					return ;
				}
				continue ;					//skip this iteration and continue the loop
			}
			case Chunked_Length_LF:
			{
				if (character == '\n')
				{
					if (_chunkLength == 0)
						this->_state = Chunked_End_CR;
					else
						this->_state = Chunked_Data;
				}
				else							//bad request, formatting is false
				{
					this->_errorcode = 400;
					if (ERROR_LOG == 1)
						std::cout << "Bad Character (Chunked_Length_LF)" << std::endl;
					return ;
				}
				continue ;					//skip this iteration and continue the loop
			}
			case Chunked_Ignore:
			{
				if (character == '\r')
					this->_state = Chunked_Length_LF;
				continue ;					//skip this iteration and continue the loop
			}
			case Chunked_Data:
			{
				this->_body.push_back(character);
				--_chunkLength;
				if (this->_chunkLength == 0)
					this->_state = Chunked_Data_CR;
				continue ;
			}
			case Chunked_Data_CR:
			{
				if (character == '\r')
					this->_state = Chunked_Data_LF;
				else							//bad request, formatting is false
				{
					this->_errorcode = 400;
					if (ERROR_LOG == 1)
						std::cout << "Bad Character (Chunked_Data_CR)" << std::endl;
					return ;
				}
				continue ;
			}
			case Chunked_Data_LF:
			{
				if (character == '\n')
					this->_state = Chunked_Length_Begin;
				else							//bad request, formatting is false
				{
					this->_errorcode = 400;
					if (ERROR_LOG == 1)
						std::cout << "Bad Character (Chunked_Data_LF)" << std::endl;
					return ;
				}
				continue ;					//skip this iteration and continue the loop
			}
			case Chunked_End_CR:
			{
				if (character != '\r')					//bad request, formatting is false
				{
					this->_errorcode = 400;
					if (ERROR_LOG == 1)
						std::cout << "Bad Character (Chunked_End_CR)" << std::endl;
					return ;
				}
				this->_state = Chunked_End_LF;
				continue ;
			}
			case Chunked_End_LF:
			{
				if (character != '\n')					//bad request, formatting is false
				{
					this->_errorcode = 400;
					if (ERROR_LOG == 1)
						std::cout << "Bad Character (Chunked_End_LF)" << std::endl;
					return ;
				}
				this->_bodyDoneFlag = true;
				this->_state = Parsing_Done;
				continue ;
			}
			case Message_Body:
			{
				if (this->_body.size() < _bodyLength)
					this->_body.push_back(character);
				if (this->_body.size() == _bodyLength)
				{
					this->_bodyDoneFlag = true;
					this->_state = Parsing_Done;
				}
				break ;
			}
			case Parsing_Done:
			{
				return ;
			}
		}
		this->_storage += character;
	}
	if (this->_state == Parsing_Done)
		this->_bodyStr.append((char*)_body.data(), this->_body.size());

	if (_path == "/" && _path.size() == 1)
	{
		_path = index;
		return ;
	}
	if (this->_path.find('.') == std::string::npos)					//Adding slash at end if directory.			// CRITICAL!
		if (_path[_path.size()-1] != '/')
			_path.append("/");

	size_t pos = _path.find(index + '/');						//Dealing with index embedded between slashes.		// CRITICAL!
	if (pos != std::string::npos && pos != 0)
	{
		if (_path[pos] == '/')
			pos++;
		_path.erase(pos, index.size());
		return ;
	}
	if (INDEX_KILLER)								//Dealing with browsers like Chrome adding "index.html" on their own.
	{
		pos = _path.find("/index.html/");
		if (pos != std::string::npos && pos != 0)
			_path.erase(++pos, 11);
		return ;
	}
}

void	Request::cutBody(int bytes)
{
	if (bytes < 0)
		return ;
	if (this->_bodyStr.size() < (size_t)bytes)		//safety measure
		bytes = _bodyStr.size();
	this->_bodyStr = this->_bodyStr.substr(bytes);		//shorten string
}


bool	Request::parsingComplete()
{
	 return _state == Parsing_Done;				//true or false
}

bool	Request::keepAlive()				//keep the connection alive?
{
	if (_headers.count("connection"))		//npos = until the end of str
	{
		if (_headers["connection"].find("close", 0) != std::string::npos)
			return (false);
	}
	return (true);
}
/*
void	Request::printMessage()
{
	std::cout << this->_methodStr[this->_method] + " " + this->_path + "?" + this->_query + "#"
		+ this->_fragment + " " + "HTTP/" << this->_verMajor  << "." << this->_verMinor << std::endl;

	for (std::map<std::string, std::string>::iterator it = this->_headers.begin();
		it != this->_headers.end(); ++it)
		std::cout << it->first + ":" + it->second << std::endl;

	for (std::vector<u_int8_t>::iterator it = this->_body.begin();
		it != this->_body.end(); ++it)
		std::cout << *it;

	std::cout << std::endl << "END OF BODY" << std::endl;
	std::cout << "BODY FLAG =" << this->_bodyFlag << "  _BOD_done_flag= "
	<< this->_bodyDoneFlag << "FIELDS FLAG = " << this->_fieldsDoneFlag << std::endl;
}
*/
void	Request::clear()
{
	this->_serverName.clear();
	this->_path.clear();
	this->_query.clear();
	this->_fragment.clear();
	this->_headers.clear();
	this->_body.clear();
	this->_bodyStr = "";
	this->_boundary.clear();
	this->_state = Request_Line;
	this->_method = NONE;
	this->_methodIndex = 1;
	this->_errorcode = 0;
	this->_bodyLength = 0;
	this->_chunkLength = 0x0;
	this->_storage.clear();
	this->_keyStorage.clear();
	this->_fieldsDoneFlag = false;
	this->_bodyFlag = false;
	this->_bodyDoneFlag = false;
	this->_completeFlag = false;
	this->_chunkedFlag = false;
	this->_multiformFlag = false;
}

void	Request::handleHeaders()
{
	std::stringstream ss;

	if (this->_headers.count("content-length"))				//is "content-length" in there?
	{
		this->_bodyFlag = true;
		ss << this->_headers["content-length"];
		ss >> this->_bodyLength;
	}
	if (this->_headers.count("transfer-encoding"))				//is "transfer-encoding" in there?
	{
		if (this->_headers["transfer-encoding"].find_first_of("chunked") != std::string::npos)
			this->_chunkedFlag = true;
		this->_bodyFlag = true;
	}
	if (this->_headers.count("host"))					//is "host" in there?
	{
		size_t pos = _headers["host"].find_first_of(':');
		this->_serverName = this->_headers["host"].substr(0, pos);
	}									//is "content-type" in there?
	if (this->_headers.count("content-type")
		&& this->_headers["content-type"].find("multipart/form-data") != std::string::npos)
	{
		size_t pos = this->_headers["content-type"].find("boundary=", 0);
		if (pos != std::string::npos)
			this->_boundary = this->_headers["content-type"].substr(pos + 9, this->_headers["content-type"].size());
		this->_multiformFlag = true;
    }
}

///////////////////////////////////////////////////////////////////////////////	SETTERS

void	Request::setPath(std::string path)
{
	this->_path = path;
}

void	Request::setMethod(Method &method)
{
	this->_method = method;
}

void	Request::setHeader(std::string &name, std::string &value)
{
	trim_str(value);				//removes whitespaces before and after the string

	size_t i(0);
	while (i < name.size())				//converting everything to lowercase characters
	{
		name[i] = std::tolower(name[i]);
		i++;
	}
	this->_headers[name] = value;
}

void	Request::setMaxBodySize(size_t max)
{
	this->_maxBodySize = max;
}

void	Request::setBody(std::string body)
{
	this->_body.clear();
	this->_body.insert(this->_body.begin(), this->_body.begin(), this->_body.end());
	this->_bodyStr = body;
}

///////////////////////////////////////////////////////////////////////////////	GETTERS

short	Request::getErrorCode()
{
	return this->_errorcode;
}

Method	&Request::getMethod()
{
	return this->_method;
}

std::string	Request::getMethodStr()
{
	return this->_methodStr[this->_method];
}

std::string	&Request::getPath()
{
	return this->_path;
}

std::string	&Request::getQuery()
{
	return this->_query;
}

std::string	&Request::getFragment()
{
	return this->_fragment;
}

std::string	Request::getHeader(std::string const &name)
{
	return this->_headers[name];
}

const std::map<std::string, std::string>	&Request::getHeaders() const
{
	return this->_headers;
}

std::string	&Request::getBody()
{
	return this->_bodyStr;
}

std::string	Request::getServerName()
{
	return this->_serverName;
}

std::string	&Request::getBoundary()
{
	return this->_boundary;
}

bool	Request::getMultiformFlag()
{
	return this->_multiformFlag;
}

///////////////////////////////////////////////////////////////////////////////	(DE)CON

Request::~Request()
{
	return;
}

Request::Request()
{
	this->_path = "";
	this->_query = "";
	this->_fragment = "";
	this->_bodyStr = "";
	this->_boundary = "";
	this->_state = Request_Line;

	this->_methodStr[::GET] = "GET";
	this->_methodStr[::POST] = "POST";
	this->_methodStr[::DELETE] = "DELETE";
	this->_methodStr[::PUT] = "PUT";
	this->_methodStr[::HEAD] = "HEAD";
	this->_method = NONE;
	this->_methodIndex = 1;

	this->_errorcode = 0;
	this->_bodyLength = 0;
	this->_chunkLength = 0;
	this->_storage = "";
	this->_keyStorage = "";

	this->_fieldsDoneFlag = false;
	this->_bodyFlag = false;
	this->_bodyDoneFlag = false;
	this->_chunkedFlag = false;
	this->_multiformFlag = false;
}