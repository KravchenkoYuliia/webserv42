/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   BaseEventHandler.cpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jgossard <jgossard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/05 10:43:36 by jgossard          #+#    #+#             */
/*   Updated: 2026/03/06 11:01:54 by jgossard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "reactor/BaseEventHandler.hpp"
#include <iostream>

BaseEventHandler::BaseEventHandler(void)
{
    std::cout << "BaseEventHandler default constructor called" << std::endl;
}

// BaseEventHandler::BaseEventHandler(const BaseEventHandler& copy)
// {
//     std::cout << "BaseEventHandler copy constructor called" << std::endl;
//     *this = copy;
// }

BaseEventHandler::~BaseEventHandler(void)
{
    std::cout << "BaseEventHandler destructor called" << std::endl;
}

// BaseEventHandler& BaseEventHandler::operator=(const BaseEventHandler& copy)
// {
//     std::cout << "BaseEventHandler assignment operator called" << std::endl;
//     if (this != &copy) {
//         // copy data members here
//     }
//     return (*this);
// }
