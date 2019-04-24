#include "SHT15.h"

#define READ_TEMP_CMD 0x03
#define READ_HUMIDITY_CMD 0x05
#define SOFTWARE_RESET_CMD 0x1E
#define READ_STATUS_CMD 0x07

#define SCL_PIN_MASK 0x01
#define SDA_PIN_MASK 0x02

static void connection_reset();
static unsigned int send_command(unsigned int command);
static unsigned int wait_for_response();
static unsigned int  get_sht15_data(unsigned int *value);
static unsigned int read_byte();
static void end_transmission();
static unsigned int send_ack();
static void send_byte(unsigned int val);
static void send_bit(unsigned int val);
static void start_sequence();
static void software_reset();
static void clock_input_mode();
static void clock_output_mode();
static void data_input_mode();
static void data_output_mode();
static void write_data_high();
static void write_data_low();
static void write_clock_high();
static void write_clock_low() ;
static unsigned int read_data();
static void clock_delay();
static void data_delay(); 

unsigned int sht15_read_temperature(float *temperature) {

    // coefficients for 14 bit measurements @ 3.3V
    const float D1 = -39.65 ;
    const float D2 = 0.01 ;
    unsigned int value ;
    unsigned int error ;

    error = send_command(READ_TEMP_CMD);
    if(error)
        goto quit ;

    error = get_sht15_data(&value);
    if(error)
        goto quit ;

    *temperature = ((((float) value) * D2) + D1);
    return 0;

quit:
    *temperature = 0.0 ;
    return error ;
}

unsigned int sht15_read_humidity(float *humidity) {

    // coefficients for 12bit humidity measurements
    const float C1 = -2.0468 ;
    const float C2 =  0.0367;
    const float C3 = -0.0000015955;
    const float T1 =  0.01;
    const float T2 =  0.00008;

    float linear_humidity;
    float temperature;
    float corrected_humidity;

    unsigned int value;
    unsigned int error ;

    error = send_command(READ_HUMIDITY_CMD);
    if(error) {
        goto quit ;
    }

    error = get_sht15_data(&value);
    if(error)
        goto quit ;

    linear_humidity = C1 + C2 * value + C3 * value * value;
    sht15_read_temperature(&temperature);
    corrected_humidity = (temperature - 25.0 ) * (T1 + T2 *((float)value)) + linear_humidity;
    *humidity = corrected_humidity ;
    return 0;

quit:

    *humidity = 0.0 ;
    return error ;
}

static unsigned int sht15_read_status(unsigned int *status) {

    unsigned int error ;
    error = send_command(READ_STATUS_CMD);
    if(error) {
        *status = 0 ;
       return error ;
    }

    *status = read_byte();
    return 0 ;
}

static void start_sequence(){

   // pull up resistor will  pull the data pin HIGH.
   data_output_mode();
   write_data_high(); data_delay();

   clock_output_mode();
   write_clock_low(); clock_delay();
   write_clock_high(); clock_delay();

   write_data_low(); data_delay();

   write_clock_low(); clock_delay();
   write_clock_high(); clock_delay();

   write_data_high(); data_delay();
   write_clock_low(); clock_delay();
}

static void send_bit(unsigned int val) {

    if(val) {
        write_data_high();
    } else {
        write_data_low();
    }

    data_delay();
    write_clock_high();
    clock_delay();

    write_clock_low();
    clock_delay();
}

static void send_byte(unsigned int val) {

    unsigned int i = 0 ;

    for(i = 0 ; i < 8 ; i++) {
        send_bit(!!(val & 0x80)) ;
        val <<= 1 ;
    }
}

static unsigned int send_command(unsigned int command) {

    unsigned int error ;

    start_sequence();
    send_byte(command);
    error = wait_for_response();

    // @todo - do we need xtra CLK pulse?
    write_clock_high();
    clock_delay();

    write_clock_low();
    clock_delay();

    // error
    if(error) {
        return SHT15_CMD_ERROR ;
    }
    return 0;
}

// @return 1: error 0:success
static unsigned int wait_for_response() {

    unsigned int ack ;
    data_input_mode();

    // command ACK should arrive in 100ms
    for(int j = 0 ; j < 100 ; j++) {
        Delay1KTCYx(1);
        ack = read_data();
        // data line is low
        if(!ack)
            goto success ;
    }

    // error
    write_clock_low();
    connection_reset();
    return 1 ;

success:

    write_clock_low();
    clock_delay();

    return 0 ;
}

static unsigned int send_ack() {

    unsigned int code;
    data_output_mode();
    write_data_low(); data_delay();

    write_clock_high(); data_delay();
    write_clock_low(); data_delay();

    write_data_high();
    data_input_mode();
    code = read_data();
    return code ;
}

static void end_transmission() {

    data_output_mode();
    write_data_high(); data_delay();

    write_clock_high(); clock_delay();
    write_clock_low(); clock_delay();

    return ;
}

static unsigned int get_sht15_data(unsigned int *value) {

    unsigned int ack;
    unsigned int error = 1 ;

    // data should be low
    data_input_mode();
    // restart clock
    clock_output_mode();
    write_clock_low();

   /*
    * wait for result | maximum measurement delays
    * 8 bit : 20ms
    * 12 bit: 80ms
    * 14 bit: 320ms
    *
    */

    for(int j = 0 ; j < 300 ; j++)
        Delay1KTCYx(1);

    for(int k = 0 ; k < 500 ; k++) {
        Delay1KTCYx(1);
        ack = read_data();
        if(!ack) {
            // success
            error = 0 ;
            break  ;
        }
    }

    if(error) {
        *value = 0 ;
        return  SHT15_GET_DATA_ERROR;
    }

    for(int j = 0 ; j < 16 ; ++j ){
        *value <<= 1 ;
        write_clock_high();
        clock_delay();
        *value |=  !!read_data();
        write_clock_low();
        clock_delay();
        if(j == 7 ) {
            send_ack();
        }
    }

    end_transmission();
    return error;
}

static void connection_reset() {

    data_output_mode();
    write_data_high();
    clock_delay();

    clock_output_mode();
    write_clock_low();
    clock_delay();

    for (unsigned int i = 0; i < 9; i++) {
        write_clock_high();
        clock_delay();

        write_clock_low();
        clock_delay();
    }
}

static void software_reset() {
    // not implemented yet
}

static unsigned int read_byte() {

    unsigned int i;
    unsigned int value = 0;
    data_input_mode();

    for (i = 0; i < 8; ++i) {
        value <<= 1;
        write_clock_high(); clock_delay();
        value |= !! read_data();
        write_clock_low(); clock_delay();
    }
    return value;
}

// port manipulation
static void clock_input_mode() {
    TRISB = TRISB | SCL_PIN_MASK ;
    // enable pullup register
    INTCON2bits.NOT_RBPU = 0;
    Delay1TCYx(10);
}

static void clock_output_mode() {
    TRISB = TRISB & (~SCL_PIN_MASK);
    Delay1TCYx(10);
}

static void data_input_mode() {
    TRISB = TRISB | SDA_PIN_MASK ;
    // enable pullup resistor
    INTCON2bits.NOT_RBPU = 0;
    Delay1TCYx(10);
}

static void data_output_mode() {
    TRISB = TRISB & (~SDA_PIN_MASK);
    Delay1TCYx(10);
}

static void write_data_high() {
    PORTB = PORTB | SDA_PIN_MASK ;
    Delay1TCYx(10);
}

static void write_data_low() {
    PORTB = PORTB & (~SDA_PIN_MASK) ;
    Delay1TCYx(10);
}

static void write_clock_high() {
    PORTB = PORTB | SCL_PIN_MASK ;
    Delay1TCYx(10);
}

static void write_clock_low() {
    PORTB = PORTB & (~SCL_PIN_MASK) ;
    Delay1TCYx(10);
}

static unsigned int read_data() {
    return (PORTB & SDA_PIN_MASK) ;
}

static void clock_delay() {
    Delay100TCYx(1) ;    
}

static void data_delay() {
    Delay100TCYx(1);
    Delay10TCYx(5);
}
