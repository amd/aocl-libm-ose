#include "libm_dynamic_load.h"

float GenerateRangeFloat(float min, float max) {
    float range = (max - min);
    float div = (float)RAND_MAX/(float)range;
    return min + ((float)rand() / div);
}

double GenerateRangeDouble(double min, double max) {
    double range = (max - min);
    double div = RAND_MAX / range;
    return min + (rand() / div);
}

