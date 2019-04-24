/* 
 * File:   DS1307.h
 * Author: Dinesh
 *
 * Created on February 6, 2015, 5:06 PM
 */

#ifndef DS1307_H
#define	DS1307_H

#define DS1307_ADD 0xD0

#define Sunday      1
#define Monday      2
#define Tuesday     3
#define Wednesday   4
#define Thursday    5
#define Friday      6
#define Saturday    7

#define SEC 0x00
#define MIN 0x01
#define HRS 0x02
#define DAY 0x03
#define DATE 0x04
#define MONTH 0x05
#define YEAR 0x06

#define AM 0
#define PM 1
#define HRS 2

void DS1307WriteByte(unsigned char, unsigned char);
unsigned char DS1307ReadByte(unsigned char);
unsigned char* DS1307GetTime(void);
unsigned char* DS1307GetDate(void);
void DS1307SetTime(unsigned char,unsigned char,unsigned char,unsigned char);
void DS1307SetDate(unsigned char,unsigned char,unsigned char,unsigned char);



#endif	/* DS1307_H */

