/*
 * File:   LCD.h
 * Author: Dinesh
 *
 * Created on February 7, 2015, 1:03 PM
 */

#ifndef LCD_H
#define	LCD_H

#define RS PORTDbits.RD0
#define RW PORTDbits.RD1
#define E PORTDbits.RD2

#define BUS PORTB

#define PORTD_DIRECT TRISD
#define PORTB_DIRECT TRISB

#define DELAY_E 1000;

void delay(int);
void LCDCmd(unsigned char);
void LCDData(unsigned char);
void LCDDispString(unsigned char*);
void LCDDispTime(unsigned char*);
void LCDDispDate(unsigned char*);
void LCDInit();
void LCDDispSec(unsigned char);

#endif	/* LCD_H */

