/*
 * File:   keypad.h
 * Author: Dinesh
 *
 * Created on February 24, 2015, 5:43 PM
 */

#ifndef KEYPAD_H
#define	KEYPAD_H

#define SW0 PORTCbits.RC0
#define SW1 PORTCbits.RC1
#define SW2 PORTCbits.RC2
#define SW3 PORTCbits.RC3

void keypad_init(void);
unsigned char read_switch(void);
unsigned char get_key(void);


#endif	/* KEYPAD_H */

