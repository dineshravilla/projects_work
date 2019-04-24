#include "includes.h"

void delay(int n) {

    while(n--) {
        ;
    }
}

void lcd_cmd(unsigned char cmd) {

    DATABUS = cmd;
    RS = 0;
    RWE = 0;
    E = 1;
    delay(100);
    E = 0;
}

void lcd_data(unsigned char data) {

    DATABUS = data;
    RS = 1;
    RWE = 0;
    E = 1;
    delay(100);
    E = 0;
}

void lcd_init() {

    RS_DIR = 0;
    RWE_DIR = 0;
    E_DIR = 0;

    DATABUS_DIR = 0x00;
    DATABUS = 0x00;

    lcd_cmd(0x38);
    lcd_cmd(0x0c);
    lcd_cmd(0x01);
    lcd_cmd(0x80);
}

void lcd_display(char *str) {

    while(*str) {
        lcd_data(*str++);
    }
}


