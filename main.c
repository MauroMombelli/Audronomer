/*
 ChibiOS/RT - Copyright (C) 2006-2013 Giovanni Di Sirio

 Licensed under the Apache License, Version 2.0 (the "License");
 you may not use this file except in compliance with the License.
 You may obtain a copy of the License at

 http://www.apache.org/licenses/LICENSE-2.0

 Unless required by applicable law or agreed to in writing, software
 distributed under the License is distributed on an "AS IS" BASIS,
 WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 See the License for the specific language governing permissions and
 limitations under the License.
 */

#include "ch.h"
#include "hal.h"

#include "lsm303dlh.h"
#include "l3g4200d.h"
#include "math.h"

#include "myusb.h"

#include "chprintf.h"
/*
 * DP resistor control is not possible on the STM32F3-Discovery, using stubs
 * for the connection macros.
 */
#define usb_lld_connect_bus(usbp)
#define usb_lld_disconnect_bus(usbp)

const uint8_t SIZE_EXTCFG = 1;
/* External interrupt configuration */
EXTConfig extcfg;

int main(void) {
	uint8_t i;
	for (i = 0; i < EXT_MAX_CHANNELS; i++) {
		extcfg.channels[i].mode = EXT_CH_MODE_DISABLED;
		extcfg.channels[i].cb = NULL;
	}

	halInit();
	chSysInit();

	/*
	 * Activates the USB driver and then the USB bus pull-up on D+.
	 * Note, a delay is inserted in order to not have to disconnect the cable
	 * after a reset.
	 */
	usbDisconnectBus(SDU1);
	chThdSleepMilliseconds(1500);
	usbStart(serusbcfg.usbp, &usbcfg);
	usbConnectBus(SDU1);

	palSetPadMode(GPIOE, GPIOE_LED3_RED, PAL_MODE_OUTPUT_PUSHPULL);

	/* set pin for i2c */
	palSetPadMode(GPIOB, 6, PAL_MODE_ALTERNATE(4) | PAL_STM32_OTYPE_OPENDRAIN);   /* SCL */
	palSetPadMode(GPIOB, 7, PAL_MODE_ALTERNATE(4) | PAL_STM32_OTYPE_OPENDRAIN);   /* SDA */

	palSetPad(GPIOE, GPIOE_LED3_RED);

	/*
	 * start listening the sensors
	 */

#ifdef GIROSCOPE_USE_INTERRUPT
	expchannel_t gyro_channel = gyroscope_ext_pin();
	extcfg.channels[gyro_channel].mode |= EXT_CH_MODE_AUTOSTART | gyroscope_interrupt_mode() | gyroscope_interrutp_port();
	extcfg.channels[gyro_channel].cb = gyroscope_interrutp_callback();
#else //not GIROSCOPE_USE_INTERRUPT	//setup temporized
#endif//end GIROSCOPE_USE_INTERRUPT#ifdef ACCELEROMETER_USE_INTERRUPT
	expchannel_t acce_channel = accelerometer_ext_pin();
	extcfg.channels[acce_channel].mode |= EXT_CH_MODE_AUTOSTART | accelerometer_interrupt_mode() | accelerometer_interrutp_port();
	extcfg.channels[gyro_channel].cb = accelerometer_interrutp_callback();
#else //not ACCELEROMETER_USE_INTERRUPT	//setup temporized
#endif//end ACCELEROMETER_USE_INTERRUPT#ifdef MAGNETOMETER_USE_INTERRUPT
	expchannel_t magne_channel = magnetometer_ext_pin();
	extcfg.channels[magne_channel].mode |= EXT_CH_MODE_AUTOSTART | magnetometer_interrupt_mode() | magnetometer_interrutp_port();
	extcfg.channels[magne_channel].cb = magnetometer_interrutp_callback();
#else //not MAGNETOMETER_USE_INTERRUPT	//setup temporized
#endif//end MAGNETOMETER_USE_INTERRUPT	extStart(&EXTD1, &extcfg);

#ifdef GIROSCOPE_USE_INTERRUPT
	extChannelEnable(&EXTD1, gyro_channel);
#endif

#ifdef ACCELEROMETER_USE_INTERRUPT
	extChannelEnable(&EXTD1, acce_channel);
#endif
#ifdef MAGNETOMETER_USE_INTERRUPT
	extChannelEnable(&EXTD1, magne_channel);
#endif

	/*
	 * start the sensors!
	 */
	gyroscope_init();
	accelerometer_init();
	magnetometer_init();
	palClearPad(GPIOE, GPIOE_LED3_RED);

	/*
	 * Initializes a serial-over-USB CDC driver.
	 */
	sduObjectInit(&SDU1);
	sduStart(&SDU1, &serusbcfg);

	int16_t gyroscope[3] = { 0 };
	int16_t accerometer[3] = { 1 };
	int16_t magnetometer[3] = { 2 };

	int8_t padStatus = 0;

	//uint8_t buff[30];
	//uint32_t written = 0;
	systime_t tmo = MS2ST(4);

	while (TRUE) {
		gyroscope_read(gyroscope); //range:
		accelerometer_read(accerometer); //range:
		magnetometer_read(magnetometer); //range: -2048 2047, -4096 = overflow

		//uint32_t distAcceSquared = sqrt(accerometer[0] * accerometer[0] + accerometer[1] * accerometer[1] + accerometer[2] * accerometer[2]);
		//uint32_t distMagneSquared = magnetometer[0] * magnetometer[0] + magnetometer[1] * magnetometer[1] + magnetometer[2] * magnetometer[2];

		//written = sprintf(buff, "G:%d,%d,%d\n", gyroscope[0], gyroscope[1], gyroscope[2]);
		//USBSendData("G:", 3, tmo);
		//written = sprintf(buff, "A:%d,%d,%d\n", accerometer[0], accerometer[1], accerometer[2]);
		//USBSendData(buff, written, tmo);
		//written = sprintf(buff, "M:%d,%d,%d\n", magnetometer[0], magnetometer[1], magnetometer[2]);
		//USBSendData(buff, written, TIME_IMMEDIATE);

		if (gyroscope[0] != 0 && gyroscope[1] != 0 && gyroscope[2] != 0) {
			USBSendData( (uint8_t *)"G:", 3, tmo );
			USBSendData( (uint8_t *)gyroscope, 2 * 3, tmo );		//2 byte x 3 value
		}

		if (accerometer[0] != 0 && accerometer[1] != 0 && accerometer[2] != 0) {
			USBSendData( (uint8_t *)"A:", 3, tmo );
			USBSendData( (uint8_t *)accerometer, 2 * 3, tmo );		//2 byte x 3 value
		}

		if (magnetometer[0] != 0 && magnetometer[1] != 0 && magnetometer[2] != 0) {
			USBSendData( (uint8_t *)"M:", 3, tmo );
			USBSendData( (uint8_t *)magnetometer, 2 * 3, tmo );		//2 byte x 3 value
		}

		//chprintf((BaseSequentialStream *)&SDU1, "letti: %d, %d\n", distAcceSquared, distMagneSquared);
		if (padStatus) {
			palSetPad(GPIOE, GPIOE_LED3_RED);
		} else {
			palClearPad(GPIOE, GPIOE_LED3_RED);
		}
		padStatus = !padStatus;
	}
}

