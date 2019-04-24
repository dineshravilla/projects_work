#include "includes.h"


void main() {

    unsigned int result;
    unsigned char buf[32];
    adc_init();
    lcd_init();
    while(1) {
        lcd_cmd(0x80);
        lcd_display("**ADC**");
        result = adc_convert();
        result = result/10;
        memset(buf,0,sizeof(buf));
        sprintf(buf,"%3d",result);
        lcd_cmd(0xc0);
        lcd_display(buf);
    }
}