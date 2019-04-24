/* 
 * File:   millis.h
 * Author: Dinesh
 *
 * Created on March 17, 2015, 2:49 PM
 */

#ifndef MILLIS_H
#define	MILLIS_H

#include <xc.h>
#include <usart.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <delays.h>

volatile unsigned long int t_millis;
unsigned char buf[32];

unsigned long int millis();
void init();
void usart_init();


#endif	/* MILLIS_H */

