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

struct PPM_Channel_Specific { //this is specific board to board
	GPIO_TypeDef *gpio;
	uint8_t pin;

	PWMConfig *config;
	PWMDriver *driver;
	uint16_t mode;
	pwmchannel_t channel;
};

static struct PPM_Channel {
	pwmcnt_t width;

	uint16_t min;
	uint16_t max;

} channels[] = {
	{0, 800, 1900},
	{0, 800, 1900},
	{0, 800, 1900},
	{0, 800, 1900},
};

static const size_t CHANNEL_NUMBER = sizeof(channels)/sizeof(channels[0]);

uint8_t pwm_init(void);
void set_pwm_motor(pwmchannel_t, pwmcnt_t);

#endif /* DRIVER_ESC_INCLUDE_ESC_PWM_H_ */
