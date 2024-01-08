/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hongbaki <hongbaki@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/05 14:01:45 by hongbaki          #+#    #+#             */
/*   Updated: 2024/01/03 16:35:14 by hongbaki         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "RPN.hpp"

int main(int ac, char *av[])
{
	if (ac != 2)
	{
		std::cout << "argc != 2" << std::endl;
		return 1;
	}
	try
	{
		yoo(av[1]);
	}
	catch (RPNException & e)
	{
		std::cout << e.what() << std::endl;
	}	
	return 0;
}

