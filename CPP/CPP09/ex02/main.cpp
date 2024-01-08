/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hongbaki <hongbaki@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/04 15:54:57 by hongbaki          #+#    #+#             */
/*   Updated: 2024/01/04 18:44:00 by hongbaki         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "PmergeMe.hpp"

int isPositiveNumber(const std::string &s)
{
	if (s.empty())
	{
		return (0);
	}
	for (std::size_t left_idx = 0; left_idx < s.length(); left_idx++)
	{
		if (!std::isdigit(s[left_idx]))
		{
			return (0);
		}
	}
	return (1);
}

int isInputValid(char **argv)
{
	for (int left_idx = 1; argv[left_idx]; left_idx++)
	{
		if (!isPositiveNumber(argv[left_idx]))
		{
			return (0);
		}
	}
	return (1);
}

int main(int argc, char **argv)
{
	if (argc < 2)
	{
		std::cout << "Error: argc < 2" << std::endl;
		return 1;
	}

	if (!isInputValid(argv))
	{
		std::cout << "Error: Invalid input" << std::endl;
		return 1;
	}

	PmergeMe	mergeMe;
	mergeMe.readInput(argv);
	mergeMe.fordJohnsonSortWrapper();

	return (0);
}

