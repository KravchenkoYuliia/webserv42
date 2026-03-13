/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   SignalHandler.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jgossard <jgossard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/13 16:05:10 by jgossard          #+#    #+#             */
/*   Updated: 2026/03/13 16:05:11 by jgossard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SIGNAL_HANDLER_HPP
#define SIGNAL_HANDLER_HPP

#include <csignal>

// Global flag checked by the reactor loop to handle Ctrl-C (SIGINT).
// - 'sig_atomic_t' ensures atomic access in the signal handler.
// - 'volatile' prevents compiler optimizations from caching the value,
//   so changes made by the signal handler are always seen in the loop.
extern volatile sig_atomic_t g_running;

// signal handler function
void handle_sigint(int signal);

#endif