/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Location.hpp          	                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: agrun <agrun@student.42berlin.de>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/16 13:08:49 by agrun             #+#    #+#             */
/*   Updated: 2023/12/16 14:10:52 by agrun            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LOCATION_HPP
# define LOCATION_HPP

# include <iostream>
# include <cstdlib>
# include <sstream>
# include <fstream>
# include <cstring>
# include <string>
# include <vector>
# include <map>

# include <sys/socket.h>
# include <netinet/in.h>
# include <sys/select.h>
# include <arpa/inet.h>

class Location
{
	private:
		std::string				_path;
		std::string				_root;
		unsigned long				_clientMAX;
		bool					_autoIndex;
		std::string				_index;

		std::vector<std::string>		_cgi;
		std::vector<std::string>		_cgi_ext;	// .py/.sh/.php
		std::string				_exec;
		std::string				_upload;
		std::string				_return;
		std::vector<std::string>		_methods;	// GET/POST/DELETE/ETC

	public:
		Location();
		Location(const Location &copy);
		Location	&operator=(const Location &copy);
		~Location();

		std::map<std::string, std::string>		_extPath;

		const std::vector<std::string>			&getCGIref() const;
		const std::vector<std::string>			&getCGIextREF() const;

		const std::map<std::string, std::string>	&getExtPathREF() const;
		std::map<std::string, std::string>::iterator	getExtPathBegin();

		void	setPath(std::string path);
		void	setRoot(std::string root);
		void	setClientMax(unsigned long c_max);
		void	setAutoIndex(bool a_index);
		void	setIndex(std::string index);
		void	setCGI(std::string cgi);
		void	setCGIext(std::string cgi_ext);
		void	setExec(std::string exec);
		void	setUpload(std::string path);
		void	setMethods(std::string method);

		std::string				getPath() const;
		std::string				getRoot() const;
		unsigned long				getClientMax() const;
		bool					getAutoIndex() const;
		std::string				getIndex() const;

		std::vector<std::string>::iterator	getCGI();
		std::vector<std::string>::iterator	getCGIend();
		std::vector<std::string>::iterator	getCGIext();
		std::vector<std::string>::iterator	getCGIextEnd();

		std::string				getExec() const;
		std::string				getUpload() const;
		std::vector<std::string>::iterator	getMethods();
		std::vector<std::string>::iterator	getMethodsEnd();

		void			setReturn(std::string upload);
		std::string		getReturn() const;
};

#endif
