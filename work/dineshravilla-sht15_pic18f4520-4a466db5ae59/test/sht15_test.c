#include "sht15.h"
#include <stdio.h>
#include <usart.h>
#include <string.h>

#define BAUD_9600 25



float temp_c=0;
float humid=0;
unsigned int error,i;
unsigned char config=0;
unsigned char buffer[32];

void main() {

    OSCCON = 0b01100000;
    ADCON1 = 0x0F;

    CloseUSART();
    config = USART_TX_INT_OFF
           & USART_RX_INT_OFF
           & USART_ASYNCH_MODE
           & USART_EIGHT_BIT
           & USART_CONT_RX
           & USART_BRGH_HIGH;

    OpenUSART(config,BAUD_9600);

    while(1) {

    putsUSART("\n\rTemperature:");

    error = sht15_read_temperature(&temp_c);
    sprintf(buffer,"%d",error);
  
    if(error) {        
        putsUSART(buffer);
    }

    memset(buffer,0,sizeof(buffer));

    sprintf(buffer,"%.2f",temp_c);
    putsUSART(buffer);
    
    memset(buffer,0,sizeof(buffer));

    putsUSART("\n\r Humidity:");
    
    error = sht15_read_humidity(&humid);
    sprintf(buffer,"%d",error);

    if(error) {
        putsUSART(buffer);
    }

    memset(buffer,0,sizeof(buffer));

    sprintf(buffer,"%.2f",humid);
    putsUSART(buffer);

    for(i = 0; i<5; i++) {
        Delay10KTCYx(100);
    }
  
    }
    
}


