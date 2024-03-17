/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CGIhandler.cpp	       		                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: agrun <agrun@student.42berlin.de>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/01/14 11:23:01 by agrun             #+#    #+#             */
/*   Updated: 2023/01/14 12:23:32 by agrun            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "CGIhandler.hpp"

/////////////////////////////////////////////////////////////////////////////// STATIC

template <typename T>
std::string to_string(const T number)				//transforming any number-type into a string
{
	std::stringstream ss;
	ss << number;
	return ss.str();
}

static unsigned int	hexConverter(const std::string &n)
{
	unsigned int		x;
	std::stringstream	ss;

	ss << n;
	ss >> std::hex >> x;
	return x;
}
/*
static void	writeLog(int mode, const char *msg, ...)	//mode 0: file output || mode 1: console output			// FOR DEBUGGING PURPOSES
{
	char	output[8192];
	va_list	args;
	int	n;

	va_start(args, msg);					//this is a variadic function, we need to use va_list
	n = vsnprintf(output, 8192, msg, args);			//vsnprintf() converts a string as if printf was used
	std::string	date;
	char		buffer[100];				//creating a timestamp-string for logging purposes

	tzset();						//this will get the appropriate timezone
	time_t		now = time(0);
	struct tm	tm = *gmtime(&now);
	tm.tm_hour = tm.tm_hour + 1;				//please test if this is the correct time-offset
	strftime(buffer, sizeof(buffer),			//this function reformats the time & date for us
		"[%Y-%m-%d  %H:%M:%S]   ", &tm);
	date = std::string(buffer);

	if (mode == 0)
	{
		if (mkdir("logs", 0777) < 0 && errno != EEXIST)		//create subfolder & does the logfile exist?
		{
			std::cerr << "mkdir() ERROR: " << strerror(errno) << std::endl;		//print error-feedback
			return ;
		}
		int FD = open("serverlog.txt", O_CREAT | O_APPEND | O_WRONLY, S_IRUSR | S_IWUSR);

		//std::cout << "fd is " << FD << " and errno is:" << strerror(errno) << std::endl;	//debugger
		write(FD, date.c_str(), date.length());
		write(FD, "   ", 3);
		write(FD, output, n);
		write(FD, "\n", 1);
		close(FD);
	}
	else if (mode == 1)
		std::cout << date << "   " << output << std::endl;
        va_end(args);
}
*/
///////////////////////////////////////////////////////////////////////////////	EXTRA

void 	CGIhandler::execute(short &errorcode)
{
	if (this->_argv[0] == NULL || this->_argv[1] == NULL)
	{
		errorcode = 500;					//execve requires two arguments
		return ;
	}
	if (pipe(this->_pipeIN) < 0)				//initializing pipe with two FDs ([0] + [1])
	{
		//writeLog(1, "pipe() failed");				//debugging
		errorcode = 500;
		return ;
	}
	if (pipe(this->_pipeOUT) < 0)				//initializing pipe with two FDs ([0] + [1])
	{
		//writeLog(1, "pipe() failed");				//debugging
		close(this->_pipeIN[0]);
		close(this->_pipeIN[1]);
		errorcode = 500;
		return ;
	}
	this->_cgiPID = fork();						//forking for pipe-support
	if (this->_cgiPID == 0)
	{
		dup2(this->_pipeIN[0], STDIN_FILENO);
		dup2(this->_pipeOUT[1], STDOUT_FILENO);
		close(this->_pipeIN[0]);
		close(this->_pipeIN[1]);
		close(this->_pipeOUT[0]);
		close(this->_pipeOUT[1]);
		this->_exit = execve(this->_argv[0], this->_argv, this->_chENV);	//execve()
		exit(this->_exit);
	}
	else if (this->_cgiPID <= 0)
	{
		std::cerr << "fork() failed" << std::endl;
		errorcode = 500;
	}
	if (!errorcode)											// CRITICAL!
		errorcode = 200;
}

void	CGIhandler::initEnvCgi(Request &request, std::vector<Location>::iterator loc)
{
	std::string	cgi_exec = ("cgi-bin/" + loc->getCGI()[0]).c_str();
	char		*cwd = getcwd(NULL, 0);					//current workdirectory

	if (this->_cgiPath[0] != '/')						//checking for slash ("/")
	{
		std::string	temp(cwd);
		temp.append("/");						//add slash to the front
		if (this->_cgiPath.length() > 0)
			this->_cgiPath.insert(0, temp);
	}
	if (request.getMethod() == POST)
	{
		std::stringstream	out;
		out << request.getBody().length();

		this->_env["CONTENT_LENGTH"] = out.str();			//convert "out" into string
		this->_env["CONTENT_TYPE"] = request.getHeader("content-type");
	}

	this->_env["GATEWAY_INTERFACE"] = std::string("CGI/1.1");
	this->_env["SCRIPT_NAME"] = cgi_exec;
	this->_env["SCRIPT_FILENAME"] = this->_cgiPath;
	this->_env["PATH_INFO"] = this->_cgiPath;
	this->_env["PATH_TRANSLATED"] = this->_cgiPath;
	this->_env["REQUEST_URI"] = this->_cgiPath;
	this->_env["SERVER_NAME"] = request.getHeader("host");

	///////////////////////////////////////////////////////////////////////////////////////////////////////	NEW
	int pos = findStart(this->_cgiPath, "cgi-bin/");
	this->_env["SERVER_PORT"]
		= (pos > 0 ? request.getHeader("host").substr(pos + 1, request.getHeader("host").size()) : "");
	///////////////////////////////////////////////////////////////////////////////////////////////////////	DEPRECATED

	//this->_env["SERVER_PORT"] = "8002";			//"keepalive" uses 8002, check source:
								//https://www.speedguide.net/port.php?port=8002
	///////////////////////////////////////////////////////////////////////////////////////////////////////

	this->_env["REQUEST_METHOD"] = request.getMethodStr();
	this->_env["SERVER_PROTOCOL"] = "HTTP/1.1";
	this->_env["REDIRECT_STATUS"] = "200";				//HTML code 200: "OK"
	this->_env["SERVER_SOFTWARE"] = "Tester";			//see [void Response::server()]

	std::map<std::string, std::string> headers = request.getHeaders();
	for (std::map<std::string, std::string>::iterator it = headers.begin();
		it != headers.end(); ++it)
	{
		std::string	name = it->first;
		std::transform(name.begin(), name.end(), name.begin(), ::toupper);	//transform() applies "toupper"
		std::string	key = "HTTP_" + name;
		this->_env[key] = it->second;				//adding headers to CGIhandler ENV-container
	}

	///////////////////////////////////////////////////////////////////////////

	this->_chENV = (char **)calloc(sizeof(char *), this->_env.size() + 1);		//calloc
	std::map<std::string, std::string>::const_iterator it = this->_env.begin();
	for (int i = 0; it != this->_env.end(); it++, i++)
	{
		std::string tmp = it->first + "=" + it->second;
		this->_chENV[i] = strdup(tmp.c_str());
	}
	this->_argv = (char **)malloc(sizeof(char *) * 3);				//malloc
	this->_argv[0] = strdup(cgi_exec.c_str());
	this->_argv[1] = strdup(this->_cgiPath.c_str());
	this->_argv[2] = NULL;

	free(cwd);									//clearing out memory
}

/*	The main problem in this function is that we haven't attempted to create
	configs with MULTIPLE cgi-extensions in a SINGLE location yet, as such
	we haven't implemented a SUPPORT for this sort of thing. 

	This function will require another look if we decide to implement such a
	feature! See string "ext_path" for that.					*/

void 	CGIhandler::initEnv(Request &request, std::vector<Location>::iterator loc)
{
	int		flag(0);
	int		pos;
	std::string	ext_path;
	std::string	extension;

	extension = this->_cgiPath.substr(this->getCGIpath().find("."));

	std::map<std::string, std::string>::iterator	it_path = loc->_extPath.find(extension);
	std::string					filename = "";
	if (request.getMethod() == DELETE)								// CRITICAL!
	{
		flag = 1;
		size_t	x = this->_cgiPath.size()-1;
		while (this->_cgiPath[x] && this->_cgiPath[x] != '/')
			x--;
		for (size_t y = x+1; y < _cgiPath.size(); ++y)
			filename += this->_cgiPath[y];
		it_path = loc->_extPath.find(".py");							// Need to use delete.py for DELETE!
	}

	if (it_path == loc->_extPath.end())
		return ;
	ext_path = loc->_extPath[extension];

	this->_env["AUTH_TYPE"] = "Basic";
	this->_env["CONTENT_LENGTH"] = request.getHeader("content-length");
	this->_env["CONTENT_TYPE"] = request.getHeader("content-type");
	this->_env["GATEWAY_INTERFACE"] = "CGI/1.1";
	pos = findStart(this->_cgiPath, "cgi-bin/");

	if (flag)											// CRTICIAL!
		this->_env["SCRIPT_NAME"] = "cgi-bin/delete.py";
	else
		this->_env["SCRIPT_NAME"] = this->_cgiPath;

	if (flag)											// CRTICIAL!
		this->_env["SCRIPT_FILENAME"] = "delete.py";
	else
	{
		this->_env["SCRIPT_FILENAME"] = ((pos < 0
			|| (size_t)(pos + 8) > this->_cgiPath.size()) ? ""
			: this->_cgiPath.substr(pos + 8, this->_cgiPath.size()));
	}

	this->_env["PATH_INFO"] = getPathInfo(request.getPath(), *loc->getCGIext());
	this->_env["PATH_TRANSLATED"] = loc->getRoot()
		+ (this->_env["PATH_INFO"] == "" ? "/" : this->_env["PATH_INFO"]);

	if (flag)											// CRITICAL!
		this->_env["QUERY_STRING"] = "filename=" + filename;
	else
		this->_env["QUERY_STRING"] = decode(request.getQuery());

	this->_env["REMOTE_ADDR"] = request.getHeader("host");
	pos = findStart(request.getHeader("host"), ":");
	this->_env["SERVER_NAME"] = (pos > 0 ? request.getHeader("host").substr(0, pos) : "");
	this->_env["SERVER_PORT"]
		= (pos > 0 ? request.getHeader("host").substr(pos + 1, request.getHeader("host").size()) : "");

	this->_env["HTTP_COOKIE"] = request.getHeader("cookie");
	this->_env["DOCUMENT_ROOT"] = loc->getRoot();

	if (flag)											// CRITICAL!
		this->_env["REQUEST_URI"] = "/cgi-bin/delete.pyfilename=" + filename;
	else
		this->_env["REQUEST_URI"] = request.getPath() + request.getQuery();

	if (flag)											// CRITICAL!
		this->_env["REQUEST_METHOD"] = "GET";
	else
		this->_env["REQUEST_METHOD"] = request.getMethodStr();

	this->_env["SERVER_PROTOCOL"] = "HTTP/1.1";
	this->_env["REDIRECT_STATUS"] = "200";				//HTML code 200: "OK"
	this->_env["SERVER_SOFTWARE"] = "Tester";			//see [void Response::server()]

	///////////////////////////////////////////////////////////////////////////

	this->_chENV = (char **)calloc(sizeof(char *), this->_env.size() + 1);		//calloc
	std::map<std::string, std::string>::const_iterator it = this->_env.begin();
	for (int i = 0; it != this->_env.end(); it++, i++)
	{
		std::string tmp = it->first + "=" + it->second;
		this->_chENV[i] = strdup(tmp.c_str());
	}
	this->_argv = (char **)malloc(sizeof(char *) * 3);
	if (flag)											// CRITICAL!
	{
		this->_argv[0] = strdup("/usr/bin/python3");
		this->_argv[1] = strdup("cgi-bin/delete.py");
	}
	else
	{
		this->_argv[0] = strdup(ext_path.c_str());
		this->_argv[1] = strdup(this->_cgiPath.c_str());
	}
	this->_argv[2] = NULL;
}

int	CGIhandler::findStart(const std::string path, const std::string delimiter)
{
	if (path.empty())
		return -1;

	size_t pos = path.find(delimiter);
	if (pos != std::string::npos)		//making sure it's not the end of string
		return pos;
	return -1;				//could not find the delimiter in path
}

std::string	CGIhandler::decode(std::string &path)
{
	size_t	t = path.find("%");
	if (t == std::string::npos)				//no conversion symbol ("%") found
		return path;

	while (t != std::string::npos)
	{
		if (path.length() < t + 2)
			break ;
		char d = hexConverter(path.substr(t + 1, 2));	//converting from hex to dec
		path.replace(t, 3, to_string(d));		//using std-function "to_string"
		t = path.find("%");				//modifying path-content
	}
	return path;
}

void	CGIhandler::clear()
{
	this->_cgiPath = "";
	this->_exit = 0;
	this->_cgiPID = -1;

	std::map<std::string, std::string>::iterator a;
	std::map<std::string, std::string>::iterator b;

	a = this->_env.begin();
	b = this->_env.end();

	while (a != b)							//clearing out container
	{
		a->second.clear();
		a++;
	}

	if (this->_chENV)						//clearing out env-attribute
	{
		for (int i = 0; this->_chENV[i]; i++)
			free(this->_chENV[i]);
		free(this->_chENV);
	}
	if (this->_argv)						//clearing out argv-attribute
	{
		for (int i = 0; this->_argv[i]; i++)
			free(this->_argv[i]);
		free(this->_argv);
	}
	this->_chENV = NULL;
	this->_argv = NULL;
}

///////////////////////////////////////////////////////////////////////////////	SETTERS

void	CGIhandler::setCGIpid(pid_t pid)
{
	this->_cgiPID = pid;
}

void	CGIhandler::setCGIpath(const std::string &path)
{
	this->_cgiPath = path;
}

///////////////////////////////////////////////////////////////////////////////	GETTERS

const std::map<std::string, std::string>	&CGIhandler::getENV() const
{
	return this->_env;
}

const pid_t	&CGIhandler::getCGIpid() const
{
	return this->_cgiPID;
}

const std::string	&CGIhandler::getCGIpath() const
{
	return this->_cgiPath;
}

std::string	CGIhandler::getPathInfo(std::string &path, std::string extension)
{
	std::string	tmp;
	size_t		start;
	size_t		end;

	start = path.find(extension);
	if (start == std::string::npos)
		return "";
	else if (start + 3 >= path.size())
		return "";
	tmp = path.substr(start + 3, path.size());
	if (!tmp[0] || tmp[0] != '/')
		return "";
	else
		end = tmp.find("?");
	return end == std::string::npos ? tmp : tmp.substr(0, end);
}

///////////////////////////////////////////////////////////////////////////////	(DE)CON

CGIhandler::~CGIhandler()
{
	std::map<std::string, std::string>::iterator a;
	std::map<std::string, std::string>::iterator b;

	a = this->_env.begin();
	b = this->_env.end();

	while (a != b)							//clearing out container
	{
		a->second.clear();
		a++;
	}
	this->_env.clear();

	if (this->_chENV)						//clearing out env-attribute
	{
		for (int i = 0; this->_chENV[i]; i++)
			free(this->_chENV[i]);
		free(this->_chENV);
	}
	if (this->_argv)						//clearing out argv-attribute
	{
		for (int i = 0; this->_argv[i]; i++)
			free(this->_argv[i]);
		free(this->_argv);
	}
	return ;
}

CGIhandler	&CGIhandler::operator=(const CGIhandler &copy)
{
	this->_cgiPath = copy._cgiPath;
	this->_exit = copy._exit;
	this->_cgiPID = copy._cgiPID;
	this->_argv = copy._argv;
	this->_chENV = copy._chENV;
	return *this;
}

CGIhandler::CGIhandler(const CGIhandler &copy)
{
	*this = copy;
}

CGIhandler::CGIhandler(std::string path)
{
	this->_cgiPath = path;
	this->_exit = 0;
	this->_cgiPID = -1;
	this->_argv = NULL;
	this->_chENV = NULL;

	this->_pipeIN[0] = -1;
	this->_pipeIN[1] = -1;
	this->_pipeOUT[0] = -1;
	this->_pipeOUT[1] = -1;
}

CGIhandler::CGIhandler()
{
	this->_cgiPath = "";
	this->_exit = 0;
	this->_cgiPID = -1;
	this->_argv = NULL;
	this->_chENV = NULL;

	this->_pipeIN[0] = -1;
	this->_pipeIN[1] = -1;
	this->_pipeOUT[0] = -1;
	this->_pipeOUT[1] = -1;
}