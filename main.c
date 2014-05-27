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

static const I2CConfig i2cconfig = { 0x00902025, //from lsm303dlhc.c
		0, 0 };

static const int LSM_ADDR_ACC = 0x19;
static const int LSM_ADDR_MAG = 0x1E;

msg_t lsm303dlh_init(void) {
	uint8_t buffer_tx[] = { 0x20, 0x77, 0x21, 0x9C, 0x23, 0xc8, };
	systime_t tmo = MS2ST(4);

	i2cStart(&I2CD1, &i2cconfig);

	i2cAcquireBus(&I2CD1);
	uint8_t i;
	for (i = 0; i < sizeof(buffer_tx) / 2; i++) {
		i2cMasterTransmitTimeout(&I2CD1, LSM_ADDR_ACC, buffer_tx + i * 2, 2, NULL, 0, tmo);
	}

	uint8_t check[29];
	buffer_tx[0] = 0x20 | 0x80;
	i2cMasterTransmitTimeout(&I2CD1, LSM_ADDR_ACC, buffer_tx, 1, check, 29, tmo);

	// Configuring Magnetometer

	uint8_t buffer_tx_mag[] = { 0x00, 0x18, 0x01, 0xE0, 0x02, 0x00 };

	for (i = 0; i < sizeof(buffer_tx_mag) / 2; i++) {
		i2cMasterTransmitTimeout(&I2CD1, LSM_ADDR_MAG, buffer_tx_mag + i * 2, 2, NULL, 0, tmo);
	}

	i2cMasterTransmitTimeout(&I2CD1, LSM_ADDR_MAG, buffer_tx_mag, 1, check, 1, tmo);
	buffer_tx_mag[0]++;
	i2cMasterTransmitTimeout(&I2CD1, LSM_ADDR_MAG, buffer_tx_mag, 1, check, 1, tmo);
	buffer_tx_mag[0]++;
	i2cMasterTransmitTimeout(&I2CD1, LSM_ADDR_MAG, buffer_tx_mag, 1, check, 1, tmo);

	i2cReleaseBus(&I2CD1);

	i2cStop(&I2CD1);

	return RDY_OK;
}

msg_t lsm303dlh_read_acceleration(void) {

	msg_t status = RDY_OK;

	uint8_t buffer_tx;
	uint8_t buffer_reg[5] = { 1, 2, 3, 4, 5 };
	uint8_t buffer_rx[7] = { 1, 2, 3, 4, 5, 6, 7 };

	systime_t tmo = MS2ST(4);

	i2cStart(&I2CD1, &i2cconfig);

	buffer_tx = 0x20 | 0x80; //0x80 means read multiple value, see 7.1.2
	i2cAcquireBus(&I2CD1);
	status = i2cMasterTransmitTimeout(&I2CD1, LSM_ADDR_ACC, &buffer_tx, 1, buffer_reg, 5, tmo);

	buffer_tx = 0x27 | 0x80;
	status = i2cMasterTransmitTimeout(&I2CD1, LSM_ADDR_ACC, &buffer_tx, 1, buffer_rx, 7, tmo);
	i2cReleaseBus(&I2CD1);

	i2cStop(&I2CD1);

	//buffer_rx[0] contains data ready bit
	int16_t accelX = buffer_rx[1];
	accelX = accelX << 8;
	accelX |= buffer_rx[2];

	int16_t accelY = buffer_rx[3];
	accelY = accelY << 8;
	accelY |= buffer_rx[4];

	int16_t accelZ = buffer_rx[5];
	accelZ = accelZ << 8;
	accelZ |= buffer_rx[6];

	return status;
}

msg_t lsm303dlh_read_magfield(void) {
	uint8_t buffer_rx[7];
	uint8_t buffer_tx;
	msg_t status;
	systime_t tmo = MS2ST(4);

	i2cStart(&I2CD1, &i2cconfig);

	buffer_tx = 0x03;
	i2cAcquireBus(&I2CD1);
	status = i2cMasterTransmitTimeout(&I2CD1, LSM_ADDR_MAG, &buffer_tx, 1, buffer_rx, 7, tmo);
	i2cReleaseBus(&I2CD1);
	i2cStop(&I2CD1);

	//buffer_rx[7] ontains data ready bit
	int16_t magnX = (int16_t) ((buffer_rx[0] << 8) | buffer_rx[1]);
	int16_t magnZ = (int16_t) ((buffer_rx[2] << 8) | buffer_rx[3]);
	int16_t magnY = (int16_t) ((buffer_rx[4] << 8) | buffer_rx[5]);

	return status;
}

int main(void) {

	halInit();
	chSysInit();

	palSetPadMode(GPIOE, GPIOE_LED3_RED, PAL_MODE_OUTPUT_PUSHPULL);

	palSetPad(GPIOE, GPIOE_LED3_RED);

	lsm303dlh_init();
	palClearPad(GPIOE, GPIOE_LED3_RED);

	//uint8_t accerometer_data[6];
	//uint8_t stato = 0;
	while (TRUE) {
		lsm303dlh_read_acceleration();
		lsm303dlh_read_magfield();
		palSetPad(GPIOE, GPIOE_LED3_RED);
		chThdSleepMilliseconds(50);palClearPad(GPIOE, GPIOE_LED3_RED);
		chThdSleepMilliseconds(50);
	}
}

