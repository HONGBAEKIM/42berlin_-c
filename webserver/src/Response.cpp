/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp	       		              	:+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: agrun <agrun@student.42berlin.de>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/03 11:23:01 by agrun             #+#    #+#             */
/*   Updated: 2024/01/03 12:23:32 by agrun            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Response.hpp"

int	error_built = 0;						// global variable for the buildError() function

template <typename T>
std::string to_string(const T number)					// transforming any number-type into a string
{
	std::stringstream ss;
	ss << number;
	return ss.str();
}

std::string	statusCodeString(short statusCode)				// list of recognized HTML-codes
{
	switch (statusCode)
	{
		case 100:
			return "Continue";
		case 101:
			return "Switching Protocol";
		case 200:
			return "OK";
		case 201:
			return "Created";
		case 202:
			return "Accepted";
		case 203:
			return "Non-Authoritative Information";
		case 204:
			return "No Content";
		case 205:
			return "Reset Content";
		case 206:
			return "Partial Content";
		case 300:
			return "Multiple Choice";
		case 301:
			return "Moved Permanently";
		case 302:
			return "Moved Temporarily";
		case 303:
			return "See Other";
		case 304:
			return "Not Modified";
		case 307:
			return "Temporary Redirect";
		case 308:
			return "Permanent Redirect";
		case 400:
			return "Bad Request";
		case 401:
			return "Unauthorized";
		case 403:
			return "Forbidden";
		case 404:
			return "Not Found";
		case 405:
			return "Method Not Allowed";
		case 406:
			return "Not Acceptable";
		case 407:
			return "Proxy Authentication Required";
		case 408:
			return "Request Timeout";
		case 409:
			return "Conflict";
		case 410:
			return "Gone";
		case 411:
			return "Length Required";
		case 412:
			return "Precondition Failed";
		case 413:
			return "Payload Too Large";
		case 414:
			return "URI Too Long";
		case 415:
			return "Unsupported Media Type";
		case 416:
			return "Requested Range Not Satisfiable";
		case 417:
			return "Expectation Failed";
		case 418:
			return "I'm a teapot";					// based on an HTML April Fools joke
		case 421:
			return "Misdirected Request";
		case 425:
			return "Too Early";
		case 426:
			return "Upgrade Required";
		case 428:
			return "Precondition Required";
		case 429:
			return "Too Many Requests";
		case 431:
			return "Request Header Fields Too Large";
		case 451:
			return "Unavailable for Legal Reasons";
		case 500:
			return "Internal Server Error";
		case 501:
			return "Not Implemented";
		case 502:
			return "Bad Gateway";
		case 503:
			return "Service Unavailable";
		case 504:
			return "Gateway Timeout";
		case 505:
			return "HTTP Version Not Supported";
		case 506:
			return "Variant Also Negotiates";
		case 507:
			return "Insufficient Storage";
		case 510:
			return "Not Extended";
		case 511:
			return "Network Authentication Required";
		default:							// default case for any other code
			return "Undefined";
	}
}

std::string	Response::extractIndex()
{
	size_t		i = 0;
	std::string	index;
	std::string	str;

	index = this->_server->second->getIndex();
	while (i < index.size() + 1)
	{
		if ((i == 0 && index[i] != '/') 
			|| (index[i] != '\r' && index[i] != '\n'))
			str += index[i];
		i++;
	}
	if (str[str.size()-1] != '/')
		str += '/';
	return str;
}

std::string	Response::checkForIndex(std::string buffer, size_t len)
{
	size_t		i;
	size_t		x(0);
	std::string	new_b;
	std::string	index(extractIndex());

	if (buffer.empty() == true)
		return buffer;
	i = buffer.find(index.c_str());
	if (i == std::string::npos)
		return buffer;
	while (x < i)
	{
		new_b += buffer[x];
		x++;
	}
	i += index.size() - 1;
	while (i < len)
	{
		new_b += buffer[i];
		i++;
	}
	return new_b;
}

int	Response::buildHtmlIndex(std::string &dir_name, std::vector<uint8_t> &body, size_t &body_len)
{
	struct dirent	*entityStruct;						// Generates a static page if autoindex is enabled.
	DIR		*directory;
	std::string	dirListPage;

	if (_code == 403 || _lastCode == 403)
		return 1;

	//////////////////////////////////////////////////
	std::string	buffer;
	std::string	backup;
	std::string	findIndex;

	backup = dir_name;
	findIndex = extractIndex();
	
	if (findIndex[findIndex.size()-1] == '/')
		findIndex.erase(findIndex.size()-1, 1);
	if (findIndex[0] != '/')
		findIndex.insert(0, 1, '/');
		
	size_t j = backup.find(findIndex.c_str());				// Removing index.html from target requests to make static indexing work.
	if (j != std::string::npos)
	{
		for (size_t i = 0; i < j; i++)
			buffer += backup[i];
		j += extractIndex().size();
		for (size_t i = j; backup[i]; i++)
			buffer += backup[i];
		dir_name = buffer;
	}
	//////////////////////////////////////////////////

	directory = opendir(dir_name.c_str());
	if (directory == NULL)
	{
		// std::cerr << "opendir() failed" << std::endl;
		// dir_name = backup;						// REVERTING DIR_NAME
		return 1;
	}

	dirListPage.append("<html>\n");
	dirListPage.append("<head>\n");
	dirListPage.append("<title> Index of ");
	dirListPage.append(dir_name);
	dirListPage.append("</title>\n");
	dirListPage.append("</head>\n");
	dirListPage.append("<body >\n");
	dirListPage.append("<h1> Index of " + dir_name + "</h1>\n");
	dirListPage.append("<table style=\"width:80%; font-size: 15px\">\n");
	dirListPage.append("<hr>\n");
	dirListPage.append("<th style=\"text-align:left\"> File Name </th>\n");
	dirListPage.append("<th style=\"text-align:left\"> Last Modification  </th>\n");
	dirListPage.append("<th style=\"text-align:left\"> File Size </th>\n");

	struct stat	file_stat;
	std::string	file_path;

	while((entityStruct = readdir(directory)) != NULL)		//caution, this will allocate memory
	{
		if(strcmp(entityStruct->d_name, ".") == 0)
			continue;					//will skip one iteration of the loop

		if (dir_name[dir_name.size()-1] != '/' && entityStruct->d_name[0] != '/')
			file_path = dir_name + '/' + entityStruct->d_name;
		else
			file_path = dir_name + entityStruct->d_name;	//making sure there's a slash in between
		stat(file_path.c_str(), &file_stat);

		dirListPage.append("<tr>\n");
		dirListPage.append("<td>\n");
		dirListPage.append("<a href=\"");
		dirListPage.append(entityStruct->d_name);
		if (S_ISDIR(file_stat.st_mode))				//add slash ("/") if directory
			dirListPage.append("/");

		dirListPage.append("\">");
		dirListPage.append(entityStruct->d_name);
		if (S_ISDIR(file_stat.st_mode))				//add slash ("/") if directory
			dirListPage.append("/");

		dirListPage.append("</a>\n");
		dirListPage.append("</td>\n");
		dirListPage.append("<td>\n");
		dirListPage.append(ctime(&file_stat.st_mtime));
		dirListPage.append("</td>\n");
		dirListPage.append("<td>\n");
		if (!S_ISDIR(file_stat.st_mode))				//not a directory?
			dirListPage.append(to_string(file_stat.st_size));
	
		dirListPage.append("</td>\n");
		dirListPage.append("</tr>\n");
	}
	closedir(directory);						//mandatory after using readdir()

	dirListPage.append("</table>\n");
	dirListPage.append("<hr>\n");
	dirListPage.append("</body>\n");
	dirListPage.append("</html>\n");

	body.insert(body.begin(), dirListPage.begin(), dirListPage.end());
	body_len = body.size();
	// dir_name = backup;							// REVERTING DIR_NAME
	return 0;
}

///////////////////////////////////////////////////////////////////////////////	STATIC

/*
static bool	fileExists (std::string &file)				//checking for valid files which can be opened
{
	std::ifstream	filestr(file.c_str());
	std::string	filex(file);

	if (filestr.good() == false)
	{
		if (filex[filex.size()-1] == '/')
		{
			filex.erase(filex.size()-1, 1);
			std::ifstream	filestrx(filex.c_str());
			if (filestrx.good() == true)
			{
				file = filex;
				return true;
			}
			return false;
		}
		return false;
	}
	return true;
	//return filestr.good();
}
*/

static bool	fileExists (const std::string &file)			//checking for valid files which can be opened
{
	std::ifstream	filestr(file.c_str());
	return filestr.good();
}

static bool	isDirectory(std::string path)				//does the given server-directory exist?
{
	struct stat	buf;

	if (stat(path.c_str(), &buf) != 0)
		return false;
	return S_ISDIR(buf.st_mode);
}

static std::string	combinePaths(std::string p1, std::string p2, std::string p3)	//combining strings into one
{
	std::string	res;
	int		len1;
	int		len2;

	len1 = p1.length();
	len2 = p2.length();
	if (p1[len1 - 1] == '/' && (!p2.empty() && p2[0] == '/') )			//removing mid-slash ("/")
		p2.erase(0, 1);
	if (p1[len1 - 1] != '/' && (!p2.empty() && p2[0] != '/'))			//adding slash to front
		p1.insert(p1.end(), '/');
	if (p2[len2 - 1] == '/' && (!p3.empty() && p3[0] == '/') )			//removing mid-slash
		p3.erase(0, 1);
	if (p2[len2 - 1] != '/' && (!p3.empty() && p3[0] != '/'))			//adding slash to front
		p2.insert(p1.end(), '/');
	res = p1 + p2 + p3;
	return res;
}

static void	appendRoot(std::vector<Location>::iterator location, Request &request, std::string &target_file)
{
	target_file = combinePaths(location->getRoot(), request.getPath(), "");
	size_t pos = target_file.find("//");						//removing potential extra slash
	if (pos != std::string::npos)
		target_file.erase(pos, 1);
	if (isDirectory(target_file) == false)						//adding slash at end for directory		// CRITICAL!
		if (target_file[target_file.size()-1] != '/')
			target_file.append("/");
}

static bool	checkReturn(std::vector<Location>::iterator loc, short &code, std::string &location)
{
	if (loc->getReturn().empty() == false)
	{
		code = 301;								//301: location moved permanently
		location = loc->getReturn();						//updating location
		if (location[0] != '/')
			location.insert(location.begin(), '/');				//adding slash to front
		return 1;
	}
	return 0;
}

static void	getLocationMatch(std::string &path, std::vector<Location>::iterator locations,
			std::vector<Location>::iterator locations_end, std::string &location_path)
{
	size_t	flag(0);
	std::vector<Location>::iterator		l = locations;
	std::vector<Location>::iterator		le = locations_end;
	std::vector<std::string>::iterator	ext;
	std::vector<std::string>::iterator	exte;
	std::vector<std::string>::iterator	extel;

	while (l != le)
	{
		ext = l->getCGIext();
		exte = l->getCGIextEnd();
		extel = l->getCGIext();
		while (ext != exte)
		{
			if (path.find(*ext) != std::string::npos)
			{
				flag = 1;
				break ;
			}
			ext++;
		}
		l++;
	}
	if (flag)
	{
		location_path = "/cgi-bin";
		return ;
	}
/*
	std::string	backup = path;
	std::string	checker = path;
	std::string	newpath("");
	std::string	extension("");
	size_t i(0);
	size_t j(0);
	size_t flagger(0);
	size_t pos = checker.find(".");
	if (pos != std::string::npos)
	{
		i = pos;
		while (checker[i])
		{
			extension += checker[i];
			i++;
		}
		if (extension != ".html" && extension != ".php")
		{
			while (extel != exte)
			{
				if (extension != *extel)
					extel++;
				else
					break ;
			}
			if (extel == exte)
			{
				i = pos;
				while (checker[i] && checker[i] != '/')
					i--;
				j = checker.size()-i;
				checker.erase(i, j);
				i = checker.size() - 1;
				while (checker[i] && checker[i] != '/')
					i--;
				while (checker[i])
				{
					newpath += checker[i];
					i++;
				}
				path = newpath;											// CRITICAL!
				flagger = 1;
			}
		}
	}
*/
	size_t	biggest_match(0);
	while (locations != locations_end)
	{
		if (path.find(locations->getPath()) == 0)				//"location_path" appearing in "path"?
		{
			if (locations->getPath() == "/" || path.length() == locations->getPath().length()
				|| path[locations->getPath().length()] == '/')
			{
				if (locations->getPath().length() > biggest_match)
				{
					biggest_match = locations->getPath().length();
					location_path = locations->getPath();		//overwriting with the closest match
				}
			}
		}
		locations++;
	}
	//if (flagger)
	//	path = backup;													// CRITICAL!
}

static std::string	getStatusErrorPage(short statusCode)		//transforming number into string
{									//and adding HTML formatting to it
	std::stringstream	ss;					//to generate an error-page which
	std::string		buffer;					//wasn't premade yet

	ss.clear();
	buffer.clear();
	ss << statusCode; ss >> buffer;

	return "<html>\r\n<head><title>" + buffer + " " + statusCodeString(statusCode) +
		"</title></head>\r\n" + "<body>\r\n" + "<center><h1>" + buffer + " " +						// space before </title>...?
		statusCodeString(statusCode) + "</h1></center>\r\n" + "</body>\r\n" + "</html>\r\n";				// + "</body>\r\n" + "</html>\r\n";
}

static int	getTypePath(std::string const path)
{
	struct stat	buffer;
	int		result;
	
	result = stat(path.c_str(), &buffer);				//stat() obtains information about a file
	if (result == 0)
	{
		if (buffer.st_mode & S_IFREG)				//S_IFREG = is it a regular file?
			return 1;
		else if (buffer.st_mode & S_IFDIR)			//S_IFDIR = is it a directory?
			return 2;
		else							//is it neither?
			return 3;
	}
	else
		return -1;						//stat() function failed...
}

static int	checkFile(std::string const path, int mode)			//can the file be accessed?
{
	return (access(path.c_str(), mode));
}

static bool	isAllowedMethod(Method &method, std::vector<Location>::iterator v, short &code)
{
	std::vector<short> methods;
	for (short i(0); i < 5; i++)					//FILLING CONTAINER WITH ZEROES
		methods.push_back(0);					//methods[0] to methods[4]

	if (v->getReturn().empty() == false)				//"Return" within a Location does not require methods!
		return 0;

	std::vector<std::string>::iterator a = v->getMethods();
	std::vector<std::string>::iterator b = v->getMethodsEnd();
	while (a != b)
	{
		if (*a == "GET")
			methods[0] = 1;
		else if (*a == "POST")
			methods[1] = 1;
		else if (*a == "DELETE")
			methods[2] = 1;
		else if (*a == "PUT")
			methods[3] = 1;
		else if (*a == "HEAD")
			methods[4] = 1;
		a++;
	}								//check GET/POST/DELETE/PUT/HEAD in this order

	if ((method == GET && !methods[0]) || (method == POST && !methods[1]) ||
		(method == DELETE && !methods[2]) || (method == PUT && !methods[3]) ||
		(method == HEAD && !methods[4]))
	{
		code = 405;						//method not allowed...
		return 1;
	}
	return 0;
}

static std::string	getErrorStatusPage(short statusCode)		//constructing an HTML-errorpage
{
	return "<html>\r\n<head><title>" + to_string(statusCode) + " " +
		statusCodeString(statusCode) + "</title></head>\r\n" +								// space before </title>...?
		"<body>\r\n" + "<center><h1>" + to_string(statusCode) + 
		" " + statusCodeString(statusCode) + "</h1></center>\r\n" +
		"</body>\r\n" + "</html>\r\n";											// + "</body>\r\n" + "</html>\r\n";
}

/*									// DEPRECATED: DISREGARD THIS PART OF THE CODE
	size_t i = backup.size() - 2;					// Shortening the target to the previous slash ("/")
	size_t j = 0;							// e.g.: docs/YoupiBanane/index.html => docs/YoupiBanane/

	while (backup[i] != '/' && backup[i])
		i--;
	while (j < i + 1)
	{
		buffer += backup[j];
		j++;
	}
	dir_name = buffer;
*/

/////////////////////////////////////////////////////////////////////////////// EXTRA

bool	Response::requestError()
{
	if (this->_request.getErrorCode())
	{
		this->_code = this->_request.getErrorCode();
		return 1;
	}
	return 0;
}

bool	Response::checkFileValidity(std::string &file, size_t len)
{
	std::ifstream	fd;
	std::string	update;
	size_t 		pos;
	int		flagged(0);					// Is it the root location aka the first index being called?

	update = file;
	if (this->_code == 501)
		return true;
	if (file.empty() == true)
	{
		this->_code = 500;
		return false;
	}
	fd.open(file.c_str());
	if (fd.is_open() == false)					// May not open because of index.html appearing as a root within string.
	{
		std::string	buffer = file;
		pos = buffer.substr(len-1).find('/');
		if (pos != std::string::npos)
			buffer.erase(len-1, 1);
		update.clear();
		update = checkForIndex(buffer, len);			// Remove any instance of index.html appearing in the middle of a string.
		fd.open(update.c_str());
		if (fd.is_open() == false)
			return false;
	}
	else
		update = file;
	if (file == this->_server->second->getRoot())
	{
		update = "/";
		flagged = 1;						// The wanted location is actually "/"
	}

	pos = update.find('.');
	if (pos == std::string::npos)					// Add the stored index if the target is just a directory. Mostly for redirects.
	{
		fd.close();
		std::vector<Location>::iterator a = this->_server->second->getLocations();
		std::vector<Location>::iterator b = this->_server->second->getLocationsEnd();
		while (a != b)
		{
			if (!flagged)
			{
				if (a->getPath() != "/")
					if (update.find(a->getPath()) != std::string::npos)
						break ;
			}
			else
				if (update.find(a->getPath()) != std::string::npos)
					break ;
			a++;
		}
		if (a == b)
		{
			this->_code = 500;
			return false;
		}
		if (a->getIndex().empty() == true)			// The affected Location does not have a valid index, need to create a static one!
		{
			this->_code = 200;
			return false;												// CRITICAL!
		}
		else
		{
			if (update[update.size()-1] != '/')
				update += '/';
			update += a->getIndex();
			fd.open(update.c_str());
			if (fd.is_open() == false)
				return false;
		}
	}
	if (this->_autoIndex)
	{
		std::ostringstream ss;
		ss << fd.rdbuf();
		fd.close();
		this->_responseBody = ss.str();
		this->_code = 200;
	}
	else
		fd.close();
	return true;
}

void	Response::createResponse()
{
	std::string	path(this->_request.getPath());
	std::string	buffer(path);

	error_built = 0;
	if (isDirectory(path) == false)								// Just entering address and port will redirect to index but
	{											// with an extra "/", this will make sure to get rid of it.
		if (buffer[0] == '/')
			buffer.erase(0, 1);
		if (buffer[buffer.size()-1] == '/')
			buffer.erase(buffer.size()-1, 1);
		if (buffer == _server->second->getIndex())
		{
			if (path[0] == '/' && path[path.size()-1] == '/')
			{
				path.erase(path.size()-1, 1);
				this->_request.setPath(path);
			}
		}
	}

	if (this->requestError() || this->buildBody())
		this->buildError();
	if (this->_cgi)
		return ;
	if (this->_autoIndex == true)
	{
		if (checkFileValidity(this->_targetFile, this->_targetFile.size()) == false)
		{
			if (this->_code != 0 && this->_code != 200)
				this->buildError();
			if (buildHtmlIndex(this->_targetFile, this->_body, this->_bodyLen))		// creates a static page
			{
				if (this->_code < 500 && this->_code != 403)
					this->_code = 404;						// originally set to 500
			}
			else
				this->_code = 200;							// ready to transmit
		}
		if (this->_code != 200)
			this->buildError();
		this->_responseBody.insert(this->_responseBody.begin(),
			this->_body.begin(), this->_body.end());
	}
	else
	{
		checkFileValidity(this->_targetFile, this->_targetFile.size());
		this->buildError();
		this->_responseBody.insert(this->_responseBody.begin(),
			this->_body.begin(), this->_body.end());
	}
	this->setStatus();
	this->setHeaders();
	if (this->_request.getMethod() != HEAD 
		&& (this->_request.getMethod() == GET || this->_code != 200))
		this->_responseContent.append(_responseBody);
}

void	Response::buildError()							//will construct a HTML errorpage
{
	size_t checker(0);
	std::map<int, std::string>::iterator a = this->_server->second->getErrorPages();
	std::map<int, std::string>::iterator b = this->_server->second->getErrorPagesEnd();

	if (this->_code == 200 || error_built)
		return ;
	if (this->_code != 0)													// CRITICAL!
		error_built = 1;
	if (this->_code == 301 && this->_autoIndex)										// CRITICAL!
	{
		this->_lastCode = this->_code;					//wouldn't want to display 301 on the new page
		return ;
	}
/*	else if (this->_code == 301 && !this->_autoIndex)			//illegal access attempt registered
	{
		this->_lastCode = 403;
		this->_code = 403;
	}
*/
	//////////////////////////////////////////////////////////////////////
/*
	std::vector<Location>::iterator it = this->_server->second->getLocations();
	std::vector<Location>::iterator ite = this->_server->second->getLocationsEnd();

	int		check(0);
	std::string	buffer = _targetFile;
	size_t		bu = _targetFile.size() - 2;

	while (buffer[bu] != '/' && bu != 0)
		bu--;
	buffer.clear();

	for (size_t x(0); x < bu; x++)
		buffer += _targetFile[x];

	if (this->_code != 404 && this->_code != 501)
	{
		while (it != ite)						//making sure no undefined Locations are accessed
		{								//otherwise deny access
			if (it->getPath().find(buffer) != std::string::npos)
			{
				check = 1;
				break ;
			}
			it++;
		}
	}
	if (this->_code != 404 && this->_code != 501 && check)			//illegal access registered
	{
		_code = 403;
		this->_lastCode = 403;
	}
*/
	//////////////////////////////////////////////////////////////////////

	while (a != b)								//checking if errorcode exists
	{
		if ((short)a->first == this->_code)
			if (a->second.empty() == false)				//does it contain a page-path?
				checker = 1;
		a++;
	}

	if (!checker || this->_request.getMethod() == DELETE
		|| this->_request.getMethod() == POST)
	{
		this->_lastCode = this->_code;						//for debugging purposes, very important
		setServerDefaultErrorPages();
		return ;
	}
	else
	{
		if (this->_code >= 400 && this->_code < 500)				//bad request or internal server error
		{
			a = this->_server->second->getErrorPages();
			b = this->_server->second->getErrorPagesEnd();
			while (a != b)
			{
				if ((short)a->first == this->_code)
				{
					this->_location = a->second;			//transferring page-path
					break ;
				}
				a++;
			}
			if (this->_location[0] != '/')
				this->_location.insert(this->_location.begin(), '/');

			this->_lastCode = this->_code;						//for debugging purposes, very important

			this->_code = 302;						//redirect to the 404 page for instance, that's why 302
		/*	a = this->_server->second->getErrorPages();
			b = this->_server->second->getErrorPagesEnd();
			while (a != b)
			{
				if ((short)a->first == this->_code)
					break ;
				a++;
			}
			if (a != b)
				this->_targetFile = this->_server->second->getRoot() + a->second;
			else
			{
				setServerDefaultErrorPages();				//no 302 errorpage available, let's create one
				return ;
			}*/
		}
		if (a != b)
			this->_targetFile = this->_server->second->getRoot() + a->second;
		short old_code = this->_code;
		if (readFile())
		{
			this->_code = old_code;
			this->_responseBody = getStatusErrorPage(this->_code);		//static function, scroll up to see
		}
	}
}

int	Response::buildBody()
{
	if (this->_request.getBody().length() > this->_server->second->getClientMax() * 1024)
	{
		this->_code = 413;						//payload too large, exceeds bandwidth
		return 1;
	}
	if (this->handleTarget())
		return 1;
	if (this->_cgi || this->_autoIndex)					//no need to build body if CGI/autoindex enabled
		return 0;
	if (this->_code)
		return 0;
	if (this->_request.getMethod() == GET || this->_request.getMethod() == HEAD)
	{
		if (readFile())
			return 1;
	}
	else if (this->_request.getMethod() == POST || this->_request.getMethod() == PUT)
	{
		if (fileExists(this->_targetFile) && this->_request.getMethod() == POST)
		{
			this->_code = 204;					//no (further) content to send
			return 0;
		}
		std::ofstream file(this->_targetFile.c_str(), std::ios::binary);
		if (file.fail())						//requested page not found
		{
			this->_code = 404;
			return 1;
		}
		if (this->_request.getMultiformFlag())
        	{
			std::string body = this->_request.getBody();
			body = removeBoundary(body, this->_request.getBoundary());
			file.write(body.c_str(), body.length());
		}
		else
			file.write(this->_request.getBody().c_str(),
			this->_request.getBody().length());
	}
	else if (this->_request.getMethod() == DELETE)
	{
		if (!fileExists(this->_targetFile))
		{
			if (_targetFile[_targetFile.size()-1] != '/')
			{
				this->_code = 404;				//requested page not found
				return 1;
			}
			_targetFile.erase(_targetFile.size()-1, 1);
			if (!fileExists(this->_targetFile))
			{
				this->_code = 404;
				return 1;
			}
		}
		if (remove(this->_targetFile.c_str()) != 0)
		{
			this->_code = 500;					//internal server error
			return 1;
		}
	}
	this->_code = 200;							//ready to transmit
	return 0;
}

int	Response::readFile()
{
	std::ifstream	file(_targetFile.c_str());

	if (file.fail())
	{
		if (this->_targetFile.size() > 0)
		{
			std::string	buffer = this->_targetFile;
			size_t		len = this->_targetFile.size();
			size_t		pos;

			pos = buffer.substr(len-1).find('/');
			if (pos != std::string::npos)
				buffer.erase(len-1, 1);
			_targetFile = checkForIndex(buffer, len);
			std::ifstream	fd; 

			fd.open(_targetFile.c_str());
			if (fd.is_open() == false)
			{
				this->_code = 404;
				return 1;
			}
			fd.close();
			std::ifstream	newfile(_targetFile.c_str());

			std::ostringstream ss;
			ss << newfile.rdbuf();
			this->_responseBody = ss.str();
			return 0;
		}
		else
		{
			this->_code = 404;					//requested page/content not found
			return 1;
		}
	}
	std::ostringstream ss;
	ss << file.rdbuf();					//rdbuf() returns a pointer to the streambuffer
	this->_responseBody = ss.str();
	return 0;
}

void	Response::clear()
{
	this->_targetFile.clear();
	this->_body.clear();
	this->_bodyLen = 0;
	this->_responseContent.clear();
	this->_responseBody.clear();
	this->_location.clear();
	this->_code = 0;
	this->_cgi = 0;
	this->_cgiLen = 0;
	// this->_autoIndex = 0;
}

void	Response::cutResponse(size_t i)
{
	if (this->_responseContent.size() < i)
		i = this->_responseContent.size();
	this->_responseContent = this->_responseContent.substr(i);
}

int	Response::handleCGI(std::string &key)
{
	std::string	path;
	std::string	extension;
	size_t		pos(0);

	std::vector<Location>::iterator v = this->_server->second->getLocations();
	std::vector<Location>::iterator e = this->_server->second->getLocationsEnd();
	while (v != e)
	{
		if (v->getPath() == key)
			break ;
		v++;
	}
	if (v == e)
	{
		this->_code = 501;						//unrecognized or non-supported request method
		return 1;
	}
	path = this->_request.getPath();
	if (path[0] && path[0] == '/')					//remove slash ("/") from front
		path.erase(0, 1);
	if (path == "cgi-bin")
		path += "/" + v->getIndex();				//add slash in between path and index
	else if (path == "cgi-bin/")
		path.append(v->getIndex());				//slash available, no need to add
	pos = path.find(".");
	if (pos == std::string::npos)
	{
		this->_code = 501;						//unrecognized or non-supported request method
		return 1;
	}

	if (this->_request.getMethod() != DELETE)				//extension is irrelevant for deleting			// CRITICAL!
	{
		extension = path.substr(pos);					//extension extraction -> .py or .sh?

		size_t					flag(0);
		std::vector<Location>::iterator		l = v;
		std::vector<std::string>::iterator	ext;
		std::vector<std::string>::iterator	exte;
		ext = l->getCGIext();
		exte = l->getCGIextEnd();
		while (ext != exte)
		{
			if (extension == *ext)
			{
				flag = 1;
				break ;
			}
			ext++;
		}

		if (!flag)
		{
			this->_code = 501;					//unrecognized or non-supported request method
			return 1;
		}
	}

	if (getTypePath(path) != 1)
	{
		this->_code = 404;						//requested page not found
		return 1;
	}
	if (checkFile(path, 1) == -1 || checkFile(path, 3) == -1)
	{
		this->_code = 403;						//illegal access, no privileges
		return 1;
	}
	if (isAllowedMethod(this->_request.getMethod(), v, this->_code))
		return 1;

	this->_cgiHandler.clear();						//CGI-Handler, very important!
	this->_cgiHandler.setCGIpath(path);
	this->_cgi = 1;
	if (pipe(this->_cgiFD) < 0)						//initializing pipe
	{
		this->_code = 500;						//piping failed, internal server error
		return 1;
	}
	this->_cgiHandler.initEnv(this->_request, v);
	this->_cgiHandler.execute(this->_code);
	this->_lastCode = _code;													// CRITICAL!
	return 0;
}

int	Response::handleCGItemp(std::string &key)
{
	std::string	path;
	path = this->_targetFile;

	this->_cgiHandler.clear();						//CGI-Handler, very important!
	this->_cgiHandler.setCGIpath(path);
	this->_cgi = 1;
	if (pipe(this->_cgiFD) < 0)
	{
		this->_code = 500;
		return 1;
	}
	std::vector<Location>::iterator v = this->_server->second->getLocations();
	std::vector<Location>::iterator e = this->_server->second->getLocationsEnd();
	while (v != e)
	{
		if (v->getPath() == key)
			break ;
		v++;
	}
	if (v == e)
	{
		this->_code = 501;						//unrecognized or non-supported request method
		return 1;
	}
	this->_cgiHandler.initEnvCgi(this->_request, v);
	this->_cgiHandler.execute(this->_code);
	return 0;
}

std::string	Response::removeBoundary(std::string &body, std::string &boundary)
{
	std::string	buffer;
	std::string	new_body;
	std::string	filename;

	bool is_boundary = false;
	bool is_content = false;

	if (body.find("--" + boundary)
		!= std::string::npos && body.find("--" + boundary + "--")
		!= std::string::npos)
	{
		for (size_t i = 0; i < body.size(); i++)
		{
			buffer.clear();
			while(body[i] != '\n')
			{
				buffer += body[i];
				i++;
			}
			if (!buffer.compare(("--" + boundary + "--\r")))
			{
				is_content = true;
				is_boundary = false;
			}
			if (!buffer.compare(("--" + boundary + "\r")))
				is_boundary = true;
			if (is_boundary)
			{
				if (!buffer.compare(0, 31, "Content-Disposition: form-data;"))
				{
					size_t start = buffer.find("filename=\"");
					if (start != std::string::npos)
					{
						size_t end = buffer.find("\"", start + 10);
						if (end != std::string::npos)
							filename = buffer.substr(start + 10, end);
					}
				}
				else if (!buffer.compare(0, 1, "\r") && !filename.empty())
				{
					is_boundary = false;
					is_content = true;
				}
			}
			else if (is_content)
			{
				if (!buffer.compare(("--" + boundary + "\r")))
					is_boundary = true;
				else if (!buffer.compare(("--" + boundary + "--\r")))
				{
					new_body.erase(new_body.end() - 1);
					break ;
				}
				else
					new_body += (buffer + "\n");
			}
		}
	}
	body.clear();
	return new_body;
}

void	Response::contentType()
{
	this->_responseContent.append("Content-Type: ");
	if (this->_targetFile.rfind(".", std::string::npos) != std::string::npos && this->_code == 200)
		this->_responseContent.append(this->getFileType(this->_targetFile.substr(this->_targetFile.rfind(".", std::string::npos))));
	else
		this->_responseContent.append(this->getFileType("default"));
	this->_responseContent.append("\r\n");
}

void	Response::contentLen()
{
	std::stringstream ss;
	ss << this->_responseBody.length();
	this->_responseContent.append("Content-Length: ");
	this->_responseContent.append(ss.str());
	this->_responseContent.append("\r\n");
}

void	Response::connection()								//threat of timeout if this fails
{
	if (this->_request.getHeader("connection") == "keep-alive")
		this->_responseContent.append("Connection: keep-alive\r\n");
}

void	Response::server()
{
	this->_responseContent.append("Server: Tester\r\n");				//PLACEHOLDER ("Tester")
}

void	Response::location()
{
	if (this->_location.length())
		this->_responseContent.append("Location: "+ this->_location +"\r\n");
}

void	Response::date()
{
	char date[1000];

	time_t 		now = time(0);
	struct tm	str = *gmtime(&now);
	strftime(date, sizeof(date), "%a, %d %b %Y %H:%M:%S %Z", &str);

	this->_responseContent.append("Date: ");
	this->_responseContent.append(date);
	this->_responseContent.append("\r\n");
}

bool	Response::checkExtensions(std::string path, std::vector<Location>::iterator v)
{
	std::vector<Location>::iterator		l = v;
	std::vector<std::string>::iterator	ext = l->getCGIext();
	std::vector<std::string>::iterator	exte = l->getCGIextEnd();

	while (ext != exte)
	{
		if (path.find(*ext) != std::string::npos)
			return true;
		ext++;
	}
	return false;
}

int	Response::handleTarget()
{
	size_t		len(this->_request.getPath().size());
	std::string	index = checkForIndex(this->_request.getPath(), len);		//making sure the cgi-bin folder is properly accessed
	if (index != this->_request.getPath())
		this->_request.setPath(index);

	std::string	location_key = "";
	getLocationMatch(this->_request.getPath(), this->_server->second->getLocations(),
		this->_server->second->getLocationsEnd(), location_key);

	std::vector<Location>::iterator v = this->_server->second->getLocations();
	std::vector<Location>::iterator e = this->_server->second->getLocationsEnd();
	while (v != e)
	{
		if (location_key == v->getPath())
			break ;
		v++;
	}
	if (v == e)
		return 1;
	if (v->getReturn().empty() == false)
	{
		location_key = v->getReturn();
		v = this->_server->second->getLocations();
		while (v != e)
		{
			if (location_key == v->getPath())
				break ;
			v++;
		}
		if (v == e)
			return 1;
		this->_request.setPath(v->getPath());
	}

	if (location_key.length() > 0)
	{
		if (isAllowedMethod(this->_request.getMethod(), v, this->_code))
		{
			// std::cout << "Method is illegal." << std::endl;
			this->_code = 405;
			return 1;
		}
		if ((this->_request.getBody().length() / 1024) > v->getClientMax())
		{
			this->_code = 413;						//content too large
			return 1;
		}
		if (checkReturn(v, this->_code, this->_location))
			return 1;
		if (checkExtensions(this->_request.getPath(), v)									// CRITICAL! BEWARE!!
			&& v->getPath().find("cgi-bin") != std::string::npos)
			return handleCGI(location_key);
		appendRoot(v, this->_request, this->_targetFile);

		if (v->getCGIext() != v->getCGIextEnd())
		{
			if (this->_targetFile.rfind(v->getCGIext()[0]) != std::string::npos)
				return handleCGItemp(location_key);
		}
		if (isDirectory(this->_targetFile))
		{
			if (this->_targetFile[this->_targetFile.length() - 1] != '/')
			{
				this->_code = 301;						//filepath was moved permanently
				this->_location = this->_request.getPath() + "/";
				return 1;
			}

			if (v->getIndex().empty() == false)
				this->_targetFile += v->getIndex();
			else if (v->getAutoIndex() == false)										// CRITICAL!
				this->_targetFile += this->_server->second->getIndex();

			if (!fileExists(this->_targetFile))
			{
				if (v->getAutoIndex())
				{
					this->_targetFile.erase(_targetFile.find_last_of('/') + 1);
					this->_autoIndex = true;
					return 0;
				}
				else
				{
					this->_code = 403;					//illegal access
					return 1;
				}
			}
			if (isDirectory(this->_targetFile))
			{
				if (v->getAutoIndex() == false)
					this->_code = 301;					//directory was moved permanently
				else
					this->_code = 200;										// CRITICAL!
				if (!v->getIndex().empty())
					this->_location = combinePaths(this->_request.getPath(), v->getIndex(), "");
				else if (v->getAutoIndex() == false)
					this->_location = combinePaths(this->_request.getPath(), _server->second->getIndex(), "");
				else
					this->_location = combinePaths(this->_request.getPath(), "", "");				// CRITICAL!
				if (this->_location[this->_location.length() - 1] != '/')
					this->_location.insert(this->_location.end(), '/');
				return 1;
			}
		}
	}
	else
	{
		this->_targetFile = combinePaths(this->_server->second->getRoot(), this->_request.getPath(), "");
		if (isDirectory(this->_targetFile))
		{
			if (this->_targetFile[this->_targetFile.length() - 1] != '/')
			{
				this->_code = 301;						//directory was moved permanently
				this->_location = this->_request.getPath() + "/";
				return 1;
			}
			this->_targetFile += this->_server->second->getIndex();
			if (!fileExists(this->_targetFile))
			{
				this->_code = 403;						//illegal access
				return 1;
			}
			if (isDirectory(this->_targetFile))
			{
				this->_code = 301;						//directory was moved permanently
				this->_location = combinePaths(this->_request.getPath(), this->_server->second->getIndex(), "");
				if (this->_location[this->_location.length() - 1] != '/')
					this->_location.insert(this->_location.end(), '/');
				return 1;
			}
		}
	}
	return 0;
}

/////////////////////////////////////////////////////////////////////////////// GETTERS

std::string	Response::getResponse()
{
	return this->_responseContent;
}

size_t	Response::getLen() const
{
	return this->_cgiLen;
}

short	Response::getCode() const
{
	return this->_code;
}

int	Response::getCGIstate()
{
	return this->_cgi;
}

std::string	Response::getFileType(std::string extension)
{
	if (this->_fileTypes.count(extension))
		return this->_fileTypes[extension];
	return this->_fileTypes["default"];
}

///////////////////////////////////////////////////////////////////////////////	SETTERS

void	Response::setRequest(Request &request)
{
	this->_request = request;
}

void	Response::setServer(std::map<size_t, Server*>::iterator server)
{
	this->_server = server;
	this->_autoIndex = _server->second->getAutoIndex();					//ADJUSTING AUTO-INDEX, NEEDS TO BE MONITORED FOR BUGS
}

void	Response::setErrorResponse(short errorcode)
{
	this->_responseContent = "";
	this->_code = errorcode;
	this->_responseBody = "";
	this->buildError();
	this->setStatus();
	this->setHeaders();
	this->_responseContent.append(_responseBody);
}

void	Response::setCGIstate(int state)
{
	this->_cgi = state;
}

void	Response::setStatus()
{
	this->_responseContent.append("HTTP/1.1 " + to_string(this->_code) + " ");
	this->_responseContent.append(statusCodeString(this->_code));
	this->_responseContent.append("\r\n");
}

void	Response::setHeaders()
{
	this->contentType();
	this->contentLen();
	this->connection();
	this->server();
	this->location();
	this->date();
	this->_responseContent.append("\r\n");
}

void	Response::setServerDefaultErrorPages()
{
	_responseBody = getErrorStatusPage(this->_code);
}

///////////////////////////////////////////////////////////////////////////////	(DE)CON

Response::~Response()
{
	std::map<std::string, std::string>::iterator a;
	std::map<std::string, std::string>::iterator b;

	a = this->_fileTypes.begin();
	b = this->_fileTypes.end();
	while (a != b)
		a++->second.erase();
	return;
}

Response::Response(Request &request) : _request(request)
{
	this->_lastCode = 0;
	this->_targetFile = "";
	this->_body.clear();
	this->_bodyLen = 0;
	this->_responseBody = "";
	this->_location = "";
	this->_code = 0;
	this->_cgi = 0;
	this->_cgiLen = 0;
	// this->_autoIndex = 0;

	_fileTypes[".html"] = "text/html";
	_fileTypes[".htm"] = "text/html";
	_fileTypes[".css"] = "text/css";
	_fileTypes[".ico"] = "image/x-icon";
	_fileTypes[".avi"] = "video/x-msvideo";
	_fileTypes[".bmp"] = "image/bmp";
	_fileTypes[".doc"] = "application/msword";
	_fileTypes[".gif"] = "image/gif";
	_fileTypes[".gz"] = "application/x-gzip";
	_fileTypes[".ico"] = "image/x-icon";
	_fileTypes[".jpg"] = "image/jpeg";
	_fileTypes[".jpeg"] = "image/jpeg";
	_fileTypes[".png"] = "image/png";
	_fileTypes[".txt"] = "text/plain";
	_fileTypes[".mp3"] = "audio/mp3";
	_fileTypes[".pdf"] = "application/pdf";
	_fileTypes["default"] = "text/html";
}

Response	&Response::operator=(const Response &copy)
{
	this->_lastCode = copy._lastCode;
	this->_server = copy._server;
	this->_targetFile = copy._targetFile;
	this->_body = copy._body;
	this->_bodyLen = copy._bodyLen;
	this->_responseBody = copy._responseBody;
	this->_location = copy._location;
	this->_code = copy._code;

	this->_cgi = copy._cgi;
	this->_cgiFD[0] = copy._cgiFD[0];
	this->_cgiFD[1] = copy._cgiFD[1];
	this->_cgiLen = copy._cgiLen;
	this->_autoIndex = copy._autoIndex;
	this->_fileTypes = copy._fileTypes;

	_fileTypes[".html"] = "text/html";
	_fileTypes[".htm"] = "text/html";
	_fileTypes[".css"] = "text/css";
	_fileTypes[".ico"] = "image/x-icon";
	_fileTypes[".avi"] = "video/x-msvideo";
	_fileTypes[".bmp"] = "image/bmp";
	_fileTypes[".doc"] = "application/msword";
	_fileTypes[".gif"] = "image/gif";
	_fileTypes[".gz"] = "application/x-gzip";
	_fileTypes[".ico"] = "image/x-icon";
	_fileTypes[".jpg"] = "image/jpeg";
	_fileTypes[".jpeg"] = "image/jpeg";
	_fileTypes[".png"] = "image/png";
	_fileTypes[".txt"] = "text/plain";
	_fileTypes[".mp3"] = "audio/mp3";
	_fileTypes[".pdf"] = "application/pdf";
	_fileTypes["default"] = "text/html";

	return *this;
}

Response::Response(const Response &copy)
{
	*this = copy;
}

Response::Response()
{
	this->_lastCode = 0;
	this->_targetFile = "";
	this->_body.clear();
	this->_bodyLen = 0;
	this->_responseBody = "";
	this->_location = "";
	this->_code = 0;
	this->_cgi = 0;
	this->_cgiLen = 0;
	// this->_autoIndex = 0;

	_fileTypes[".html"] = "text/html";
	_fileTypes[".htm"] = "text/html";
	_fileTypes[".css"] = "text/css";
	_fileTypes[".ico"] = "image/x-icon";
	_fileTypes[".avi"] = "video/x-msvideo";
	_fileTypes[".bmp"] = "image/bmp";
	_fileTypes[".doc"] = "application/msword";
	_fileTypes[".gif"] = "image/gif";
	_fileTypes[".gz"] = "application/x-gzip";
	_fileTypes[".ico"] = "image/x-icon";
	_fileTypes[".jpg"] = "image/jpeg";
	_fileTypes[".jpeg"] = "image/jpeg";
	_fileTypes[".png"] = "image/png";
	_fileTypes[".txt"] = "text/plain";
	_fileTypes[".mp3"] = "audio/mp3";
	_fileTypes[".pdf"] = "application/pdf";
	_fileTypes["default"] = "text/html";
}
