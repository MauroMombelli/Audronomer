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

#include "dcm/dcm.h"
#include "mixer/mixer.h"
#include "driver_rx/rx_ppm.h"

#include "sensors_thread/read_thread.h"

#include "static_db/engine_db.h"

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

struct {
	const uint16_t START_FREQUENCY;
	uint16_t packet_sent;
	const uint8_t START[8];
} commProtocol = { .START = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff },
		.START_FREQUENCY = 1000, .packet_sent = 0 };

void protocol_send_start(void) {
	//SDU1.vmt->writet(&SDU1, commProtocol.START, sizeof(commProtocol.START), 1000);
	//chIOPut(&SD1, i);
	chnWriteTimeout(&SD1, commProtocol.START, sizeof(commProtocol.START), 1000);
	//uartStartSend(&UARTD1, sizeof(commProtocol.START), commProtocol.START);
}

void protocol_send(uint8_t type, uint8_t* data, size_t size) {
	commProtocol.packet_sent++;
	if (commProtocol.packet_sent >= commProtocol.START_FREQUENCY) {
		protocol_send_start();
		commProtocol.packet_sent = 0;
	}
	//SDU1.vmt->writet(&SDU1, &type, 1, 1000);
	//SDU1.vmt->writet(&SDU1, data, size, 1000);
	chnWriteTimeout(&SD1, &type, 1, 1000);
	chnWriteTimeout(&SD1, data, size, 1000);
	//uartStartSend(&UARTD1, 1, &type);
	//uartStartSend(&UARTD1, size, data);
}

void send(struct sensors_data* tmp) {
	protocol_send('d', (uint8_t *) tmp, 6);
}

void send_gyro(struct Vector3f* tmp) {
	protocol_send('g', (uint8_t *) tmp, 6);
}

void send_acce(struct Vector3i16* tmp) {
	protocol_send('a', (uint8_t *) tmp, 6);
}

void send_magne(struct Vector3i16* tmp) {
	protocol_send('m', (uint8_t *) tmp, 6);
}

void send_dcm_quaternion(struct Quaternion4f *tmp) {
	protocol_send('q', (uint8_t *) tmp, sizeof(float) * 4);
}

void i2c_init(void) {
	/* set pin for i2c */
	palSetPadMode(GPIOB, 6, PAL_MODE_ALTERNATE(4) | PAL_STM32_OTYPE_OPENDRAIN); /* SCL */
	palSetPadMode(GPIOB, 7, PAL_MODE_ALTERNATE(4) | PAL_STM32_OTYPE_OPENDRAIN); /* SDA */
}

int main(void) {
	//setup external interrupt channels
	uint8_t i;
	for (i = 0; i < EXT_MAX_CHANNELS; i++) {
		//extcfg.channels[i].mode = EXT_CH_MODE_DISABLED; //TODO: set to disable. THIS IS ONLY FOR DEBUG!
		extcfg.channels[i].mode = EXT_CH_MODE_RISING_EDGE
				| EXT_CH_MODE_AUTOSTART | EXT_MODE_GPIOE;
		extcfg.channels[i].cb = unkonw_interrupt;
	}

	expchannel_t gyro_channel = 1;
	extcfg.channels[gyro_channel].mode = EXT_CH_MODE_RISING_EDGE
			| EXT_CH_MODE_AUTOSTART | EXT_MODE_GPIOE;
	extcfg.channels[gyro_channel].cb = gyroscope_read_ready;

	expchannel_t magne_channel = 2;
	extcfg.channels[magne_channel].mode = EXT_CH_MODE_RISING_EDGE
			| EXT_CH_MODE_AUTOSTART | EXT_MODE_GPIOE;
	extcfg.channels[magne_channel].cb = magnetometer_read_ready;

	expchannel_t acce_channel = 4;
	extcfg.channels[acce_channel].mode = EXT_CH_MODE_RISING_EDGE
			| EXT_CH_MODE_AUTOSTART | EXT_MODE_GPIOE;
	extcfg.channels[acce_channel].cb = accelerometer_read_ready;

	halInit();
	chSysInit();

	init_static_generics();

	i2c_init();

	dcm_init();

	pwm_init();

	palSetPadMode(GPIOC, 4, PAL_MODE_ALTERNATE(7)); // used function : USART1_TX
	palSetPadMode(GPIOC, 5, PAL_MODE_ALTERNATE(7)); // used function : USART1_RX

	sdInit();

	sdStart(&SD1, NULL);

	//PREPARE LED RED
	palSetPadMode(GPIOE, GPIOE_LED3_RED, PAL_MODE_OUTPUT_PUSHPULL); palSetPad(GPIOE, GPIOE_LED3_RED);

	/*
	 * START READ THREAD
	 */
	readThreadPointer = chThdCreateStatic(readThreadWorkingArea,
			sizeof(readThreadWorkingArea),
			NORMALPRIO, /* Initial priority.    */
			readThread, /* Thread function.     */
			NULL); /* Thread parameter.    */

	// start external interrupt handler
	extStart(&EXTD1, &extcfg);

	// start external interrupt specific channels
	for (i = 0; i < EXT_MAX_CHANNELS; i++) {
		extChannelEnable(&EXTD1, i);
	}

	protocol_send_start();

	/*
	 * LETS THE LOGIC BEGIN
	 */
	int8_t padStatus = 0;

	systime_t start = chTimeNow(), elapsed;

	uint8_t lastUpdate = 0;
	struct sensors_data sensor;

	uint8_t update, diff;

	while (TRUE) {

		//maybe an interrupt is better :)
		chThdYield();

		update = get_sensors_data(&sensor);

		diff = update - lastUpdate;

		if (diff) {
			lastUpdate = update;

			send(&sensor);

			//time to run the DCM!
			dcm_step(sensor.gyro);
			struct Quaternion4f q;
			dcm_get_quaternion(&q);

			send_dcm_quaternion(&q);

			struct Vector3f desired = { 0, 0, 0 };
			doMixer(q, desired);

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
		}
	}
}

