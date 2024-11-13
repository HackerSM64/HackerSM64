#include "camera/camera_math.h"
#include "camera/camera_modes.h"
#include "engine/math_util.h"
#include "engine/surface_collision.h"
#include "game/game_init.h"
#include "game/level_update.h"
#include "game/camera.h"

/**
 * Zoom the camera out of C-Up mode, avoiding moving into a wall, if possible, by searching for an open
 * direction.
 */
void exit_c_up(struct Camera *c) {
    struct Surface *surface;
    Vec3f checkFoc;
    Vec3f curPos;
    // Variables for searching for an open direction
    s32 searching = FALSE;
    /// The current sector of the circle that we are checking
    s32 sector;
    f32 ceilHeight;
    f32 floorHeight;
    f32 curDist;
    f32 d;
    s16 curPitch;
    s16 curYaw;
    s16 checkYaw = 0;

    if ((gCameraMovementFlags & CAM_MOVE_C_UP_MODE) && !(gCameraMovementFlags & CAM_MOVE_STARTED_EXITING_C_UP)) {
        vec3f_copy(checkFoc, c->focus);
        checkFoc[0] = sMarioCamState->pos[0];
        checkFoc[2] = sMarioCamState->pos[2];
        vec3f_get_dist_and_angle(checkFoc, c->pos, &curDist, &curPitch, &curYaw);
        vec3f_copy(curPos, c->pos);
        curDist = 80.f;

        // Search for an open direction to zoom out in, if the camera is changing to close, free roam,
        // or spiral-stairs mode
        if (sModeInfo.lastMode == CAMERA_MODE_SPIRAL_STAIRS || sModeInfo.lastMode == CAMERA_MODE_CLOSE
            || sModeInfo.lastMode == CAMERA_MODE_FREE_ROAM) {
            searching = TRUE;
            // Check the whole circle around Mario for an open direction to zoom out to
            for (sector = 0; sector < 16 && searching == 1; sector++) {
                vec3f_set_dist_and_angle(checkFoc, curPos, curDist, 0, curYaw + checkYaw);

                // If there are no walls this way,
                if (f32_find_wall_collision(&curPos[0], &curPos[1], &curPos[2], 20.f, 50.f) == 0) {

                    // Start close to Mario, check for walls, floors, and ceilings all the way to the
                    // zoomed out distance
                    for (d = curDist; d < gCameraZoomDist; d += 20.f) {
                        vec3f_set_dist_and_angle(checkFoc, curPos, d, 0, curYaw + checkYaw);

                        // Check if we're zooming out into a floor or ceiling
                        ceilHeight = find_ceil(curPos[0], curPos[1] - 150.f, curPos[2], &surface) + -10.f;
                        if (surface != NULL && ceilHeight < curPos[1]) {
                            break;
                        }
                        floorHeight = find_floor(curPos[0], curPos[1] + 150.f, curPos[2], &surface) + 10.f;
                        if (surface != NULL && floorHeight > curPos[1]) {
                            break;
                        }

                        // Stop checking this direction if there is a wall blocking the way
                        if (f32_find_wall_collision(&curPos[0], &curPos[1], &curPos[2], 20.f, 50.f) == 1) {
                            break;
                        }
                    }

                    // If there was no collision found all the way to the max distance, it's an opening
                    if (d >= gCameraZoomDist) {
                        searching = FALSE;
                    }
                }

                // Alternate left and right, checking each 1/16th (22.5 degrees) of the circle
                if (searching == 1) {
                    checkYaw = -checkYaw;
                    if (checkYaw < 0) {
                        checkYaw -= 0x1000;
                    } else {
                        checkYaw += 0x1000;
                    }
                }
            }

            // Update the stored focus and pos to the direction found in the search
            if (!searching) {
                vec3f_set_dist_and_angle(checkFoc, sCameraStoreCUp.pos, gCameraZoomDist, 0, curYaw + checkYaw);
                vec3f_copy(sCameraStoreCUp.focus, checkFoc);
                vec3f_sub(sCameraStoreCUp.pos, sMarioCamState->pos);
                vec3f_sub(sCameraStoreCUp.focus, sMarioCamState->pos);
            }

            gCameraMovementFlags |= CAM_MOVE_STARTED_EXITING_C_UP;
            transition_next_state(c, 15);
        } else {
            // Let the next camera mode handle it
            gCameraMovementFlags &= ~(CAM_MOVE_STARTED_EXITING_C_UP | CAM_MOVE_C_UP_MODE);
            vec3f_set_dist_and_angle(checkFoc, c->pos, curDist, curPitch, curYaw + checkYaw);
        }
        play_sound_cbutton_down();
    }
}

/**
 * The mode used when C-Up is pressed.
 */
s32 update_c_up(UNUSED struct Camera *c, Vec3f focus, Vec3f pos) {
    s16 pitch = sCUpCameraPitch;
    s16 yaw = sMarioCamState->faceAngle[1] + sModeOffsetYaw + DEGREES(180);

    focus_on_mario(focus, pos, 125.f, 125.f, 250.f, pitch, yaw);
    return sMarioCamState->faceAngle[1];
}

/**
 * Update the camera during 8 directional mode
 */
s32 update_8_directions_camera(struct Camera *c, Vec3f focus, Vec3f pos) {
    s16 camYaw = s8DirModeBaseYaw + s8DirModeYawOffset;
    s16 pitch = look_down_slopes(camYaw);
    f32 posY;
    f32 focusY;
    f32 yOff = 125.f;
    f32 baseDist = 1000.f;

    sAreaYaw = camYaw;
    calc_y_to_curr_floor(&posY, 1.f, 200.f, &focusY, 0.9f, 200.f);
    focus_on_mario(focus, pos, posY + yOff, focusY + yOff, sLakituDist + baseDist, pitch, camYaw);
    pan_ahead_of_player(c);
#ifdef ENABLE_VANILLA_LEVEL_SPECIFIC_CHECKS
    if (gCurrLevelArea == AREA_DDD_SUB) {
        camYaw = clamp_positions_and_find_yaw(pos, focus, 6839.f, 995.f, 5994.f, -3945.f);
    }
#endif
    return camYaw;
}


/**
 * A mode that only has 8 camera angles, 45 degrees apart
 */
void mode_8_directions_camera(struct Camera *c) {
    Vec3f pos;
    s16 oldAreaYaw = sAreaYaw;

    radial_camera_input(c);

    if (gPlayer1Controller->buttonPressed & R_CBUTTONS) {
        s8DirModeYawOffset += DEGREES(45);
        play_sound_cbutton_side();
    }
    if (gPlayer1Controller->buttonPressed & L_CBUTTONS) {
        s8DirModeYawOffset -= DEGREES(45);
        play_sound_cbutton_side();
    }
#ifdef PARALLEL_LAKITU_CAM
    // extra functionality
    else if (gPlayer1Controller->buttonPressed & U_JPAD) {
        s8DirModeYawOffset = 0;
        s8DirModeYawOffset = gMarioState->faceAngle[1] - 0x8000;
    }
    else if (gPlayer1Controller->buttonDown & L_JPAD) {
        s8DirModeYawOffset -= DEGREES(2);
    }
    else if (gPlayer1Controller->buttonDown & R_JPAD) {
        s8DirModeYawOffset += DEGREES(2);
    }
    else if (gPlayer1Controller->buttonPressed & D_JPAD) {
        s8DirModeYawOffset = snap_to_45_degrees(s8DirModeYawOffset);
    }
#endif

    lakitu_zoom(400.f, 0x900);
    c->nextYaw = update_8_directions_camera(c, c->focus, pos);
    c->pos[0] = pos[0];
    c->pos[2] = pos[2];
    sAreaYawChange = sAreaYaw - oldAreaYaw;
    set_camera_height(c, pos[1]);
}
