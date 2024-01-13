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

int	main(int argc, char **argv)
{
	if (argc != 2 || !argv[1])
		return 1;
	try
	{
		ConfigFile cfg(argv[1]);
		std::map<size_t, Server*>::iterator it = cfg.getServers();	//Test-Client using the first Server-
		Client tester(it);						//instance from within the container
		

		//////////////////<hb1>////////
		int port = 8080; // Example port
		Server myServer(port);
		myServer.initServer();
		myServer.startIOmultiplexing(); // Call this method to start the I/O multiplexing loop
		//////////////////<hb2>////////
	}
	catch (const std::exception &e)
	{
		std::cerr << e.what() << std::endl;
	}
	return 0;
}