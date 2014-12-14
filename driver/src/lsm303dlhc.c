/*
 * lsm303dlh.c
 *
 *  Created on: May 28, 2014
 *      Author: mauro
 */

#include "lsm303dlhc.h"

static const I2CConfig i2cconfig = { 0x00902025, //from lsm303dlhc.c
		0, 0 };

static const int LSM_ADDR_ACC = 0x19;
static const int LSM_ADDR_MAG = 0x1E;

#define ACCELEROMETER_USE_INTERRUPT TRUE
#define MAGNETOMETER_USE_INTERRUPT FALSE

uint8_t last_update_m=0;
uint8_t last_update_a=0;

msg_t accelerometer_init(void) {
	//acc
	uint8_t buffer_tx[] = { 0x20, 0x97, 0x22, 0x04, 0x2E, 0x20, 0x25, 0x08 };

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
	if (status == RDY_OK && (buffer_rx[0] & 0x8)) {
		struct raw_accelerometer tmp;

		//we use gyro as reference, deal with it!
		tmp.y = -((int16_t) ((uint16_t) buffer_rx[1] << 8) + buffer_rx[2]);
		tmp.x = ((int16_t) ((uint16_t) buffer_rx[3] << 8) + buffer_rx[4]);
		tmp.z = ((int16_t) ((uint16_t) buffer_rx[5] << 8) + buffer_rx[6]);

		put_raw_accelerometer(&tmp);

		return RDY_OK;
	} else {
		return RDY_RESET;
	}
}

uint8_t accelerometer_interrupt_mode(void) {
	return EXT_CH_MODE_RISING_EDGE;
}
uint8_t accelerometer_interrutp_port(void) {
	return EXT_MODE_GPIOE;
}
uint8_t accelerometer_ext_pin(void) {
	return 2;
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
	if (status == RDY_OK /*&& (buffer_rx[6] & 0x1) */) { //does not seems to work
		struct raw_magnetometer tmp;

		tmp.y = -((int16_t) ((uint16_t) buffer_rx[0] << 8) + buffer_rx[1]);
		tmp.z = ((int16_t) ((uint16_t) buffer_rx[2] << 8) + buffer_rx[3]);
		tmp.x = ((int16_t) ((uint16_t) buffer_rx[4] << 8) + buffer_rx[5]);

		put_raw_magnetometer(&tmp);
		/*
		 values[0] = ((int16_t)((uint16_t)buffer_rx[0] << 8) + buffer_rx[1]);
		 values[2] = ((int16_t)((uint16_t)buffer_rx[2] << 8) + buffer_rx[3]);
		 values[1] = ((int16_t)((uint16_t)buffer_rx[4] << 8) + buffer_rx[5]);
		 */
	} else {
		//values[0] = values[1] = values[2] = 0;
	}

	return status;
}

void get_estimated_error_acce(union quaternion q, union vector3f *ris) {
	struct raw_accelerometer tmp;

	uint8_t update = get_raw_accelerometer(&tmp);
	if ((update-last_update_a > 0) && (tmp.x != 0.0f || tmp.y != 0.0f || tmp.z != 0.0f)) {
		last_update_a = update;

		float halfvx, halfvy, halfvz;

		// Normalise accelerometer measurement
		float recipNorm = invSqrt(tmp.x * tmp.x + tmp.y * tmp.y + tmp.z * tmp.z);
		tmp.x *= recipNorm;
		tmp.y *= recipNorm;
		tmp.z *= recipNorm;

		// Estimated direction of gravity
		halfvx = q.q1*q.q3 - q.q0*q.q2;
		halfvy = q.q0*q.q1 + q.q2*q.q3;
		halfvz = q.q0*q.q0 - 0.5f + q.q3*q.q3;

		// Error is sum of cross product between estimated direction and measured direction of field vectors
		ris->x += (tmp.y * halfvz - tmp.z * halfvy);
		ris->y += (tmp.z * halfvx - tmp.x * halfvz);
		ris->z += (tmp.x * halfvy - tmp.y * halfvx);


		//System.out.println("HalfV|"+"halfvx: "+halfvx+" halfvy: "+halfvy+" halfvz: "+halfvz);
		//System.out.println("Halferr|"+"halfex: "+halfex+" halfey: "+halfey+" halfez: "+halfez);
	}
}

void get_estimated_error_magne(union quaternion q, union vector3f *ris) {
	struct raw_magnetometer tmp;

	uint8_t update = get_raw_magnetometer(&tmp);

	if ((update-last_update_m > 0) && (tmp.x != 0.0f || tmp.y != 0.0f || tmp.z != 0.0f)) {
		last_update_m = update;

		float hx, hy, bx, bz;
		float halfwx, halfwy, halfwz;

		// Normalise magnetometer measurement
		float recipNorm = invSqrt(tmp.x * tmp.x + tmp.y * tmp.y + tmp.z * tmp.z);
		tmp.x *= recipNorm;
		tmp.y *= recipNorm;
		tmp.z *= recipNorm;

		// Reference direction of Earth's magnetic field
		hx = 2.0f * (tmp.x * (0.5f - q.q2 * q.q2 - q.q3 * q.q3) + tmp.y * (q.q1 * q.q2 - q.q0 * q.q3) + tmp.z * (q.q1 * q.q3 + q.q0 * q.q2));
		hy = 2.0f * (tmp.x * (q.q1 * q.q2 + q.q0 * q.q3) + tmp.y * (0.5f - q.q1 * q.q1 - q.q3 * q.q3) + tmp.z * (q.q2 * q.q3 - q.q0 * q.q1));
		bx = sqrtf(hx * hx + hy * hy);
		bz = 2.0f * (tmp.x * (q.q1 * q.q3 - q.q0 * q.q2) + tmp.y * (q.q2 * q.q3 + q.q0 * q.q1) + tmp.z * (0.5f - q.q1 * q.q1 - q.q2 * q.q2));

		// Estimated direction of magnetic field
		halfwx = bx * (0.5f - q.q2 * q.q2 - q.q3 * q.q3) + bz * (q.q1 * q.q3 - q.q0 * q.q2);
		halfwy = bx * (q.q1 * q.q2 - q.q0 * q.q3) + bz * (q.q0 * q.q1 + q.q2 * q.q3);
		halfwz = bx * (q.q0 * q.q2 + q.q1 * q.q3) + bz * (0.5f - q.q1 * q.q1 - q.q2 * q.q2);

		// Normalize estimated reference field
		float norm = invSqrt(halfwx * halfwx + halfwy * halfwy + halfwz * halfwz);
		halfwx *= norm;
		halfwy *= norm;
		halfwz *= norm;

		// Error is sum of cross product between estimated direction and measured direction of field vectors
		ris->x += (tmp.y * halfwz - tmp.z * halfwy);
		ris->y += (tmp.z * halfwx - tmp.x * halfwz);
		ris->z += (tmp.x * halfwy - tmp.y * halfwx);
	}
}
