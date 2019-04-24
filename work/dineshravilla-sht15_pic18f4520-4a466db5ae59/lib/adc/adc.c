#include "includes.h"

void adc_init() {

    TRISA = 0xFF;
    PORTA = 0x00;
    ADCON0 = 0x00;
    ADCON1 = 0x00;
}

unsigned int adc_convert() {

    ADCON0=0x01;
    ADCON2= 0b10000000;
    Delay1KTCYx(50);
    ADCON0bits.GO=1;
    while(ADCON0bits.GO==1);
    return((ADRESH<<8) + ADRESL);
}
