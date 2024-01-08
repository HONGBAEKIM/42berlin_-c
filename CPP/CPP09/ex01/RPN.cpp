/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RPN.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hongbaki <hongbaki@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/05 14:01:38 by hongbaki          #+#    #+#             */
/*   Updated: 2024/01/03 16:34:08 by hongbaki         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "RPN.hpp"

void	yoo(std::string argv1)
{
    std::deque<int> storage;
    std::stringstream ss;
    int nb;
    yoopointer operation = 0;

    for (size_t i = 0; i < argv1.length(); i++)
    {
        if (std::isdigit(argv1[i]))
        {
            ss << argv1[i];
            ss >> nb;
            ss.clear();
            storage.push_front(nb);
            //std::cout << "storage.front(): " << storage.front() << std::endl;
        }
        else if (!std::isspace(argv1[i]))
        {
            operation = getoperations(argv1[i]);
            if (operation)
            {
                //at least 2 number exist
                if (storage.size() < 2)
					throw (RPNException());
                    
				int right = storage.front();
                //Removeing the front element
                storage.pop_front();
                
				int	left = storage.front();
                //Removeing the front element
				storage.pop_front();
				
                //performs the actual calculation
                //operation is a pointer to one of the functions(plus, subtract, divide, multiply)
                int result = operation(left, right);
                storage.push_front(result);
            }
            else
                throw (RPNException());
        }
    }
    //to catch >= 10 
    if (storage.size() != 1)
	{
    	throw (RPNException());
    }
    std::cout << storage.front() << std::endl;
}

yoopointer getoperations(char c)
{
    char symbols[] = {'+', '-', '/', '*'};
    yoopointer operations[] = {&plus, &subtract, &divide, &multiply};

    for (size_t i = 0; i < 4; i++)
    {
        if (c == symbols[i])
            return (operations[i]);
    }
    return (0);
}

int plus(int left, int right)
{
    return left + right;
}

int subtract(int left, int right)
{
    return left - right;
}

int divide(int left, int right)
{
    if (right == 0)
        throw (RPNException());
    return left / right;
}

int multiply(int left, int right)
{
    return left * right;
}

