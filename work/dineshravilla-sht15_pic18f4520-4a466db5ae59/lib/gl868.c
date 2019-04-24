#include  "includes.h"

static void gl868();
static void gl868_set_port(uint8_t port);
static void gl868_set_debug_port(uint8_t dport);

static void gl868_set_baud_rate(const char *baud_rate);

static void gl868_switch_on();
static void gl868_switch_off();
static void gl868_init();
static uint8_t gl868_is_ready() ;
static uint8_t gl868_is_registered();
static void gl868_diagnosis();
static void gl868_flush();
static uint8_t gl868_get_state();
static void gl868_set_state(uint8_t state);
static char* gl868_get_line();
static char* gl868_get_sd_response();
static void gl868_skip_http_headers(unsigned long timeout);
static void gl868_send(const char* buffer);
static void gl868_send_char(unsigned char ch);
static uint8_t gl868_send_command(const char* command,const char* token, unsigned long timeout);
static uint8_t gl868_read_buffer(unsigned long timeout, const char* token);
static uint8_t gl868_read_lines(unsigned long timeout, const char* token);
static void gl868_read_line(unsigned long timeout);
static uint8_t gl868_signal_level();
static uint8_t gl868_check_network_registration();
static uint8_t gl868_check_gprs_state();
static uint8_t gl868_activate_gprs();
static uint8_t gl868_deactivate_gprs();
static uint8_t gl868_open_socket(const char *host, const char *port);
static uint8_t gl868_send_http_get(const char *host, const char *data);
static uint8_t gl868_send_http_post(const char *host, const char *data);


static uint8_t _modem_port;
static uint8_t _debug_port;
static uint8_t on_off_pin;
static char _buffer[GL868_BUFFER_SIZE];
static char _line[GL868_LINE_SIZE];
static char _sd_response[GL868_LINE_SIZE];
static uint8_t _state;
static const char *baud_rate;
static char buf[72];


// max 32 char
static char *socket_host;
// max 4 char
static char *socket_port;
struct gl868_response_t response;
static uint8_t _busy_flag;
static unsigned long _fsm_halt_time;
static unsigned long _fsm_halt_interval;
static uint8_t _fsm_halt_flag;
static int gsm_run_fsm(uint8_t state);
static void gsm_halt_fsm(uint8_t seconds);
static void gsm_init_response();
static uint16_t gsm_get_nginx_response_code(char *response);



// modem on_off_pin is PH4
#define MODEM_PIN_MASK 0x10


static void gl868() {
    _state = GL868_ON;
    // set PH4 for output
     TRISD &= ~MODEM_PIN_MASK;
    Delay1TCYx(10);

    _debug_port = 0;
    _modem_port = 1 ;
    baud_rate = "9600" ;
}

static void gl868_set_port(uint8_t port) {
    _modem_port = port ;
}

static void gl868_set_debug_port(uint8_t dport) {
    _debug_port = dport ;
}

static void gl868_set_baud_rate(const char *rate) {
    baud_rate = rate ;
}

static void gl868_switch_off() {
    gl868_send_command("AT#SHDN", "OK", 5000L);
    gl868_set_state(GL868_OFF);
}

static void gl868_switch_on() {
    memset(buf,0,sizeof(buf));
    sprintf(buf,"switching on gsm module\n");
    uart_puts(1,buf);

    // pull modem pin HIGH
    // first HIGH needs 300 ms
    PORTD |= MODEM_PIN_MASK ;

    for(int i =0 ; i < 300; i++)
        Delay1KTCYx(1);

    // modem pin LOW needs 1100 ms
    // @porting required
    PORTD &= ~MODEM_PIN_MASK ;
    for(int j =0 ; j < 1200; j++)
        Delay1KTCYx(1);
    gl868_set_state(GL868_ON);

    // extra delay
    for(int k =0 ; k < 500; k++)
        Delay1KTCYx(1);
}

static uint8_t gl868_get_state() {
    return _state ;
}

static char* gl868_get_line() {
    return _line ;
}

static char* gl868_get_sd_response() {
    return _sd_response ;
}

static void gl868_set_state(uint8_t state) {
    _state = state ;
}

static uint8_t gl868_is_ready() {
    uint8_t code = (_state == GL868_READY) ? 1: 0 ;
    return code;
}

static uint8_t gl868_is_registered() {
    uint8_t code = (_state == GL868_NETWORK_REGISTERED) ? 1: 0 ;
    return code;
}

static void gl868_diagnosis() {
    memset(buf,0,sizeof(buf));
    sprintf(buf,"\n === GL868 diagnosis === \n");
    uart_puts(1,buf);

    gl868_send_command("AT+CPIN?","__NULL__",1000L);
    gl868_read_lines(3000UL, "OK");

    gl868_send_command("AT#CEERNET","__NULL__",1000L);
    gl868_read_lines(5000UL, "OK");

    gl868_send_command("AT#CEER","__NULL__",1000L);
    // flush rest of response
    gl868_read_lines(5000UL, "OK");

    return ;
}

static void gl868_init() {
    char command_buffer[16] ;
    memset(&command_buffer[0],0,sizeof(command_buffer));

    memset(buf,0,sizeof(buf));
    sprintf(buf,"GL868 init() entry \n");
    uart_puts(1,buf);
    uint8_t code = gl868_send_command("AT", "OK",2000UL);
    if(!code) return ;

    sprintf(command_buffer,"AT+IPR=%s",baud_rate);
    code = gl868_send_command(command_buffer, "OK",2000UL);
    if(!code) return ;

    // select newer interface
    code = gl868_send_command("AT#SELINT=2", "OK",2000UL);
    if(!code) return ;

    // turn on verbose errors for Cxxx commands
    code = gl868_send_command("AT+CMEE=2", "OK",2000UL);
    if(!code) return ;

    // disable hardware flow control
    // otherwise AT#SD just hangs
    code = gl868_send_command("AT&K0", "OK",2000UL);
    if(!code) return ;

    // turn off echo
    code = gl868_send_command("ATE0", "OK",2000UL);
    if(!code) return ;

    gl868_set_state(GL868_READY);
    memset(buf,0,sizeof(buf));
    sprintf(buf,"GL868 init() exit \n");
    uart_puts(1,buf);
    return ;
}

static uint8_t gl868_check_network_registration() {
    memset(buf,0,sizeof(buf));
    sprintf(buf,"GL868 check_network_registration() entry \n");
    uart_puts(1,buf);
    uint8_t network = 41 ;
    uint8_t code = gl868_send_command("AT+CREG?", "OK",5000UL);

    if(code) {
        char* b = strstr(_buffer, "+CREG:");
        // +CREG: <n>,<stat>
        if(b != NULL && (strlen(_buffer) >= 10)) {
            char sb[2] ;
            sb[0] = b[9] ;
            sb[1] = 0 ;
            // check error
            network = atoi(sb);
        }
    }

    memset(buf,0,sizeof(buf));
    sprintf(buf,"Network %d\n check_network_registration() exit\n",network);
    uart_puts(1,buf);
    return network ;
}

/*
 * activate_gprs assumes following
 *
 * 1) PDP context is already stored in slot-1 using AT+CGDCONT
 * (verify using AT+CGDCONT?)
 * 2) Telit easyIP TCP/IP stack is configured using AT#SCFG
 * (context_id 1 is bound to to socket_id 1)
 * (verify using AT#SCFG?)
 *
 */

static uint8_t gl868_activate_gprs() {
    // right attach sequence ON/OFF/ON?
    uint8_t code =  gl868_send_command("AT#SGACT=1,1", "OK",10000UL);

	if(!code){
		gl868_send_command("AT#SGACT=1,0","OK",10000UL);
                Delay1KTCYx(100);
		code =  gl868_send_command("AT#SGACT=1,1", "OK",10000UL);
	}

    return code ;
}

static uint8_t gl868_deactivate_gprs() {
    uint8_t code = gl868_send_command("AT#SGACT=1,0", "OK",10000UL);
    return code ;
}

static uint8_t gl868_check_gprs_state() {
    memset(buf,0,sizeof(buf));
    sprintf(buf,"GL868 check_gprs_state() entry \n") ;
    uart_puts(1,buf);

    uint8_t state = 0 ;
    uint8_t code = gl868_send_command("AT#SGACT?", "OK",5000UL);

    if(code) {
        char* b = strstr(_buffer, "#SGACT:");

        if(b != NULL && (strlen(_buffer) >= 11)) {

            char sb[2] ;

            sb[0] = b[10] ;
            /* string terminate */
            sb[1] = 0 ;
            /* check error */
            state = atoi(sb);
        }
    }
    
    memset(buf,0,sizeof(buf));
    sprintf(buf,"GPRS state: %d\n check_gprs_state() exit\n",state);
    uart_puts(1,buf);
    return state ;
}

static uint8_t gl868_open_socket(const char *host, const char *port) {
    memset(buf,0,sizeof(buf));
    sprintf(buf,"GL868 open_socket() entry\n") ;
    uart_puts(1,buf);
    uint8_t code = 0 ;
    char command_buffer[64];

    /*
     * open_socket assumes that
     *
     * 1) PDP context-1 has been activated
     * 2) easyIP TCP/IP stack is configured
     * 3) GPRS has been activated with AT#SGACT=1,1
     * --------------------------------------
     *
     * AT#SD=<socket_id>,<type>,<remote_port>,<IP>,<closure_type>
     *    socket_type : 0 for TCP sockets
     * socket_closure_type: [0|255]
     *    MT when remote server closes connection
     *    0 - closes immediately (default, is good)
     *    255 - close after an +++ escape sequence
     * -----------------------------------------
     *
     * Flushing socket
     * ================
     * socket timeout for CONNECT is determined by AT#SCFG settings
     * max timeout is 160s for AT#SD (DNS lookup + socket dial)
     *
     * connection will wait till transmission_timeout (5s) if minimum packet
     * size (300 bytes) is not reached. so we can either
     *
     * (a) use PADFWD char to flush the socket
     * (b) change transmission timeout using AT#SCFG
     * (c) buffer 300 (or min packet size data) before transmission
     *
     * (3) is not practical so it is down to (1) or (2)
     * (1) To use PADFWD chars
     *
     *  send_command("AT#PADCMD=0","OK",1000UL);
     *  send_command("AT#PADFWD=7","OK",1000UL);
     *
     *  This will set PADFWD to terminal bell (\a) in c.
     *  Then flush the PADFWD char as part of request
     *  strcat(this->_request,"\r\n\r\n\r\n\a");
     *
     *  To disable PAD char fwding, use PADCMD=0
     *
     * However modem should not forward the PADFWD char to the server. (@todo)
     * Otherwise a webserver e.g. nginx can interpret the PADFWD char as part of
     * request resulting in 400 (BAD request)
     *
     * (2) you can change socket transmission timeout
     * during modem context activation.
     *
     * closing socket
     * ==============
     *
     * NGINX (or HTTP/1.1) connections are keep-alive by default and server
     * will not close the socket unless modem explicitly does so. Nginx will
     * close the socket after keepalive_timeout duration. (set in nginx.conf)
     *
     * so we can flush multiple requests on same socket after connection.
     *
     * To disconnect, we need to first suspend socket using escape char(+++)
     * and then issue AT#SH. To make MT consider +++ as valid escape sequence
     * we need to provide pause before and after escape sequence +++.
     *
     * The sequence is
     * --------------------
     * work in command mode
     * AT#SD - socket dial - CONNECT - modem in online mode
     * flush data to server on socket
     * pause 1s (set by ATS12 command - default is 1s)
     * send +++  (don't flush anything else, e.g. \r\n )
     * pause 1s
     * AT#SH
     * command-mode
     *
     * set #SKIPESC to 1 to avoid sending escape char to server.
     *
     */

    if(host == NULL || port == NULL) {
        memset(buf,0,sizeof(buf));
        sprintf(buf,"socket host has not been intialized. \n") ;
        uart_puts(1,buf);
	return 0;
    }

    if(strlen(host) > 32) {
        memset(buf,0,sizeof(buf));
        sprintf(buf,"socket host exceeds 32 char limit \n") ;
        uart_puts(1,buf);
	return 0;
    }

    if(strlen(port) > 4) {
        memset(buf,0,sizeof(buf));
        sprintf(buf,"socket port exceeds 4 char limit \n") ;
        uart_puts(1,buf);
        return 0;
    }

    memset(&command_buffer[0], 0 , sizeof(command_buffer));

    gl868_send_command("AT#SKIPESC=1", "OK",2000UL);
    sprintf(command_buffer,"AT#SD=1,0,%s,\"%s\"",port,host);
    gl868_send_command(command_buffer, "__NULL__",1000UL);

    code = gl868_read_lines(160000UL, "CONNECT");

    if(!code) {
        // last modem line should have +CME error
        strncpy(_sd_response, _line, GL868_LINE_SIZE);
        _sd_response[GL868_LINE_SIZE-1] = '\0' ;
    }

    memset(buf,0,sizeof(buf));
    sprintf(buf,"GL868 open_socket() exit\n") ;
    uart_puts(1,buf);
    return code ;
}

static uint8_t gl868_send_http_get(const char *host, const char* data) {
    memset(buf,0,sizeof(buf));
    sprintf(buf,"GL868 send_http_get() entry\n") ;
    uart_puts(1,buf);

    char request[GL868_GET_REQUEST_SIZE] ;
    memset(&request[0], 0 , sizeof(request));

    int dlen = strlen(data);
    if(dlen > GL868_GET_DATA_SIZE) {
        return GL868_GET_DATA_ERROR ;
    }

    /*
     * command buffer size = [GL868_GET_REQUEST_SIZE],
     * more than 255 char in GET request size may be a problem
     * with older proxies and servers (whatever that means!)
     *
     * out of 255 chars, 128 is allocated for data
     * 24 + 12 + 25 = 61 chars to form the HTTP headers
     * any special tokens should be sent as part of payload only.
     *
     *
     * from HTTP/1.1 RFC2616
     * Request-Line  = Method SP Request-URI SP HTTP-Version CRLF
     *
     * Nginx needs HTTP/1.1 : otherwise 404
     * Nginx needs Host: otherwise 400
     *
     */

    // 24 chars
    strcat(request, "GET /api/gl868.php?data=");
    strcat(request,data);
    // 12 chars
    strcat(request," HTTP/1.1 \r\n");
    // 25 chars
    strcat(request,"Host: ");
    strcat(request,host);
    strcat(request," \r\n\r\n");

    gl868_send(request);
    uart_puts(1,request);
    memset(buf,0,sizeof(buf));
    sprintf(buf,"\nGL868 send_http_get() exit\n") ;
    uart_puts(1,buf);

    return 0 ;
}

static uint8_t gl868_send_http_post(const char* host, const char* data) {
    memset(buf,0,sizeof(buf));
    sprintf(buf,"GL868 send_http_post() entry\n") ;
    uart_puts(1,buf);

    char request[GL868_POST_REQUEST_SIZE] ;
    memset(&request[0], 0 , sizeof(request));

    uint16_t dlen = strlen(data);
    char ds[4] ;

    if(dlen > GL868_POST_BODY_SIZE) {
        return GL868_POST_BODY_ERROR ;
    }

    /*
     * POST body size can be 512 bytes (1/2 kb)
     *
     * 34 +64+24+14+4 = 140  chars
     *
     * from HTTP/1.1 RFC2616
     * Request-Line  = Method SP Request-URI SP HTTP-Version CRLF
     *
     */

    strcat(request, "POST /api/gl868v2.php HTTP/1.1 \r\n");
    strcat(request, "content-type:application/x-www-form-urlencoded;charset=utf-8 \r\n");
    strcat(request,"Host: ");
    strcat(request,host);
    strcat(request," \r\n");

    strcat(request,"content-length: ");
    itoa(dlen+5,ds,10);
    strcat(request,ds);

    strcat(request," \r\n\r\n");

    // 5 chars
    strcat(request,"data=");
    strcat(request,data);
    gl868_send(request);

    uart_puts(1,request);
    memset(buf,0,sizeof(buf));
    sprintf(buf,"\nGL868 send_http_post() exit\n") ;
    uart_puts(1,buf);

    return 0 ;
}

/*
 * check signal strength
 * 10->14 OK, 14->19 Good, 20->31 excellent
 * 41 means no response from modem.
 *
 */

static uint8_t gl868_signal_level() {
    memset(buf,0,sizeof(buf));
    sprintf(buf,"GL868 signal_level() entry \n") ;
    uart_puts(1,buf);

    uint8_t level = 41 ;
    uint8_t code  = gl868_send_command("AT+CSQ", "OK",5000UL);

    if(code) {
        // +CSQ: 15,0
        char* b = strstr(_buffer, "+CSQ:");
        // we need chars at b+6 and b+7

        if(b != NULL && (strlen(_buffer) >= 8)) {

            char sb[3] ;
            sb[0] = b[6] ;

            if(b[7] != ',') {
                sb[1] = b[7] ;
                sb[2] = 0 ;
            } else {
                sb[1] = 0 ;
                sb[2] = 0 ;
            }

            level = atoi(sb);
        }
    }
    memset(buf,0,sizeof(buf));
    sprintf(buf,"signal %d\n signal_level() exit\n",level);
    uart_puts(1,buf);
    return level ;
}

static void gl868_flush() {
    // flush modem buffer    
    uart_putc(0,12);
}

static void gl868_send(const char *buffer) {
    uart_puts(0,buffer);
}

static void gl868_send_char(unsigned char ch) {
    uart_putc(0,ch);
}

/*
 * @param command AT command
 * return success on finding token
 * @return 1 on success 0 on error
 *
 */
static uint8_t gl868_send_command(const char* command, const char* token, unsigned long timeout) {
    uint8_t code = 0 ;
    // reset this->_buffer
    memset(&_buffer[0], 0 , sizeof(_buffer));

    // modem needs at least 20ms between commands */
    for(int i = 0 ; i < 100; i++)
        Delay1KTCYx(1);

    gl868_flush();
    memset(buf,0,sizeof(buf));
    sprintf(buf,"GL868 >> %s\n", command);
    uart_puts(1,buf);

    // send modem command
    uart_puts(0,command);
    uart_puts(0,"\r\n");

    // wait a bit
    Delay1KTCYx(1);

    // fire modem command and forget: modem will return error
    // caller is not expected to handle return code when token == __NULL__
    if(strcmp(token, "__NULL__") == 0 ){
        memset(buf,0,sizeof(buf));
        sprintf(buf,"called with __NULL__ token: send command and return \n") ;
        uart_puts(1,buf);
        return 0 ;
    }

    // get modem response
    code = gl868_read_buffer(timeout, token);
    return code ;
}

static uint8_t gl868_read_buffer(unsigned long timeout, const char* token) {

    uint8_t code = 0 ;

    // reset buffer
    memset(&_buffer[0], 0 , sizeof(_buffer));
    _buffer[0] = '\0' ;
    uint8_t count = 1 ;

    unsigned long start_time = millis();
    unsigned long end_time = millis() + timeout;
    Delay1KTCYx(1);

    volatile unsigned long current_time = millis() ;

    while((current_time - start_time) <= timeout) {

        gl868_read_line(end_time - current_time) ;
        uint8_t len = strlen(_line) ;

        if( (count + len) < (GL868_BUFFER_SIZE -1)) {
            // can fit: copy line into buffer
            strcat(_buffer,_line);
            count = count + len ;

        } else {
            goto buffer_overflow ;
        }

        if(strstr(_buffer,token) != NULL) {
            code = 1 ; goto quit ;
        }

        if(strstr(_buffer, "ERROR") != NULL) {
            code = 0 ; goto quit ;
        }

        current_time = millis();
    }

    // token not found till timeout
    gl868_flush();
    return code ;

buffer_overflow:
    memset(buf,0,sizeof(buf));
    sprintf(buf,"modem buffer overflow: \n");
    uart_puts(1,buf);
    uart_puts(1,_line);
    gl868_flush();
    return code ;

quit:
    gl868_flush();
    return code ;
}

/*
 * read modem response line-by-line
 * stop on [ token | error | timeout ]
 * @return 0 on error and 1 on success
 *
 */
static uint8_t gl868_read_lines(unsigned long timeout, const char* token) {
    uint8_t code = 0  ;
    uint8_t check_token = 1 ;

    volatile unsigned long start_time = millis() ;
    volatile unsigned long end_time = millis() + timeout;
    Delay1KTCYx(1) ;

    volatile unsigned long current_time = millis();

    if(strcmp(token, "__NULL__") == 0 ){
        check_token = 0 ;
    }

    while((current_time - start_time) <= timeout) {

        gl868_read_line(end_time - current_time) ;

        if(check_token) {
            // found token
            if(strstr(_line,token) != NULL) {
                code = 1 ; goto quit ;
            }
        }

        if(strstr(_line, "ERROR") != NULL) {
            code = 0 ; goto quit ;
        }

        Delay1KTCYx(1);
        current_time = millis();
    }

    // did not find token or error
    gl868_flush();
    return code ;

quit:
    gl868_flush();
    return code ;
}

// read a line of modem response into line_buffer
// line is terminated on GL868_LINE_SIZE or \n
static void gl868_read_line(unsigned long timeout) {
    char ch = 0 ;
    int count = 0 ;

    // reset line buffer
    memset(&_line[0], 0 , sizeof(_line));
    volatile unsigned long start_time = millis() ;
    Delay1KTCYx(1);

    volatile unsigned long current_time = millis();

    while((current_time - start_time) <= timeout) {

        if(DataRdyUSART()) {
            ch = uart_getc(0);
            _line[count] = ch ;
            count++ ;
        }

        if(ch == '\n' || (count == GL868_LINE_SIZE -1)) {
            break ;
        }

        current_time = millis();
    }

    // either timeout or we encountered EOL
    _line[count] = '\0';
    
    memset(buf,0,sizeof(buf));
    sprintf(buf,"modem >> ");
    uart_puts(1,buf);
    uart_puts(1,_line) ;
    uart_puts(1,'\n');
}

static void gl868_skip_http_headers(unsigned long timeout) {
    char ch = -1 ;
    char pch = -1 ;
    char nch ;

    volatile unsigned long start_time = millis() ;
    Delay1KTCYx(1);
    volatile unsigned long current_time = millis();

    /* Host will send headers followed by a blank line and then body
     * HEADERS CRLF
     * CRLF
     * BODY
     *
     * break out of stream when we get LFCRLF pattern
     */

    while((current_time - start_time) <= timeout) {
        if(DataRdyUSART()) {
            ch = uart_getc(0);
            // LF-CR-LF
            if((pch == '\n') && (ch == '\r')) {
                nch = uart_getc(0);
                if(nch == '\n')
                    break ;
            }

            pch = ch ;
        }

        current_time = millis();
    }

    return ;
}

void gsm_set_socket_host(char *host,char *port) {
    socket_host = host ;
    socket_port = port ;
}

void gsm_init(uint8_t port, const char *baud_rate, uint8_t dport) {
    _debug_port = dport ;

    gl868_set_port(port);
    gl868_set_debug_port(dport);
    if(baud_rate != NULL) {
       gl868_set_baud_rate(baud_rate);
    }

    // modem initialization, wait 4 seconds
    for(int i = 0 ; i < 4000; i++)
        Delay1KTCYx(1);

    // initialize variables
    _fsm_halt_time = millis();
    _fsm_halt_interval = 1UL ;
    Delay1KTCYx(1) ;

    _fsm_halt_flag = 0 ;
    _busy_flag = 0;
}

static void gsm_halt_fsm(uint8_t seconds) {
    gl868_diagnosis();
    gl868_set_state(GL868_FSM_HALT);
    _fsm_halt_time = millis() ;
    _fsm_halt_interval =  ((unsigned long)(1000UL* seconds));
    _fsm_halt_flag = 1 ;

    memset(buf,0,sizeof(buf));
    sprintf(buf,"GL868 state machine halted for %d seconds\n",seconds);
    uart_puts(1,buf);
}

static void gsm_init_response() {
    response.signal = 0 ;
    response.network = 0 ;
    response.last_on = 0 ;
    response.fsm_halt = 0 ;
    response.reboot = 0 ;
    response.gprs = 0 ;
    response.socket = 0 ;
    response.scode = 500 ;
    response.error = 0 ;
    response.t_time = 0 ;
}

struct gl868_response_t gsm_get_response() {
    return response ;
}

/*
 * @param timeout : timeout in seconds, e.g. 90
 * @return : code 1 on success, 0 on error
 *
 */

uint8_t gsm_connect(int timeout) {

    uint8_t code = 1 ;
    // initialize response
    gsm_init_response();

    if(_busy_flag) {
        memset(buf,0,sizeof(buf));
        sprintf(buf,"Abort: last transmission in progress\n");
        uart_puts(1,buf);
        response.last_on = 1 ;
        return 0 ;
    }

    memset(buf,0,sizeof(buf));
    sprintf(buf,"GL868 transmission entry() \n");
    uart_puts(1,buf);
    unsigned long current_time = millis() ;

    // use ((current_time - start_time) > interval) pattern to fix millis() rollover
    if(_fsm_halt_flag && ((current_time - _fsm_halt_time) <= _fsm_halt_interval)) {
        memset(buf,0,sizeof(buf));
        sprintf(buf,"Abort: GL868 state machine is halted \n");
        uart_puts(1,buf);
        response.fsm_halt = 1 ;
        return 0 ;

    } else {
        // run the state machine
        _fsm_halt_flag = 0 ;
        response.fsm_halt = 0 ;
    }

    Delay1KTCYx(5);
    // start processing
    _busy_flag = 1;

    volatile uint8_t state ;
    unsigned long milli_timeout = timeout *1000UL ;

    // assume that modem is switched On
    gl868_set_state(GL868_ON);
    state = gsm_run_fsm(GL868_ON);

    unsigned long start_time = millis() ;
    Delay1KTCYx(5);
    current_time = millis();

    while((state != GL868_CONNECTED) && ((current_time - start_time) <= milli_timeout)) {

        state = gsm_run_fsm(state);
        if(state == GL868_FSM_HALT) {
            break;
        }

        // artificial delay before next fsm run
        for(int j = 0 ; j < 100; j++)
            Delay1KTCYx(1);

        memset(buf,0,sizeof(buf));
        sprintf(buf,"GL868 state %d timeout in %lu \n",state, (current_time - start_time));
        uart_puts(1,buf);
        current_time = millis();
    }

    _busy_flag = 0;
    code = (state == GL868_CONNECTED) ? 1 : 0 ;
    return code ;
}

/*
 * @param method HTTP Method [GET|POST]
 * @param data   POST Body or GET URL
 * @param response : server response line
 * @return 1: if we get +YUKTIX: line 0: otherwise
 *
 */

uint8_t gsm_send_http(const char *method,char *data, char *nginx_response) {
    volatile uint8_t state ;
    uint8_t error = 0 ;

    memset(nginx_response, 0, sizeof(nginx_response));
    if(_busy_flag) {
        memset(buf,0,sizeof(buf));
        sprintf(buf,"Abort: last transmission in progress\n");
        uart_puts(1,buf);
        response.last_on = 1 ;
        goto quit ;
    }

    _busy_flag = 1;
    state = gl868_get_state();
    if(state !=  GL868_CONNECTED) {
        response.socket = 0;
        return 0 ;
    }

    // GPRS connection is active and we have a socket open to server
    if(strcmp(method,"GET") == 0) {
        error = gl868_send_http_get(socket_host,data);
    } else if(strcmp(method,"POST") == 0 ) {
        error = gl868_send_http_post(socket_host,data);
    } else {
        error = GL868_WRONG_HTTP_METHOD_ERROR ;
    }

    if(error) {
        memset(buf,0,sizeof(buf));
        sprintf(buf,"send_http() error %d \n",error);
        uart_puts(1,buf);
        response.error = error ;
        response.scode = 400;
        goto quit ;
    }

    gl868_skip_http_headers(20000UL);

    if(gl868_read_lines(3000UL,"+YUKTIX:")) {

        strncpy(nginx_response, gl868_get_line(), GL868_LINE_SIZE);
        nginx_response[GL868_LINE_SIZE-1] = '\0' ;

        response.scode = gsm_get_nginx_response_code(nginx_response);
        response.t_time = millis();
        goto success ;
    }

    // @imp: wait for 2s after getting +YUKTIX: line
    gl868_read_lines(2000UL, "__NULL__");
    _busy_flag = 0;
    return 0 ;

quit:
    // @imp: wait for 2s after getting +YUKTIX: line
    gl868_read_lines(2000UL, "__NULL__");
    _busy_flag = 0 ;
    return 0 ;

success:
    // @imp: wait for 2s after getting +YUKTIX: line
    gl868_read_lines(2000UL, "__NULL__");
    _busy_flag = 0 ;
    return 1 ;
}

uint8_t gsm_disconnect() {
    uint16_t loop ;
    uint8_t code = 0 ;
    char escape[4] ;

    // null terminated for UART
    escape[0] = '+' ;
    escape[1] = '+' ;
    escape[2] = '+' ;
    escape[3] = '\0' ;

    /*
     * for MT to detect +++ as escape sequence to suspend socket
     * we need to provide pause before and after +++ as set by
     * ATS12 command (default is 1s)
     * <pause> +++ </pause>
     *
     * use modem.send() to flush +++ without \r\n
     * send_command adds CRLF chars and so we actually send +++\r\n
     * and that is different from required escape sequence (+++)
     *
     *
     */

    for(loop = 1100 ; loop > 0 ; loop--) Delay1KTCYx(1) ;
    gl868_send(escape);

    code = gl868_read_lines(2000UL, "OK");
    for(loop = 1100 ; loop > 0 ; loop--) Delay1KTCYx(1) ;

    // close the socket
    gl868_send_command("AT#SH=1", "__NULL__",1000UL) ;
    code = gl868_read_lines(2000UL, "OK");

    // deactivate GPRS context
    gl868_deactivate_gprs();
    gl868_set_state(GL868_ON);
    _busy_flag = 0 ;
    return code;
}

static uint16_t gsm_get_nginx_response_code(char *response) {
    uint16_t scode = 500 ;
    // scan modem response lines for +YUKTIX: token
    // we need chars between b+9 => next space
    // YUKTIX code is guaranteed to be a 3 digit number followed by SP
    char* b = strstr(response, "+YUKTIX:");

    if(b != NULL && (strlen(response) >= 12)) {
        char sb[4] ;
        sb[0] = b[9] ;
        sb[1] = b[10] ;
        sb[2] = b[11] ;
        sb[3] = '\0' ;
        scode = atoi(sb);
    }
    return scode ;
}

void gsm_get_sd_response(char *buffer) {
    strncpy(buffer, gl868_get_sd_response(), GL868_LINE_SIZE);
}

void gsm_get_response_in_buffer(char *buffer) {
    // initialize buffer before use
    memset(&buffer[0], 0 , sizeof(buffer));

    sprintf(buffer,"SIGNAL:%02d,NETWORK:%1d,ABORT:%1d,HALT:%1d,REBOOT:%1d,GPRS:%1d,SOCKET:%1d,CODE:%03d",
            response.signal,
            response.network,
            response.last_on,
            response.fsm_halt,
            response.reboot,
            response.gprs,
            response.socket,
            response.scode);

            buffer[68] = '\n' ;
            buffer[69] = '\0' ;
}

static int gsm_run_fsm(uint8_t state) {
    uint8_t network = 0 ;
    uint8_t signal = 0 ;
    uint8_t i = 0;
    uint8_t attempts = 1 ;
    int intervals[4];
    int delta ;
    uint8_t retcode = 0 ;

    switch(state) {
        case GL868_OFF :
            gl868_switch_on();
            gl868_init();
            if(!gl868_is_ready()) {
                // wait 2s
                for(int j = 0 ; j < 2000; j++)
                    Delay1KTCYx(1);
                gl868_init();
            }

            response.reboot = 1 ;
            break;

        case GL868_ON :
            gl868_init();
            if(gl868_is_ready())
                break ;

            if(!gl868_is_ready())
                gl868_set_state(GL868_OFF);
            break ;

        case GL868_READY:
            i = 0 ;
            retcode = 0 ;

            // make 4 tries for signal at 2,5,8,15 seconds interval
            intervals[0] = 2000 ;
            intervals[1] = 5000 ;
            intervals[2] = 8000 ;
            intervals[3] = 15000 ;

            attempts = 4 ;

            do {
                signal = gl868_signal_level();
                if((9 <= signal) && (signal <= 31)) {
                    gl868_set_state(GL868_GOOD_SIGNAL);
                    retcode = 1 ;
                    break ;
                }

            // signal can drop for no good reason
            //      wait before next try

                delta = intervals[i] ;
                for(int j =0 ; j < delta ; j++)
                    Delay1KTCYx(1);

            } while(i++ < attempts);

            response.signal = signal ;
            if(!retcode) {
                // no signal: halt fsm for 120 seconds
                gsm_halt_fsm(113);
            }

            break;

        case GL868_GOOD_SIGNAL :
            network = gl868_check_network_registration();
            response.network = network ;

                // we need Home or Roaming: can't do much about network
            if(network == 1 || network == 5) {
                gl868_set_state(GL868_NETWORK_REGISTERED);
            } else {
                // no network: halt fsm for 240 seconds
                // stay within uint8_t limits 
                gsm_halt_fsm(241);
            }
            break;

        case GL868_NETWORK_REGISTERED:
            if(gl868_check_gprs_state() == 1) {
                gl868_set_state(GL868_GPRS_ACTIVE);
                response.gprs = 1 ;
            } else {
                gl868_set_state(GL868_GPRS_INACTIVE);
                response.gprs = 0 ;
            }
            break;

        case GL868_GPRS_INACTIVE :
            retcode = 0 ;
            i = 0 ;
            attempts = 3 ;

            intervals[0] = 4000 ;
            intervals[1] = 8000 ;
            intervals[2] = 15000 ;
            intervals[3] = 15000 ;

            do {
                if(gl868_activate_gprs()) {
                    gl868_set_state(GL868_GPRS_ACTIVE);
                    retcode = 1 ;
                    break;
                }

                // GPRS OFF : wait before next try
                delta = intervals[i] ;

                for(int j = 0 ; j < delta ; j++)
                    Delay1KTCYx(1);

            } while (i++ < attempts);

            if(!retcode) {
                // check GPRS after 2 minutes
                gsm_halt_fsm(241);
            }
            break;

        case GL868_GPRS_ACTIVE :
            // open_socket has built-in waiting for CONNECT
            if(gl868_open_socket(socket_host, socket_port)) {
                gl868_set_state(GL868_CONNECTED);
                response.socket = 1 ;
            } else {
                gsm_halt_fsm(241);
            }
            break;

        default:
            break ;
    }

    state = gl868_get_state();
    return state ;
}


