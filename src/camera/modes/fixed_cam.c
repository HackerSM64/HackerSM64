#include "camera/camera_math.h"
#include "camera/camera_modes.h"
#include "engine/math_util.h"
#include "engine/surface_collision.h"
#include "game/camera.h"


/**
 * Updates the camera during fixed mode.
 */
s32 update_fixed_camera(struct Camera *c, Vec3f focus, UNUSED Vec3f pos) {
    f32 focusFloorOff;
    f32 goalHeight;
    f32 ceilHeight;
    f32 heightOffset;
    f32 distCamToFocus;
    f32 scaleToMario = 0.5f;
    s16 pitch;
    s16 yaw;
    Vec3s faceAngle;
    struct Surface *ceiling;
    Vec3f basePos;

    play_camera_buzz_if_c_sideways();

    // Don't move closer to Mario in these areas
    switch (gCurrLevelArea) {
        case AREA_RR:
            scaleToMario = 0.f;
            heightOffset = 0.f;
            break;

        case AREA_CASTLE_LOBBY:
            scaleToMario = 0.3f;
            heightOffset = 0.f;
            break;

        case AREA_BBH:
            scaleToMario = 0.f;
            heightOffset = 0.f;
            break;
    }

    handle_c_button_movement(c);
    play_camera_buzz_if_cdown();

    calc_y_to_curr_floor(&focusFloorOff, 1.f, 200.f, &focusFloorOff, 0.9f, 200.f);
    vec3f_copy(focus, sMarioCamState->pos);
    focus[1] += focusFloorOff + 125.f;
    vec3f_get_dist_and_angle(focus, c->pos, &distCamToFocus, &faceAngle[0], &faceAngle[1]);
    faceAngle[2] = 0;

    vec3f_copy(basePos, sFixedModeBasePosition);
    vec3f_add(basePos, sCastleEntranceOffset);

    if (sMarioGeometry.currFloorType != SURFACE_DEATH_PLANE
        && sMarioGeometry.currFloorHeight != FLOOR_LOWER_LIMIT) {
        goalHeight = sMarioGeometry.currFloorHeight + basePos[1] + heightOffset;
    } else {
        goalHeight = gLakituState.goalPos[1];
    }

    if (300 > distCamToFocus) {
        goalHeight += 300 - distCamToFocus;
    }

    ceilHeight = find_ceil(c->pos[0], goalHeight - 100.f, c->pos[2], &ceiling);
    if (ceilHeight != CELL_HEIGHT_LIMIT) {
        if (goalHeight > (ceilHeight -= 125.f)) {
            goalHeight = ceilHeight;
        }
    }

    if (sStatusFlags & CAM_FLAG_SMOOTH_MOVEMENT) {
        camera_approach_f32_symmetric_bool(&c->pos[1], goalHeight, 15.f);
    } else {
        if (goalHeight < sMarioCamState->pos[1] - 500.f) {
            goalHeight = sMarioCamState->pos[1] - 500.f;
        }
        c->pos[1] = goalHeight;
    }

    c->pos[0] = basePos[0] + (sMarioCamState->pos[0] - basePos[0]) * scaleToMario;
    c->pos[2] = basePos[2] + (sMarioCamState->pos[2] - basePos[2]) * scaleToMario;

    if (scaleToMario != 0.f) {
        vec3f_get_dist_and_angle(c->focus, c->pos, &distCamToFocus, &pitch, &yaw);
        if (distCamToFocus > 1000.f) {
            distCamToFocus = 1000.f;
            vec3f_set_dist_and_angle(c->focus, c->pos, distCamToFocus, pitch, yaw);
        }
    }

    return faceAngle[1];
}

/**
 * Fixed camera mode, the camera rotates around a point and looks and zooms toward Mario.
 */
void mode_fixed_camera(struct Camera *c) {
#ifdef ENABLE_VANILLA_LEVEL_SPECIFIC_CHECKS
    if (gCurrLevelNum == LEVEL_BBH) {
        set_fov_function(CAM_FOV_BBH);
    } else {
        set_fov_function(CAM_FOV_APP_45);
    }
#else
    set_fov_function(CAM_FOV_APP_45);
#endif
    c->nextYaw = update_fixed_camera(c, c->focus, c->pos);
    c->yaw = c->nextYaw;
    pan_ahead_of_player(c);
    vec3_zero(sCastleEntranceOffset);
}
