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

#include "math.h"

#include "read_thread.h"

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

	sduObjectInit(&SDU1);
	sduStart(&SDU1, &serusbcfg);
	usbDisconnectBus(serusbcfg.usbp);
	chThdSleepMilliseconds(1000);
	usbStart(serusbcfg.usbp, &usbcfg);
	usbConnectBus(serusbcfg.usbp);


	chSequentialStreamWrite(&SDU1, "start\r\n", 7);
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

	uint8_t lastUpdateG = 0, lastUpdateA = 0, lastUpdateM = 0;
	struct raw_gyroscope tmp_gyro;
	struct raw_accelerometer tmp_acce;
	struct raw_magnetometer tmp_magne;

	uint8_t update, diff;
	uint16_t g = 0, m = 0, a = 0;

	uint16_t tmpOut = -32768;
	while (TRUE) {

		//chThdSleepMicroseconds(5000); //MUST FIND A BETTER WAY!

		chThdYield();

		update = get_raw_gyroscope(&tmp_gyro);

		diff = update - lastUpdateG;
		if (diff) {
			//USBSendData((uint8_t *) "G", 1, tmo);
			//USBSendData((uint8_t *) &tmp_gyro, 2 * 3, tmo);		//2 byte x 3 value
			//chprintf((BaseSequentialStream *)&SDU1, "G%" PRIu16 " %" PRIu16 " %" PRIu16 "\n", tmp_gyro.x, tmp_gyro.y, tmp_gyro.z );
			//chSequentialStreamPut((BaseSequentialStream * )&SDU1, 'G');
			//chSequentialStreamWrite((BaseSequentialStream * )&SDU1, (const uint8_t * )&tmp_gyro, 6);
			//chThdSleepMicroseconds(87 * 1);
			tmpOut = -32768;
			chSequentialStreamWrite(&SDU1, &tmpOut, 2);
			chSequentialStreamWrite(&SDU1, &tmp_gyro, 6);
			//chThdSleepMicroseconds(87 * 6);

			g += diff;
			lastUpdateG = update;
			if (diff > 1) {
				//USBSendData((uint8_t *) "ESLOWG", 6, tmo);
				//USBSendData((uint8_t *) &diff, 1, tmo);
				tmpOut = -32765;
				chSequentialStreamWrite(&SDU1, &tmpOut, 2);
				chSequentialStreamWrite(&SDU1, &tmp_gyro, 6);
			}
		}

		update = get_raw_accelerometer(&tmp_acce);

		diff = update - lastUpdateA;
		if (diff) {
			//USBSendData((uint8_t *) "A", 1, tmo);
			//USBSendData((uint8_t *) &tmp_acce, 2 * 3, tmo);		//2 byte x 3 value
			//chprintf((BaseSequentialStream *)&SDU1, "A%" PRIu16 " %" PRIu16 " %" PRIu16 "\n", tmp_acce.x, tmp_acce.y, tmp_acce.z );
			//chSequentialStreamPut((BaseSequentialStream * )&SDU1, 'A');
			//chSequentialStreamWrite((BaseSequentialStream * )&SDU1, (const uint8_t * )&tmp_acce, 6);
			//uartStartSend(&UARTD1, 1, "A");
			//uartStartSend(&UARTD1, 6, &tmp_gyro);
			//chSequentialStreamWrite(&SDU1, "A", 1);
			//chThdSleepMicroseconds(87 * 1);
			//chSequentialStreamWrite(&SDU1, -32767, 2);
			tmpOut = -32767;
			chSequentialStreamWrite(&SDU1, &tmpOut, 2);
			chSequentialStreamWrite(&SDU1, &tmp_acce, 6);
			//chThdSleepMicroseconds(87 * 6);
			a += diff;
			lastUpdateA = update;
			if (diff > 1) {
				tmpOut = -32765;
				//USBSendData((uint8_t *) "ESLOWA", 6, tmo);
				//USBSendData((uint8_t *) &diff, 1, tmo);
				chSequentialStreamWrite(&SDU1, &tmpOut, 2);
				chSequentialStreamWrite(&SDU1, &tmp_acce, 6);
			}
		}

		update = get_raw_magnetometer(&tmp_magne);

		diff = update - lastUpdateM;
		if (diff) {
			//USBSendData((uint8_t *) "M", 1, tmo);
			//USBSendData((uint8_t *) &tmp_magne, 2 * 3, tmo);		//2 byte x 3 value
			//chprintf((BaseSequentialStream *)&SDU1, "M%" PRIu16 " %" PRIu16 " %" PRIu16 "\n", tmp_magne.x, tmp_magne.y, tmp_magne.z );
			//chSequentialStreamPut((BaseSequentialStream * )&SDU1, 'M');
			//chSequentialStreamWrite((BaseSequentialStream * )&SDU1, (const uint8_t * )&tmp_magne, 6);
			//chSequentialStreamWrite(&SDU1, "M", 1);
			//chThdSleepMicroseconds(87 * 1);
			//chSequentialStreamWrite(&SDU1, -32766, 2);
			tmpOut = -32766;
			chSequentialStreamWrite(&SDU1, &tmpOut, 2);
			chSequentialStreamWrite(&SDU1, &tmp_magne, 6);
			//chThdSleepMicroseconds(87 * 6);

			m += diff;
			lastUpdateM = update;
			if (diff > 1) {
				tmpOut = -32765;
				//USBSendData((uint8_t *) "ESLOWM", 6, tmo);
				//USBSendData((uint8_t *) &diff, 1, tmo);
				chSequentialStreamWrite(&SDU1, &tmpOut, 2);
				chSequentialStreamWrite(&SDU1, &tmp_magne, 6);
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

