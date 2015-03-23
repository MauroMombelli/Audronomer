/*
 * pwm.c
 *
 *  Created on: 01/mar/2015
 *      Author: mauro
 */

#include "esc_pwm.h"


static const struct PPM_Channel channels[] = {
	{GPIOD, 3, &PWMD2, 0, 0, 800, 1900},
	{GPIOD, 4, &PWMD2, 1, 0, 800, 1900},
	{GPIOD, 6, &PWMD2, 2, 0, 800, 1900},
	{GPIOD, 7, &PWMD2, 3, 0, 800, 1900},
};
const uint8_t CHANNEL_NUMBER = sizeof(channels)/sizeof(channels[0]);

static const PWMConfig pwmcfg2 = {
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

void pwm_init(void){
	uint8_t i;
	for (i=0; i < CHANNEL_NUMBER; i++){
		palSetPadMode(engines[i].gpio, engines[i].pin, PAL_MODE_ALTERNATE(2));
	}

	/*TIMER 2*/
	for (i=0; i < CHANNEL_NUMBER; i++){
		pwmStart(channels[i].driver, &pwmcfg2);
	}
	for (i=0; i < CHANNEL_NUMBER; i++){
		pwmEnableChannel(channels[i].driver, channels[i].channel, pwmcfg2.frequency/2); //50%
	}
}

void set_pwm_to_channel(pwmchannel_t channel, pwmcnt_t width){
	pwmEnableChannel(&PWMD2, channel, width);
}

void set_pwm(uint8_t index, pwmcnt_t width){

	if (index >= CHANNEL_NUMBER){
		return;
	}
	/*force MIN_WIDTH_PWM <= width <= MAX_WIDTH_PWM */
	width = width < channels[index].min?channels[index].min:width;
	width = width > channels[index].max?channels[index].max:width;

	channels[index].width = width;

	pwmEnableChannel(channels[index].driver, channels[index].channel, channels[index].width);
}
