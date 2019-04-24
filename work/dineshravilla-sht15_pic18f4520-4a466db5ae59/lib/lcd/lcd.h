/*
 * File:   lcd.h
 * Author: Dinesh
 *
 * Created on March 2, 2015, 1:53 PM
 */

#ifndef LCD_H
#define	LCD_H

#define RS PORTCbits.RC5
#define RWE PORTCbits.RC6
#define E PORTCbits.RC7
#define RS_DIR TRISCbits.RC5
#define RWE_DIR TRISCbits.RC6
#define E_DIR TRISCbits.RC7

#define DATABUS PORTD
#define DATABUS_DIR TRISD

void delay(int);
void lcd_cmd(unsigned char);
void lcd_data(unsigned char);
void lcd_init(void);
void lcd_display(char *);

#endif	/* LCD_H */

