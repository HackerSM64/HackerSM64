#include "camera/camera_math.h"
#include "camera/camera_modes.h"
#include "engine/math_util.h"
#include "game/game_init.h"
#include "game/level_update.h"
#include "game/camera.h"


/**
 * Handles input for radial, outwards radial, parallel tracking, and 8 direction mode.
 */
void radial_camera_input(struct Camera *c) {
    if ((gCameraMovementFlags & CAM_MOVE_ENTERED_ROTATE_SURFACE) || !(gCameraMovementFlags & CAM_MOVE_ROTATE)) {

        // If C-L or C-R are pressed, the camera is rotating
        if (gPlayer1Controller->buttonPressed & (L_CBUTTONS | R_CBUTTONS)) {
            gCameraMovementFlags &= ~CAM_MOVE_ENTERED_ROTATE_SURFACE;
            //  @bug this does not clear the rotation flags set by the surface. It's possible to set
            //       both ROTATE_LEFT and ROTATE_RIGHT, locking the camera.
            //       Ex: If a surface set CAM_MOVE_ROTATE_RIGHT and the user presses C-R, it locks the
            //       camera until a different mode is activated
        }

        // Rotate Right and left
        if (gPlayer1Controller->buttonPressed & R_CBUTTONS) {
            if (sModeOffsetYaw > -0x800) {
                // The camera is now rotating right
                if (!(gCameraMovementFlags & CAM_MOVE_ROTATE_RIGHT)) {
                    gCameraMovementFlags |= CAM_MOVE_ROTATE_RIGHT;
                }

                if (c->mode == CAMERA_MODE_RADIAL) {
                    // if > ~48 degrees, we're rotating for the second time.
                    if (sModeOffsetYaw > 0x22AA) {
                        s2ndRotateFlags |= CAM_MOVE_ROTATE_RIGHT;
                    }

                    if (sModeOffsetYaw == DEGREES(105)) {
                        play_sound_button_change_blocked();
                    } else {
                        play_sound_cbutton_side();
                    }
                } else {
                    if (sModeOffsetYaw == DEGREES(60)) {
                        play_sound_button_change_blocked();
                    } else {
                        play_sound_cbutton_side();
                    }
                }
            } else {
                gCameraMovementFlags |= CAM_MOVE_RETURN_TO_MIDDLE;
                play_sound_cbutton_up();
            }
        }
        if (gPlayer1Controller->buttonPressed & L_CBUTTONS) {
            if (sModeOffsetYaw < 0x800) {
                if (!(gCameraMovementFlags & CAM_MOVE_ROTATE_LEFT)) {
                    gCameraMovementFlags |= CAM_MOVE_ROTATE_LEFT;
                }

                if (c->mode == CAMERA_MODE_RADIAL) {
                    // if < ~48 degrees, we're rotating for the second time.
                    if (sModeOffsetYaw < -0x22AA) {
                        s2ndRotateFlags |= CAM_MOVE_ROTATE_LEFT;
                    }

                    if (sModeOffsetYaw == DEGREES(-105)) {
                        play_sound_button_change_blocked();
                    } else {
                        play_sound_cbutton_side();
                    }
                } else {
                    if (sModeOffsetYaw == DEGREES(-60)) {
                        play_sound_button_change_blocked();
                    } else {
                        play_sound_cbutton_side();
                    }
                }
            } else {
                gCameraMovementFlags |= CAM_MOVE_RETURN_TO_MIDDLE;
                play_sound_cbutton_up();
            }
        }
    }

    // Zoom in / enter C-Up
    if (gPlayer1Controller->buttonPressed & U_CBUTTONS) {
        if (gCameraMovementFlags & CAM_MOVE_ZOOMED_OUT) {
            gCameraMovementFlags &= ~CAM_MOVE_ZOOMED_OUT;
            play_sound_cbutton_up();
        } else {
            set_mode_c_up(c);
        }
    }

    // Zoom out
    if (gPlayer1Controller->buttonPressed & D_CBUTTONS) {
        if (gCameraMovementFlags & CAM_MOVE_ZOOMED_OUT) {
            gCameraMovementFlags |= CAM_MOVE_ALREADY_ZOOMED_OUT;
            play_camera_buzz_if_cdown();
        } else {
            gCameraMovementFlags |= CAM_MOVE_ZOOMED_OUT;
            play_sound_cbutton_down();
        }
    }
}

/**
 * Rotates the camera around the area's center point.
 */
s32 update_radial_camera(struct Camera *c, Vec3f focus, Vec3f pos) {
    f32 cenDistX = sMarioCamState->pos[0] - c->areaCenX;
    f32 cenDistZ = sMarioCamState->pos[2] - c->areaCenZ;
    s16 camYaw = atan2s(cenDistZ, cenDistX) + sModeOffsetYaw;
    s16 pitch = look_down_slopes(camYaw);
    f32 posY, focusY;
    f32 yOff = 125.f;
    f32 baseDist = 1000.f;

    sAreaYaw = camYaw - sModeOffsetYaw;
    calc_y_to_curr_floor(&posY, 1.f, 200.f, &focusY, 0.9f, 200.f);
    focus_on_mario(focus, pos, posY + yOff, focusY + yOff, sLakituDist + baseDist, pitch, camYaw);
#ifdef ENABLE_VANILLA_LEVEL_SPECIFIC_CHECKS
    camYaw = find_in_bounds_yaw_wdw_bob_thi(pos, focus, camYaw);
#endif
    return camYaw;
}

/**
 * Moves the camera for the radial and outward radial camera modes.
 *
 * If sModeOffsetYaw is 0, the camera points directly at the area center point.
 */
void radial_camera_move(struct Camera *c) {
    s16 maxAreaYaw = DEGREES(60);
    s16 minAreaYaw = DEGREES(-60);
    s16 rotateSpeed = 0x1000;
    s16 avoidYaw;
    f32 areaDistX = sMarioCamState->pos[0] - c->areaCenX;
    f32 areaDistZ = sMarioCamState->pos[2] - c->areaCenZ;

    // How much the camera's yaw changed
    s16 yawOffset = calculate_yaw(sMarioCamState->pos, c->pos) - atan2s(areaDistZ, areaDistX);

    if (yawOffset > maxAreaYaw) {
        yawOffset = maxAreaYaw;
    }
    if (yawOffset < minAreaYaw) {
        yawOffset = minAreaYaw;
    }

    // Check if Mario stepped on a surface that rotates the camera. For example, when Mario enters the
    // gate in BoB, the camera turns right to face up the hill path
    if (!(gCameraMovementFlags & CAM_MOVE_ROTATE)) {
        if (sMarioGeometry.currFloorType == SURFACE_CAMERA_MIDDLE
            && sMarioGeometry.prevFloorType != SURFACE_CAMERA_MIDDLE) {
            gCameraMovementFlags |= (CAM_MOVE_RETURN_TO_MIDDLE | CAM_MOVE_ENTERED_ROTATE_SURFACE);
        }
        if (sMarioGeometry.currFloorType == SURFACE_CAMERA_ROTATE_RIGHT
            && sMarioGeometry.prevFloorType != SURFACE_CAMERA_ROTATE_RIGHT) {
            gCameraMovementFlags |= (CAM_MOVE_ROTATE_RIGHT | CAM_MOVE_ENTERED_ROTATE_SURFACE);
        }
        if (sMarioGeometry.currFloorType == SURFACE_CAMERA_ROTATE_LEFT
            && sMarioGeometry.prevFloorType != SURFACE_CAMERA_ROTATE_LEFT) {
            gCameraMovementFlags |= (CAM_MOVE_ROTATE_LEFT | CAM_MOVE_ENTERED_ROTATE_SURFACE);
        }
    }

    if (gCameraMovementFlags & CAM_MOVE_ENTERED_ROTATE_SURFACE) {
        rotateSpeed = 0x200;
    }

    if (c->mode == CAMERA_MODE_OUTWARD_RADIAL) {
        areaDistX = -areaDistX;
        areaDistZ = -areaDistZ;
    }

    // Avoid obstructing walls
    s32 avoidStatus = rotate_camera_around_walls(c, c->pos, &avoidYaw, 0x400);
    if (avoidStatus == AVOID_STATUS_WALL_COVERING_MARIO) {
        if (avoidYaw - atan2s(areaDistZ, areaDistX) + DEGREES(90) < 0) {
            avoidYaw += DEGREES(180);
        }

        // We want to change sModeOffsetYaw so that the player is no longer obstructed by the wall.
        // So, we make avoidYaw relative to the yaw around the area center
        avoidYaw -= atan2s(areaDistZ, areaDistX);

        // Bound avoid yaw to radial mode constraints
        if (avoidYaw > DEGREES(105)) {
            avoidYaw = DEGREES(105);
        }
        if (avoidYaw < DEGREES(-105)) {
            avoidYaw = DEGREES(-105);
        }
    }

    if (gCameraMovementFlags & CAM_MOVE_RETURN_TO_MIDDLE) {
        if (camera_approach_s16_symmetric_bool(&sModeOffsetYaw, 0, rotateSpeed) == 0) {
            gCameraMovementFlags &= ~CAM_MOVE_RETURN_TO_MIDDLE;
        }
    } else {
        // Prevent the player from rotating into obstructing walls
        if ((gCameraMovementFlags & CAM_MOVE_ROTATE_RIGHT) && avoidStatus == 3
            && avoidYaw + 0x10 < sModeOffsetYaw) {
            sModeOffsetYaw = avoidYaw;
            gCameraMovementFlags &= ~(CAM_MOVE_ROTATE_RIGHT | CAM_MOVE_ENTERED_ROTATE_SURFACE);
        }
        if ((gCameraMovementFlags & CAM_MOVE_ROTATE_LEFT) && avoidStatus == 3
            && avoidYaw - 0x10 > sModeOffsetYaw) {
            sModeOffsetYaw = avoidYaw;
            gCameraMovementFlags &= ~(CAM_MOVE_ROTATE_LEFT | CAM_MOVE_ENTERED_ROTATE_SURFACE);
        }

        // If it's the first time rotating, just rotate to +-60 degrees
        if (!(s2ndRotateFlags & CAM_MOVE_ROTATE_RIGHT) && (gCameraMovementFlags & CAM_MOVE_ROTATE_RIGHT)
            && camera_approach_s16_symmetric_bool(&sModeOffsetYaw, maxAreaYaw, rotateSpeed) == 0) {
            gCameraMovementFlags &= ~(CAM_MOVE_ROTATE_RIGHT | CAM_MOVE_ENTERED_ROTATE_SURFACE);
        }
        if (!(s2ndRotateFlags & CAM_MOVE_ROTATE_LEFT) && (gCameraMovementFlags & CAM_MOVE_ROTATE_LEFT)
            && camera_approach_s16_symmetric_bool(&sModeOffsetYaw, minAreaYaw, rotateSpeed) == 0) {
            gCameraMovementFlags &= ~(CAM_MOVE_ROTATE_LEFT | CAM_MOVE_ENTERED_ROTATE_SURFACE);
        }

        // If it's the second time rotating, rotate all the way to +-105 degrees.
        if ((s2ndRotateFlags & CAM_MOVE_ROTATE_RIGHT) && (gCameraMovementFlags & CAM_MOVE_ROTATE_RIGHT)
            && camera_approach_s16_symmetric_bool(&sModeOffsetYaw, DEGREES(105), rotateSpeed) == 0) {
            gCameraMovementFlags &= ~(CAM_MOVE_ROTATE_RIGHT | CAM_MOVE_ENTERED_ROTATE_SURFACE);
            s2ndRotateFlags &= ~CAM_MOVE_ROTATE_RIGHT;
        }
        if ((s2ndRotateFlags & CAM_MOVE_ROTATE_LEFT) && (gCameraMovementFlags & CAM_MOVE_ROTATE_LEFT)
            && camera_approach_s16_symmetric_bool(&sModeOffsetYaw, DEGREES(-105), rotateSpeed) == 0) {
            gCameraMovementFlags &= ~(CAM_MOVE_ROTATE_LEFT | CAM_MOVE_ENTERED_ROTATE_SURFACE);
            s2ndRotateFlags &= ~CAM_MOVE_ROTATE_LEFT;
        }
    }
    if (!(gCameraMovementFlags & CAM_MOVE_ROTATE)) {
        // If not rotating, rotate away from walls obscuring Mario from view
        if (avoidStatus == 3) {
            approach_s16_asymptotic_bool(&sModeOffsetYaw, avoidYaw, 10);
        } else {
            if (c->mode == CAMERA_MODE_RADIAL) {
                // sModeOffsetYaw only updates when Mario is moving
                rotateSpeed = gMarioStates[0].forwardVel / 32.f * 128.f;
                camera_approach_s16_symmetric_bool(&sModeOffsetYaw, yawOffset, rotateSpeed);
            }
            if (c->mode == CAMERA_MODE_OUTWARD_RADIAL) {
                sModeOffsetYaw = offset_yaw_outward_radial(c, atan2s(areaDistZ, areaDistX));
            }
        }
    }

    // Bound sModeOffsetYaw within (-120, 120) degrees
    if (sModeOffsetYaw > 0x5554) {
        sModeOffsetYaw = 0x5554;
    }
    if (sModeOffsetYaw < -0x5554) {
        sModeOffsetYaw = -0x5554;
    }
}

void radial_camera_input_default(struct Camera *c) {
    radial_camera_input(c);
}

/**
 * Makes Lakitu cam's yaw match the angle turned towards in C-Up mode, and makes Lakitu slowly fly back
 * to the distance he was at before C-Up
 */
void update_yaw_and_dist_from_c_up(UNUSED struct Camera *c) {
    sModeOffsetYaw = sModeInfo.transitionStart.yaw - sAreaYaw;
    sLakituDist = sModeInfo.transitionStart.dist - 1000.0f;
    // No longer in C-Up
    gCameraMovementFlags &= ~CAM_MOVING_INTO_MODE;
}

/**
 * Handles input and updates for the radial camera mode
 */
void mode_radial_camera(struct Camera *c) {
    Vec3f pos;
    s16 oldAreaYaw = sAreaYaw;

    if (gCameraMovementFlags & CAM_MOVING_INTO_MODE) {
        update_yaw_and_dist_from_c_up(c);
    }

    radial_camera_input_default(c);
    radial_camera_move(c);

    if (c->mode == CAMERA_MODE_RADIAL) {
        lakitu_zoom(400.f, 0x900);
    }
    c->nextYaw = update_radial_camera(c, c->focus, pos);
    c->pos[0] = pos[0];
    c->pos[2] = pos[2];
    sAreaYawChange = sAreaYaw - oldAreaYaw;
    if (sMarioCamState->action == ACT_RIDING_HOOT) {
        pos[1] += 500.f;
    }
    set_camera_height(c, pos[1]);
    pan_ahead_of_player(c);
}
