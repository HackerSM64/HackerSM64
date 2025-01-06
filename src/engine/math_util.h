#ifndef MATH_UTIL_H
#define MATH_UTIL_H

#include <PR/ultratypes.h>

#include "types.h"
#include "game/puppyprint.h"

#define NEAR_ZERO   __FLT_EPSILON__
#define NEAR_ONE    (1.0f - __FLT_EPSILON__)

#define FLOAT_ONE   0x3F800000

extern Vec3f gVec3fX;
extern Vec3f gVec3fY;
extern Vec3f gVec3fZ;
extern Vec3f gVec3fNX;
extern Vec3f gVec3fNY;
extern Vec3f gVec3fNZ;
extern Vec3f gVec3fZero;
extern Vec3s gVec3sZero;
extern Vec3i gVec3iZero;
extern Vec3f gVec3fOne;
extern Vec3s gVec3sOne;


// Angles

/**
 * Converts an angle in degrees to sm64's s16 angle units. For example, DEGREES(90) == 0x4000
 * This should be used mainly to make camera code clearer at first glance.
 */
// #define DEGREES(x) ((x) * 0x10000 / 360)
#define DEGREES(x) ((x) * 0x2000 / 45)

// Trig functions

extern f32 gSineTable[];
#define gCosineTable (gSineTable + 0x400)

#define sins(x) gSineTable[  (u16) (x) >> 4]
#define coss(x) gCosineTable[(u16) (x) >> 4]
#define tans(x) (sins(x) / coss(x))
#define cots(x) (coss(x) / sins(x))
#define atans(x) gArctanTable[(s32)((((x) * 1024) + 0.5f))] // is this correct? used for atan2_lookup

// Angle conversion macros

#define RAD_PER_DEG (M_PI / 180.0f)
#define DEG_PER_RAD (180.0f / M_PI)

#define angle_to_degrees(  x) (f32)(((s16)(x) / 65536.0f) * 360.0f)
#define degrees_to_angle(  x) (s16)(((f32)(x) * 0x10000 ) / 360   )
#define angle_to_radians(  x) (f32)(((s16)(x) * M_PI    ) / 0x8000)
#define radians_to_angle(  x) (s16)(((f32)(x) / M_PI    ) * 0x8000)
#define degrees_to_radians(x) (f32)( (f32)(x) * RAD_PER_DEG       )
#define radians_to_degrees(x) (f32)( (f32)(x) * DEG_PER_RAD       )


// Various basic helper macros

// Get the square of a number
#define sqr(x) ({         \
    __auto_type _x = (x); \
    _x * _x; })

// Get the sign of a number
#define signum_positive(x) (((x) < 0) ? -1 : 1)

// Absolute value
#define ABS(x) ({         \
    __auto_type _x = (x); \
    _x > 0 ? _x : -_x; })
#define absi ABS
#define abss ABS
#define absf ABS

// Get the minimum / maximum of a set of numbers
#undef MIN
#define MIN(a, b) ({      \
    __auto_type _a = (a); \
    __auto_type _b = (b); \
    _a < _b ? _a : _b; })

#undef MAX
#define MAX(a, b) ({      \
    __auto_type _a = (a); \
    __auto_type _b = (b); \
    _a > _b ? _a : _b; })

#define min_3(a, b, c) MIN(MIN(a, b), c)

#define max_3(a, b, c) MAX(MAX(a, b), c)

#define min_3f min_3
#define min_3i min_3
#define min_3s min_3

#define max_3f max_3
#define max_3i max_3
#define max_3s max_3

void min_max_3f(f32 a, f32 b, f32 c, f32 *min, f32 *max);
void min_max_3i(s32 a, s32 b, s32 c, s32 *min, s32 *max);
void min_max_3s(s16 a, s16 b, s16 c, s16 *min, s16 *max);

// From Wiseguy
// Round a float to the nearest integer
ALWAYS_INLINE s32 roundf(f32 in) {
    f32 tmp;
    s32 out;
    __asm__("round.w.s %0,%1" : "=f" (tmp) : "f" (in ));
    __asm__("mfc1      %0,%1" : "=r" (out) : "f" (tmp));
    return out;
}

#define round_float roundf

#define FLT_IS_NONZERO(x) (absf(x) > NEAR_ZERO)


// Integer limits and clamping

#define S8_MAX   127
#define S8_MIN  -128
#define U8_MAX   255
#define S16_MAX  32767
#define S16_MIN -32768
#define U16_MAX  65535
#define S32_MAX  2147483647
#define S32_MIN -2147483648
#define U32_MAX  4294967295

// Clamp a value inbetween a range
#define CLAMP(x, low, high)  MIN(MAX((x), (low)), (high))

// Clamp a value to the range of a specific data type
#define CLAMP_U8( x)        CLAMP((x),       0,  U8_MAX)
#define CLAMP_S8( x)        CLAMP((x),  S8_MIN,  S8_MAX)
#define CLAMP_U16(x)        CLAMP((x),       0, U16_MAX)
#define CLAMP_S16(x)        CLAMP((x), S16_MIN, S16_MAX)


// Vector operations

// Set all elements of a vector to the same constant
#define vec2_same(v, s)     (((v)[0]) = ((v)[1])                       = (s))
#define vec3_same(v, s)     (((v)[0]) = ((v)[1]) = ((v)[2])            = (s))
#define vec4_same(v, s)     (((v)[0]) = ((v)[1]) = ((v)[2]) = ((v)[3]) = (s))

// Set all elements of a vector to zero
#define vec2_zero(v)        (vec2_same((v), 0))
#define vec3_zero(v)        (vec3_same((v), 0))
#define vec4_zero(v)        (vec4_same((v), 0))

// Sum of the squares of all elements of a vector
#define vec2_sumsq(v)       (sqr((v)[0]) + sqr((v)[1]))
#define vec3_sumsq(v)       (sqr((v)[0]) + sqr((v)[1]) + sqr((v)[2]))
#define vec4_sumsq(v)       (sqr((v)[0]) + sqr((v)[1]) + sqr((v)[2]) + sqr((v)[3]))

// Calculate the magnitude of a vector
#define vec2_mag(v)         (sqrtf(vec2_sumsq(v)))
#define vec3_mag(v)         (sqrtf(vec3_sumsq(v)))
#define vec4_mag(v)         (sqrtf(vec4_sumsq(v)))

/**
 * Set 'dest' to the normal vector of a triangle with vertices a, b and c.
 * Equivalent to cross((c-b), (c-a)).
 */
#define find_vector_perpendicular_to_plane(dest, a, b, c) {                                     \
    (dest)[0] = ((b)[1] - (a)[1]) * ((c)[2] - (b)[2]) - ((c)[1] - (b)[1]) * ((b)[2] - (a)[2]);  \
    (dest)[1] = ((b)[2] - (a)[2]) * ((c)[0] - (b)[0]) - ((c)[2] - (b)[2]) * ((b)[0] - (a)[0]);  \
    (dest)[2] = ((b)[0] - (a)[0]) * ((c)[1] - (b)[1]) - ((c)[0] - (b)[0]) * ((b)[1] - (a)[1]);  \
}

// Set the elements of vector 'dst' to the given values
#define vec2_set(dst, x, y) {           \
    (dst)[0] = (x);                     \
    (dst)[1] = (y);                     \
}
#define vec3_set(dst, x, y, z) {        \
    (dst)[0] = (x);                     \
    (dst)[1] = (y);                     \
    (dst)[2] = (z);                     \
}
#define vec4_set(dst, x, y, z, w) {     \
    (dst)[0] = (x);                     \
    (dst)[1] = (y);                     \
    (dst)[2] = (z);                     \
    (dst)[3] = (w);                     \
}

#define vec3f_set vec3_set
#define vec3i_set vec3_set
#define vec3s_set vec3_set

// Copy vector 'src' to vector 'dst'
#define vec2_copy(dst, src) {           \
    __auto_type _x = (src)[0];          \
    __auto_type _y = (src)[1];          \
    (dst)[0] = _x;                      \
    (dst)[1] = _y;                      \
}
#define vec3_copy(dst, src) {           \
    __auto_type _x = (src)[0];          \
    __auto_type _y = (src)[1];          \
    __auto_type _z = (src)[2];          \
    (dst)[0] = _x;                      \
    (dst)[1] = _y;                      \
    (dst)[2] = _z;                      \
}
#define vec4_copy(dst, src) {           \
    __auto_type _x = (src)[0];          \
    __auto_type _y = (src)[1];          \
    __auto_type _z = (src)[2];          \
    __auto_type _w = (src)[3];          \
    (dst)[0] = _x;                      \
    (dst)[1] = _y;                      \
    (dst)[2] = _z;                      \
    (dst)[3] = _w;                      \
}

#define vec3f_copy vec3_copy
#define vec3i_copy vec3_copy
#define vec3s_copy vec3_copy
#define vec3s_to_vec3i vec3_copy
#define vec3s_to_vec3f vec3_copy
#define vec3i_to_vec3s vec3_copy
#define vec3i_to_vec3f vec3_copy
#define vec3f_to_vec3s vec3_copy
#define vec3f_to_vec3i vec3_copy

#define surface_normal_to_vec3f(dst, surf) vec3f_copy((dst), &((surf)->normal.x))

// Copy vector 'src' to vector 'dst' and add a scalar to the y component
#define vec3_copy_y_off(dst, src, y) {  \
    __auto_type _x = (src)[0];          \
    __auto_type _y = (src)[1] + (y);    \
    __auto_type _z = (src)[2];          \
    (dst)[0] = _x;                      \
    (dst)[1] = _y;                      \
    (dst)[2] = _z;                      \
}

#define vec3f_copy_y_off vec3_copy_y_off

// Set vector 'dst' to the sum of vectors 'src1' and 'src2'
#define vec2_sum(dst, src1, src2) {         \
    __auto_type _x = (src1)[0] + (src2)[0]; \
    __auto_type _y = (src1)[1] + (src2)[1]; \
    (dst)[0] = _x;                          \
    (dst)[1] = _y;                          \
}
#define vec3_sum(dst, src1, src2) {         \
    __auto_type _x = (src1)[0] + (src2)[0]; \
    __auto_type _y = (src1)[1] + (src2)[1]; \
    __auto_type _z = (src1)[2] + (src2)[2]; \
    (dst)[0] = _x;                          \
    (dst)[1] = _y;                          \
    (dst)[2] = _z;                          \
}
#define vec4_sum(dst, src1, src2) {         \
    __auto_type _x = (src1)[0] + (src2)[0]; \
    __auto_type _y = (src1)[1] + (src2)[1]; \
    __auto_type _z = (src1)[2] + (src2)[2]; \
    __auto_type _w = (src1)[3] + (src2)[3]; \
    (dst)[0] = _x;                          \
    (dst)[1] = _y;                          \
    (dst)[2] = _z;                          \
    (dst)[3] = _w;                          \
}

#define vec3f_sum vec3_sum
#define vec3i_sum vec3_sum
#define vec3s_sum vec3_sum

// Add the vector 'src' to vector 'dst'
#define vec2_add(dst, src) vec2_sum((dst), (dst), (src))
#define vec3_add(dst, src) vec3_sum((dst), (dst), (src))
#define vec4_add(dst, src) vec4_sum((dst), (dst), (src))

#define vec3f_add vec3_add
#define vec3i_add vec3_add
#define vec3s_add vec3_add

// Set vector 'dst' to the difference of vectors 'src1' and 'src2'
#define vec2_diff(dst, src1, src2) {        \
    __auto_type _x = (src1)[0] - (src2)[0]; \
    __auto_type _y = (src1)[1] - (src2)[1]; \
    (dst)[0] = _x;                          \
    (dst)[1] = _y;                          \
}
#define vec3_diff(dst, src1, src2) {        \
    __auto_type _x = (src1)[0] - (src2)[0]; \
    __auto_type _y = (src1)[1] - (src2)[1]; \
    __auto_type _z = (src1)[2] - (src2)[2]; \
    (dst)[0] = _x;                          \
    (dst)[1] = _y;                          \
    (dst)[2] = _z;                          \
}
#define vec4_diff(dst, src1, src2) {        \
    __auto_type _x = (src1)[0] - (src2)[0]; \
    __auto_type _y = (src1)[1] - (src2)[1]; \
    __auto_type _z = (src1)[2] - (src2)[2]; \
    __auto_type _w = (src1)[3] - (src2)[3]; \
    (dst)[0] = _x;                          \
    (dst)[1] = _y;                          \
    (dst)[2] = _z;                          \
    (dst)[3] = _w;                          \
}

#define vec3f_diff vec3_diff
#define vec3i_diff vec3_diff
#define vec3s_diff vec3_diff

// Subtract the vector 'src' from vector 'dst'
#define vec2_sub(dst, src) vec2_diff((dst), (dst), (src))
#define vec3_sub(dst, src) vec3_diff((dst), (dst), (src))
#define vec4_sub(dst, src) vec4_diff((dst), (dst), (src))

#define vec3f_sub vec3_sub
#define vec3i_sub vec3_sub
#define vec3s_sub vec3_sub

// Set vector 'dst' to the product of vectors 'src1' and 'src2'
#define vec2_prod(dst, src1, src2) {        \
    __auto_type _x = (src1)[0] * (src2)[0]; \
    __auto_type _y = (src1)[1] * (src2)[1]; \
    (dst)[0] = _x;                          \
    (dst)[1] = _y;                          \
}
#define vec3_prod(dst, src1, src2) {        \
    __auto_type _x = (src1)[0] * (src2)[0]; \
    __auto_type _y = (src1)[1] * (src2)[1]; \
    __auto_type _z = (src1)[2] * (src2)[2]; \
    (dst)[0] = _x;                          \
    (dst)[1] = _y;                          \
    (dst)[2] = _z;                          \
}
#define vec4_prod(dst, src1, src2) {        \
    __auto_type _x = (src1)[0] * (src2)[0]; \
    __auto_type _y = (src1)[1] * (src2)[1]; \
    __auto_type _z = (src1)[2] * (src2)[2]; \
    __auto_type _w = (src1)[3] * (src2)[3]; \
    (dst)[0] = _x;                          \
    (dst)[1] = _y;                          \
    (dst)[2] = _z;                          \
    (dst)[3] = _w;                          \
}

#define vec3f_prod vec3_prod
#define vec3i_prod vec3_prod
#define vec3s_prod vec3_prod

// Multiply vector 'dst' by vector 'src'
#define vec2_mul(dst, src) vec2_prod((dst), (dst), (src))
#define vec3_mul(dst, src) vec3_prod((dst), (dst), (src))
#define vec4_mul(dst, src) vec4_prod((dst), (dst), (src))

#define vec3f_mul vec3_mul
#define vec3i_mul vec3_mul
#define vec3s_mul vec3_mul

// Set vector 'dst' to vector 'src' scaled by the scalar 'x'
#define vec2_scale_dest(dst, src, x) {  \
    __auto_type _x = (src)[0] * (x);    \
    __auto_type _y = (src)[1] * (x);    \
    (dst)[0] = _x;                      \
    (dst)[1] = _y;                      \
}
#define vec3_scale_dest(dst, src, x) {  \
    __auto_type _x = (src)[0] * (x);    \
    __auto_type _y = (src)[1] * (x);    \
    __auto_type _z = (src)[2] * (x);    \
    (dst)[0] = _x;                      \
    (dst)[1] = _y;                      \
    (dst)[2] = _z;                      \
}
#define vec4_scale_dest(dst, src, x) {  \
    __auto_type _x = (src)[0] * (x);    \
    __auto_type _y = (src)[1] * (x);    \
    __auto_type _z = (src)[2] * (x);    \
    __auto_type _w = (src)[3] * (x);    \
    (dst)[0] = _x;                      \
    (dst)[1] = _y;                      \
    (dst)[2] = _z;                      \
    (dst)[3] = _w;                      \
}

// Scale vector 'dst' by the scalar 'x'
#define vec2_scale(dst, x) vec2_scale_dest(dst, dst, x)
#define vec3_scale(dst, x) vec3_scale_dest(dst, dst, x)
#define vec4_scale(dst, x) vec4_scale_dest(dst, dst, x)

// Set vector 'dst' to vector 'src1' divided by vector 'src2'
#define vec2_quot(dst, src1, src2) {        \
    __auto_type _x = (src1)[0] / (src2)[0]; \
    __auto_type _y = (src1)[1] / (src2)[1]; \
    (dst)[0] = _x;                          \
    (dst)[1] = _y;                          \
}
#define vec3_quot(dst, src1, src2) {        \
    __auto_type _x = (src1)[0] / (src2)[0]; \
    __auto_type _y = (src1)[1] / (src2)[1]; \
    __auto_type _z = (src1)[2] / (src2)[2]; \
    (dst)[0] = _x;                          \
    (dst)[1] = _y;                          \
    (dst)[2] = _z;                          \
}
#define vec4_quot(dst, src1, src2) {        \
    __auto_type _x = (src1)[0] / (src2)[0]; \
    __auto_type _y = (src1)[1] / (src2)[1]; \
    __auto_type _z = (src1)[2] / (src2)[2]; \
    __auto_type _w = (src1)[3] / (src2)[3]; \
    (dst)[0] = _x;                          \
    (dst)[1] = _y;                          \
    (dst)[2] = _z;                          \
    (dst)[3] = _w;                          \
}

#define vec3f_quot vec3_quot
#define vec3i_quot vec3_quot
#define vec3s_quot vec3_quot

// Divide vector 'dst' by vector 'src'
#define vec2_div(dst, src) vec2_quot((dst), (dst), (src))
#define vec3_div(dst, src) vec3_quot((dst), (dst), (src))
#define vec4_div(dst, src) vec4_quot((dst), (dst), (src))

#define vec3f_div vec3_div
#define vec3i_div vec3_div
#define vec3s_div vec3_div

// The yaw between two points in 3D space
#define vec3_yaw(from, to)  (atan2s(((to)[2] - (from)[2]), ((to)[0] - (from)[0])))

// Calculate the dot product of two vectors
#define vec2_dot(a, b)       (((a)[0] * (b)[0]) + ((a)[1] * (b)[1]))
#define vec3_dot(a, b)      (vec2_dot((a), (b)) + ((a)[2] * (b)[2]))
#define vec4_dot(a, b)      (vec3_dot((a), (b)) + ((a)[3] * (b)[3]))

#define vec3f_dot vec3_dot

// Make vector 'dest' the cross product of vectors a and b.
#define vec3_cross(dst, a, b) {                             \
    __auto_type _x = ((a)[1] * (b)[2]) - ((a)[2] * (b)[1]); \
    __auto_type _y = ((a)[2] * (b)[0]) - ((a)[0] * (b)[2]); \
    __auto_type _z = ((a)[0] * (b)[1]) - ((a)[1] * (b)[0]); \
    (dst)[0] = _x;                                          \
    (dst)[1] = _y;                                          \
    (dst)[2] = _z;                                          \
}

#define vec3f_cross vec3_cross

// Scale vector 'v' so it has length 1
#define vec3_normalize(v) {                       \
    f32 _v_invmag = vec3_mag((v));                 \
    _v_invmag = (1.0f / MAX(_v_invmag, NEAR_ZERO)); \
    vec3_scale((v), _v_invmag);                    \
}

#define vec3f_normalize vec3_normalize

// If the magnitude of vector 'v' is greater than 'max', scale it down to 'max'
#define vec3_set_max_dist(v, max) { \
    f32 _v_mag = vec3_mag(v);        \
    f32 _max = max;                 \
    _v_mag = MAX(_v_mag, NEAR_ZERO);  \
    if (_v_mag > _max) {             \
        _v_mag = (_max / _v_mag);     \
        vec3_scale(v, _v_mag);       \
    }                               \
}

// Transform the vector 'srcV' by the matrix 'mtx' and store the result in 'dstV'. Ignores translation.
#define linear_mtxf_mul_vec3(mtx, dstV, srcV) {                                                         \
    PUPPYPRINT_ADD_COUNTER(gPuppyCallCounter.matrix);                                                   \
    __auto_type _x = ((mtx)[0][0] * (srcV)[0]) + ((mtx)[1][0] * (srcV)[1]) + ((mtx)[2][0] * (srcV)[2]); \
    __auto_type _y = ((mtx)[0][1] * (srcV)[0]) + ((mtx)[1][1] * (srcV)[1]) + ((mtx)[2][1] * (srcV)[2]); \
    __auto_type _z = ((mtx)[0][2] * (srcV)[0]) + ((mtx)[1][2] * (srcV)[1]) + ((mtx)[2][2] * (srcV)[2]); \
    (dstV)[0] = _x;                                                                                     \
    (dstV)[1] = _y;                                                                                     \
    (dstV)[2] = _z;                                                                                     \
}

// Transform the vector 'srcV' by the matrix 'mtx' including translation, and store the result in 'dstV'
#define linear_mtxf_mul_vec3_and_translate(mtx, dstV, srcV) { \
    linear_mtxf_mul_vec3((mtx), (dstV), (srcV));              \
    vec3_add((dstV), (mtx)[3]);                               \
}

// Transform the vector 'srcV' by the transpose of the matrix 'mtx'
// and store the result in 'dstV'. Ignores translation.
// For most transformation matrices, this will apply the inverse of the transformation.
#define linear_mtxf_transpose_mul_vec3(mtx, dstV, srcV) { \
    PUPPYPRINT_ADD_COUNTER(gPuppyCallCounter.matrix);     \
    __auto_type _x = vec3_dot((mtx)[0], (srcV));          \
    __auto_type _y = vec3_dot((mtx)[1], (srcV));          \
    __auto_type _z = vec3_dot((mtx)[2], (srcV));          \
    (dstV)[0] = _x;                                       \
    (dstV)[1] = _y;                                       \
    (dstV)[2] = _z;                                       \
}

#define linear_mtxf_mul_vec3f linear_mtxf_mul_vec3
#define linear_mtxf_mul_vec3f_and_translate linear_mtxf_mul_vec3_and_translate
#define linear_mtxf_transpose_mul_vec3f linear_mtxf_transpose_mul_vec3


// Angles and distances between vectors

/// Finds the distance between two vectors
#define vec3_get_dist(from, to, dist) { \
    Vec3f _d;                           \
    vec3_diff(_d, (to), (from));        \
    *(dist) = vec3_mag((_d));           \
}

#define vec3f_get_dist vec3_get_dist
#define vec3s_get_dist vec3_get_dist

/// Finds the horizontal distance between two vectors
#define vec3_get_lateral_dist(from, to, lateralDist) { \
    Vec3f _d;                                          \
    vec3_diff(_d, (to), (from));                       \
    *(lateralDist) = sqrtf(sqr(_d[0]) + sqr(_d[2]));   \
}

#define vec3f_get_lateral_dist vec3_get_lateral_dist
#define vec3s_get_lateral_dist vec3_get_lateral_dist

/// Finds the pitch between two vectors
#define vec3_get_pitch(from, to, pitch) {                     \
    Vec3f _d;                                                 \
    vec3_diff(_d, (to), (from));                              \
    *(pitch) = atan2s(sqrtf(sqr(_d[0]) + sqr(_d[2])), _d[1]); \
}

#define vec3f_get_pitch vec3_get_pitch
#define vec3s_get_pitch vec3_get_pitch

/// Finds the yaw between two vectors
#define vec3_get_yaw(from, to, yaw) { \
    f32 _dx = ((to)[0] - (from)[0]);  \
    f32 _dz = ((to)[2] - (from)[2]);  \
    *(yaw) = atan2s(_dz, _dx);        \
}

#define vec3f_get_yaw vec3_get_yaw
#define vec3s_get_yaw vec3_get_yaw

// Finds the distance, pitch, and yaw between two vectors
#define vec3_get_dist_and_angle(from, to, dist, pitch, yaw) { \
    Vec3f _d;                                                 \
    vec3f_diff(_d, (to), (from));                             \
    f32 _xz = (sqr(_d[0]) + sqr(_d[2]));                      \
    *(dist)  = sqrtf(_xz + sqr(_d[1]));                         \
    *(pitch) = atan2s(sqrtf(_xz), _d[1]);                       \
    *(yaw)   = atan2s(_d[2], _d[0]);                            \
}

#define vec3f_get_dist_and_angle vec3_get_dist_and_angle
#define vec3s_get_dist_and_angle vec3_get_dist_and_angle

// Constructs the 'to' point which is distance 'dist' away from the 'from' position,
// and has the angles pitch and yaw.
#define vec3_set_dist_and_angle(from, to, dist, pitch, yaw) { \
    f32 _dcos = ((dist) * coss(pitch));                         \
    __auto_type _x = ((from)[0] + (_dcos  * sins(yaw)));       \
    __auto_type _y = ((from)[1] + ((dist) * sins(pitch)));     \
    __auto_type _z = ((from)[2] + (_dcos  * coss(yaw)));       \
    (to)[0] = _x;                                             \
    (to)[1] = _y;                                             \
    (to)[2] = _z;                                             \
}

#define vec3f_set_dist_and_angle vec3_set_dist_and_angle
#define vec3s_set_dist_and_angle vec3_set_dist_and_angle


// Matrices

#define MAT4_VEC_DOT_PROD(R, A, B, row, col) {              \
    (R)[(row)][(col)]  = ((A)[(row)][0] * (B)[0][(col)]);   \
    (R)[(row)][(col)] += ((A)[(row)][1] * (B)[1][(col)]);   \
    (R)[(row)][(col)] += ((A)[(row)][2] * (B)[2][(col)]);   \
}
#define MAT4_DOT_PROD(R, A, B, row, col) {                  \
    (R)[(row)][(col)]  = ((A)[(row)][0] * (B)[0][(col)]);   \
    (R)[(row)][(col)] += ((A)[(row)][1] * (B)[1][(col)]);   \
    (R)[(row)][(col)] += ((A)[(row)][2] * (B)[2][(col)]);   \
    (R)[(row)][(col)] += ((A)[(row)][3] * (B)[3][(col)]);   \
}

#define MAT4_MULTIPLY(R, A, B) {        \
    MAT4_DOT_PROD((R), (A), (B), 0, 0); \
    MAT4_DOT_PROD((R), (A), (B), 0, 1); \
    MAT4_DOT_PROD((R), (A), (B), 0, 2); \
    MAT4_DOT_PROD((R), (A), (B), 0, 3); \
    MAT4_DOT_PROD((R), (A), (B), 1, 0); \
    MAT4_DOT_PROD((R), (A), (B), 1, 1); \
    MAT4_DOT_PROD((R), (A), (B), 1, 2); \
    MAT4_DOT_PROD((R), (A), (B), 1, 3); \
    MAT4_DOT_PROD((R), (A), (B), 2, 0); \
    MAT4_DOT_PROD((R), (A), (B), 2, 1); \
    MAT4_DOT_PROD((R), (A), (B), 2, 2); \
    MAT4_DOT_PROD((R), (A), (B), 2, 3); \
    MAT4_DOT_PROD((R), (A), (B), 3, 0); \
    MAT4_DOT_PROD((R), (A), (B), 3, 1); \
    MAT4_DOT_PROD((R), (A), (B), 3, 2); \
    MAT4_DOT_PROD((R), (A), (B), 3, 3); \
}

#define MTXF_END(mtx) {                         \
    (mtx)[0][3] = (mtx)[1][3] = (mtx)[2][3] = 0;\
    ((u32 *)(mtx))[15] = FLOAT_ONE;             \
}

u16 random_u16(void);
f32 random_float(void);
s32 random_sign(void);

void mtxf_copy(Mat4 dest, Mat4 src);
void mtxf_identity(Mat4 mtx);
void mtxf_translate(Mat4 dest, Vec3f b);
void mtxf_lookat(Mat4 mtx, Vec3f from, Vec3f to, s16 roll);
void mtxf_rotate_zxy_and_translate(Mat4 dest, Vec3f trans, Vec3s rot);
void mtxf_rotate_xyz_and_translate(Mat4 dest, Vec3f trans, Vec3s rot);
void mtxf_rotate_zxy_and_translate_and_mul(Vec3s rot, Vec3f trans, Mat4 dest, Mat4 src);
void mtxf_rotate_xyz_and_translate_and_mul(Vec3s rot, Vec3f trans, Mat4 dest, Mat4 src);
void mtxf_billboard(Mat4 dest, Mat4 mtx, Vec3f position, Vec3f scale, s16 angle);
void mtxf_shadow(Mat4 dest, Vec3f upDir, Vec3f pos, Vec3f scale, s16 yaw);
void mtxf_align_terrain_normal(Mat4 dest, Vec3f upDir, Vec3f pos, s16 yaw);
void mtxf_align_terrain_triangle(Mat4 mtx, Vec3f pos, s16 yaw, f32 radius);
void mtxf_mul(Mat4 dest, Mat4 a, Mat4 b);
void mtxf_scale_vec3f(Mat4 dest, Mat4 mtx, Vec3f s);
void mtxf_mul_vec3s(Mat4 mtx, Vec3s b);

extern void mtxf_to_mtx_fast(s16 *dest, float *src);
ALWAYS_INLINE void mtxf_to_mtx(void *dest, void *src) {
    mtxf_to_mtx_fast((s16*)dest, (float*)src);
    // guMtxF2L(src, dest);
}

void mtxf_rotate_xy(Mtx *mtx, s16 angle);

s16 approach_s16(s16 current, s16 target, s16 inc, s16 dec);
s32 approach_s32(s32 current, s32 target, s32 inc, s32 dec);
f32 approach_f32(f32 current, f32 target, f32 inc, f32 dec);
Bool32 approach_s16_bool(s16 *current, s16 target, s16 inc, s16 dec);
Bool32 approach_s32_bool(s32 *current, s32 target, s32 inc, s32 dec);
Bool32 approach_f32_bool(f32 *current, f32 target, f32 inc, f32 dec);
#define approach_s16_symmetric(current, target, inc) approach_s16((current), (target), (inc), (inc))
#define approach_s32_symmetric(current, target, inc) approach_s32((current), (target), (inc), (inc))
#define approach_f32_symmetric(current, target, inc) approach_f32((current), (target), (inc), (inc))
#define approach_s16_symmetric_bool(current, target, inc) approach_s16_bool((current), (target), (inc), (inc))
#define approach_s32_symmetric_bool(current, target, inc) approach_s32_bool((current), (target), (inc), (inc))
#define approach_f32_symmetric_bool(current, target, inc) approach_f32_bool((current), (target), (inc), (inc))
#define approach_angle approach_s16_symmetric
#define approach_angle_bool approach_s16_symmetric_bool
s32 approach_f32_signed(f32 *current, f32 target, f32 inc);
s32 approach_f32_asymptotic_bool(f32 *current, f32 target, f32 multiplier);
f32 approach_f32_asymptotic(f32 current, f32 target, f32 multiplier);
s16 approach_s16_asymptotic_bool(s16 *current, s16 target, s16 divisor);
s16 approach_s16_asymptotic(s16 current, s16 target, s16 divisor);
s16 abs_angle_diff(s16 a0, s16 a1);
s16 atan2s(f32 y, f32 x);
f32 atan2f(f32 a, f32 b);
void spline_get_weights(Vec4f result, f32 t, UNUSED s32 c);
void anim_spline_init(Vec4s *keyFrames);
s32  anim_spline_poll(Vec3f result);
f32 find_surface_on_ray(Vec3f orig, Vec3f dir, struct Surface **hit_surface, Vec3f hit_pos, s32 flags);

ALWAYS_INLINE f32 remap(f32 x, f32 fromA, f32 toA, f32 fromB, f32 toB) {
    return (x - fromA) / (toA - fromA) * (toB - fromB) + fromB;
}

ALWAYS_INLINE f32 lerpf(f32 from, f32 to, f32 amount) {
    return (from + (to - from) * amount);
}

ALWAYS_INLINE f32 to_smoothstop(f32 x) {
    f32 sq = sqr(1.0f - x);
    return 1.0f - sq;
}

// Commonly known as ease-in
ALWAYS_INLINE f32 smoothstart(f32 from, f32 to, f32 amount) {
    return lerpf(from, to, sqr(amount));
}

// Commonly known as ease-out
ALWAYS_INLINE f32 smoothstop(f32 from, f32 to, f32 amount) {
    return lerpf(from, to, to_smoothstop(amount));
}

// Commonly known as ease-in-out
ALWAYS_INLINE f32 smoothstep(f32 from, f32 to, f32 amount) {
    amount = sqr(amount) * (3.0f - 2.0f * amount);

    return lerpf(from, to, amount);
}

#endif // MATH_UTIL_H
