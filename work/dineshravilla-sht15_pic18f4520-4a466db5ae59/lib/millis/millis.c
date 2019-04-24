#include "millis.h"

void interrupt timer1_ISR() {
    if(TMR0IF ==1) {
        t_millis=t_millis+4;
        TMR0IF =0;
    }
}

unsigned long int millis() {
    return t_millis;
}

void init() {
    GIE =1;
    PEIE =1;
    TMR0IE =1;
    T0CON =0xC3;
}

void usart_init() {
    unsigned char config=0;
    CloseUSART();
    config = USART_TX_INT_OFF
          & USART_RX_INT_OFF
          & USART_ASYNCH_MODE
          & USART_EIGHT_BIT
          & USART_CONT_RX
          & USART_BRGH_HIGH;
    OpenUSART(config,25);
}

