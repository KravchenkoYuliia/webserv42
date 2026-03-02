/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jgossard <jgossard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/27 17:54:23 by jgossard          #+#    #+#             */
/*   Updated: 2026/02/27 18:04:09 by jgossard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "utils/Utils.hpp"
#include <fcntl.h>          // fcntl
#include <stdexcept>

void Utils::setNonBlocking(int fd)
{
    int	flag = fcntl( fd, F_GETFL );
    if ( flag == -1 )
        throw std::runtime_error("fcntl GET failed");
    if ( fcntl( fd, F_SETFL, flag | O_NONBLOCK ) == -1 )
        throw std::runtime_error("fcntl SET failed");
}

