/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: agrun <agrun@student.42berlin.de>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/15 11:09:41 by agrun             #+#    #+#             */
/*   Updated: 2023/12/15 11:09:44 by agrun            ###   ########.fr       */
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
	}
	catch (const std::exception &e)
	{
		std::cerr << e.what() << std::endl;
	}
	return 0;
}