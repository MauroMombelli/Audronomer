#ifndef ACCELEROMETER_H_
#define ACCELEROMETER_H_

#include "../my_math/my_math.h"
#include "../static_db/engine_db.h"

void get_estimated_error_acce(struct Quaternion4f q, struct Vector3f *ris);

#endif
