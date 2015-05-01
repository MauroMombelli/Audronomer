/*
 * esc_logic.h
 *
 *  Created on: 16/mar/2015
 *      Author: mauro
 */

#ifndef DRIVER_ESC_INCLUDE_ESC_LOGIC_H_
#define DRIVER_ESC_INCLUDE_ESC_LOGIC_H_

#include "esc_pwm.h"
#include "vector3f.h"
#include "quaternionf.h"

const struct {
	Vector3f engine_versor;
	Vector3f engine_versor_orthogonal;
	size_t ppm_channel[];
} correctors[] = { //here there is the list of angle "correctors"
		{ { 0, 0, 1 }, { 1, 0, 0 }, {0} }, //alettone sinistro
		{ { 0, 0, -1 }, { 1, 0, 0 }, {1} } //alettone destro
};

const uint8_t CORRECTORS_NUMBER = sizeof(correctors) / sizeof(correctors[0]);

pwmcnt_t tmp_output[CHANNEL_NUMBER];

void stabilize(struct Quaternion4f dcm, Vector3f desired_angle) {
	uint8_t i;
	for (i = 0; i < CHANNEL_NUMBER; i++) {
		tmp_output[i] = 0;
	}

	for (i = 0; i < CORRECTORS_NUMBER; i++) {
		float angle = getRotationFromQuad
	}
}

float getRotationFromQuad(struct Vector3f axis, struct Vector3f orto1, struct Quaternion4f dcm) {
	//from http://stackoverflow.com/questions/3684269/component-of-a-quaternion-rotation-around-an-axis

	struct Vector3f tmp;

	vector3helper.copy(orto1, &tmp); //copy orto1 to tmp

	quaterionf_helper.transform(tmp, dcm, &tmp); //rotate tmp with dmc

	float dot_ris = vector3helper.dot(tmp, axis); //dot product of tmp and axis

	vector3helper.mult(axis, dot_ris, &axis); //mult axis by dot_ris, result stored it into axis

	vector3helper.sub(tmp, axis, &tmp); //subtract axis from tmp, store in tmp

	vector3helper.normalize(tmp, &tmp);

	return acos(vector3helper.dot(orto1, tmp));
}

#endif /* DRIVER_ESC_INCLUDE_ESC_LOGIC_H_ */
