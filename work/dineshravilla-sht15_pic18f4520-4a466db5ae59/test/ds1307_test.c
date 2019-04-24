

#include <xc.h>
#include <pic18f4520.h>
#include "i2c.h"
#include "LCD.h"
#include "config.h"
#include "DS1307.h"

int main() {

    PORTD_DIRECT = 0x00;
    PORTB_DIRECT = 0x00;

    LCDInit();
    I2CInit();

    DS1307SetTime(PM, 07, 00, 00);
    DS1307SetDate(02,03,15,Monday);


    while(1) {

        LCDDispTime ( DS1307GetTime() );
        LCDDispDate ( DS1307GetDate() );
        Delay10KTCYx(100);
    }

    
    
    return(0);

}
