/*
 * l3g4200d.h
 *
 *  Created on: May 28, 2014
 *      Author: mauro
 */

#ifndef DCM_H_
#define DCM_H_

#include "ch.h"
#include "hal.h"

union Vector
{
    float x, y, z; /*use it as variable*/
    float a[3]; /* or as a vector*/
};

void step(float gx, float gy, float gz, float ax, float ay, float az, float mx, float my, float mz);
float invSqrt(float x);

#endif /* DCM_H_ */
