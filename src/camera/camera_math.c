#include "types.h"
#include "camera/camera_math.h"
#include "engine/math_util.h"
#include "game/camera.h"
#include "game/level_update.h"

/**
 * Rotates the offset `to` according to the pitch and yaw values in `rotation`.
 * Adds `from` to the rotated offset, and stores the result in `dst`.
 *
 * @warning Flips the Z axis, so that relative to `rotation`, -Z moves forwards and +Z moves backwards.
 */
void offset_rotated(Vec3f dst, Vec3f from, Vec3f to, Vec3s rotation) {
    Vec3f pitchRotated;

    // First rotate the direction by rotation's pitch
    //! The Z axis is flipped here.
    pitchRotated[2] = -(to[2] * coss(rotation[0]) - to[1] * sins(rotation[0]));
    pitchRotated[1] =   to[2] * sins(rotation[0]) + to[1] * coss(rotation[0]);
    pitchRotated[0] =   to[0];

    // Rotate again by rotation's yaw
    dst[0] = from[0] + pitchRotated[2] * sins(rotation[1]) + pitchRotated[0] * coss(rotation[1]);
    dst[1] = from[1] + pitchRotated[1];
    dst[2] = from[2] + pitchRotated[2] * coss(rotation[1]) - pitchRotated[0] * sins(rotation[1]);
}

/**
 * Rotates the offset defined by (`xTo`, `yTo`, `zTo`) according to the pitch and yaw values in `rotation`.
 * Adds `from` to the rotated offset, and stores the result in `dst`.
 *
 * @warning Flips the Z axis, so that relative to `rotation`, -Z moves forwards and +Z moves backwards.
 */
void offset_rotated_coords(Vec3f dst, Vec3f from, Vec3s rotation, f32 xTo, f32 yTo, f32 zTo) {
    Vec3f to;

    vec3f_set(to, xTo, yTo, zTo);
    offset_rotated(dst, from, to, rotation);
}


void object_pos_to_vec3f(Vec3f dst, struct Object *obj) {
    dst[0] = obj->oPosX;
    dst[1] = obj->oPosY;
    dst[2] = obj->oPosZ;
}

void vec3f_to_object_pos(struct Object *obj, Vec3f src) {
    obj->oPosX = src[0];
    obj->oPosY = src[1];
    obj->oPosZ = src[2];
}

/**
 * Compare a vector to a position, return TRUE if they match.
 */
s32 vec3f_compare(Vec3f pos, f32 posX, f32 posY, f32 posZ) {
    return pos[0] == posX
        && pos[1] == posY
        && pos[2] == posZ;
}

void clamp_pitch(Vec3f from, Vec3f to, s16 maxPitch, s16 minPitch) {
    s16 pitch;
    s16 yaw;
    f32 dist;

    vec3f_get_dist_and_angle(from, to, &dist, &pitch, &yaw);
    pitch = CLAMP(pitch, minPitch, maxPitch);
    vec3f_set_dist_and_angle(from, to, dist, pitch, yaw);
}

s32 is_within_100_units_of_mario(f32 posX, f32 posY, f32 posZ) {
    Vec3f pos;
    vec3f_set(pos, posX, posY, posZ);

    return calc_abs_dist_squared(sMarioCamState->pos, pos) < sqr(100.f);
}

s32 set_or_approach_f32_asymptotic(f32 *dst, f32 goal, f32 scale) {
    if (sStatusFlags & CAM_FLAG_SMOOTH_MOVEMENT) {
        approach_f32_asymptotic_bool(dst, goal, scale);
    } else {
        *dst = goal;
    }
    if (*dst == goal) {
        return FALSE;
    } else {
        return TRUE;
    }
}

/**
 * Applies the approach_f32_asymptotic_bool function to each of the X, Y, & Z components of the given
 * vector.
 */
void approach_vec3f_asymptotic(Vec3f current, Vec3f target, f32 xMul, f32 yMul, f32 zMul) {
    approach_f32_asymptotic_bool(&current[0], target[0], xMul);
    approach_f32_asymptotic_bool(&current[1], target[1], yMul);
    approach_f32_asymptotic_bool(&current[2], target[2], zMul);
}

/**
 * Applies the set_or_approach_f32_asymptotic_bool function to each of the X, Y, & Z components of the
 * given vector.
 */
void set_or_approach_vec3f_asymptotic(Vec3f dst, Vec3f goal, f32 xMul, f32 yMul, f32 zMul) {
    set_or_approach_f32_asymptotic(&dst[0], goal[0], xMul);
    set_or_approach_f32_asymptotic(&dst[1], goal[1], yMul);
    set_or_approach_f32_asymptotic(&dst[2], goal[2], zMul);
}

/**
 * Applies the approach_s32_asymptotic function to each of the X, Y, & Z components of the given
 * vector.
 */
void approach_vec3s_asymptotic(Vec3s current, Vec3s target, s16 xMul, s16 yMul, s16 zMul) {
    approach_s16_asymptotic_bool(&current[0], target[0], xMul);
    approach_s16_asymptotic_bool(&current[1], target[1], yMul);
    approach_s16_asymptotic_bool(&current[2], target[2], zMul);
}

s32 camera_approach_s16_symmetric_bool(s16 *current, s16 target, s16 increment) {
    s16 dist = target - *current;

    if (increment < 0) {
        increment = -1 * increment;
    }
    if (dist > 0) {
        dist -= increment;
        if (dist >= 0) {
            *current = target - dist;
        } else {
            *current = target;
        }
    } else {
        dist += increment;
        if (dist <= 0) {
            *current = target - dist;
        } else {
            *current = target;
        }
    }
    if (*current == target) {
        return FALSE;
    } else {
        return TRUE;
    }
}

s32 camera_approach_s16_symmetric(s16 current, s16 target, s16 increment) {
    s16 dist = target - current;

    if (increment < 0) {
        increment = -1 * increment;
    }
    if (dist > 0) {
        dist -= increment;
        if (dist >= 0) {
            current = target - dist;
        } else {
            current = target;
        }
    } else {
        dist += increment;
        if (dist <= 0) {
            current = target - dist;
        } else {
            current = target;
        }
    }
    return current;
}

s32 set_or_approach_s16_symmetric(s16 *current, s16 target, s16 increment) {
    if (sStatusFlags & CAM_FLAG_SMOOTH_MOVEMENT) {
        return camera_approach_s16_symmetric_bool(current, target, increment);
    } else {
        *current = target;
    }
    if (*current == target) {
        return FALSE;
    } else {
        return TRUE;
    }
}

/**
 * Approaches a value by a given increment, returns FALSE if the target is reached.
 * Appears to be a strange way of implementing approach_f32_symmetric from object_helpers.c.
 * It could possibly be an older version of the function
 */
s32 camera_approach_f32_symmetric_bool(f32 *current, f32 target, f32 increment) {
    f32 dist = target - *current;

    if (increment < 0) {
        increment = -1 * increment;
    }
    if (dist > 0) {
        dist -= increment;
        if (dist > 0) {
            *current = target - dist;
        } else {
            *current = target;
        }
    } else {
        dist += increment;
        if (dist < 0) {
            *current = target - dist;
        } else {
            *current = target;
        }
    }
    if (*current == target) {
        return FALSE;
    } else {
        return TRUE;
    }
}

/**
 * Nearly the same as the above function, this one returns the new value in place of a bool.
 */
f32 camera_approach_f32_symmetric(f32 current, f32 target, f32 increment) {
    f32 dist = target - current;

    if (increment < 0) {
        increment = -1 * increment;
    }
    if (dist > 0) {
        dist -= increment;
        if (dist > 0) {
            current = target - dist;
        } else {
            current = target;
        }
    } else {
        dist += increment;
        if (dist < 0) {
            current = target - dist;
        } else {
            current = target;
        }
    }
    return current;
}

/**
 * Generate a vector with all three values about zero. The
 * three ranges determine how wide the range about zero.
 */
void random_vec3s(Vec3s dst, s16 xRange, s16 yRange, s16 zRange) {
    f32 randomFloat;
    f32 tempXRange;
    f32 tempYRange;
    f32 tempZRange;

    randomFloat = random_float();
    tempXRange = xRange;
    dst[0] = randomFloat * tempXRange - tempXRange / 2;

    randomFloat = random_float();
    tempYRange = yRange;
    dst[1] = randomFloat * tempYRange - tempYRange / 2;

    randomFloat = random_float();
    tempZRange = zRange;
    dst[2] = randomFloat * tempZRange - tempZRange / 2;
}

/**
 * Decrease value by multiplying it by the distance from (`posX`, `posY`, `posZ`) to
 * the camera divided by `maxDist`
 *
 * @return the reduced value
 */
s16 reduce_by_dist_from_camera(s16 value, f32 maxDist, f32 posX, f32 posY, f32 posZ) {
    Vec3f pos;
    f32 dist;
    s16 pitch, yaw;
    s16 goalPitch, goalYaw;
    s16 result = 0;
    // Direction from pos to (Lakitu's) goalPos
    f32 goalDX = gLakituState.goalPos[0] - posX;
    f32 goalDY = gLakituState.goalPos[1] - posY;
    f32 goalDZ = gLakituState.goalPos[2] - posZ;

    dist = sqrtf(goalDX * goalDX + goalDY * goalDY + goalDZ * goalDZ);
    if (maxDist > dist) {
        pos[0] = posX;
        pos[1] = posY;
        pos[2] = posZ;
        vec3f_get_dist_and_angle(gLakituState.goalPos, pos, &dist, &pitch, &yaw);
        if (dist < maxDist) {
            calculate_angles(gLakituState.goalPos, gLakituState.goalFocus, &goalPitch, &goalYaw);
            pitch -= goalPitch;
            yaw -= goalYaw;
            dist -= 2000.f;
            if (dist < 0.f) {
                dist = 0.f;
            }
            maxDist -= 2000.f;
            if (maxDist < 2000.f) {
                maxDist = 2000.f;
            }
            result = value * (1.f - dist / maxDist);
            if (pitch < -0x1800 || pitch > 0x400 ||
                yaw   < -0x1800 || yaw >   0x1800) {
                result /= 2;
            }
        }
    }
    return result;
}

s32 clamp_positions_and_find_yaw(Vec3f pos, Vec3f origin, f32 xMax, f32 xMin, f32 zMax, f32 zMin) {
    s16 yaw = gCamera->nextYaw;

    if (pos[0] >= xMax) {
        pos[0] = xMax;
    }
    if (pos[0] <= xMin) {
        pos[0] = xMin;
    }
    if (pos[2] >= zMax) {
        pos[2] = zMax;
    }
    if (pos[2] <= zMin) {
        pos[2] = zMin;
    }
    yaw = calculate_yaw(origin, pos);
    return yaw;
}

/**
 * The yaw passed here is the yaw of the direction FROM Mario TO Lakitu.
 *
 * wallYaw always has 90 degrees added to it before this is called -- it's parallel to the wall.
 *
 * @return the new yaw from Mario to rotate towards.
 *
 * @warning this is jank. It actually returns the yaw that will rotate further INTO the wall. So, the
 *          developers just add 180 degrees to the result.
 */
s32 calc_avoid_yaw(s16 yawFromMario, s16 wallYaw) {
    s16 yawDiff;
    yawDiff = wallYaw - yawFromMario + DEGREES(90);

    if (yawDiff < 0) {
        // Deflect to the right
        yawFromMario = wallYaw;
    } else {
        // Note: this favors the left side if the wall is exactly perpendicular to the camera.
        // Deflect to the left
        yawFromMario = wallYaw + DEGREES(180);
    }
    return yawFromMario;
}


/**
 * Checks if `surf` is within the rect prism defined by xMax, yMax, and zMax
 *
 * @param surf surface to check
 * @param xMax absolute-value max size in x, set to -1 to ignore
 * @param yMax absolute-value max size in y, set to -1 to ignore
 * @param zMax absolute-value max size in z, set to -1 to ignore
 */
s32 is_surf_within_bounding_box(struct Surface *surf, f32 xMax, f32 yMax, f32 zMax) {
    // Surface vertex coordinates
    Vec3s sx, sy, sz;
    // Max delta between x, y, and z
    s16 dxMax = 0;
    s16 dyMax = 0;
    s16 dzMax = 0;
    // Current deltas between x, y, and z
    f32 dx, dy, dz;
    s32 i, j;
    // result
    s32 smaller = FALSE;

    sx[0] = surf->vertex1[0];
    sx[1] = surf->vertex2[0];
    sx[2] = surf->vertex3[0];
    sy[0] = surf->vertex1[1];
    sy[1] = surf->vertex2[1];
    sy[2] = surf->vertex3[1];
    sz[0] = surf->vertex1[2];
    sz[1] = surf->vertex2[2];
    sz[2] = surf->vertex3[2];

    for (i = 0; i < 3; i++) {
        j = i + 1;
        if (j >= 3) {
            j = 0;
        }
        dx = abss(sx[i] - sx[j]);
        if (dx > dxMax) {
            dxMax = dx;
        }
        dy = abss(sy[i] - sy[j]);
        if (dy > dyMax) {
            dyMax = dy;
        }
        dz = abss(sz[i] - sz[j]);
        if (dz > dzMax) {
            dzMax = dz;
        }
    }
    if (yMax != -1.f) {
        if (dyMax < yMax) {
            smaller = TRUE;
        }
    }
    if (xMax != -1.f && zMax != -1.f) {
        if (dxMax < xMax && dzMax < zMax) {
            smaller = TRUE;
        }
    }
    return smaller;
}

/**
 * Checks if `pos` is behind the surface, using the dot product.
 *
 * Because the function only uses `surf`s first vertex, some surfaces can shadow others.
 */
s32 is_behind_surface(Vec3f pos, struct Surface *surf) {
    s32 behindSurface = 0;
    // Surface normal
    f32 normX = (surf->vertex2[1] - surf->vertex1[1]) * (surf->vertex3[2] - surf->vertex2[2]) -
                (surf->vertex3[1] - surf->vertex2[1]) * (surf->vertex2[2] - surf->vertex1[2]);
    f32 normY = (surf->vertex2[2] - surf->vertex1[2]) * (surf->vertex3[0] - surf->vertex2[0]) -
                (surf->vertex3[2] - surf->vertex2[2]) * (surf->vertex2[0] - surf->vertex1[0]);
    f32 normZ = (surf->vertex2[0] - surf->vertex1[0]) * (surf->vertex3[1] - surf->vertex2[1]) -
                (surf->vertex3[0] - surf->vertex2[0]) * (surf->vertex2[1] - surf->vertex1[1]);
    f32 dirX = surf->vertex1[0] - pos[0];
    f32 dirY = surf->vertex1[1] - pos[1];
    f32 dirZ = surf->vertex1[2] - pos[2];

    if (dirX * normX + dirY * normY + dirZ * normZ < 0) {
        behindSurface = 1;
    }
    return behindSurface;
}

/**
 * Checks if the whole circular sector is behind the surface.
 */
s32 is_range_behind_surface(Vec3f from, Vec3f to, struct Surface *surf, s16 range, s16 surfType) {
    s32 behindSurface = TRUE;
    s32 leftBehind = 0;
    s32 rightBehind = 0;
    f32 checkDist;
    s16 checkPitch;
    s16 checkYaw;
    Vec3f checkPos;

    if (surf != NULL) {
        if (surfType == SURFACE_NULL || surf->type != surfType) {
            if (range == 0) {
                behindSurface = is_behind_surface(to, surf);
            } else {
                vec3f_get_dist_and_angle(from, to, &checkDist, &checkPitch, &checkYaw);
                vec3f_set_dist_and_angle(from, checkPos, checkDist, checkPitch, checkYaw + range);
                leftBehind = is_behind_surface(checkPos, surf);
                vec3f_set_dist_and_angle(from, checkPos, checkDist, checkPitch, checkYaw - range);
                rightBehind = is_behind_surface(checkPos, surf);
                behindSurface = leftBehind * rightBehind;
            }
        }
    }
    return behindSurface;
}

s32 is_mario_behind_surface(UNUSED struct Camera *c, struct Surface *surf) {
    s32 behindSurface = is_behind_surface(sMarioCamState->pos, surf);

    return behindSurface;
}

/**
 * Calculates the distance between two points and sets a vector to a point
 * scaled along a line between them. Typically, somewhere in the middle.
 */
void scale_along_line(Vec3f dst, Vec3f from, Vec3f to, f32 scale) {
    dst[0] = (to[0] - from[0]) * scale + from[0];
    dst[1] = (to[1] - from[1]) * scale + from[1];
    dst[2] = (to[2] - from[2]) * scale + from[2];
}
/**
 * Effectively created a rectangular prism defined by a vector starting at the center
 * and extending to the corners. If the position is in this box, the function returns true.
 */
s32 is_pos_in_bounds(Vec3f pos, Vec3f center, Vec3f bounds, s16 boundsYaw) {
    Vec3f rel;
    vec3_diff(rel, center, pos);

    rotate_in_xz(rel, rel, boundsYaw);

    return (-bounds[0] < rel[0] && rel[0] < bounds[0] &&
            -bounds[1] < rel[1] && rel[1] < bounds[1] &&
            -bounds[2] < rel[2] && rel[2] < bounds[2]);
}

s16 calculate_pitch(Vec3f from, Vec3f to) {
    f32 dx = to[0] - from[0];
    f32 dy = to[1] - from[1];
    f32 dz = to[2] - from[2];
    s16 pitch = atan2s(sqrtf(dx * dx + dz * dz), dy);

    return pitch;
}

s16 calculate_yaw(Vec3f from, Vec3f to) {
    f32 dx = to[0] - from[0];
    // UNUSED f32 dy = to[1] - from[1];
    f32 dz = to[2] - from[2];

    return atan2s(dz, dx);
}

/**
 * Calculates the pitch and yaw between two vectors.
 */
void calculate_angles(Vec3f from, Vec3f to, s16 *pitch, s16 *yaw) {
    f32 dx = to[0] - from[0];
    f32 dy = to[1] - from[1];
    f32 dz = to[2] - from[2];

    *pitch = atan2s(sqrtf(sqr(dx) + sqr(dz)), dy);
    *yaw = atan2s(dz, dx);
}

/**
 * Finds the distance between two vectors.
 */
f32 calc_abs_dist(Vec3f a, Vec3f b) {
    register f32 distX = b[0] - a[0];
    register f32 distY = b[1] - a[1];
    register f32 distZ = b[2] - a[2];

    return sqrtf(sqr(distX) + sqr(distY) + sqr(distZ));
}

f32 calc_abs_dist_squared(Vec3f a, Vec3f b) {
    register f32 distX = b[0] - a[0];
    register f32 distY = b[1] - a[1];
    register f32 distZ = b[2] - a[2];

    return (sqr(distX) + sqr(distY) + sqr(distZ));
}

/**
 * Finds the horizontal distance between two vectors.
 */
f32 calc_hor_dist(Vec3f a, Vec3f b) {
    register f32 distX = b[0] - a[0];
    register f32 distZ = b[2] - a[2];

    return sqrtf(sqr(distX) + sqr(distZ));
}

/**
 * Rotates a vector in the horizontal plane and copies it to a new vector.
 */
void rotate_in_xz(Vec3f dst, Vec3f src, s16 yaw) {
    register f32 x = src[0];
    register f32 z = src[2];
    register f32 sy = sins(yaw);
    register f32 cy = coss(yaw);

    dst[0] = z * sy + x * cy;
    dst[1] = src[1];
    dst[2] = z * cy - x * sy;
}

/**
 * Rotates a vector in the YZ plane and copies it to a new vector.
 *
 * Note: This function also flips the Z axis, so +Z moves forward, not backward like it would in world
 * space. If possible, use vec3f_set_dist_and_angle()
 */
void rotate_in_yz(Vec3f dst, Vec3f src, s16 pitch) {
    dst[2] = -(src[2] * coss(pitch) - src[1] * sins(pitch));
    dst[1] =   src[2] * sins(pitch) + src[1] * coss(pitch);
    dst[0] =   src[0];
}

/**
 * Start shaking the camera's pitch (up and down)
 */
void set_camera_pitch_shake(s16 mag, s16 decay, s16 inc) {
    if (gLakituState.shakeMagnitude[0] < mag) {
        gLakituState.shakeMagnitude[0] = mag;
        gLakituState.shakePitchDecay = decay;
        gLakituState.shakePitchVel = inc;
    }
}

/**
 * Start shaking the camera's yaw (side to side)
 */
void set_camera_yaw_shake(s16 mag, s16 decay, s16 inc) {
    if (abss(mag) > abss(gLakituState.shakeMagnitude[1])) {
        gLakituState.shakeMagnitude[1] = mag;
        gLakituState.shakeYawDecay = decay;
        gLakituState.shakeYawVel = inc;
    }
}

/**
 * Start shaking the camera's roll (rotate screen clockwise and counterclockwise)
 */
void set_camera_roll_shake(s16 mag, s16 decay, s16 inc) {
    if (gLakituState.shakeMagnitude[2] < mag) {
        gLakituState.shakeMagnitude[2] = mag;
        gLakituState.shakeRollDecay = decay;
        gLakituState.shakeRollVel = inc;
    }
}

/**
 * Start shaking the camera's pitch, but reduce `mag` by it's distance from the camera
 */
void set_pitch_shake_from_point(s16 mag, s16 decay, s16 inc, f32 maxDist, f32 posX, f32 posY, f32 posZ) {
    mag = reduce_by_dist_from_camera(mag, maxDist, posX, posY, posZ);
    if (mag != 0) {
        set_camera_pitch_shake(mag, decay, inc);
    }
}

/**
 * Start shaking the camera's yaw, but reduce `mag` by it's distance from the camera
 */
void set_yaw_shake_from_point(s16 mag, s16 decay, s16 inc, f32 maxDist, f32 posX, f32 posY, f32 posZ) {
    mag = reduce_by_dist_from_camera(mag, maxDist, posX, posY, posZ);
    if (mag != 0) {
        set_camera_yaw_shake(mag, decay, inc);
    }
}

/**
 * Update the shake offset by `increment`
 */
void increment_shake_offset(s16 *offset, s16 increment) {
    if (increment == -0x8000) {
        *offset = (*offset & 0x8000) + 0xC000;
    } else {
        *offset += increment;
    }
}

/**
 * Apply a vertical shake to the camera by adjusting its pitch
 */
void shake_camera_pitch(Vec3f pos, Vec3f focus) {
    f32 dist;
    s16 pitch, yaw;

    if (gLakituState.shakeMagnitude[0] | gLakituState.shakeMagnitude[1]) {
        vec3f_get_dist_and_angle(pos, focus, &dist, &pitch, &yaw);
        pitch += gLakituState.shakeMagnitude[0] * sins(gLakituState.shakePitchPhase);
        vec3f_set_dist_and_angle(pos, focus, dist, pitch, yaw);
        increment_shake_offset(&gLakituState.shakePitchPhase, gLakituState.shakePitchVel);
        if (camera_approach_s16_symmetric_bool(&gLakituState.shakeMagnitude[0], 0,
                                               gLakituState.shakePitchDecay) == 0) {
            gLakituState.shakePitchPhase = 0;
        }
    }
}

/**
 * Apply a horizontal shake to the camera by adjusting its yaw
 */
void shake_camera_yaw(Vec3f pos, Vec3f focus) {
    f32 dist;
    s16 pitch, yaw;

    if (gLakituState.shakeMagnitude[1] != 0) {
        vec3f_get_dist_and_angle(pos, focus, &dist, &pitch, &yaw);
        yaw += gLakituState.shakeMagnitude[1] * sins(gLakituState.shakeYawPhase);
        vec3f_set_dist_and_angle(pos, focus, dist, pitch, yaw);
        increment_shake_offset(&gLakituState.shakeYawPhase, gLakituState.shakeYawVel);
        if (camera_approach_s16_symmetric_bool(&gLakituState.shakeMagnitude[1], 0,
                                               gLakituState.shakeYawDecay) == 0) {
            gLakituState.shakeYawPhase = 0;
        }
    }
}

/**
 * Apply a rotational shake to the camera by adjusting its roll
 */
void shake_camera_roll(s16 *roll) {
    if (gLakituState.shakeMagnitude[2] != 0) {
        increment_shake_offset(&gLakituState.shakeRollPhase, gLakituState.shakeRollVel);
        *roll += gLakituState.shakeMagnitude[2] * sins(gLakituState.shakeRollPhase);
        if (camera_approach_s16_symmetric_bool(&gLakituState.shakeMagnitude[2], 0,
                                               gLakituState.shakeRollDecay) == 0) {
            gLakituState.shakeRollPhase = 0;
        }
    }
}

/**
 * Add an offset to the camera's yaw, used in levels that are inside a rectangular building, like the
 * pyramid or TTC.
 */
s32 offset_yaw_outward_radial(struct Camera *c, s16 areaYaw) {
    s16 yawGoal = DEGREES(60);
    s16 yaw = sModeOffsetYaw;
    Vec3f areaCenter;
    s16 dYaw;
    switch (gCurrLevelArea) {
        case AREA_TTC:
            areaCenter[0] = c->areaCenX;
            areaCenter[1] = sMarioCamState->pos[1];
            areaCenter[2] = c->areaCenZ;
            if (sqr(800.f) > calc_abs_dist_squared(areaCenter, sMarioCamState->pos)) {
                yawGoal = 0x3800;
            }
            break;
        case AREA_SSL_PYRAMID:
            // This mask splits the 360 degrees of yaw into 4 corners. It adds 45 degrees so that the yaw
            // offset at the corner will be 0, but the yaw offset near the center will face more towards
            // the direction Mario is running in.
            yawGoal = (areaYaw & 0xC000) - areaYaw + DEGREES(45);
            if (yawGoal < 0) {
                yawGoal = -yawGoal;
            }
            yawGoal = yawGoal / 32 * 48;
            break;
        case AREA_LLL_OUTSIDE:
            yawGoal = 0;
            break;
    }
    dYaw = gMarioStates[0].forwardVel / 32.f * 128.f;

    if (sAreaYawChange < 0) {
        camera_approach_s16_symmetric_bool(&yaw, -yawGoal, dYaw);
    }
    if (sAreaYawChange > 0) {
        camera_approach_s16_symmetric_bool(&yaw, yawGoal, dYaw);
    }
    // When the final yaw is out of [-60,60] degrees, approach yawGoal faster than dYaw will ever be,
    // making the camera lock in one direction until yawGoal drops below 60 (or Mario presses a C button)
    if (yaw < -DEGREES(60)) {
        //! Maybe they meant to reverse yawGoal's sign?
        camera_approach_s16_symmetric_bool(&yaw, -yawGoal, 0x200);
    }
    if (yaw > DEGREES(60)) {
        //! Maybe they meant to reverse yawGoal's sign?
        camera_approach_s16_symmetric_bool(&yaw, yawGoal, 0x200);
    }
    return yaw;
}

/*************************** SPLINES ***************************/

/**
 * Produces values using a cubic b-spline curve. Basically Q is the used output,
 * u is a value between 0 and 1 that represents the position along the spline,
 * and a0-a3 are parameters that define the spline.
 *
 * The spline is described at www2.cs.uregina.ca/~anima/408/Notes/Interpolation/UniformBSpline.htm
 */
void evaluate_cubic_spline(f32 u, Vec3f Q, Vec3f spline1, Vec3f spline2, Vec3f spline3, Vec3f spline4) {
    f32 B[4];
    if (u > 1.0f) u = 1.0f;

    register f32 nu = 1.0f - u;
    register f32 su = sqr(u);
    register f32 hcu = (su * u) / 2.0f;

    B[0] = (nu * nu * nu) / 6.0f;
    B[1] = hcu - su + (2.0f / 3.0f);
    B[2] = -hcu + (su / 2.0f) + (u / 2.0f) + (1.0f / 6.0f);
    B[3] =  hcu / 3.0f;

    Q[0] = (B[0] * spline1[0]) + (B[1] * spline2[0]) + (B[2] * spline3[0]) + (B[3] * spline4[0]);
    Q[1] = (B[0] * spline1[1]) + (B[1] * spline2[1]) + (B[2] * spline3[1]) + (B[3] * spline4[1]);
    Q[2] = (B[0] * spline1[2]) + (B[1] * spline2[2]) + (B[2] * spline3[2]) + (B[3] * spline4[2]);
}

/**
 * Computes the point that is `progress` percent of the way through segment `splineSegment` of `spline`,
 * and stores the result in `p`. `progress` and `splineSegment` are updated if `progress` becomes >= 1.0.
 *
 * When neither of the next two points' speeds == 0, the number of frames is between 1 and 255. Otherwise
 * it's infinite.
 *
 * To calculate the number of frames it will take to progress through a spline segment:
 * If the next two speeds are the same and nonzero, it's 1.0 / firstSpeed.
 *
 * s1 and s2 are short hand for first/secondSpeed. The progress at any frame n is defined by a recurrency relation:
 *      p(n+1) = (s2 - s1 + 1) * p(n) + s1
 * Which can be written as
 *      p(n) = (s2 * ((s2 - s1 + 1)^(n) - 1)) / (s2 - s1)
 *
 * Solving for the number of frames:
 *      n = log(((s2 - s1) / s1) + 1) / log(s2 - s1 + 1)
 *
 * @return 1 if the point has reached the end of the spline, when `progress` reaches 1.0 or greater, and
 * the 4th CutsceneSplinePoint in the current segment away from spline[splineSegment] has an index of -1.
 */
s32 move_point_along_spline(Vec3f p, struct CutsceneSplinePoint spline[], s16 *splineSegment, f32 *progress) {
    s32 finished = FALSE;
    Vec3f controlPoints[4];
    s32 i = 0;
    f32 u = *progress;
    f32 progressChange;
    f32 firstSpeed = 0;
    f32 secondSpeed = 0;
    s32 segment = *splineSegment;

    if (*splineSegment < 0) {
        segment = 0;
        u = 0;
    }
    if (spline[segment].index == -1 || spline[segment + 1].index == -1 || spline[segment + 2].index == -1) {
        return 1;
    }

    for (i = 0; i < 4; i++) {
        controlPoints[i][0] = spline[segment + i].point[0];
        controlPoints[i][1] = spline[segment + i].point[1];
        controlPoints[i][2] = spline[segment + i].point[2];
    }
    evaluate_cubic_spline(u, p, controlPoints[0], controlPoints[1], controlPoints[2], controlPoints[3]);

    if (spline[*splineSegment + 1].speed != 0) {
        firstSpeed = 1.0f / spline[*splineSegment + 1].speed;
    }
    if (spline[*splineSegment + 2].speed != 0) {
        secondSpeed = 1.0f / spline[*splineSegment + 2].speed;
    }
    progressChange = (secondSpeed - firstSpeed) * *progress + firstSpeed;

    if (1 <= (*progress += progressChange)) {
        (*splineSegment)++;
        if (spline[*splineSegment + 3].index == -1) {
            *splineSegment = 0;
            finished = 1;
        }
        (*progress)--;
    }
    return finished;
}

/**
 * Change the spherical coordinates of `to` relative to `from` by `incDist`, `incPitch`, and `incYaw`
 *
 * @param from    the base position
 * @param[out] to the destination position
 */
void rotate_and_move_vec3f(Vec3f to, Vec3f from, f32 incDist, s16 incPitch, s16 incYaw) {
    f32 dist;
    s16 pitch, yaw;

    vec3f_get_dist_and_angle(from, to, &dist, &pitch, &yaw);
    pitch += incPitch;
    yaw += incYaw;
    dist += incDist;
    vec3f_set_dist_and_angle(from, to, dist, pitch, yaw);
}

