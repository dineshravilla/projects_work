#ifndef GL868_H
#define	GL868_H

#include <xc.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <usart.h>
#include <delays.h>



#define GL868_OFF 0 
#define GL868_ON 1 
#define GL868_READY 2 
#define GL868_GOOD_SIGNAL 3 
#define GL868_NETWORK_REGISTERED 4 
#define GL868_GPRS_INACTIVE 5
#define GL868_GPRS_ACTIVE 6
#define GL868_CONNECTED 7 
#define GL868_FSM_HALT 23
#define GL868_BUFFER_SIZE 512 
#define GL868_LINE_SIZE 128 
#define GL868_LOG_SIZE 80
#define GL868_GET_DATA_SIZE  128 
#define GL868_POST_BODY_SIZE 255
#define GL868_POST_REQUEST_SIZE 255
#define GL868_GET_REQUEST_SIZE  255 
#define GL868_RESPONSE_BUFFER_SIZE 72 
#define GL868_GET_DATA_ERROR 1 
#define GL868_POST_BODY_ERROR 2 
#define GL868_WRONG_HTTP_METHOD_ERROR 3  

struct gl868_response_t{
    uint8_t signal;
    uint8_t network;
    uint8_t last_on;
    uint8_t fsm_halt ;
    uint8_t reboot ;
    uint8_t gprs ;
    uint8_t socket ;
    uint8_t error ;
    uint16_t scode ;
    unsigned long t_time ;
};

extern void gsm_init (uint8_t port, const char *baud_rate,uint8_t dport);
extern void gsm_set_socket_host(char *host, char *port);
extern uint8_t gsm_connect(int timeout);
extern uint8_t gsm_send_http(const char *method, char *data, char *response);
extern uint8_t gsm_disconnect();
struct gl868_response_t gsm_get_response();
extern void gsm_get_response_in_buffer(char *s) ;
extern void gsm_get_sd_response(char *s) ;

#endif

