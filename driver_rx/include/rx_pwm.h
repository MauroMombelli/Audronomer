/*
 * pwm.h
 *
 *  Created on: 01/mar/2015
 *      Author: mauro
 */

#ifndef DRIVER_ESC_INCLUDE_ESC_PWM_H_
#define DRIVER_ESC_INCLUDE_ESC_PWM_H_


#include "ch.h"
#include "hal.h"

void pwm_init(void);
void set_pwm_motor(pwmchannel_t, pwmcnt_t);

#endif /* DRIVER_ESC_INCLUDE_ESC_PWM_H_ */
