/*
 * read_thread.h
 *
 *  Created on: 26/ago/2014
 *      Author: mauro
 */

#ifndef READ_THREAD_H_
#define READ_THREAD_H_

#include <ch.h>

#include "l3g4200d.h"
#include "lsm303dlhc.h"

#define EVENT_GYRO_READY 0b00000001
#define EVENT_ACCE_READY 0b00000010
#define EVENT_MAGN_READY 0b00000100

/*
 * Working area for the read thread.
 */
static WORKING_AREA(readThreadWorkingArea, 128);

static volatile uint16_t event_read = 0;
static volatile uint16_t event_read_acce = 0;
static volatile uint16_t event_read_gyro = 0;
static volatile uint16_t event_read_magn = 0;

static Thread *readThreadPointer;
/*
 * read thread. This thread's heart beat LED is RED
 */
static msg_t readThread(void *arg) {
	(void) arg;
	/*
	 * start the sensors!
	 */

	accelerometer_init();
	magnetometer_init();

	magnetometer_read();
	gyroscope_read();
	accelerometer_read();

	//systime_t start = 0;
	//systime_t counteg;

	while (TRUE) {

		/* wait for a ready from acc or gyro or magne */
		eventmask_t event = chEvtWaitAny(EVENT_GYRO_READY | EVENT_ACCE_READY | EVENT_MAGN_READY);

		event_read++;

		//counteg = RTT2US( halGetCounterValue() - start );
		//start = halGetCounterValue();

		if ( (event & (EVENT_ACCE_READY) ) != 0) {
			//TODO: why accelerometer does not work with its interrupt?
			//TODO: debug
			//accelerometer_read();
		}

		if ( (event & EVENT_MAGN_READY) != 0 ){
			//TODO: debug
			magnetometer_read();
		}

		if ( (event & EVENT_GYRO_READY) != 0) {
			gyroscope_read();
			//TODO: why accelerometer does not work with its interrupt?
			accelerometer_read();
		}
	}
	/* WHAT?! should NEVER be HERE! */
	return RDY_RESET;
}

/*INTERRUPT MANAGMENT*/
void gyroscope_read_ready(EXTDriver *extp, expchannel_t channel) {
	/* Wakes up the thread.*/
	(void) extp;
	(void) channel;

	//CH_IRQ_PROLOGUE();

	event_read_gyro++;

	chSysLockFromIsr();
	chEvtSignalI(readThreadPointer, (eventmask_t) EVENT_GYRO_READY);
	chSysUnlockFromIsr();

	//CH_IRQ_EPILOGUE();

}

/*INTERRUPT MANAGMENT*/
void accelerometer_read_ready(EXTDriver *extp, expchannel_t channel) {
	/* Wakes up the thread.*/
	(void) extp;
	(void) channel;

	//CH_IRQ_PROLOGUE();

	event_read_acce++;

	chSysLockFromIsr();
	chEvtSignalI(readThreadPointer, (eventmask_t) EVENT_ACCE_READY);
	chSysUnlockFromIsr();

	//CH_IRQ_EPILOGUE();

}

/*INTERRUPT MANAGMENT*/
void magnetometer_read_ready(EXTDriver *extp, expchannel_t channel) {
	/* Wakes up the thread.*/
	(void) extp;
	(void) channel;

	//CH_IRQ_PROLOGUE();

	event_read_magn++;

	chSysLockFromIsr();
	chEvtSignalI(readThreadPointer, (eventmask_t) EVENT_MAGN_READY);
	chSysUnlockFromIsr();

	//CH_IRQ_EPILOGUE();

}

#endif /* READ_THREAD_H_ */
