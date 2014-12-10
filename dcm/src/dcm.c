/*
 * dcm.c
 *
 *  Created on: Dec 8, 2014
 *      Author: mauro
 */

#include "dcm.h"
#include "math.h"

/* PID factor */
float twoKp = 2.0f * 0.5f;
float twoKi = 2.0f * 0.0f;

/* frequency of sensor reading */
float sample_freq = 767;

/* Quaternion */
//float q0 = 1, q1 = 0, q2 = 0, q3 = 0;
union quaternion q;

/* integral result */
union vector3f integralFB;

void dcm_init(){
	q.q[0] = 1;
	q.q[1] = q.q[2] = q.q[3] = 0;

	integralFB.x = integralFB.y = integralFB.z = 0;
}

void dcm_step(union vector3f g) {

	float recipNorm;

	union vector3f halfe;

	size_t i;
	for (i=0; i<sizeof(sensors_errors);i++){//pay attention, sensors_errors must be NOT pointer
		(*sensors_errors[i].get_estimated_error)(q, &halfe);
	}

	// Apply feedback only when valid data has been gathered from sensors
	if (halfe.x != 0.0f || halfe.y != 0.0f || halfe.z != 0.0f) {
		// Compute and apply integral feedback if enabled
		if (twoKi > 0.0f) {
			integralFB.x += twoKi * halfe.x * (1.0f / sample_freq);  // integral error scaled by Ki
			integralFB.y += twoKi * halfe.y * (1.0f / sample_freq);
			integralFB.z += twoKi * halfe.z * (1.0f / sample_freq);
			g.x += integralFB.x;  // apply integral feedback
			g.y += integralFB.y;
			g.z += integralFB.z;
		} /*TODO: why is this here?
		else {
			integralFBx = 0.0f; // prevent integral windup
			integralFBy = 0.0f;
			integralFBz = 0.0f;
		}*/

		// Apply proportional feedback
		g.x += twoKp * halfe.x;
		g.y += twoKp * halfe.y;
		g.z += twoKp * halfe.z;
	}

	// Integrate rate of change of quaternion
	g.x *= (0.5f * (1.0f / sample_freq));   // pre-multiply common factors
	g.y *= (0.5f * (1.0f / sample_freq));
	g.z *= (0.5f * (1.0f / sample_freq));

	float qa, qb, qc;
	qa = q.q0;
	qb = q.q1;
	qc = q.q2;
	q.q0 += (-qb * g.x - qc * g.y - q.q3 * g.z);
	q.q1 += (qa * g.x + qc * g.z - q.q3 * g.y);
	q.q2 += (qa * g.y - qb * g.z + q.q3 * g.x);
	q.q3 += (qa * g.z + qb * g.y - qc * g.x);

	// Normalise quaternion
	recipNorm = invSqrt(q.q0 * q.q0 + q.q1 * q.q1 + q.q2 * q.q2 + q.q3 * q.q3);
	q.q0 *= recipNorm;
	q.q1 *= recipNorm;
	q.q2 *= recipNorm;
	q.q3 *= recipNorm;
}
