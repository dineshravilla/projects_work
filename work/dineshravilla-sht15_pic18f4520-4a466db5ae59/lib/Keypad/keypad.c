#include "includes.h"

void keypad_init() {

    TRISCbits.RC0 = 1;
    TRISCbits.RC1 = 1;
    TRISCbits.RC2 = 1;
    TRISCbits.RC3 = 1;


}

unsigned char read_switch() {


    if(SW0 == 1) {
        while(SW0 == 1);
        return '0';
    }

    if(SW1 == 1) {
        while(SW1 == 1);
        return '1';
    }

    if(SW2 == 1) {
        return '2';
    }

    if(SW3 == 1) {
        while(SW3 == 1);
        return '3';
    }

    return 'n';
}

unsigned char get_key() {

    char key = 'n';

    while(key == 'n') {
        key = read_switch();
    }

    return key;
}
