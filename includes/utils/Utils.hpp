/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Utils.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jgossard <jgossard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/27 17:51:18 by jgossard          #+#    #+#             */
/*   Updated: 2026/03/11 10:43:27 by jgossard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef UTILS_HPP
#define UTILS_HPP

class Utils {
public:
    static void         setNonBlocking(int fd);
    static std::string  toString(size_t n);
    static std::string  toLower(const std::string& str);
    static std::string  trim(const std::string& str);
};

#endif // UTILS_HPP
