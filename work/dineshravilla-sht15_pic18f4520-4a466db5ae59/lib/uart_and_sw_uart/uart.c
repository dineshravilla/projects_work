#include  "includes.h"

void uart_init(int port) {
    unsigned int config=0;
    if(port == 0) {
        CloseUSART();
        config = USART_TX_INT_OFF
               & USART_RX_INT_OFF
               & USART_ASYNCH_MODE
               & USART_EIGHT_BIT
               & USART_CONT_RX
               & USART_BRGH_HIGH;

        OpenUSART(config,51);
    }
    if(port == 1) {
        software_uart_init();
    }
}

unsigned char uart_getc(int port) {
    unsigned char ch;
    if(port == 0) {
        ch = getcUSART();
    }
    if(port == 1) {
        ch = software_uart_receive();
    }
    return ch;
}

void uart_putc(int port, char ch) {
    if(port == 0) {
        putcUSART(ch);
    }
    if(port == 1) {
        software_uart_transmit(ch);
    }
}

void uart_puts(int port, char *s) {
    if(port == 0) {
        putsUSART(s);
    }
    if(port == 1) {
        software_uart_display(s);
    }
}


