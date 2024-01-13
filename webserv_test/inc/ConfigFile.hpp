/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigFile.hpp          	                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: agrun <agrun@student.42berlin.de>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/15 13:08:49 by agrun             #+#    #+#             */
/*   Updated: 2023/12/15 14:10:52 by agrun            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONFIGFILE_HPP
# define CONFIGFILE_HPP

# include <iostream>
# include <cstdlib>
# include <cctype>
# include <sstream>
# include <fstream>
# include <cstring>
# include <string>
# include <map>
# include <vector>

# include "Server.hpp"
# include "Location.hpp"

class Server;
class Location;

class ConfigFile
{
	private:
		std::map<size_t, Server*>		_servers;
		size_t					_n;
		std::vector<size_t>			_data;		//server instances within config

		ConfigFile();						//no need

	public:
		ConfigFile(const char *file);
		ConfigFile(const ConfigFile &copy);
		ConfigFile	&operator=(const ConfigFile &copy);
		~ConfigFile();

		void		printServers();
		bool		checkServers();
		bool		checkConfig(std::ifstream *fd);

		void		parseConfig(std::ifstream *fd);
		size_t		parseString(const std::string str, std::map<size_t, Server*>::iterator s,
					size_t line, size_t line_end, std::ifstream *fd);
		size_t		parseLocation(std::vector<Location>::iterator v, size_t line,
				size_t line_end, std::ifstream *fd);
		size_t		parseErrorPages(std::map<size_t, Server*>::iterator s, std::string str);
		bool		checkErrorPages(std::map<size_t, Server*>::iterator s);
		bool		checkLocations(std::map<size_t, Server*>::iterator s);

		void		changeNumber(size_t n);
		size_t		getNumber() const;

		Server					*createServer();
		std::map<size_t, Server*>::iterator	getServers();
		std::map<size_t, Server*>::iterator	getServersEnd();

		/////////////////////////////////////////////////////////////////////

		class FileNotFound : public std::exception
		{
			public:
				virtual const char	*what() const throw();
		};

		class FileFaulty : public std::exception
		{
			public:
				virtual const char	*what() const throw();
		};

		class FaultyPath : public std::exception
		{
			public:
				virtual const char	*what() const throw();
		};
};

#endif
