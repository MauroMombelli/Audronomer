/*
 * quaternion.h
 *
 *  Created on: 19/mar/2015
 *      Author: mauro
 */

#ifndef MY_MATH_INCLUDE_QUATERNIONF_H_
#define MY_MATH_INCLUDE_QUATERNIONF_H_

#include "vector3f.h"

struct Quaternion4f {
	float x, y, z, w;
};

static void conjugate(struct Quaternion4f origin, struct Quaternion4f *result) {
	result->x = -origin.x;
	result->y = -origin.y;
	result->z = -origin.z;
	result->w = origin.w;
}

static void multiply(struct Quaternion4f q1, struct Quaternion4f q2, struct Quaternion4f *result) {
	result->w = q1.w * q2.w - q1.x * q2.x - q1.y * q2.y - q1.z * q2.z;
	result->x = q1.w * q2.x + q2.w * q1.x + q1.y * q2.z - q1.z * q2.y;
	result->y = q1.w * q2.y + q2.w * q1.y - q1.x * q2.z + q1.z * q2.x;
	result->z = q1.w * q2.z + q2.w * q1.z + q1.x * q2.y - q1.y * q2.x;
}

static void transform(struct Vector3f v, struct Quaternion4f q, struct Vector3f *result) { //static make it "private"
	struct Quaternion4f tmp2;
	conjugate(q, &tmp2); // coniugate Q and put result in tmp2

	struct Quaternion4f tmp1;
	tmp1.x = v.x;
	tmp1.y = v.y;
	tmp1.z = v.z;
	tmp1.w = 0;

	multiply(tmp1, tmp2, &tmp2); //mul tmp1 and tmp2, store in tmp2
	multiply(q, tmp2, &tmp2); //mul q and tmp2, store in tmp2

	result->x = tmp2.x;
	result->y = tmp2.y;
	result->z = tmp2.z;
}

struct {
	void (*conjugate)(struct Quaternion4f, struct Quaternion4f*);
	void (*multiply)(struct Quaternion4f, struct Quaternion4f, struct Quaternion4f*);
	void (*transform)(struct Vector3f, struct Quaternion4f, struct Vector3f*);
} quaterionf_helper = { conjugate, multiply, transform }; //rotate tmp with dmc

#endif /* MY_MATH_INCLUDE_QUATERNIONF_H_ */
