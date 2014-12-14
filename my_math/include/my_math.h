/*
 * my_math.h
 *
 *  Created on: 10/dic/2014
 *      Author: mauro
 */

#ifndef MY_MATH_H_
#define MY_MATH_H_

#include <math.h>

struct vector3f
{
	float x, y, z;
};

union quaternion
{
    float q[4];
};

float invSqrt(float x);

#endif /* MY_MATH_H_ */
