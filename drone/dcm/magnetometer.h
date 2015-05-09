#ifndef MAGNETOMETER_H_
#define MAGNETOMETER_H_

#include "../my_math/my_math.h"
#include "../static_db/engine_db.h"

void get_estimated_error_magne(struct Quaternion4f q, struct Vector3f *ris);

#endif
