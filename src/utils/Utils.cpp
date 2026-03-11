/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jgossard <jgossard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/27 17:54:23 by jgossard          #+#    #+#             */
/*   Updated: 2026/03/11 11:20:40 by jgossard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <fcntl.h>          // fcntl
#include <cctype>           // std::tolower, std::isspace
#include <stdexcept>
#include <sstream>          // std::stringstream
#include "utils/Utils.hpp"

void Utils::setNonBlocking(int fd)
{
    int	flag = fcntl( fd, F_GETFL );
    if ( flag == -1 )
        throw std::runtime_error("fcntl GET failed");
    if ( fcntl( fd, F_SETFL, flag | O_NONBLOCK ) == -1 )
        throw std::runtime_error("fcntl SET failed");
}

std::string Utils::toString(size_t n)
{
    std::stringstream ss;
    ss << n;
    return (ss.str());
}

std::string Utils::toLower(const std::string& str)
{
    std::string lowercase_string = str;

    for (size_t i = 0; i < lowercase_string.length(); ++i)
    {
        lowercase_string[i] = std::tolower(static_cast<unsigned char>(lowercase_string[i]));
    }
    return (lowercase_string);
}

std::string Utils::trim(const std::string& str)
{
    size_t start = 0;
    size_t end = str.length();

    while (start < end && std::isspace(static_cast<unsigned char>(str[start])))
        ++start;

    while (end > start && std::isspace(static_cast<unsigned char>(str[end])))
        --end;

    return (str.substr(start, end - start));
}
