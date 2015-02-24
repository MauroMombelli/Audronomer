/*
 * lsm303dlh.h
 *
 *  Created on: May 28, 2014
 *      Author: mauro
 */

#ifndef LSM303DLH_H_
#define LSM303DLH_H_

#include "ch.h"
#include "hal.h"

#include "engine_db.h"
#include "my_math.h"

msg_t accelerometer_init(void);
msg_t accelerometer_read(void);

uint8_t accelerometer_interrupt_mode(void);
uint8_t accelerometer_interrutp_port(void);
uint8_t accelerometer_ext_pin(void);

msg_t magnetometer_init(void);
msg_t magnetometer_read(void);

#endif /* LSM303DLH_H_ */
