/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ResponseBuilder.hpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jgossard <jgossard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/04 16:46:17 by jgossard          #+#    #+#             */
/*   Updated: 2026/03/11 11:09:58 by jgossard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef RESPONSE_BUILDER_HPP
#define RESPONSE_BUILDER_HPP

#include "http/HttpResponse.hpp"
#include "http/HttpRequest.hpp"

class ResponseBuilder {
public:
    // ---------- Constructors / Destructor ----------

    ResponseBuilder(void);
    ResponseBuilder(const ResponseBuilder& copy);
    ~ResponseBuilder(void);

    // ---------- Overloading Operators Methods -------

    ResponseBuilder& operator=(const ResponseBuilder& copy);

    // ---------- Getter and Setter Methods ------------

    // ---------- Member Methods -----------------------

    HttpResponse     build(const HttpRequest& request);
    
protected:
    // ---------- Protected Data Members ---------------------

private:
    // ---------- Private Data members -------------------------
};

#endif // RESPONSE_BUILDER_HPP
