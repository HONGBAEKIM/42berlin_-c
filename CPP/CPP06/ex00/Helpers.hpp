/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Helpers.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hongbaki <hongbaki@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/29 15:29:27 by hongbaki          #+#    #+#             */
/*   Updated: 2023/12/04 11:05:24 by hongbaki         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HELPERS_HPP
# define HELPERS_HPP
# include <string>
# include <iostream>


bool isSpecial(std::string const &val);
bool isFloat(std::string const &val);
bool isDouble(std::string const &val);
bool isInteger(std::string const &val);

#endif
