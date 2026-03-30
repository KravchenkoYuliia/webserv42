/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Utils.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jgossard <jgossard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/27 17:51:18 by jgossard          #+#    #+#             */
/*   Updated: 2026/03/27 14:39:18 by jgossard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef UTILS_HPP
#define UTILS_HPP

#include <string>

class Utils {
public:
    static void             setNonBlocking(int fd);
    static std::string      toString(size_t n);
    static std::string      toLower(const std::string& str);
    static std::string      trim(const std::string& str);
    static long long        parseLongLong(const std::string &str, bool &success, int base);
};

#endif // UTILS_HPP
