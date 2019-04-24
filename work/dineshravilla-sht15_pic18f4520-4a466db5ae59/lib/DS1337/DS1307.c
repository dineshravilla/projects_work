#include "includes.h"

unsigned char RTC[4];
unsigned char temp;

void DS1307WriteByte( unsigned char Add, unsigned char Data ) {

    I2CStart();
    while( I2CWriteByte( DS1307_ADD ) == 1 )
        ;
    I2CWriteByte(Add);
    I2CWriteByte(Data);
    I2CStop();

}

unsigned char DS1307ReadByte( unsigned char Add) {

    unsigned char Byte = 0;
    I2CStart();
    while( I2CWriteByte( DS1307_ADD ) == 1 )
        ;
    I2CWriteByte(Add);
    I2CRestart();
    I2CWriteByte( DS1307_ADD + 1 );
    Byte = I2CReadByte();
    I2CNack();
    I2CStop();
    return Byte;

}

void DS1307SetTime(unsigned char Mode, unsigned char Hours, unsigned char Mins, unsigned char Secs)
{
	// Convert Hours, Mins, Secs into BCD
	RTC[0] = (((unsigned char)(Secs/10))<<4)|((unsigned char)(Secs%10));
	RTC[1] = (((unsigned char)(Mins/10))<<4)|((unsigned char)(Mins%10));
	RTC[2] = (((unsigned char)(Hours/10))<<4)|((unsigned char)(Hours%10));

	switch(Mode)	// Set mode bits
	{
            case AM: 	RTC[2] |= 0x40;
                        break;
            case PM: 	RTC[2] |= 0x60;
                        break;
	    default:	break;	// do nothing for 24HoursMode
	}

	// WritepRTCArray to DS1307
	DS1307WriteByte(0x00, RTC[0]);
        DS1307WriteByte(0x01, RTC[1]);
        DS1307WriteByte(0x02, RTC[2]);
}


unsigned char* DS1307GetTime() {

        RTC[0]=DS1307ReadByte(SEC);
        RTC[1]=DS1307ReadByte(MIN);
        RTC[2]=DS1307ReadByte(HRS);

	temp = RTC[0];
	RTC[0] = ((temp&0x7F)>>4)*10 + (temp&0x0F);

	temp = RTC[1];
	RTC[1] = (temp>>4)*10 + (temp&0x0F);

        if(RTC[2]&0x40)	// if 12 hours mode
	{
		if(RTC[2]&0x20)	// if PM Time
			RTC[3] = PM;
		else		// if AM time
			RTC[3] = AM;

		temp = RTC[2];
		RTC[2] = ((temp&0x1F)>>4)*10 + (temp&0x0F);
	}
	else		// if 24 hours mode
	{
		temp = RTC[2];
		RTC[2] = (temp>>4)*10 + (temp&0x0F);
		RTC[3] = HRS;
	}

	return RTC;

}

void DS1307SetDate(unsigned char Date, unsigned char Month, unsigned char Year, unsigned char Day)
{
	// Convert Year, Month, Date, Day into BCD
	RTC[0] = (((unsigned char)(Day/10))<<4)|((unsigned char)(Day%10));
	RTC[1] = (((unsigned char)(Date/10))<<4)|((unsigned char)(Date%10));
	RTC[2] = (((unsigned char)(Month/10))<<4)|((unsigned char)(Month%10));
	RTC[3] = (((unsigned char)(Year/10))<<4)|((unsigned char)(Year%10));

	// WritepRTCArray to DS1307
	DS1307WriteByte(0x03, RTC[0]);
        DS1307WriteByte(0x04, RTC[1]);
        DS1307WriteByte(0x05, RTC[2]);
        DS1307WriteByte(0x06, RTC[3]);
}

unsigned char* DS1307GetDate() {

    RTC[0] = DS1307ReadByte(DAY);
    RTC[1] = DS1307ReadByte(DATE);
    RTC[2] = DS1307ReadByte(MONTH);
    RTC[3] = DS1307ReadByte(YEAR);

    temp = RTC[1];
    RTC[1] = (temp >> 4)*10 + (temp & 0x0F);

    temp = RTC[2];
    RTC[2] = (temp >> 4)*10 + (temp & 0x0F);

    temp = RTC[3];
    RTC[3] = (temp >> 4)*10 + (temp & 0x0F);

    return RTC;

}


