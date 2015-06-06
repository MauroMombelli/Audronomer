/* BEGIN: just some struct to test, fell free to move them on external header file */
#ifndef SENSOR_READ_STRUCT_H
#define SENSOR_READ_STRUCT_H

#include "TrigonomeC/my_math.h"

struct sensors_data {
	struct Vector3f gyro;
	struct Vector3i16 acce;
	struct Vector3i16 magne;
};

#endif
/* END: just some struct to test, fell free to move them on external header file */
