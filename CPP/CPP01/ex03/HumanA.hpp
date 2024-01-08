/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HumanA.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hongbaki <hongbaki@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/01 15:31:50 by hongbaki          #+#    #+#             */
/*   Updated: 2023/11/01 17:00:22 by hongbaki         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HUMANA_H
# define HUMANA_H

#include "Weapon.hpp"

class HumanA
{
    private:
        Weapon &_weapon;
        std::string _name;
    public:
        void attack();
        HumanA(std::string name, Weapon &weapon);
        ~HumanA();
};

#endif