/*
 * mixer.c
 *
 *  Created on: 09/mag/2015
 *      Author: mauro
 */

#include "mixer.h"

/*
 * Here there is the array of components of the "mixer", every line is a versor influence.
 * If a channel is used multiple times, the effect stack.
 * If more channel has the same correction is good idea to add them to the array of 'ppm_channel' so the calculation wont be done multiple times.
 */
struct mixer_component mixer[] = {
	{ { 0, 0, 1 }, { 1, 0, 0 }, 0 }, //alettone sinistro
	{ { 0, 0, -1 }, { 1, 0, 0 }, 1 } //alettone destro
};

const uint8_t MIXER_ELEMENTS = sizeof(mixer) / sizeof(mixer[0]);

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

void doMixer(struct Quaternion4f dcm, struct Vector3f desired_angle) {
	pwmcnt_t tmp_output[CHANNEL_NUMBER];
	uint8_t i;
	for (i = 0; i < CHANNEL_NUMBER; i++) {
		tmp_output[i] = 0;
	}

	for (i = 0; i < MIXER_ELEMENTS; i++) {
		float angle = getRotationFromQuad(mixer[i].engine_versor, mixer[i].engine_versor_orthogonal, dcm);
		tmp_output[mixer[i].output_channel] += angle;
	}
	/* TODO: implement the desired angle from user */
}
