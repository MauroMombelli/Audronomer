/*
 * lsm303dlh.c
 *
 *  Created on: May 28, 2014
 *      Author: mauro
 */


#include "lsm303dlh.h"

static const I2CConfig i2cconfig = { 0x00902025, //from lsm303dlhc.c
		0,
		0 };

static const int LSM_ADDR_ACC = 0x19;
static const int LSM_ADDR_MAG = 0x1E;

#define ACCELEROMETER_USE_INTERRUPT TRUE
#define MAGNETOMETER_USE_INTERRUPT FALSE

volatile uint8_t read_accelerometer = 0;

msg_t accelerometer_init(void) {
	//acc
	uint8_t buffer_tx[] = { 0x20, 0x77, 0x21, 0x90, 0x23, 0xc8, };

	systime_t tmo = MS2ST(4);

	uint8_t i;

	msg_t ris;

	i2cAcquireBus(&I2CD1);
	i2cStart(&I2CD1, &i2cconfig);

	//Configure Accelerometer
	for (i = 0; i < sizeof(buffer_tx) / 2; i++) {
		ris |= i2cMasterTransmitTimeout(&I2CD1, LSM_ADDR_ACC, buffer_tx + i * 2, 2, NULL, 0, tmo);
	}

	i2cStop(&I2CD1);
	i2cReleaseBus(&I2CD1);

	return ris;
}

msg_t accelerometer_read(void) {

	msg_t status;

	uint8_t buffer_tx = 0x27 | 0x80; //0x80 is for multiple read, as by datasheet
	uint8_t buffer_rx[7];

	systime_t tmo = MS2ST(4);



	i2cAcquireBus(&I2CD1);
	i2cStart(&I2CD1, &i2cconfig);
	status = i2cMasterTransmitTimeout(&I2CD1, LSM_ADDR_ACC, &buffer_tx, 1, buffer_rx, sizeof(buffer_rx), tmo); //please, sizeof works only on array, never use with pointer, even if pointing array
	i2cStop(&I2CD1);
	i2cReleaseBus(&I2CD1);



	//buffer_rx[0] contains data ready bit
	if ( status == RDY_OK && (buffer_rx[0] & 0x8) ){
		/*
		values[0] = ((int16_t)((uint16_t)buffer_rx[1] << 8) + buffer_rx[2]);
		values[1] = ((int16_t)((uint16_t)buffer_rx[3] << 8) + buffer_rx[4]);
		values[2] = ((int16_t)((uint16_t)buffer_rx[5] << 8) + buffer_rx[6]);
		*/
	}else{
		//values[0] = values[1] = values[2] = 0;
	}

	return status;
}

uint8_t accelerometer_interrupt_mode(void){
	return EXT_CH_MODE_RISING_EDGE;
}
uint8_t accelerometer_interrutp_port(void){
	return EXT_MODE_GPIOE;
}
uint8_t accelerometer_ext_pin(void){
	return 2;
}
void accelerometer_interrutp(EXTDriver *extp, expchannel_t channel) {
	(void)extp;
	(void)channel;
	read_accelerometer = 1;
}
extcallback_t accelerometer_interrutp_callback(void){
	return accelerometer_interrutp;
}

msg_t magnetometer_init(void) {

	//magne
	uint8_t buffer_tx_mag[] = { 0x00, 0x18, 0x01, 0xE0, 0x02, 0x00 };

	systime_t tmo = MS2ST(4);

	uint8_t i;

	msg_t ris;

	i2cAcquireBus(&I2CD1);
	i2cStart(&I2CD1, &i2cconfig);

	// Configure Magnetometer
	for (i = 0; i < sizeof(buffer_tx_mag) / 2; i++) {
		ris |= i2cMasterTransmitTimeout(&I2CD1, LSM_ADDR_MAG, buffer_tx_mag + i * 2, 2, NULL, 0, tmo);
	}
	i2cStop(&I2CD1);
	i2cReleaseBus(&I2CD1);

	return ris;
}

msg_t magnetometer_read(void) {
	uint8_t buffer_rx[7];
	uint8_t buffer_tx = 0x03;
	msg_t status;

	systime_t tmo = MS2ST(4);

	i2cStart(&I2CD1, &i2cconfig);

	i2cAcquireBus(&I2CD1);
	status = i2cMasterTransmitTimeout(&I2CD1, LSM_ADDR_MAG, &buffer_tx, 1, buffer_rx, 7, tmo);
	i2cReleaseBus(&I2CD1);

	i2cStop(&I2CD1);

	//buffer_rx[6] ontains data ready bit
	if ( status == RDY_OK /*&& (buffer_rx[6] & 0x1) */){//does not seems to work
		/*
		values[0] = ((int16_t)((uint16_t)buffer_rx[0] << 8) + buffer_rx[1]);
		values[2] = ((int16_t)((uint16_t)buffer_rx[2] << 8) + buffer_rx[3]);
		values[1] = ((int16_t)((uint16_t)buffer_rx[4] << 8) + buffer_rx[5]);
		*/
	}else{
		//values[0] = values[1] = values[2] = 0;
	}

	return status;
}
