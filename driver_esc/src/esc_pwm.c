/*
 * pwm.c
 *
 *  Created on: 01/mar/2015
 *      Author: mauro
 */

#include "esc_pwm.h"

void pwm_init(void){
	palSetPadMode(GPIOD, 3, PAL_MODE_ALTERNATE(2));
	palSetPadMode(GPIOD, 4, PAL_MODE_ALTERNATE(2));
	palSetPadMode(GPIOD, 6, PAL_MODE_ALTERNATE(2));
	palSetPadMode(GPIOD, 7, PAL_MODE_ALTERNATE(2));

	/*TIMER 2*/
	pwmStart(&PWMD2, &pwmcfg2);

	pwmEnableChannel(&PWMD2, 0, pwmcfg2.frequency/2); //50%
	pwmEnableChannel(&PWMD2, 1, pwmcfg2.frequency/2); //50%
	pwmEnableChannel(&PWMD2, 2, pwmcfg2.frequency/2); //50%
	pwmEnableChannel(&PWMD2, 3, pwmcfg2.frequency/2); //50%
}

void set_pwm_motor(pwmchannel_t channel, pwmcnt_t width){

	/*force MIN_WIDTH_PWM <= width <= MAX_WIDTH_PWM */
	width = width<MIN_WIDTH_PWM?MIN_WIDTH_PWM:width;
	width = width>MAX_WIDTH_PWM?MAX_WIDTH_PWM:width;

	pwmEnableChannel(&PWMD2, channel, width);
}
