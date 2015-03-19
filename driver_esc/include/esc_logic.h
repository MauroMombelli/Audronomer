/*
 * esc_logic.h
 *
 *  Created on: 16/mar/2015
 *      Author: mauro
 */

#ifndef DRIVER_ESC_INCLUDE_ESC_LOGIC_H_
#define DRIVER_ESC_INCLUDE_ESC_LOGIC_H_

#include "vector3f.h"
#include "quaternionf.h"

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
