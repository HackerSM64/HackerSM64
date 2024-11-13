#include "types.h"
#include "camera_math.h"
#include "camera_modes.h"
#include "engine/math_util.h"
#include "engine/surface_collision.h"
#include "game/camera.h"
#include "game/game_init.h"

void focus_on_mario(Vec3f focus, Vec3f pos, f32 posYOff, f32 focYOff, f32 dist, s16 pitch, s16 yaw) {
    Vec3f marioPos;

    marioPos[0] = sMarioCamState->pos[0];
    marioPos[1] = sMarioCamState->pos[1] + posYOff;
    marioPos[2] = sMarioCamState->pos[2];

    vec3f_set_dist_and_angle(marioPos, pos, dist, pitch + sLakituPitch, yaw);

    focus[0] = sMarioCamState->pos[0];
    focus[1] = sMarioCamState->pos[1] + focYOff;
    focus[2] = sMarioCamState->pos[2];
}

/**
 * Updates the camera based on which C buttons are pressed this frame
 */
void handle_c_button_movement(struct Camera *c) {
    s16 cSideYaw;

    // Zoom in
    if (gPlayer1Controller->buttonPressed & U_CBUTTONS) {
        if (c->mode != CAMERA_MODE_FIXED && (gCameraMovementFlags & CAM_MOVE_ZOOMED_OUT)) {
            gCameraMovementFlags &= ~CAM_MOVE_ZOOMED_OUT;
            play_sound_cbutton_up();
        } else {
            set_mode_c_up(c);
            if (sZeroZoomDist > gCameraZoomDist) {
                sZoomAmount = -gCameraZoomDist;
            } else {
                sZoomAmount = gCameraZoomDist;
            }
        }
    }
    if (c->mode != CAMERA_MODE_FIXED) {
        // Zoom out
        if (gPlayer1Controller->buttonPressed & D_CBUTTONS) {
            if (gCameraMovementFlags & CAM_MOVE_ZOOMED_OUT) {
                gCameraMovementFlags |= CAM_MOVE_ALREADY_ZOOMED_OUT;
                sZoomAmount = gCameraZoomDist + 400.f;
                play_camera_buzz_if_cdown();
            } else {
                gCameraMovementFlags |= CAM_MOVE_ZOOMED_OUT;
                sZoomAmount = gCameraZoomDist + 400.f;
                play_sound_cbutton_down();
            }
        }

        // Rotate left or right
        cSideYaw = 0x1000;
        if (gPlayer1Controller->buttonPressed & R_CBUTTONS) {
            if (gCameraMovementFlags & CAM_MOVE_ROTATE_LEFT) {
                gCameraMovementFlags &= ~CAM_MOVE_ROTATE_LEFT;
            } else {
                gCameraMovementFlags |= CAM_MOVE_ROTATE_RIGHT;
                if (sCSideButtonYaw == 0) {
                    play_sound_cbutton_side();
                }
                sCSideButtonYaw = -cSideYaw;
            }
        }
        if (gPlayer1Controller->buttonPressed & L_CBUTTONS) {
            if (gCameraMovementFlags & CAM_MOVE_ROTATE_RIGHT) {
                gCameraMovementFlags &= ~CAM_MOVE_ROTATE_RIGHT;
            } else {
                gCameraMovementFlags |= CAM_MOVE_ROTATE_LEFT;
                if (sCSideButtonYaw == 0) {
                    play_sound_cbutton_side();
                }
                sCSideButtonYaw = cSideYaw;
            }
        }
    }
}

/**
 * Checks for any walls obstructing Mario from view, and calculates a new yaw that the camera should
 * rotate towards.
 *
 * @param[out] avoidYaw the angle (from Mario) that the camera should rotate towards to avoid the wall.
 *                      The camera then approaches avoidYaw until Mario is no longer obstructed.
 *                      avoidYaw is always parallel to the wall.
 * @param yawRange      how wide of an arc to check for walls obscuring Mario.
 *
 * @return 3 if a wall is covering Mario, 1 if a wall is only near the camera.
 */
s32 rotate_camera_around_walls(UNUSED struct Camera *c, Vec3f cPos, s16 *avoidYaw, s16 yawRange) {
    struct WallCollisionData colData;
    struct Surface *wall;
    f32 dummyDist, checkDist;
    f32 coarseRadius;
    f32 fineRadius;
    s16 wallYaw, horWallNorm;
    s16 dummyPitch;
    // The yaw of the vector from Mario to the camera.
    s16 yawFromMario;
    s32 status = 0;
    /// The current iteration. The algorithm takes 8 equal steps from Mario back to the camera.
    s32 step = 0;

    vec3f_get_dist_and_angle(sMarioCamState->pos, cPos, &dummyDist, &dummyPitch, &yawFromMario);
    sStatusFlags &= ~CAM_FLAG_CAM_NEAR_WALL;
    colData.offsetY = 100.0f;
    // The distance from Mario to Lakitu
    checkDist = 0.0f;
    /// The radius used to find potential walls to avoid.
    /// @bug Increases to 250.f, but the max collision radius is 200.f
    coarseRadius = 150.0f;
    /// This only increases when there is a wall collision found in the coarse pass
    fineRadius = 100.0f;

    for (step = 0; step < 8; step++) {
        // Start at Mario, move backwards to Lakitu's position
        colData.x = sMarioCamState->pos[0] + ((cPos[0] - sMarioCamState->pos[0]) * checkDist);
        colData.y = sMarioCamState->pos[1] + ((cPos[1] - sMarioCamState->pos[1]) * checkDist);
        colData.z = sMarioCamState->pos[2] + ((cPos[2] - sMarioCamState->pos[2]) * checkDist);
        colData.radius = coarseRadius;
        // Increase the coarse check radius
        camera_approach_f32_symmetric_bool(&coarseRadius, 250.f, 30.f);

        if (find_wall_collisions(&colData) != 0) {
            wall = colData.walls[colData.numWalls - 1];

            // If we're over halfway from Mario to Lakitu, then there's a wall near the camera, but
            // not necessarily obstructing Mario
            if (step >= 5) {
                sStatusFlags |= CAM_FLAG_CAM_NEAR_WALL;
                if (status <= 0) {
                    status = 1;
                    wall = colData.walls[colData.numWalls - 1];
                    // wallYaw is parallel to the wall, not perpendicular
                    wallYaw = atan2s(wall->normal.z, wall->normal.x) + DEGREES(90);
                    // Calculate the avoid direction. The function returns the opposite direction so add 180
                    // degrees.
                    *avoidYaw = calc_avoid_yaw(yawFromMario, wallYaw) + DEGREES(180);
                }
            }

            colData.x = sMarioCamState->pos[0] + ((cPos[0] - sMarioCamState->pos[0]) * checkDist);
            colData.y = sMarioCamState->pos[1] + ((cPos[1] - sMarioCamState->pos[1]) * checkDist);
            colData.z = sMarioCamState->pos[2] + ((cPos[2] - sMarioCamState->pos[2]) * checkDist);
            colData.radius = fineRadius;
            // Increase the fine check radius
            camera_approach_f32_symmetric_bool(&fineRadius, 200.f, 20.f);

            if (find_wall_collisions(&colData) != 0) {
                wall = colData.walls[colData.numWalls - 1];
                horWallNorm = atan2s(wall->normal.z, wall->normal.x);
                wallYaw = horWallNorm + DEGREES(90);
                // If Mario would be blocked by the surface, then avoid it
                if ((is_range_behind_surface(sMarioCamState->pos, cPos, wall, yawRange, SURFACE_WALL_MISC) == 0)
                    && (is_mario_behind_surface(c, wall) == TRUE)
                    // Also check if the wall is tall enough to cover Mario
                    && (is_surf_within_bounding_box(wall, -1.f, 150.f, -1.f) == FALSE)) {
                    // Calculate the avoid direction. The function returns the opposite direction so add 180
                    // degrees.
                    *avoidYaw = calc_avoid_yaw(yawFromMario, wallYaw) + DEGREES(180);
                    camera_approach_s16_symmetric_bool(avoidYaw, horWallNorm, yawRange);
                    status = 3;
                    step = 8;
                }
            }
        }
        checkDist += 0.125f;
    }

    return status;
}

/**
 * Set the camera's y coordinate to goalHeight, respecting floors and ceilings in the way
 */
void set_camera_height(struct Camera *c, f32 goalHeight) {
    struct Surface *surface;
    f32 marioFloorHeight, marioCeilHeight, camFloorHeight;
    f32 baseOff = 125.f;
    f32 camCeilHeight = find_ceil(c->pos[0], gLakituState.goalPos[1] - 50.f, c->pos[2], &surface);
#ifdef FAST_VERTICAL_CAMERA_MOVEMENT
    f32 approachRate = 20.0f;
#endif

    if (sMarioCamState->action & ACT_FLAG_HANGING) {
        marioCeilHeight = sMarioGeometry.currCeilHeight;
        marioFloorHeight = sMarioGeometry.currFloorHeight;

        if (marioFloorHeight < marioCeilHeight - 400.f) {
            marioFloorHeight = marioCeilHeight - 400.f;
        }

        goalHeight = marioFloorHeight + (marioCeilHeight - marioFloorHeight) * 0.4f;

        if (sMarioCamState->pos[1] - 400 > goalHeight) {
            goalHeight = sMarioCamState->pos[1] - 400;
        }

        approach_camera_height(c, goalHeight, 5.f);
    } else {
        camFloorHeight = find_floor(c->pos[0], c->pos[1] + 100.f, c->pos[2], &surface) + baseOff;
        marioFloorHeight = baseOff + sMarioGeometry.currFloorHeight;

        if (camFloorHeight < marioFloorHeight) {
            camFloorHeight = marioFloorHeight;
        }
        if (goalHeight < camFloorHeight) {
            goalHeight = camFloorHeight;
            c->pos[1] = goalHeight;
        }
        // Warp camera to goalHeight if further than 1000 and Mario is stuck in the ground
        if (sMarioCamState->action == ACT_BUTT_STUCK_IN_GROUND ||
            sMarioCamState->action == ACT_HEAD_STUCK_IN_GROUND ||
            sMarioCamState->action == ACT_FEET_STUCK_IN_GROUND) {
            if (absf(c->pos[1] - goalHeight) > 1000.0f) {
                c->pos[1] = goalHeight;
            }
        }

#ifdef FAST_VERTICAL_CAMERA_MOVEMENT
        approachRate += ABS(c->pos[1] - goalHeight) / 20;
        approach_camera_height(c, goalHeight, approachRate);
#else
        approach_camera_height(c, goalHeight, 20.f);
#endif

        if (camCeilHeight != CELL_HEIGHT_LIMIT) {
            camCeilHeight -= baseOff;
            if ((c->pos[1] > camCeilHeight && sMarioGeometry.currFloorHeight + baseOff < camCeilHeight)
                || (sMarioGeometry.currCeilHeight != CELL_HEIGHT_LIMIT
                    && sMarioGeometry.currCeilHeight > camCeilHeight && c->pos[1] > camCeilHeight)) {
                c->pos[1] = camCeilHeight;
            }
        }
    }
}

/**
 * Pitch the camera down when the camera is facing down a slope
 */
s16 look_down_slopes(s16 camYaw) {
    struct Surface *floor;
    // Default pitch
    s16 pitch = 0x05B0;
    // x and z offsets towards the camera
    f32 xOff = sMarioCamState->pos[0] + sins(camYaw) * 40.f;
    f32 zOff = sMarioCamState->pos[2] + coss(camYaw) * 40.f;

    f32 floorDY = find_floor(xOff, sMarioCamState->pos[1], zOff, &floor) - sMarioCamState->pos[1];

    if (floor != NULL) {
        if (floor->type != SURFACE_WALL_MISC && floorDY > 0) {
            if (floor->normal.z == 0.f && floorDY < 100.f) {
                pitch = 0x05B0;
            } else {
                // Add the slope's angle of declination to the pitch
                pitch += atan2s(40.f, floorDY);
            }
        }
    }

    return pitch;
}

/**
 * Look ahead to the left or right in the direction the player is facing
 * The calculation for pan[0] could be simplified to:
 *      yaw = -yaw;
 *      pan[0] = sins(sMarioCamState->faceAngle[1] + yaw) * sins(0xC00) * dist;
 * Perhaps, early in development, the pan used to be calculated for both the x and z directions
 *
 * Since this function only affects the camera's focus, Mario's movement direction isn't affected.
 */
void pan_ahead_of_player(struct Camera *c) {
    f32 dist;
    s16 pitch, yaw;
    Vec3f pan = { 0, 0, 0 };

    // Get distance and angle from camera to Mario.
    vec3f_get_dist_and_angle(c->pos, sMarioCamState->pos, &dist, &pitch, &yaw);

    // The camera will pan ahead up to about 30% of the camera's distance to Mario.
    pan[2] = sins(0xC00) * dist;

    rotate_in_xz(pan, pan, sMarioCamState->faceAngle[1]);
    // rotate in the opposite direction
    yaw = -yaw;
    rotate_in_xz(pan, pan, yaw);
    // Only pan left or right
    pan[2] = 0.f;

    // If Mario is long jumping, or on a flag pole (but not at the top), then pan in the opposite direction
    if (sMarioCamState->action == ACT_LONG_JUMP ||
       (sMarioCamState->action != ACT_TOP_OF_POLE && (sMarioCamState->action & ACT_FLAG_ON_POLE))) {
        pan[0] = -pan[0];
    }

    // Slowly make the actual pan, sPanDistance, approach the calculated pan
    // If Mario is sleeping, then don't pan
    if (sStatusFlags & CAM_FLAG_SLEEPING) {
        approach_f32_asymptotic_bool(&sPanDistance, 0.f, 0.025f);
    } else {
        approach_f32_asymptotic_bool(&sPanDistance, pan[0], 0.025f);
    }

    // Now apply the pan. It's a dir vector to the left or right, rotated by the camera's yaw to Mario
    pan[0] = sPanDistance;
    yaw = -yaw;
    rotate_in_xz(pan, pan, yaw);
    vec3f_add(c->focus, pan);
}
