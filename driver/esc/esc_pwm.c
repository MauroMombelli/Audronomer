/*
 * pwm.c
 *
 *  Created on: 01/mar/2015
 *      Author: mauro
 */

#include "../esc/esc_pwm.h"

/* TODO: move this things to a file witch contains the board specific configuration! */

static PWMConfig pwmcfg2 = {
	1000000, /* 1Mhz PWM clock frequency */
	2500, /* PWM period 20000 tick */
	NULL,  /* No callback */
	/* All channel disabled, they will be enabled at the init only if needed */
	{
		{PWM_OUTPUT_ACTIVE_HIGH, NULL},
		{PWM_OUTPUT_ACTIVE_HIGH, NULL},
		{PWM_OUTPUT_ACTIVE_HIGH, NULL},
		{PWM_OUTPUT_ACTIVE_HIGH, NULL},
	},
	0,
	0
};

static const struct PPM_Channel_Specific channels_hardware[] = {
	{GPIOD, 3, &pwmcfg2, &PWMD2, PAL_MODE_ALTERNATE(2), 0},
	{GPIOD, 4, &pwmcfg2, &PWMD2, PAL_MODE_ALTERNATE(2), 1},
	{GPIOD, 6, &pwmcfg2, &PWMD2, PAL_MODE_ALTERNATE(2), 2},
	{GPIOD, 7, &pwmcfg2, &PWMD2, PAL_MODE_ALTERNATE(2), 3},
};
/* end TODO*/

/*
 * Because channels and channels_hardware are parallel array, we MUST be sure they have the same size.
 * And i love compile time check
 */
_Static_assert(sizeof(channels)/sizeof(channels[0]) == sizeof(channels_hardware)/sizeof(channels_hardware[0]), "Number of virtual channels does not match declared Hardware channels");

uint8_t pwm_init(void){
	uint8_t i;
	for (i=0; i < CHANNEL_NUMBER; i++){
		palSetPadMode(channels_hardware[i].gpio, channels_hardware[i].pin, channels_hardware[i].mode);
		if (channels_hardware[i].driver->state != PWM_STOP){
			pwmObjectInit(channels_hardware[i].driver);
		}
	}

	for (i=0; i < CHANNEL_NUMBER; i++){
		if (channels_hardware[i].driver->state == PWM_STOP){
			pwmStart(channels_hardware[i].driver, channels_hardware[i].config);
		}
		chDbgAssert((channels_hardware[i].config->channels[channels_hardware[i].channel].mode == PWM_OUTPUT_ACTIVE_HIGH), "drone_pwm_init(), #1", "invalid mode");
	}
	for (i=0; i < CHANNEL_NUMBER; i++){
		pwmEnableChannel(channels_hardware[i].driver, channels_hardware[i].channel, channels_hardware[i].config->frequency/2); //50%
	}
	return 0;
}

void set_pwm(uint8_t index, pwmcnt_t width){

	if (index >= CHANNEL_NUMBER){
		return;
	}
	/*force MIN_WIDTH_PWM <= width <= MAX_WIDTH_PWM */
	width = width < channels[index].min?channels[index].min:width;
	width = width > channels[index].max?channels[index].max:width;

	channels[index].width = width;

	pwmEnableChannel(channels_hardware[index].driver, channels_hardware[index].channel, channels[index].width);
}
