#pragma once

#include "types.h"
#include "game/camera.h"

s16 reduce_by_dist_from_camera(s16 value, f32 maxDist, f32 posX, f32 posY, f32 posZ);

s32 vec3f_compare(Vec3f pos, f32 posX, f32 posY, f32 posZ);
s32 is_within_100_units_of_mario(f32 posX, f32 posY, f32 posZ);
s32 set_or_approach_f32_asymptotic(f32 *dst, f32 goal, f32 scale);
s32 camera_approach_s16_symmetric_bool(s16 *current, s16 target, s16 increment);
s32 camera_approach_s16_symmetric(s16 current, s16 target, s16 increment);
s32 set_or_approach_s16_symmetric(s16 *current, s16 target, s16 increment);
s32 camera_approach_f32_symmetric_bool(f32 *current, f32 target, f32 increment);
s32 clamp_positions_and_find_yaw(Vec3f pos, Vec3f origin, f32 xMax, f32 xMin, f32 zMax, f32 zMin);
s32 calc_avoid_yaw(s16 yawFromMario, s16 wallYaw);
s32 is_surf_within_bounding_box(struct Surface *surf, f32 xMax, f32 yMax, f32 zMax);
s32 is_behind_surface(Vec3f pos, struct Surface *surf);
s32 is_range_behind_surface(Vec3f from, Vec3f to, struct Surface *surf, s16 range, s16 surfType);
s32 is_mario_behind_surface(UNUSED struct Camera *c, struct Surface *surf);
s32 is_pos_in_bounds(Vec3f pos, Vec3f center, Vec3f bounds, s16 boundsYaw);
s32 offset_yaw_outward_radial(struct Camera *c, s16 areaYaw);
s32 move_point_along_spline(Vec3f p, struct CutsceneSplinePoint spline[], s16 *splineSegment, f32 *progress);

f32 camera_approach_f32_symmetric(f32 current, f32 target, f32 increment);
s16 calculate_pitch(Vec3f from, Vec3f to);
s16 calculate_yaw(Vec3f from, Vec3f to);
f32 calc_abs_dist(Vec3f a, Vec3f b);
f32 calc_abs_dist_squared(Vec3f a, Vec3f b);
f32 calc_hor_dist(Vec3f a, Vec3f b);

void object_pos_to_vec3f(Vec3f dst, struct Object *obj);
void vec3f_to_object_pos(struct Object *obj, Vec3f src);
void clamp_pitch(Vec3f from, Vec3f to, s16 maxPitch, s16 minPitch);
void approach_vec3f_asymptotic(Vec3f current, Vec3f target, f32 xMul, f32 yMul, f32 zMul);
void set_or_approach_vec3f_asymptotic(Vec3f dst, Vec3f goal, f32 xMul, f32 yMul, f32 zMul);
void approach_vec3s_asymptotic(Vec3s current, Vec3s target, s16 xMul, s16 yMul, s16 zMul);
void random_vec3s(Vec3s dst, s16 xRange, s16 yRange, s16 zRange);
void scale_along_line(Vec3f dst, Vec3f from, Vec3f to, f32 scale);
void calculate_angles(Vec3f from, Vec3f to, s16 *pitch, s16 *yaw);
void rotate_in_xz(Vec3f dst, Vec3f src, s16 yaw);
void rotate_in_yz(Vec3f dst, Vec3f src, s16 pitch);
void set_camera_pitch_shake(s16 mag, s16 decay, s16 inc);
void set_camera_yaw_shake(s16 mag, s16 decay, s16 inc);
void set_camera_roll_shake(s16 mag, s16 decay, s16 inc);
void set_pitch_shake_from_point(s16 mag, s16 decay, s16 inc, f32 maxDist, f32 posX, f32 posY, f32 posZ);
void set_yaw_shake_from_point(s16 mag, s16 decay, s16 inc, f32 maxDist, f32 posX, f32 posY, f32 posZ);
void increment_shake_offset(s16 *offset, s16 increment);
void shake_camera_pitch(Vec3f pos, Vec3f focus);
void shake_camera_yaw(Vec3f pos, Vec3f focus);
void shake_camera_roll(s16 *roll);
void evaluate_cubic_spline(f32 u, Vec3f Q, Vec3f spline1, Vec3f spline2, Vec3f spline3, Vec3f spline4);
void rotate_and_move_vec3f(Vec3f to, Vec3f from, f32 incDist, s16 incPitch, s16 incYaw);
void offset_rotated(Vec3f dst, Vec3f from, Vec3f to, Vec3s rotation);
void offset_rotated_coords(Vec3f dst, Vec3f from, Vec3s rotation, f32 xTo, f32 yTo, f32 zTo);
