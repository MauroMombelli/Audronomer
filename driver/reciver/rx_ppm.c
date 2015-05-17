/*
 * pwm.c
 *
 *  Created on: 01/mar/2015
 *      Author: mauro
 */

#include "../reciver/rx_ppm.h"

void ppm_init(void){
	/*
	palSetPadMode(GPIOD, 3, PAL_MODE_ALTERNATE(2));
	palSetPadMode(GPIOD, 4, PAL_MODE_ALTERNATE(2));
	palSetPadMode(GPIOD, 6, PAL_MODE_ALTERNATE(2));
	palSetPadMode(GPIOD, 7, PAL_MODE_ALTERNATE(2));

	pwmStart(&PWMD2, &pwmcfg2);

	pwmEnableChannel(&PWMD2, 0, pwmcfg2.frequency/2); //50%
	pwmEnableChannel(&PWMD2, 1, pwmcfg2.frequency/2); //50%
	pwmEnableChannel(&PWMD2, 2, pwmcfg2.frequency/2); //50%
	pwmEnableChannel(&PWMD2, 3, pwmcfg2.frequency/2); //50%
	*/
}
