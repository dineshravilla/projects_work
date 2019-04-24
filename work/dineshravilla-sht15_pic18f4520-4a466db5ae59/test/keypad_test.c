#include  "includes.h"

void main() {

    unsigned char k, buf[32];
    unsigned int test=10;
    unsigned int test1=10;
    unsigned int test2=10;

    lcd_init();
    keypad_init();


    while (1) {

        lcd_cmd(0x80);
        lcd_display("KEYPAD TEST:");
        k = get_key();

        if ( k == '0') {
            //test = 0;
            test = test+1;
            memset(buf,0,sizeof(buf));
            sprintf(buf,"%3d",test);
            lcd_cmd(0xc0);
            lcd_display(buf);
        }

        if(k == '1') {
           // test1 = 1;
            test1 = test1+5;
            memset(buf,0,sizeof(buf));
            sprintf(buf,"%3d",test1);
            lcd_cmd(0xc0);
            lcd_display(buf);
        }

        if(k == '2') {
           // test2 = 2;
            test2 = test2+1;
            memset(buf,0,sizeof(buf));
            sprintf(buf,"%3d",test2);
            lcd_cmd(0xc0);
            lcd_display(buf);
        }

        if(k == '3') {
            lcd_cmd(0xc0);
            memset(buf,0,sizeof(buf));
            sprintf(buf,"%3c",k);
            lcd_display(buf);
        }
    }

}
