#include "camera/camera_math.h"
#include "camera/camera_modes.h"
#include "engine/math_util.h"
#include "engine/surface_collision.h"
#include "game/camera.h"

/**
 * Stores Mario's yaw around the stairs, relative to the camera's position.
 */
s16 sSpiralStairsYawOffset;

/**
 * Rotates the camera around the spiral staircase.
 */
s32 update_spiral_stairs_camera(struct Camera *c, Vec3f focus, Vec3f pos) {
    /// The returned yaw
    s16 camYaw;
    /// The focus (Mario)'s yaw around the stairs
    s16 focYaw;
    /// The camera's yaw around the stairs
    s16 posYaw;
    Vec3f cPos;
    Vec3f checkPos;
    struct Surface *floor;
    f32 focusHeight;
    f32 floorHeight;
    f32 focY;

    handle_c_button_movement(c);
    // Set base pos to the center of the staircase
    vec3f_set(sFixedModeBasePosition, -1280.f, 614.f, 1740.f);

    // Focus on Mario, and move the focus up the staircase with him
    calc_y_to_curr_floor(&focusHeight, 1.f, 200.f, &focusHeight, 0.9f, 200.f);
    focus[0] = sMarioCamState->pos[0];
    focY = sMarioCamState->pos[1] + 125.f + focusHeight;
    focus[2] = sMarioCamState->pos[2];

    vec3f_copy(cPos, pos);
    vec3f_get_yaw(sFixedModeBasePosition, focus, &focYaw);
    vec3f_get_yaw(sFixedModeBasePosition, cPos,  &posYaw);

    sSpiralStairsYawOffset = posYaw - focYaw;
    // posYaw will change if Mario is more than 90 degrees around the stairs, relative to the camera
    if (sSpiralStairsYawOffset < DEGREES(-90)) {
        sSpiralStairsYawOffset = DEGREES(-90);
    }
    if (sSpiralStairsYawOffset > DEGREES(90)) {
        sSpiralStairsYawOffset = DEGREES(90);
    }
    focYaw += sSpiralStairsYawOffset;
    posYaw = focYaw;

    vec3f_set_dist_and_angle(sFixedModeBasePosition, cPos, 300.f, 0, posYaw);

    // Move the camera's y coord up/down the staircase
    checkPos[0] = focus[0] + (cPos[0] - focus[0]) * 0.7f;
    checkPos[1] = focus[1] + (cPos[1] - focus[1]) * 0.7f + 300.f;
    checkPos[2] = focus[2] + (cPos[2] - focus[2]) * 0.7f;
    floorHeight = find_floor(checkPos[0], checkPos[1] + 50.f, checkPos[2], &floor);

    if (floorHeight != FLOOR_LOWER_LIMIT) {
        if (floorHeight < sMarioGeometry.currFloorHeight) {
            floorHeight = sMarioGeometry.currFloorHeight;
        }
        pos[1] = approach_f32(pos[1], (floorHeight += 125.f), 30.f, 30.f);
    }

    camera_approach_f32_symmetric_bool(&focus[1], focY, 30.f);
    pos[0] = cPos[0];
    pos[2] = cPos[2];
    camYaw = calculate_yaw(focus, pos);

    return camYaw;
}

/**
 * The mode used in the spiral staircase in the castle
 */
void mode_spiral_stairs_camera(struct Camera *c) {
    c->nextYaw = update_spiral_stairs_camera(c, c->focus, c->pos);
}
