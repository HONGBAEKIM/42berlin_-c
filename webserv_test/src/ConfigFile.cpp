/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigFile.cpp	       		                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: agrun <agrun@student.42berlin.de>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/15 12:23:01 by agrun             #+#    #+#             */
/*   Updated: 2023/12/15 12:23:01 by agrun            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ConfigFile.hpp"

///////////////////////////////////////////////////////////////////////////////	SETTERS

void	ConfigFile::changeNumber(size_t n)
{
	this->_n += n;
	return ;
}

Server	*ConfigFile::createServer()
{
	return new Server();
}

///////////////////////////////////////////////////////////////////////////////	PARSING

bool	ConfigFile::checkLocations(std::map<size_t, Server*>::iterator s)
{
	std::vector<Location>::iterator v;
	std::vector<Location>::iterator e;

	std::string	str;
	size_t		i(0);
	int		flag(0);			//making sure the path isn't filled with whitespaces

	v = s->second->getLocations();
	e = s->second->getLocationsEnd();
	if (v == e)								//empty, no Location(s) found
		return false;
	while (v != e)
	{
		str.clear(); i = 0; flag = 0;
		str = v->getPath();
		while (str[i])
		{
			if (!isspace(str[i]))
				flag = 1;
			i++;
		}
		if (v->getPath().empty() == true				//empty, no Location path defined
			|| v->getPath() == "/" || !flag)
			return false;
		v++;
	}
	return true;
}

bool	ConfigFile::checkErrorPages(std::map<size_t, Server*>::iterator s)
{
	int	e_403(0);							//403 and 404 found?
	int	e_404(0);
	std::ifstream	fd;							//for testing paths

	std::map<int, std::string>::iterator e = s->second->getErrorPages();
	while (e != s->second->getErrorPagesEnd())
	{
		if (e->first == 403)
			e_403 = 1;
		else if (e->first == 404)
			e_404 = 1;
		e++;
	}
	if (!e_403)
		s->second->setErrorPages(403, "/html/error_pages/403.html");
	if (!e_404)
		s->second->setErrorPages(404, "/html/error_pages/404.html");
/*										//NEEDS FURTHER TESTING/FIXING!
	e = s->second->getErrorPages();
	while (e != s->second->getErrorPagesEnd())
	{
		fd.open(e->second.data());
		if (!fd.is_open())						//does the html-file exist/work?
			return false;
		fd.close();
		e++;
	}
*/
	return true;
}

bool	ConfigFile::checkServers()
{
	std::map<size_t, Server*>::iterator s = this->_servers.begin();
	std::map<size_t, Server*>::iterator e = this->_servers.end();

	std::string		buffer_1;
	std::string		buffer_2;
	std::stringstream	cstr;
	struct sockaddr_in	ip_addr;
	char			str[INET_ADDRSTRLEN];

	ip_addr = s->second->getHost();						//extracting Host IP
	inet_ntop(AF_INET, &(ip_addr.sin_addr), str, INET_ADDRSTRLEN);
	cstr << str; cstr >> buffer_1;
	cstr.clear();

	ip_addr = s->second->getServerIP();					//extracting Server IP
	inet_ntop(AF_INET, &(ip_addr.sin_addr), str, INET_ADDRSTRLEN);
	cstr << str; cstr >> buffer_2;

	//&& s->second->getLocations() != s->second->getLocationsEnd())		//leftover/outdated
	//&& (buffer_2.empty() == false && buffer_2 != "0.0.0.0"		//Server IP
	//&& s->second->getListenFD()) < 0					//Listen FD

	while (s != e)
	{
		if (s->second->getPort() > 0
			&& (buffer_1.empty() == false && buffer_1 != "0.0.0.0")
			&& s->second->getServerName().empty() == false
			&& s->second->getRoot().empty() == false
			&& s->second->getClientMax() > 0
			&& s->second->getIndex().empty() == false
			&& this->checkErrorPages(s) == true
			&& this->checkLocations(s) == true)
			s++;
		else
			return false;
	}
	return true;
}

void	ConfigFile::printServers()
{
	std::map<size_t, Server*>::iterator s = this->_servers.begin();
	std::map<size_t, Server*>::iterator e = this->_servers.end();

	std::map<int, std::string>::iterator	m;
	std::vector<Location>::iterator		l;
	std::vector<std::string>::iterator	st;

	struct sockaddr_in	ip_addr;
	char			str[INET_ADDRSTRLEN];

	std::cout << "Number of servers: [" << this->getNumber() << "]" << std::endl;
	while (s != e)
	{
		std::cout << std::endl;
		std::cout << "      Port: " << s->second->getPort() << std::endl;

		ip_addr = s->second->getHost();
		inet_ntop(AF_INET, &(ip_addr.sin_addr), str, INET_ADDRSTRLEN);
		std::cout << "      Host: " << str << std::endl;

		std::cout << "Servername: " << s->second->getServerName() << std::endl;
		std::cout << "      Root: " << s->second->getRoot() << std::endl;
		std::cout << "Client Max: " << s->second->getClientMax() << std::endl;
		std::cout << "     Index: " << s->second->getIndex() << std::endl;

		std::cout << "Auto-Index: ";
		if (s->second->getAutoIndex() == false)
			std::cout << "no" << std::endl;
		else
			std::cout << "yes" << std::endl;
		
		std::cout << "Errorpages:" << std::endl;
		m = s->second->getErrorPages();
		while (m != s->second->getErrorPagesEnd())
		{
			std::cout << "            " << m->first << " " << m->second << std::endl;
			m++;
		}

		std::cout << "Locations:" << std::endl;
		l = s->second->getLocations();
		while (l != s->second->getLocationsEnd())
		{
			std::cout << "           L-Path: " << l->getPath() << std::endl;
			std::cout << "           L-Root: " << l->getRoot() << std::endl;
			std::cout << "     L-Client Max: " << l->getClientMax() << std::endl;
			std::cout << "          L-Index: " << l->getIndex() << std::endl;
			std::cout << "      L-CGI & ext: " << l->getCGI() << " " << l->getCGIext() << std::endl;
			std::cout << "     L-Executable: " << l->getExec() << std::endl;
			std::cout << "  Allowed methods: ";
			st = l->getMethods();
			while (st != l->getMethodsEnd())
			{
				std::cout << *st << " ";
				st++;
			}
			std::cout << std::endl;
			std::cout << std::endl;
			l++;
		}
		ip_addr = s->second->getServerIP();
		inet_ntop(AF_INET, &(ip_addr.sin_addr), str, INET_ADDRSTRLEN);
		std::cout << " Server IP: " << str << std::endl;

		std::cout << " Listen FD: " << s->second->getListenFD() << std::endl;
		std::cout << std::endl;
		s++;
	}
	return ;
}

bool	ConfigFile::checkConfig(std::ifstream *fd)
{
	int		open_scope(0);
	int		close_scope(0);
	int		flag(0);			//disregard commentary within config-file
	size_t		line(1);			//keeping track where server-data is stored
	char		c(0);
	std::string	str;

	while (!fd->eof() && *fd >> std::noskipws >> c)
	{
		if (c == ' ' || c == 9 || c == '\n'
			|| c == '#' || c == ';' || c == '{')			//delimiters
		{
			if (str.empty() == false && str == "server")		//counting server(s)
			{
				this->_n++;
				this->_data.push_back(line);
			}
			str.clear();
		}
		if (c == '#')							//commentary detected
			flag = 1;
		else if (c == '\n')
		{
			flag = 0;
			str.clear();
			line++;
		}
		else if (c == '{' && !flag)
			open_scope++;
		else if (c == '}' && !flag)
			close_scope++;
		if (!isspace(c) && !flag)
			str += c;
	}
	if (open_scope != close_scope || this->_n == 0)			//faulty formatting or no server
		return false;
	return true;
}

void	ConfigFile::parseConfig(std::ifstream *fd)
{
	int		flag(0);					//catching commentary ('#')
	size_t		line_buffer;					//keeping track of linechanges

	char		c(0);
	size_t		j(0);
	size_t		k(0);
	size_t		line(1);
	std::string	str;
									//creating Server class object(s)
	for (size_t i = 0; i < this->_n; i++)
		this->_servers.insert(std::pair<size_t, Server*>(i + 1, this->createServer()));
	std::map<size_t, Server*>::iterator s = _servers.begin();

	for (size_t i = 0; i < this->_n; i++)				//extracting Server information
	{
		str.clear();
		j = this->_data[i];					//lines with "server" in them
		if (i + 1 < this->_n)					//line with next server instance
			k = this->_data[i + 1];
		else
			k = 2147483647;						//placeholder
		while (!fd->eof() && line < j && *fd >> std::noskipws >> c)
			if (c == '\n')
				line++;
		while (!fd->eof() && line < k && *fd >> std::noskipws >> c)
		{
			if (c != 32 && c != 9 && c != '\n' && c != ';'
				&& c != '#' && !flag)
				str += c;
			else if (c == '#')
				flag = line;
			else
			{
				line_buffer = line;
				line = this->parseString(str, s, line, k, fd);	//check & implementation
				if (!line)					//error return is 0 here
				{
					fd->close();
					throw FileFaulty();			//throwing exception
				}
				str.clear();
				if (line_buffer < line)
					flag = 0;
				if (c == '\n')
				{
					flag = 0;
					line++;
				}
			}
		}
		s++;
	}
	return ;
}

size_t	ConfigFile::parseString(const std::string str, std::map<size_t, Server*>::iterator s,
	size_t line, size_t line_end, std::ifstream *fd)
{
	//How to extract the variables "_serverIP" and "_listenFD" from the config?	WIP...

	//inet_pton(AF_INET, addr, &(server.sin_addr));	
	//struct sockaddr_in	serverIP;		//use .data() to turn string into char*

	std::string		cstr;
	std::stringstream	cs;
	char			c(0);

	int			n_flag(0);		//go for end of line
	int			flag(-1);
	int			type(2);		//1) numbers 2) mixed 3) multi-lines 4) errorcodes

	struct sockaddr_in	host;			//placeholders for later conversion
	int			port;
	unsigned long		clients;
	bool			autoindex(true);

	std::vector<Location>::iterator v;		//Location container

	std::string keywords[] = {"listen", "host", "server_name", "root", "autoindex",
		"index", "location", "error_page", "client_max_body_size"};

	if (str.empty() == true)
		return line;
	for (int x = 0; x < 9; x++)
		if (str == keywords[x])
			flag = x;
	if (flag == -1)
		return line;

	switch (flag)
	{
		case -1:				//leave function, nothing to be done here
			return line; break;
		case 0:					//extracting port (numbers only)
			type = 1; break;
		case 6:					//extracting Location(s)
			type = 3; break;
		case 7:
			type = 4; break;		//extracting errorcodes & errorpages
		case 8:
			type = 1; break;		//extracting client_max (numbers only)
	}

	cstr.empty();
	while (type < 3 && !fd->eof() && *fd >> std::noskipws >> c && c != '\n')
	{
		if (c == '#' || c == ';')
			n_flag = 1;
		if (!n_flag && type == 1 && isdigit(c))			//extracting digits
			cstr += c;
		else if (!n_flag && type == 2 && c != 9 && c != 10)	//extracting everything
			cstr += c;
	}

	while (type == 4 && !fd->eof() && *fd >> std::noskipws >> c && !isdigit(c) && c != '\n')
		(void)c;
	if (type == 4)
		cstr += c;
	while (type == 4 && !fd->eof() && *fd >> std::noskipws >> c && c != '\n')
		if (c != ';')
			cstr += c;
	if (c == '\n')
		line++;

	if (type == 3)						//parsing Location(s)
	{
		s->second->embedLocation();
		v = s->second->getLocationsEnd(); v--;
		line = this->parseLocation(v, line, line_end, fd);
	}
	cs.clear();

	switch (flag)
	{
		case 0:						//LISTEN
			cs << cstr; cs >> port;			//difference between listen and port?
			if (!s->second->getPort())
				s->second->setPort(port);
			break;
		case 1:						//HOST
			if (cstr == "localhost")		//converting localhost to 127.0.0.1
				cstr = "127.0.0.1";
			inet_pton(AF_INET, cstr.data(), &(host.sin_addr));
			s->second->setHost(host);
			break;
		case 2:						//SERVER_NAME
			if (s->second->getServerName().empty() == true)
				s->second->setServerName(cstr);
			break;
		case 3:						//ROOT
			if (s->second->getRoot().empty() == true)
				s->second->setRoot(cstr);
			break;
		case 4:						//AUTOINDEX
			if (cstr == "no")
				autoindex = false;
			s->second->setAutoIndex(autoindex);
			break;
		case 5:						//INDEX
			if (s->second->getIndex().empty() == true)
				s->second->setIndex(cstr);
			break;
		case 6:						//LOCATION	//do nothing, already settled
			break;
		case 7:						//ERROR_PAGE	//multiple errorpages can be stored
			if (this->parseErrorPages(s, cstr))
				return 0;					//faulty format detected
			break;
		case 8:						//CLIENT_MAX_BODY_SIZE
			cs << cstr; cs >> clients;
			if (!s->second->getClientMax())
				s->second->setClientMax(clients);
			break;
	}
	return line;
}

size_t	ConfigFile::parseLocation(std::vector<Location>::iterator v, size_t line, size_t line_end,
	std::ifstream *fd)
{
	int			flag(0);				//ignoring commentary (#)
	int			key(-1);				//used for keywords
	size_t			pos(0);					//position within string

	unsigned long		clients(0);
	std::string		cstr;
	std::stringstream	cs;
	char			c(0);

	std::string		str;
	std::string		method;

	std::string keywords[] = {"root", "client_max_body_size", "index",
		"cgi", "cgi_ext", "exec", "upload_path", "allowed_method"};

	while (!fd->eof() && *fd >> std::noskipws >> c && (c == 9 || c == 32))	//skipping whitespaces
		(void)c;							//LATEST DEBUG ADDITION
	if (c != '{')
		str += c;							//will be most likely ('/')

	while (!fd->eof() && *fd >> std::noskipws >> c && c != '}')
	{
		if (c == '#')							//commentary found (#)
			flag = 1;
		else if (c == '\n')
		{
			flag = 0;
			line++;
			break ;
		}
		else if (!flag && c != '{'&& c != 9
			&& c != 10 && c != 32 && c != ';')			//ignore whitespaces in path
			str += c;
		else if (c == 9 || c == 32)					//ignore everything after whitespace
			flag = 1;						//LATEST DEBUG ADDITION
	}
	if (v->getPath().empty() == true)					//defining Location path (/)
		v->setPath(str);
	while (!fd->eof() && line < line_end && c != '}')
	{
		key = -1;
		str.clear();
		while (!fd->eof() && *fd >> std::noskipws >> c && c != 10 && c != '}')
			if (isspace(c) == false)				//skipping to (next) keyword
				break ;
		if (c == '#')							//commentary found (#)
			flag = 1;
		else if (c == '\n')
		{
			flag = 0;
			line++;
		}
		else if (!flag && c != '\n')
			str += c;
		if (c == '}')							//closed scope catcher
			return line;
		while (!fd->eof() && *fd >> std::noskipws >> c && c != '}')
		{
			if (c == 9 || c == 10 || c == 32 || c == ';')		//delimiter(s)
			{
				for (int j = 0; j < 8; j++)
					if (str == keywords[j])
						key = j;
				if (c == '\n')
					line++;
				break ;
			}
			else
				str += c;
		}
		while (!fd->eof() && *fd >> std::noskipws >> c && c != 10 && c != '}')
			if (isspace(c) == false)				//skipping whitespaces
				break ;
		str.clear();
		if (c != 10)							//securing first character
			str += c;

		while (!fd->eof() && *fd >> std::noskipws >> c && c != '}')	//information behind keyword
		{
			if (key != 7 && (c == 9 || c == 32 || c == ';'))	//only one wordstring per line
				flag = 1;										//...except for allowed methods
			else if (c != '\n' && c != '#' && !flag)
				str += c;
			else
			{
				if (c == '#')
					flag = 1;
				else if (c == '\n')
				{
					flag = 0;
					line++;
					break ;
				}
			}
		}
		cs.clear();
		if (key > -1)
		{
			switch (key)
			{
				case 0:					//ROOT
					if (v->getRoot().empty() == true)
						v->setRoot(str);
					break;
				case 1:					//CLIENT_MAX_BODY_SIZE
					cs << cstr; cs >> clients;
					if (!v->getClientMax())
						v->setClientMax(clients);
					break ;
				case 2:					//INDEX
					if (v->getIndex().empty() == true)
						v->setIndex(str);
					break ;
				case 3:					//CGI
					if (v->getCGI().empty() == true)
						v->setCGI(str);
					break ;
				case 4:					//CGI_EXT
					if (v->getCGIext().empty() == true)
						v->setCGIext(str);
					break ;
				case 5:					//EXEC
					if (v->getExec().empty() == true)
						v->setExec(str);
					break ;
				case 6:					//UPLOAD_PATH
					if (v->getUpload().empty() == true)
						v->setUpload(str);
					break ;
				case 7:					//ALLOWED_METHOD
					if (str.empty() == false)
					{
						pos = 0;
						while (pos < str.size())
						{
							if (str[pos] != 9 && str[pos] != 32
								&& str[pos] != ';')
								method += str[pos];
							else
							{
								if (method.empty() == false)
									v->setMethods(method);
								method.clear();
							}
							pos++;
						}
					}
					break;
			}
		}
	}
	return line;
}

size_t	ConfigFile::parseErrorPages(std::map<size_t, Server*>::iterator s, std::string str)
{
	std::stringstream	cs;		//used for type-conversion
	std::string		cstr;		//extracting stringcontent
	std::string		path;		//errorpage path

	size_t			h;
	size_t			i(0);
	size_t			j(0);
	size_t			n(0);		//number of gathered HTML-errorcodes

	int				x;
	std::vector<int>		err;	//keeping track of errorcodes
	std::vector<int>::iterator	v;	//there can be multiple in a line
						//e.g.: 503 504 /example50x.html
	if (str.empty() == true)
		return 1;
	h = str.size();
	while (str[i] && i < h)
	{
		j = 0;
		cs.clear();
		cstr.clear();
		while (str[i] && (str[i] == 9 || str[i] == 32))		//CAREFUL!
			i++;
		while (str[i] && str[i] != 9 && str[i] != 32		//stop at whitespaces
			&& str[i] != 10	&& str[i] != ';')
		{
			cstr += str[i];
			i++;
		}
		if (!n && cstr[0] == '/')				//is there a path
			return 1;					//with no number?
		else if (cstr[0] != '/')
		{
			while (cstr[j])
			{
				if (cstr[j] < '0' || cstr[j] > '9')	//is it an errorcode?
					return 1;			//format error detected
				j++;
			}
			n++;
			cs << cstr; cs >> x;				//converting string into int
			err.push_back(x);
		}
		else if (n && cstr[0] == '/')
			path = cstr;
	}
	v = err.begin();
	for (size_t b(0); b < err.size(); b++)				//in case of multiple codes
	{
		s->second->setErrorPages(*v, path);
		v++;
	}
	return 0;
}

///////////////////////////////////////////////////////////////////////////////	GETTERS

size_t	ConfigFile::getNumber() const
{
	return this->_n;
}

std::map<size_t, Server*>::iterator	ConfigFile::getServers()
{
	return this->_servers.begin();
}

std::map<size_t, Server*>::iterator	ConfigFile::getServersEnd()
{
	return this->_servers.end();
}

///////////////////////////////////////////////////////////////////////////////	(DE)CON

ConfigFile::~ConfigFile()
{
	std::map<size_t, Server*>::iterator s;
	s = this->_servers.begin();

	for (size_t i = 0; i < this->_n; i++)
	{
		if (s != this->_servers.end())
			delete s++->second;
	}
	return ;
}

ConfigFile	&ConfigFile::operator=(const ConfigFile &copy)
{
	this->_servers = copy._servers;
	this->_n = copy._n;
	this->_data = copy._data;

	return *this;
}

ConfigFile::ConfigFile(const ConfigFile &copy)
{
	*this = copy;
}

ConfigFile::ConfigFile(const char *file)
{
	this->_n = 0;
	std::ifstream	fd;

	fd.open(file);
	if (!fd.is_open())
		throw FileNotFound();
	if (this->checkConfig(&fd) == false)
	{
		fd.close();
		throw FileFaulty();
	}
	fd.close();					//resetting FD after reading it once
	fd.open(file);
	this->parseConfig(&fd);				//can throw an exception which will close fd
	if (fd.is_open() == true)
		fd.close();
	if (this->checkServers() == false)		//making sure the server-info is complete
	{
		std::map<size_t, Server*>::iterator s = this->getServers();
		std::map<size_t, Server*>::iterator e = this->getServersEnd();
		while (s != e)
			delete s++->second;
		throw FileFaulty();
	}
	this->printServers();
	return ;
}

/////////////////////////////////////////////////////////////////////////////// EXCEPTIONS

const char	*ConfigFile::FileNotFound::what() const throw()
{
	return ("Config-file could not be opened! Shutting down.");
};

const char	*ConfigFile::FileFaulty::what() const throw()
{
	return ("Config-file contains faulty or incomplete input! Shutting down.");
};

const char	*ConfigFile::FaultyPath::what() const throw()
{
	return ("Config-file contains faulty or nonexisting paths! Shutting down.");
}
