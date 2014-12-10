/*
 * my_math.h
 *
 *  Created on: 10/dic/2014
 *      Author: mauro
 */

#ifndef MY_MATH_H_
#define MY_MATH_H_

#include <math.h>

union vector3f
{
    float x, y, z; /*use it as variable*/
    float v[3]; /* or as a vector*/
};

union quaternion
{
    float q0, q1, q2, q3; /*use it as variable*/
    float q[4]; /* or as a vector*/
};

float invSqrt(float x);

#endif /* MY_MATH_H_ */
