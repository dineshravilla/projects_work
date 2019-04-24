#include "includes.h"

void I2CInit() {

        DIRECT_SDA = 1;
        DIRECT_SCL = 1;

        SSPADD = I2C_SPEED;
        SSPSTAT = SLEW_OFF_SSPSTAT;
        SSPCON1 = SSPEN_MASTER_SSPCON1;

    }

void I2CStart() {

        SSPCON2bits.SEN = 1;
        while ( !SSPIF )
            ;
        SSPIF = 0;

    }

void I2CRestart() {

        SSPCON2bits.RSEN = 1;
        while ( !SSPIF )
            ;
        SSPIF = 0;
    }

void I2CStop() {

        SSPCON2bits.PEN = 1;
        while ( !SSPIF )
            ;
        SSPIF = 0;
    }

void I2CAck() {

        SSPCON2bits.ACKDT = 0;          //Active Low Pin - 0 means Acknowledge
        SSPCON2bits.ACKEN = 1;
        while ( !SSPIF )
            ;
        SSPIF = 0;
    }

void I2CNack() {

        SSPCON2bits.ACKDT = 1;              // 1 - Not Ack
        SSPCON2bits.ACKEN = 1;
        while ( !SSPIF )
            ;
        SSPIF = 0;
    }

bit I2CWriteByte(unsigned char Byte) {

        SSPBUF = Byte;
        while ( !SSPIF )
            ;
        SSPIF = 0;
        return SSPCON2bits.ACKSTAT;
    }

unsigned char I2CReadByte() {

        SSPCON2bits.RCEN = 1;
        while ( !SSPIF )
            ;
        SSPIF = 0;
        return SSPBUF;
    }

