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
#include <stdlib.h>
#include <inttypes.h>

#include "ch.h"
#include "hal.h"

#include "usbcfg.h"

#include "engine_db.h"

#include "read_thread.h"

#include "dcm.h"

/* External interrupt configuration */
EXTConfig extcfg;

volatile uint16_t interruptCounter[EXT_MAX_CHANNELS] = { 0 };

void unkonw_interrupt(EXTDriver *extp, expchannel_t channel) {
	/* Wakes up the thread.*/
	(void) extp;
	(void) channel;

	interruptCounter[channel]++;

}

#define usb_lld_connect_bus(usbp)
#define usb_lld_disconnect_bus(usbp)
/* Virtual serial port over USB.*/
SerialUSBDriver SDU1;

int main(void) {
	//setup external interrupt channels
	uint8_t i;
	for (i = 0; i < EXT_MAX_CHANNELS; i++) {
		//extcfg.channels[i].mode = EXT_CH_MODE_DISABLED; //TODO: set to disable. THIS IS ONLY FOR DEBUG!
		extcfg.channels[i].mode = EXT_CH_MODE_RISING_EDGE | EXT_CH_MODE_AUTOSTART | EXT_MODE_GPIOE;
		extcfg.channels[i].cb = unkonw_interrupt;
	}

	expchannel_t gyro_channel = 1;
	extcfg.channels[gyro_channel].mode = EXT_CH_MODE_RISING_EDGE | EXT_CH_MODE_AUTOSTART | EXT_MODE_GPIOE;
	extcfg.channels[gyro_channel].cb = gyroscope_read_ready;

	expchannel_t magne_channel = 2;
	extcfg.channels[magne_channel].mode = EXT_CH_MODE_RISING_EDGE | EXT_CH_MODE_AUTOSTART | EXT_MODE_GPIOE;
	extcfg.channels[magne_channel].cb = magnetometer_read_ready;

	expchannel_t acce_channel = 4;
	extcfg.channels[acce_channel].mode = EXT_CH_MODE_RISING_EDGE | EXT_CH_MODE_AUTOSTART | EXT_MODE_GPIOE;
	extcfg.channels[acce_channel].cb = accelerometer_read_ready;

	/*ACCE??
	 expchannel_t acce_channel = 4;
	 extcfg.channels[acce_channel].mode = EXT_CH_MODE_AUTOSTART | accelerometer_interrupt_mode() | accelerometer_interrutp_port();
	 extcfg.channels[acce_channel].cb = accelerometer_read_ready;
	 */

	halInit();
	chSysInit();

	init_static_generics();

	dcm_init();

	sduObjectInit(&SDU1);
	sduStart(&SDU1, &serusbcfg);
	usbDisconnectBus(serusbcfg.usbp);
	chThdSleepMilliseconds(1000);
	usbStart(serusbcfg.usbp, &usbcfg);
	usbConnectBus(serusbcfg.usbp);

	chThdSleepMilliseconds(2000);

	SDU1.vmt->writet(&SDU1, (uint8_t * )"start", 5, 1000);

	/*
	 * Activates the serial driver 1, PA9 and PA10 are routed to USART1.
	 */
	/*
	 uartObjectInit(&UARTD1);
	 uartStart(&UARTD1, &uart_cfg_1);
	 palSetPadMode(GPIOA, 9, PAL_MODE_ALTERNATE(7)); // USART1 TX.
	 palSetPadMode(GPIOA, 10, PAL_MODE_ALTERNATE(7)); // USART1 RX.


	 sdStart(&SD1, &sd1_config);
	 */
	/*
	 * Starts the transmission, it will be handled entirely in background.
	 */
	//uartStartSend(&UARTD1, 13, "Starting...\r\n");
	//PREOPARE LED RED
	palSetPadMode(GPIOE, GPIOE_LED3_RED, PAL_MODE_OUTPUT_PUSHPULL);

	/* set pin for i2c */
	palSetPadMode(GPIOB, 6, PAL_MODE_ALTERNATE(4) | PAL_STM32_OTYPE_OPENDRAIN); /* SCL */
	palSetPadMode(GPIOB, 7, PAL_MODE_ALTERNATE(4) | PAL_STM32_OTYPE_OPENDRAIN); /* SDA */

	palSetPad(GPIOE, GPIOE_LED3_RED);

	/*
	 * START READ THREAD
	 */
	readThreadPointer = chThdCreateStatic(readThreadWorkingArea, sizeof(readThreadWorkingArea),
	NORMALPRIO, /* Initial priority.    */
	readThread, /* Thread function.     */
	NULL); /* Thread parameter.    */

	// start external interrupt handler
	extStart(&EXTD1, &extcfg);

	// start external interrupt specific channels
	for (i = 0; i < EXT_MAX_CHANNELS; i++) {
		extChannelEnable(&EXTD1, i);
	}

	/*
	 * LETS THE LOGIC BEGIN
	 */
	int8_t padStatus = 0;

	//systime_t tmo = TIME_INFINITE; //no timeout

	systime_t start = chTimeNow(), elapsed;

	uint8_t lastUpdateG = 0;
	uint8_t lastUpdateA = 0, lastUpdateM = 0;
	struct raw_gyroscope tmp_gyro;
	struct raw_accelerometer tmp_acce;
	struct raw_magnetometer tmp_magne;

	uint8_t update, diff;
	uint16_t g = 0;
	uint16_t m = 0, a = 0;

	uint16_t tmpOut = -32768;

	while (TRUE) {

		//maybe an interrupt is better :)
		chThdYield();
		update = get_raw_gyroscope(&tmp_gyro);

		diff = update - lastUpdateG;

		systime_t timeout_write = 10;
		if (diff) {
			lastUpdateG = update;

			tmpOut = -32768;
			SDU1.vmt->writet(&SDU1, (uint8_t * )&tmpOut, 2, timeout_write);
			SDU1.vmt->writet(&SDU1, (uint8_t * )&tmp_gyro, 6, timeout_write);

			g += diff;

			if (diff > 1) {
				tmpOut = -32765;
				SDU1.vmt->writet(&SDU1, (uint8_t * )&tmpOut, 2, timeout_write);
				SDU1.vmt->writet(&SDU1, (uint8_t * )&tmp_gyro, 6, timeout_write);
			}
/*
			//time to run the DCM!
			struct vector3f tmp;
			const float dps = 17.5f;
			const float degree_to_radiant = 0.0174532925f;
			tmp.x = ( (tmp_gyro.x * dps) / 1000.0 ) * degree_to_radiant;
			tmp.y = ( (tmp_gyro.y * dps) / 1000.0 ) * degree_to_radiant;
			tmp.z = ( (tmp_gyro.z * dps) / 1000.0 ) * degree_to_radiant;
			dcm_step(tmp);
			union quaternion q;
			dcm_get_quaternion(&q);
			tmpOut = -32762;
			SDU1.vmt->writet(&SDU1, (uint8_t * )&tmpOut, 2, timeout_write);
			SDU1.vmt->writet(&SDU1, (uint8_t * )&q, 16, timeout_write);
*/
		}

		update = get_raw_accelerometer(&tmp_acce);

		diff = update - lastUpdateA;
		if (diff) {
			tmpOut = -32767;
			SDU1.vmt->writet(&SDU1, (uint8_t * )&tmpOut, 2, timeout_write);
			SDU1.vmt->writet(&SDU1, (uint8_t * )&tmp_acce, 6, timeout_write);
			a += diff;
			lastUpdateA = update;
			if (diff > 1) {
				tmpOut = -32764;
				SDU1.vmt->writet(&SDU1, (uint8_t * )&tmpOut, 2, timeout_write);
				SDU1.vmt->writet(&SDU1, (uint8_t * )&tmp_acce, 6, timeout_write);
			}
		}

		update = get_raw_magnetometer(&tmp_magne);

		diff = update - lastUpdateM;
		if (diff) {
			tmpOut = -32766;
			SDU1.vmt->writet(&SDU1, (uint8_t * )&tmpOut, 2, timeout_write);
			SDU1.vmt->writet(&SDU1, (uint8_t * )&tmp_magne, 6, timeout_write);

			m += diff;
			lastUpdateM = update;
			if (diff > 1) {
				tmpOut = -32763;
				SDU1.vmt->writet(&SDU1, (uint8_t * )&tmpOut, 2, timeout_write);
				SDU1.vmt->writet(&SDU1, (uint8_t * )&tmp_magne, 6, timeout_write);
			}
		}

		elapsed = chTimeElapsedSince(start);
		if (elapsed >= S2ST(1)) { //every second
			start = chTimeNow();
			if (padStatus) {
				palSetPad(GPIOE, GPIOE_LED3_RED);
			} else {
				palClearPad(GPIOE, GPIOE_LED3_RED);
			}
			padStatus = !padStatus;

			uint8_t i;
			for (i = 0; i < EXT_MAX_CHANNELS; i++) {
				interruptCounter[i] = 0;
			}
			/*
			 USBSendData((uint8_t *) "S", 1, tmo);
			 USBSendData((uint8_t *) &event_read_gyro, 2, tmo);
			 USBSendData((uint8_t *) &event_read_acce, 2, tmo);
			 uint16_t tmp = event_read; //should be atomic
			 g = a = m = event_read = event_read_gyro = event_read_acce = 0;
			 USBSendData((uint8_t *) &tmp, 2, tmo); //send number of event read
			 */

		}
	}
}

