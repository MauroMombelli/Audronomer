/*
 * l3g4200d.h
 *
 *  Created on: May 28, 2014
 *      Author: mauro
 */

#ifndef L3G4200D_H_
#define L3G4200D_H_

#include "ch.h"
#include "hal.h"

#include "static_db/engine_db.h"
#include "my_math/my_math.h"

msg_t gyroscope_init(void);
msg_t gyroscope_read(void);

uint8_t gyroscope_interrupt_mode(void);
uint8_t gyroscope_interrutp_port(void);
uint8_t gyroscope_ext_pin(void);

#endif /* L3G4200D_H_ */
