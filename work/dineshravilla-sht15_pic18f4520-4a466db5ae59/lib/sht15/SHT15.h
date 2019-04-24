/* 
 * File:   SHT15.h
 * Author: Dinesh
 *
 * Created on February 17, 2015, 2:47 PM
 */

#ifndef SHT15_H
#define	SHT15_H

#include <xc.h>
#include <delays.h>
#include "config.h"

#define SHT15_NO_ERROR 0
#define SHT15_CMD_ERROR 1
#define SHT15_GET_DATA_ERROR 2


extern unsigned int sht15_read_humidity(float *humidity);
extern unsigned int sht15_read_temperature(float *temperature);
extern unsigned int sht15_read_status(unsigned int *status);

#endif