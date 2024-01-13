/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Location.cpp	       		                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: agrun <agrun@student.42berlin.de>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/15 12:23:01 by agrun             #+#    #+#             */
/*   Updated: 2023/12/15 12:23:01 by agrun            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Location.hpp"

///////////////////////////////////////////////////////////////////////////////	SETTERS

void	Location::setPath(std::string path)
{
	if (path.empty() == false)
		this->_path = path;
}

void	Location::setRoot(std::string root)
{
	if (root.empty() == false)
		this->_root = root;
}

void	Location::setClientMax(unsigned long c_max)
{
	if (c_max > 0)
		this->_clientMAX = c_max;
}

void	Location::setAutoIndex(bool a_index)
{
	this->_autoIndex = a_index;
}

void	Location::setIndex(std::string index)
{
	if (index.empty() == false)
		this->_index = index;
}

void	Location::setCGI(std::string cgi)
{
	if (cgi.empty() == false)
		this->_cgi = cgi;
}

void	Location::setCGIext(std::string cgi_ext)
{
	if (cgi_ext.empty() == false)
		this->_cgi_ext = cgi_ext;
}

void	Location::setExec(std::string exec)
{
	if (exec.empty() == false)
		this->_exec = exec;
}

void	Location::setUpload(std::string path)
{
	this->_upload = path;
}

void	Location::setReturn(std::string ret)					//REDUNDANT?
{
	if (ret.empty() == false)
		this->_return = ret;
}

void	Location::setMethods(std::string method)
{
	if (method.empty() == false)
		this->_methods.push_back(method);
}

///////////////////////////////////////////////////////////////////////////////	GETTERS

std::string	Location::getPath() const
{
	return this->_path;
}

std::string	Location::getRoot() const
{
	return this->_root;
}

unsigned long	Location::getClientMax() const
{
	return this->_clientMAX;
}

bool	Location::getAutoIndex() const
{
	return this->_autoIndex;
}

std::string	Location::getIndex() const
{
	return this->_index;
}

std::string	Location::getCGI() const
{
	return this->_cgi;
}

std::string	Location::getCGIext() const
{
	return this->_cgi_ext;
}

std::string	Location::getExec() const
{
	return this->_exec;
}

std::string	Location::getUpload() const
{
	return this->_upload;
}

std::string	Location::getReturn() const					//REDUNDANT?
{
	return this->_return;
}

std::vector<std::string>::iterator	Location::getMethods()
{
	return this->_methods.begin();
}

std::vector<std::string>::iterator	Location::getMethodsEnd()
{
	return this->_methods.end();
}

///////////////////////////////////////////////////////////////////////////////	(DE)CON

Location::~Location()				//only containers require clearing out
{
	size_t	n(0);

	std::vector<std::string>::iterator v;
	if (this->_methods.empty() == false)
		n = this->_methods.size();

	v = this->_methods.begin();
	for (size_t i = 0; i < n; i++)
	{
		if (v != this->_methods.end())
			this->_methods.erase(v++);
	}
	return ;
}

Location	&Location::operator=(const Location &copy)
{	
	this->_path = copy._path;
	this->_root = copy._root;
	this->_clientMAX = copy._clientMAX;
	this->_autoIndex = copy._autoIndex;
	this->_index = copy._index;
	this->_cgi = copy._cgi;
	this->_cgi_ext = copy._cgi_ext;
	this->_exec = copy._exec;
	this->_upload = copy._upload;
	this->_return = copy._return;
	this->_methods = copy._methods;

	return *this;
}

Location::Location(const Location &copy)
{
	*this = copy;
}

Location::Location()
{
	this->_path = "";
	this->_root = "";
	this->_clientMAX = 0;
	this->_autoIndex = true;
	this->_index = "";
	this->_cgi = "";
	this->_cgi_ext = "";
	this->_exec = "";
	this->_upload = "";
	this->_return = "";							//REDUNDANT?
	return ;
}