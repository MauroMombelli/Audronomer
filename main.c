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

volatile uint8_t read_magne = 0, read_acce = 0, read_gyro = 0;

/**
 * @brief HMC5983 external interrupt callback.
 *
 * @param[in] extp Pointer to EXT Driver.
 * @param[in] channel EXT Channel whom fired the interrupt.
 */
void sensor_interrutp(EXTDriver *extp, expchannel_t channel) {
	(void) extp;
	if (channel == 2) {
		read_magne = read_acce = 1;
	} else if (channel == 1) {
		read_gyro = 1;
	} else {
		//error?
	}
}
const uint8_t SIZE_EXTCFG = 1;
/* External interrupt configuration */
static const EXTConfig extcfg[SIZE_EXTCFG];

int main(void) {
	uint8_t i, a;
	for (i=0; i<SIZE_EXTCFG; i++){
		for (a=0; a<EXT_MAX_CHANNELS; a++){
			extcfg[i][a].mode = EXT_CH_MODE_DISABLED;
			extcfg[i][a].cb = NULL;
		}
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

	/*TODO: check rigt pin and set them*/
	/*palSetPadMode(GPIOB, 8, PAL_MODE_ALTERNATE(4) | PAL_STM32_OTYPE_OPENDRAIN);   /* SCL */
	/*palSetPadMode(GPIOB, 9, PAL_MODE_ALTERNATE(4) | PAL_STM32_OTYPE_OPENDRAIN);   /* SDA */

	palSetPad(GPIOE, GPIOE_LED3_RED);

	/*
	 * start listening the sensors
	 */

	expchannel_t gyro_channel = gyroscope_ext_pin();
	if (gyro_channel >= 0 && gyro_channel < sizeof(extcfg)) { //waring, this sizeof may hit your ass
		extcfg[gyro_channel].mode |= EXT_CH_MODE_AUTOSTART | gyroscope_interrupt_mode() | gyroscope_interrutp_port();
		extcfg[gyro_channel].cb = gyroscope_interrutp_callback();
	}
	expchannel_t acce_channel = accelerometer_ext_pin();
	if (acce_channel >= 0 && acce_channel < sizeof(extcfg)) { //waring, this sizeof may hit your ass
		extcfg[acce_channel].mode |= EXT_CH_MODE_AUTOSTART | accelerometer_interrupt_mode() | accelerometer_interrutp_port();
		extcfg[gyro_channel].cb = accelerometer_interrutp_callback();
	}
	expchannel_t magne_channel = magnetometer_ext_pin();
	if (magneChannel >= 0 && magneChannel < sizeof(extcfg)) { //waring, this sizeof may hit your ass
		extcfg[magneChannel].mode |= EXT_CH_MODE_AUTOSTART | magnetometer_interrupt_mode() | magnetometer_interrutp_port();
		extcfg[gyro_channel].cb = magnetometer_interrutp_callback();
	}

	extStart(&EXTD1, &extcfg);

	if (gyro_channel >= 0 && gyro_channel < sizeof(extcfg)) { //waring, this sizeof may hit your ass
		extChannelEnable(&EXTD1, gyro_channel);
	}
	if (acce_channel >= 0 && acce_channel < sizeof(extcfg)) { //waring, this sizeof may hit your ass
		extChannelEnable(&EXTD1, acce_channel);
	}
	if (magneChannel >= 0 && magneChannel < sizeof(extcfg)) { //waring, this sizeof may hit your ass
		extChannelEnable(&EXTD1, magneChannel);
	}

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

	uint8_t buff[30];
	uint32_t written = 0;
	systime_t tmo = MS2ST(4);

	while (TRUE) {
		read_gyroscope(gyroscope); //range:
		read_acceleration(accerometer); //range:
		read_magetometer(magnetometer); //range: -2048 2047, -4096 = overflow

		uint32_t distAcceSquared = sqrt(accerometer[0] * accerometer[0] + accerometer[1] * accerometer[1] + accerometer[2] * accerometer[2]);
		uint32_t distMagneSquared = magnetometer[0] * magnetometer[0] + magnetometer[1] * magnetometer[1] + magnetometer[2] * magnetometer[2];

//		written = sprintf(buff, "G:%d,%d,%d\n", gyroscope[0], gyroscope[1], gyroscope[2]);
		//USBSendData("G:", 3, tmo);
//		written = sprintf(buff, "A:%d,%d,%d\n", accerometer[0], accerometer[1], accerometer[2]);
		//USBSendData(buff, written, tmo);
//		written = sprintf(buff, "M:%d,%d,%d\n", magnetometer[0], magnetometer[1], magnetometer[2]);
		//USBSendData(buff, written, TIME_IMMEDIATE);

		if (read_gyro) {
			read_gyro = !read_gyro;
			USBSendData("G:", 3, tmo);
		}

		if (read_acce) {
			read_acce = !read_acce;
			USBSendData("A:", 3, tmo);
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

