/*
 * lsm303dlh.h
 *
 *  Created on: May 28, 2014
 *      Author: mauro
 */

#ifndef LSM303DLHC_H_
#define LSM303DLHC_H_

#include "../../Dependencies/TrigonomeC/my_math.h"
#include "ch.h"
#include "hal.h"


msg_t accelerometer_init(void);
msg_t accelerometer_read(struct Vector3i16*);

uint8_t accelerometer_interrupt_mode(void);
uint8_t accelerometer_interrutp_port(void);
uint8_t accelerometer_ext_pin(void);

msg_t magnetometer_init(void);
msg_t magnetometer_read(struct Vector3i16*);

#endif /* LSM303DLH_H_ */
