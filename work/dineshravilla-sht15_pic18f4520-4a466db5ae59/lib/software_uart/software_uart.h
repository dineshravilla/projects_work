/* 
 * File:   software_uart.h
 * Author: Dinesh
 *
 * Created on March 16, 2015, 3:08 PM
 */

#ifndef SOFTWARE_UART_H
#define	SOFTWARE_UART_H

#ifndef _XTAL_FREQ
    #define _XTAL_FREQ 8000000
#endif

#define Baudrate              2400
#define OneBitDelay           (1000000/Baudrate)
#define DataBitCount          8
#define UART_RX               PORTDbits.RD1
#define UART_TX               PORTDbits.RD0
#define UART_RX_DIR           TRISDbits.RD1
#define UART_TX_DIR           TRISDbits.RD0

void software_uart_init();
unsigned char software_uart_receive();
void software_uart_transmit(char);
void software_uart_display(char *);

#endif	

