/*
 * dcm.c
 *
 *  Created on: Dec 8, 2014
 *      Author: mauro
 */

#include "dcm.h"
#include "math.h"
#include "accelerometer.h"
#include "magnetometer.h"

/* PID factor */
float twoKp = 2.0f * 0.5f;
float twoKi = 2.0f * 0.0f;

/* frequency of sensor reading */
const float sample_freq = 355;

/* Quaternion */
//float q0 = 1, q1 = 0, q2 = 0, q3 = 0;
union quaternion q;

/* integral result */
struct vector3f integralFB;

static struct errori sensors_errors[] = { {&get_estimated_error_acce}, {&get_estimated_error_magne} };

void dcm_init(){
	q.q[0] = 1;
	q.q[1] = q.q[2] = q.q[3] = 0;

	integralFB.x = integralFB.y = integralFB.z = 0;
}

void dcm_step(struct vector3f g) {

	float recipNorm;

	struct vector3f halfe;
	halfe.x = halfe.y = halfe.z = 0;

	size_t i;
	for (i=0; i<sizeof(sensors_errors)/sizeof(sensors_errors[0]);i++){//pay attention, sensors_errors must NOT be pointer
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
		}

		// Apply proportional feedback
		g.x += twoKp * halfe.x;
		g.y += twoKp * halfe.y;
		g.z += twoKp * halfe.z;
	}else{
		return;
	}

	// Integrate rate of change of quaternion
	g.x *= (0.5f * (1.0f / sample_freq));   // pre-multiply common factors
	g.y *= (0.5f * (1.0f / sample_freq));
	g.z *= (0.5f * (1.0f / sample_freq));

	float qa, qb, qc;
	qa = q.q[0];
	qb = q.q[1];
	qc = q.q[2];
	q.q[0] += (-qb * g.x - qc * g.y - q.q[3] * g.z);
	q.q[1] += (qa * g.x + qc * g.z - q.q[3] * g.y);
	q.q[2] += (qa * g.y - qb * g.z + q.q[3] * g.x);
	q.q[3] += (qa * g.z + qb * g.y - qc * g.x);

	// Normalise quaternion
	recipNorm = invSqrt(q.q[0] * q.q[0] + q.q[1] * q.q[1] + q.q[2] * q.q[2] + q.q[3] * q.q[3]);
	q.q[0] *= recipNorm;
	q.q[1] *= recipNorm;
	q.q[2] *= recipNorm;
	q.q[3] *= recipNorm;
}

void dcm_get_quaternion(union quaternion *qRis){
	int i=0;
	for (i=0; i<4;i++){
		qRis->q[i] = q.q[i];
	}
}
