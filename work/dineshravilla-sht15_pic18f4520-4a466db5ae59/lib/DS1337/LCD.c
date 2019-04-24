#include "includes.h"

void delay ( int n ) {

    while ( n-- )
        ;

}

void LCDCmd ( unsigned char cmd ) {

    BUS = cmd;
    RS = 0;
    RW = 0;
    E = 1;
    delay( 1000 );
    E = 0;

}

void LCDData ( unsigned char data ) {

    BUS = data;
    RS = 1;
    RW = 0;
    E = 1;
    delay ( 1000 );
    E = 0;

}

void LCDInit() {

    LCDCmd( 0x38 );
    LCDCmd( 0x0c );
    LCDCmd( 0x01 );
    LCDCmd( 0x80 );

}

void LCDDispString ( unsigned char *pdat) {

    while ( *pdat )
        LCDData(*pdat++);

}

void LCDDispTime( unsigned char *time ) {

    LCDCmd(0x80);

    LCDData( (time[2]/10) + 0x30 );
    LCDData( (time[2]%10) + 0x30 );

    LCDData(':');

    LCDData( (time[1]/10) + 0x30 );
    LCDData( (time[1]%10) + 0x30 );

    LCDData(':');

    LCDData( (time[0]/10) + 0x30 );
    LCDData( (time[0]%10) + 0x30 );

    switch(time[3]) {
        case AM:    LCDDispString("AM");
                    break;
        case PM:    LCDDispString("PM");
                    break;
        default:    LCDData('H');
                    break;
    }

}

void LCDDispDate ( unsigned char *date ) {

    LCDCmd(0xC0);

    LCDData( (date[1]/10) + 0x30 );
    LCDData( (date[1]%10) + 0x30 );

    LCDData('/');

    LCDData( (date[2]/10) + 0x30 );
    LCDData( (date[2]%10) + 0x30 );

    LCDData('/');

    LCDData( (date[3]/10) + 0x30 );
    LCDData( (date[3]%10) + 0x30 );

    LCDData(' ');

    switch ( date[0] ) {

        case Monday:    LCDDispString("MON");   break;

        case Tuesday:   LCDDispString("TUE");   break;
        
        case Wednesday: LCDDispString("WED");   break;

        case Thursday:  LCDDispString("THU");   break;

        case Friday:    LCDDispString("FRI");   break;

        case Saturday:  LCDDispString("SAT");   break;

        case Sunday:    LCDDispString("SUN");   break;

        default:        LCDDispString("???");   break;

    }


}