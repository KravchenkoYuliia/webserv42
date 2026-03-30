/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jgossard <jgossard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/27 17:54:23 by jgossard          #+#    #+#             */
/*   Updated: 2026/03/27 15:12:35 by jgossard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <fcntl.h>          // fcntl
#include <cctype>           // std::tolower, std::isspace
#include <stdexcept>        // std::runtime_error
#include <sstream>          // std::stringstream
#include <cstdlib>          // strtoll
#include <errno.h>          // errno // TODO: to keep?
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
long long        Utils::parseLongLong(const std::string &str, bool &success, int base)
{
    std::string     trimmed_str = Utils::trim(str);
    char            *endptr = 0;
    long long       value;
    if (trimmed_str.empty() || trimmed_str[0] == '-' || trimmed_str[0] == '+')
    {
        success = false;
        return (0);
    }
    errno = 0; // TODO: to keep?
    value = std::strtoll(trimmed_str.c_str(), &endptr, base);
    // TODO: this errno check cases of overflow...can be kept?
    if (errno == ERANGE || *endptr != '\0' || value < 0)
    {
        success = false;
        return (0);
    }
    success = true;
    return (value);
}
