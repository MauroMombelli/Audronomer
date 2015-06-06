/*
 * mixer.h
 *
 *  Created on: 09/mag/2015
 *      Author: mauro
 */

#ifndef DRONE_MIXER_MIXER_H_
#define DRONE_MIXER_MIXER_H_

#include "driver/esc/esc_pwm.h"
#include "TrigonomeC/my_math.h"


struct mixer_component{
	struct Vector3f engine_versor;
	struct Vector3f engine_versor_orthogonal;
	size_t output_channel;
};

float getRotationFromQuad(struct Vector3f axis, struct Vector3f orto1, struct Quaternion4f dcm);

void doMixer(struct Quaternion4f dcm, struct Vector3f desired_angle);

#endif /* DRONE_MIXER_MIXER_H_ */
