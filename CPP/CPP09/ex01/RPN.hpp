/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RPN.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hongbaki <hongbaki@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/05 14:01:38 by hongbaki          #+#    #+#             */
/*   Updated: 2024/01/03 15:53:11 by hongbaki         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef RPN_HPP
# define RPN_HPP

# include <iostream>
# include <sstream>
//Efficient Insertion and Removal at Both Ends: 
//RPN calculations often involve adding elements to the end of a sequence 
//and removing them from the end as you process the expression. 
//std::deque (double-ended queue) allows efficient insertion 
//and deletion at both the front and the back. 
//This flexibility can be useful in RPN evaluation.
# include <deque>
# include <exception>


typedef int (*yoopointer)(int, int);

void	yoo(std::string exp);
yoopointer getoperations(char c);
int plus(int left, int right);
int subtract(int left, int right);
int divide(int left, int right);
int multiply(int left, int right);


class	RPNException: public std::exception
{
    public:
        virtual const char *what() const throw()
        {
            return ("Error");
        }
};

#endif
