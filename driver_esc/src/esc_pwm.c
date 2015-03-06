/*
 * pwm.c
 *
 *  Created on: 01/mar/2015
 *      Author: mauro
 */

#include "esc_pwm.h"


struct Engine engines[] = {
	{GPIOD, 3, &PWMD2, 0, 0, 800, 1900},
	{GPIOD, 4, &PWMD2, 1, 0, 800, 1900},
	{GPIOD, 6, &PWMD2, 2, 0, 800, 1900},
	{GPIOD, 7, &PWMD2, 3, 0, 800, 1900},
};
static const uint8_t ENGINE_NUMBER = sizeof(engines)/sizeof(struct Engine);

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

void pwm_init(void){

	uint8_t i;
	for (i=0; i < ENGINE_NUMBER; i++){
		palSetPadMode(engines[i].gpio, engines[i].pin, PAL_MODE_ALTERNATE(2));
	}

	/*TIMER 2*/
	for (i=0; i < ENGINE_NUMBER; i++){
		pwmStart(engines[i].driver, &pwmcfg2);
	}
	for (i=0; i < ENGINE_NUMBER; i++){
		pwmEnableChannel(engines[i].driver, engines[i].channel, pwmcfg2.frequency/2); //50%
	}
}

void set_pwm_to_channel(pwmchannel_t channel, pwmcnt_t width){

	pwmEnableChannel(&PWMD2, channel, width);
}

void set_pwm(uint8_t index, pwmcnt_t width){

	if (index >= ENGINE_NUMBER){
		return;
	}
	/*force MIN_WIDTH_PWM <= width <= MAX_WIDTH_PWM */
	width = width<engines[index].min?engines[index].min:width;
	width = width>engines[index].max?engines[index].max:width;

	engines[index].width = width;

	pwmEnableChannel(engines[index].driver, engines[index].channel, engines[index].width);
}
