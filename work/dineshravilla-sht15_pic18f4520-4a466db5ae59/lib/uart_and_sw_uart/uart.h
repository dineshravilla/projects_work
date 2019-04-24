/*
 * File:   uart.h
 * Author: Dinesh
 *
 * Created on March 18, 2015, 3:53 PM
 */

#ifndef UART_H
#define	UART_H

#include "includes.h"

extern void uart_puts(int port, char *s);
extern unsigned char uart_getc(int port);
extern void uart_putc(int port, char ch);
extern void uart_init(int);

#endif	/* UART_H */

