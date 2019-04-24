#include "software_uart.h"
#include <xc.h>

void software_uart_init() {
    UART_TX = 1;
    UART_RX_DIR = 1;
    UART_TX_DIR = 0;
    UART_RX = 0;
}

unsigned char software_uart_receive() {
    unsigned char data = 0;
    while(UART_RX==1)
        ;

    __delay_us(OneBitDelay);
    __delay_us(OneBitDelay/2);

    for ( unsigned char i = 0; i < DataBitCount; i++ ) {
        if ( UART_RX == 1 ) {
            data += (1<<i);
        }
        __delay_us(OneBitDelay);
    }


    if ( UART_RX == 1 ) {
        __delay_us(OneBitDelay/2);
        return data;
    }
    else {
        __delay_us(OneBitDelay/2);
        return 0x000;
    }
}

void software_uart_transmit(char data) {
    UART_TX = 0;
    __delay_us(OneBitDelay);

    for ( unsigned char i = 0; i < DataBitCount; i++ ) {
        if( ((data>>i)&0x1) == 0x1 ) {
            UART_TX = 1;
        }
        else
        {
            UART_TX = 0;
        }
        __delay_us(OneBitDelay);
    }

    UART_TX = 1;
    __delay_us(OneBitDelay);
}

void software_uart_display(char *str) {
    while(*str) {
        software_uart_transmit(*str++);
    }
}