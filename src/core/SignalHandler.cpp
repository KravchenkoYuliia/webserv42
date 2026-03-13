/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   SignalHandler.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jgossard <jgossard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/13 16:09:39 by jgossard          #+#    #+#             */
/*   Updated: 2026/03/13 16:09:40 by jgossard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "SignalHandler.hpp"

volatile sig_atomic_t g_running = 1;

void handle_sigint(int signal)
{
    (void)signal; // unused
    g_running = 0; // tell the reactor to stop
}