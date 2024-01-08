/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   PmergeMe.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hongbaki <hongbaki@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/05 14:02:25 by hongbaki          #+#    #+#             */
/*   Updated: 2024/01/04 18:18:38 by hongbaki         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PMERGE_ME_HPP
#define PMERGE_ME_HPP

#include <iostream>
#include <sstream>
#include <cstdlib>
#include <cctype>

//dynamic array
//Provides fast random access
//It allocates memory dynamically
#include <vector>

//fixed-size arrays
//Provides fast random access(not as fast as std::vector due to its more complex internal structure.)
//Grow in both directions and doesn't usually need to move existing elements when it grows.
#include <deque>

//<Summary>
//Use std::vector for efficient end operations and when contiguous memory is beneficial. 
//Use std::deque when you need efficient operations at both the beginning and the end of the container.

#include <algorithm>

class PmergeMe
{
	public:
		PmergeMe();
		~PmergeMe();
		PmergeMe(const PmergeMe &other);
		PmergeMe &operator=(const PmergeMe &other);
			
		std::vector<int> intVector;
		std::deque<int> intDeque;
		
		void intVectorPrinter();
		void intDequePrinter();
		void readInput(char **argv);

		
		template<typename T>
		void fordJohnsonSort(T &lst, int lst_start_idx, int lst_end_idx);
		
		void fordJohnsonSortWrapper();
		
		template<typename T>
		void mergeSublists(T &lst, int lst_start_idx, int mid, int lst_end_idx);
};

#endif
