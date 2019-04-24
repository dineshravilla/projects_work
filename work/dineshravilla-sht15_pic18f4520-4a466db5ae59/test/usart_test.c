#include <xc.h>
#include <usart.h>

#define BAUD_9600 25
#define BUFFER_SIZE 32

void main() {

    unsigned char config=0,ch,buffer[32];
    unsigned int i;

    CloseUSART();

    config = USART_TX_INT_OFF
            & USART_RX_INT_OFF
            & USART_ASYNCH_MODE
            & USART_EIGHT_BIT
            & USART_CONT_RX
            & USART_BRGH_HIGH;

    OpenUSART(config,BAUD_9600);
    while(1) {

       while ( i < BUFFER_SIZE ) {

           while( DataRdyUSART() == 0 )
                ;
            ch = ReadUSART();
            if ( ch == '\r' || ch == '\n')
                break;
            buffer[i++] = ch;

        }
        buffer[i] = 0;              //Flushing Buffer
        putcUSART(12);
        putsUSART(buffer);
        

    }
}
