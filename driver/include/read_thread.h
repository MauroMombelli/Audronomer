/*
 * read_thread.h
 *
 *  Created on: 26/ago/2014
 *      Author: mauro
 */

#ifndef READ_THREAD_H_
#define READ_THREAD_H_

#include <ch.h>

#define EVENT_GYRO_READY 0b00000001
#define EVENT_ACCE_READY 0b00000010

/* calculate magnetometer period in US and translate it to systemclock*/
#define MAGNETOMETER_FREQUENCY 60
#define SYSTIME_BETWEEN_MAGNETOMETER_READ US2ST(1000000/MAGNETOMETER_FREQUENCY)

/*
 * Working area for the read thread.
 */
static WORKING_AREA(readThreadWorkingArea, 128);

/*
 * read thread. This thread's heart beat LED is RED
 */
static msg_t readThread(void *arg) {
	(void)arg;
	/*
	 * start the sensors!
	 */
	gyroscope_init();
	accelerometer_init();
	magnetometer_init();

	systime_t start_magne_read = chTimeNow(), elapsed;

	uint8_t missing_magne_reading_to_second = MAGNETOMETER_FREQUENCY;

	while (TRUE) {

		/* wait for a ready from acc or gyro */
		eventmask_t event = chEvtWaitAny(EVENT_GYRO_READY | EVENT_ACCE_READY);

		/* if we need to read accelerometer */
		if (event | EVENT_ACCE_READY) {
			/*check if we can also read magnetometer */
			elapsed = chTimeElapsedSince(start_magne_read);

			if (elapsed > SYSTIME_BETWEEN_MAGNETOMETER_READ) { //polling
				magnetometer_read();

				missing_magne_reading_to_second--;
				if (missing_magne_reading_to_second <= 0) {
					//led_red_blink(); //TODO: implement heart beat here
					missing_magne_reading_to_second = MAGNETOMETER_FREQUENCY;
				}

				start_magne_read = chTimeNow();
			} else {
				accelerometer_read();
			}

		}

		/* if we need to read gyroscope */
		if (event | EVENT_GYRO_READY) { //if we need to read gyroscope
			gyroscope_read();
		}
	}
	/* WHAT?! should NEVER be HERE! */
	return RDY_RESET;
}

Thread *readThreadPointer;

/*INTERRUPT MANAGMENT*/
void gyroscope_read_ready(EXTDriver *extp, expchannel_t channel) {
	/* Wakes up the thread.*/
	(void)extp;
	(void)channel;
	chSysLockFromIsr();
	chEvtSignalI(readThreadPointer, (eventmask_t) EVENT_GYRO_READY);
	chSysUnlockFromIsr();
}

/*INTERRUPT MANAGMENT*/
void accelerometer_read_ready(EXTDriver *extp, expchannel_t channel) {
	/* Wakes up the thread.*/
	(void)extp;
	(void)channel;
	chSysLockFromIsr();
	chEvtSignalI(readThreadPointer, (eventmask_t) EVENT_ACCE_READY);
	chSysUnlockFromIsr();
}

#endif /* READ_THREAD_H_ */
