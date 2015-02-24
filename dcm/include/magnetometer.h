#include "math.h"

struct vector3f zero_magne = { 0, 0, 0 };
uint8_t last_update_m = 0;

void get_estimated_error_magne(union quaternion q, struct vector3f *ris) {
	struct raw_magnetometer tmpM;
	uint8_t update = get_raw_magnetometer(&tmpM);
	//to float
	struct vector3f tmp;
	tmp.x = tmpM.x - zero_magne.x;
	tmp.y = tmpM.y - zero_magne.y;
	tmp.z = tmpM.z - zero_magne.z;
	update -= last_update_m; //now in update there is the diff
	if ( update && (tmp.x != 0.0f || tmp.y != 0.0f || tmp.z != 0.0f)) {
		last_update_m += update; //now last_update_m is equal to update prior the diff

		float hx, hy, bx, bz;
		float halfwx, halfwy, halfwz;

		// Normalise magnetometer measurement
		float recipNorm = invSqrt(tmp.x * tmp.x + tmp.y * tmp.y + tmp.z * tmp.z);
		tmp.x *= recipNorm;
		tmp.y *= recipNorm;
		tmp.z *= recipNorm;

		// Reference direction of Earth's magnetic field
		hx = 2.0f * (tmp.x * (0.5f - q.q[2] * q.q[2] - q.q[3] * q.q[3]) + tmp.y * (q.q[1] * q.q[2] - q.q[0] * q.q[3]) + tmp.z * (q.q[1] * q.q[3] + q.q[0] * q.q[2]));
		hy = 2.0f * (tmp.x * (q.q[1] * q.q[2] + q.q[0] * q.q[3]) + tmp.y * (0.5f - q.q[1] * q.q[1] - q.q[3] * q.q[3]) + tmp.z * (q.q[2] * q.q[3] - q.q[0] * q.q[1]));
		bx = sqrtf(hx * hx + hy * hy);
		bz = 2.0f * (tmp.x * (q.q[1] * q.q[3] - q.q[0] * q.q[2]) + tmp.y * (q.q[2] * q.q[3] + q.q[0] * q.q[1]) + tmp.z * (0.5f - q.q[1] * q.q[1] - q.q[2] * q.q[2]));

		// Estimated direction of magnetic field
		halfwx = bx * (0.5f - q.q[2] * q.q[2] - q.q[3] * q.q[3]) + bz * (q.q[1] * q.q[3] - q.q[0] * q.q[2]);
		halfwy = bx * (q.q[1] * q.q[2] - q.q[0] * q.q[3]) + bz * (q.q[0] * q.q[1] + q.q[2] * q.q[3]);
		halfwz = bx * (q.q[0] * q.q[2] + q.q[1] * q.q[3]) + bz * (0.5f - q.q[1] * q.q[1] - q.q[2] * q.q[2]);

		// Normalize estimated reference field
		float norm = invSqrt(halfwx * halfwx + halfwy * halfwy + halfwz * halfwz);
		halfwx *= norm;
		halfwy *= norm;
		halfwz *= norm;

		// Error is sum of cross product between estimated direction and measured direction of field vectors
		ris->x += (tmp.y * halfwz - tmp.z * halfwy);
		ris->y += (tmp.z * halfwx - tmp.x * halfwz);
		ris->z += (tmp.x * halfwy - tmp.y * halfwx);
	}
}
