/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Utils.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yukravch <yukravch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/27 17:51:18 by jgossard          #+#    #+#             */
/*   Updated: 2026/03/10 13:36:21 by yukravch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef UTILS_HPP
#define UTILS_HPP

#include "ServerConfig.hpp"

class Utils {
public:
    static void setNonBlocking(int fd);
};

#endif // UTILS_HPP
