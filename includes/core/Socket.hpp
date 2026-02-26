/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Socket.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jgossard <jgossard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/24 17:45:19 by jgossard          #+#    #+#             */
/*   Updated: 2026/03/02 15:23:33 by jgossard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SOCKET_HPP
#define SOCKET_HPP

#include <inttypes.h> // uint16_t

class Socket {
public:
    // ---------- Constructors / Destructor ----------

    Socket(void);
    Socket(const Socket& copy);
    ~Socket(void);

    // ---------- Overloading Operators Methods -------

    Socket& operator=(const Socket& copy);

    // ---------- Getter and Setter Methods ------------

    int     getFd() const;

    // ---------- Member Methods -----------------------

    void    create();
    void    bind( uint16_t port );
    void    listen();
    int     accept();
    void    setNonBlocking();
    void    setReusable();

protected:
    // ---------- Protected Data Members ---------------------

private:
    // ---------- Private Data members -------------------------

    static const int          kDefaultFd = -1;
    static const int          kDefaultPort = 8080;

    int         fd_;
    uint16_t    bind_port_;
};

#endif // SOCKET_HPP
