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

static PWMConfig pwmcfg2 = {
  1000000, /* 1Mhz PWM clock frequency */
  2500, /* PWM period 20000 tick */
  NULL,  /* No callback */
  /* All channel enabled */
  {
    {PWM_OUTPUT_ACTIVE_HIGH, NULL},
    {PWM_OUTPUT_ACTIVE_HIGH, NULL},
    {PWM_OUTPUT_ACTIVE_HIGH, NULL},
    {PWM_OUTPUT_ACTIVE_HIGH, NULL},
  },
  0,
  0
};

static const pwmcnt_t MIN_WIDTH_PWM = 800;
static const pwmcnt_t MAX_WIDTH_PWM = 2000;

void pwm_init(void);
void set_pwm_motor(pwmchannel_t, pwmcnt_t);

#endif /* DRIVER_ESC_INCLUDE_ESC_PWM_H_ */
