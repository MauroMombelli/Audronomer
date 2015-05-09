/*
 * esc_logic.h
 *
 *  Created on: 16/mar/2015
 *      Author: mauro
 */

#ifndef MIXER_MIXER_H_
#define MIXER_MIXER_H_

#include "../my_math/my_math.h"
#include "../driver_esc/esc_pwm.h"

struct mixer_component{
	struct Vector3f engine_versor;
	struct Vector3f engine_versor_orthogonal;
	size_t output_channel;
};

float getRotationFromQuad(struct Vector3f axis, struct Vector3f orto1, struct Quaternion4f dcm);

void doMixer(struct Quaternion4f dcm, struct Vector3f desired_angle);

#endif /* DRIVER_ESC_INCLUDE_MIXER_H_ */
