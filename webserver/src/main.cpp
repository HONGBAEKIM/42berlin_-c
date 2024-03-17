/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hongbaki <hongbaki@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/15 11:09:41 by agrun             #+#    #+#             */
/*   Updated: 2024/01/13 15:34:15 by hongbaki         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "Client.hpp"
# include "Request.hpp"
# include "Response.hpp"
# include "ConfigFile.hpp"
# include "Location.hpp"
# include "Server.hpp"
# include "ServerManager.hpp"

int	main(int argc, char **argv)
{
	if (argc != 2 || !argv[1])
		return 1;
	try
	{
		ConfigFile cfg(argv[1]);
		std::map<size_t, Server*>::iterator a = cfg.getServers();
		std::map<size_t, Server*>::iterator b = cfg.getServersEnd();

		if (a == b)
		{
			std::cerr << "\x1B[31mServer setup error; quitting program.\x1B[0m" << std::endl;
			return 1;
		}
		ServerManager manager(a, b);
		if (manager.initServer())
			return 1;
		manager.startIOmultiplexing();
	}
	catch (const std::exception &e)
	{
		std::cerr << e.what() << std::endl;
	}
	return 0;
}