#ifndef MATH_H
#define MATH_H

#define M_PI 3.14159265358979323846

float sinf(float);
double sin(double);
float cosf(float);
double cos(double);

#ifndef sqrtf
#define sqrtf(v) __builtin_sqrtf(v)
#endif

#endif
