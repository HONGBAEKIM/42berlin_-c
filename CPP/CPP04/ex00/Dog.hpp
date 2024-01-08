/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Dog.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hongbaki <hongbaki@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/09 13:28:13 by hongbaki          #+#    #+#             */
/*   Updated: 2023/11/14 10:24:06 by hongbaki         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef DOG_HPP
# define DOG_HPP

#include "Animal.hpp"
 
class Dog : public Animal
{
    //when you use protected instead of private at main class Animal
    //derived class(cat and dog) do not need to declear private member variable
    private:
        
    public:
        Dog(void);
        Dog(const Dog &_Dog);
        Dog& operator=(const Dog &_Dog);
        ~Dog(void);

        void makeSound() const;
};

#endif
