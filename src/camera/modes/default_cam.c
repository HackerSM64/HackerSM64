#include "camera/camera_math.h"
#include "camera/camera_modes.h"
#include "engine/math_util.h"
#include "engine/surface_collision.h"
#include "game/game_init.h"
#include "game/level_update.h"
#include "game/camera.h"

/**
 * Check `pos` for collisions within `radius`, and update `pos`
 *
 * @return the number of collisions found
 */
s32 collide_with_walls(Vec3f pos, f32 offsetY, f32 radius) {
    struct WallCollisionData collisionData;
    struct Surface *wall = NULL;
    f32 normX, normY, normZ;
    f32 originOffset;
    f32 offset;
    f32 offsetAbsolute;
    Vec3f newPos[MAX_REFERENCED_WALLS];
    s32 i;
    s32 numCollisions = 0;

    collisionData.x = pos[0];
    collisionData.y = pos[1];
    collisionData.z = pos[2];
    collisionData.radius = radius;
    collisionData.offsetY = offsetY;
    numCollisions = find_wall_collisions(&collisionData);
    if (numCollisions != 0) {
        for (i = 0; i < collisionData.numWalls; i++) {
            wall = collisionData.walls[collisionData.numWalls - 1];
            vec3f_copy(newPos[i], pos);
            normX = wall->normal.x;
            normY = wall->normal.y;
            normZ = wall->normal.z;
            originOffset = wall->originOffset;
            offset = normX * newPos[i][0] + normY * newPos[i][1] + normZ * newPos[i][2] + originOffset;
            offsetAbsolute = absf(offset);
            if (offsetAbsolute < radius) {
                newPos[i][0] += normX * (radius - offset);
                newPos[i][2] += normZ * (radius - offset);
                vec3f_copy(pos, newPos[i]);
            }
        }
    }
    return numCollisions;
}

/**
 * Update the camera in default, close, and free roam mode
 *
 * The camera moves behind Mario, and can rotate all the way around
 */
s16 update_default_camera(struct Camera *c) {
    Vec3f tempPos;
    Vec3f cPos;
    struct Surface *marioFloor;
    struct Surface *cFloor;
    struct Surface *tempFloor;
    struct Surface *ceil;
    f32 camFloorHeight;
    f32 tempFloorHeight;
    f32 marioFloorHeight;
    f32 dist;
    f32 zoomDist;
    f32 waterHeight;
    f32 gasHeight;
    s16 avoidYaw;
    s16 pitch;
    s16 yaw;
    s16 yawGoal = sMarioCamState->faceAngle[1] + DEGREES(180);
    f32 posHeight;
    f32 focHeight;
    f32 distFromWater;
    s16 tempPitch;
    s16 tempYaw;
    f32 xzDist;
    s16 nextYawVel;
    s16 yawVel = 0;
    f32 scale;
    s32 avoidStatus = 0;
    s32 closeToMario = FALSE;
    f32 ceilHeight = find_ceil(gLakituState.goalPos[0],
                               gLakituState.goalPos[1],
                               gLakituState.goalPos[2], &ceil);
    s16 yawDir;

    handle_c_button_movement(c);
    vec3f_get_dist_and_angle(sMarioCamState->pos, c->pos, &dist, &pitch, &yaw);

    // If C-Down is active, determine what distance the camera should be from Mario
    if (gCameraMovementFlags & CAM_MOVE_ZOOMED_OUT) {
        //! In Mario mode, the camera is zoomed out further than in Lakitu mode (1400 vs 1200)
        if (set_cam_angle(0) == CAM_ANGLE_MARIO) {
            zoomDist = gCameraZoomDist + 1050;
        } else {
            zoomDist = gCameraZoomDist + 400;
        }
    } else {
        zoomDist = gCameraZoomDist;
    }

    if (sMarioCamState->action & ACT_FLAG_HANGING ||
        sMarioCamState->action == ACT_RIDING_HOOT) {
        zoomDist *= 0.8f;
        set_handheld_shake(HAND_CAM_SHAKE_HANG_OWL);
    }

    // If not zooming out, only allow dist to decrease
    if (sZoomAmount == 0.f) {
        if (dist > zoomDist) {
            if ((dist -= 50.f) < zoomDist) {
                dist = zoomDist;
            }
        }
    } else {
        if ((sZoomAmount -= 30.f) < 0.f) {
            sZoomAmount = 0.f;
        }
        if (dist > zoomDist) {
            if ((dist -= 30.f) < zoomDist) {
                dist = zoomDist;
            }
        }
        if (dist < zoomDist) {
            if ((dist += 30.f) > zoomDist) {
                dist = zoomDist;
            }
        }
    }

    // Determine how fast to rotate the camera
    if (sCSideButtonYaw == 0) {
        if (c->mode == CAMERA_MODE_FREE_ROAM) {
            nextYawVel = 0xC0;
        } else {
            nextYawVel = 0x100;
        }
        if ((gPlayer1Controller->stickX != 0.f || gPlayer1Controller->stickY != 0.f) != 0) {
            nextYawVel = 0x20;
        }
    } else {
        if (sCSideButtonYaw < 0) {
            yaw += 0x200;
        }
        if (sCSideButtonYaw > 0) {
            yaw -= 0x200;
        }
        camera_approach_s16_symmetric_bool(&sCSideButtonYaw, 0, 0x100);
        nextYawVel = 0;
    }
    sYawSpeed = 0x400;
    xzDist = calc_hor_dist(sMarioCamState->pos, c->pos);

    if (sStatusFlags & CAM_FLAG_BEHIND_MARIO_POST_DOOR) {
        if (xzDist >= 250) {
            sStatusFlags &= ~CAM_FLAG_BEHIND_MARIO_POST_DOOR;
        }
        if (abss((sMarioCamState->faceAngle[1] - yaw) / 2) < 0x1800) {
            sStatusFlags &= ~CAM_FLAG_BEHIND_MARIO_POST_DOOR;
            yaw = sCameraYawAfterDoorCutscene + DEGREES(180);
            dist = 800.f;
            sStatusFlags |= CAM_FLAG_BLOCK_SMOOTH_MOVEMENT;
        }
    } else if (xzDist < 250) {
        // Turn rapidly if very close to Mario
        c->pos[0] += (250 - xzDist) * sins(yaw);
        c->pos[2] += (250 - xzDist) * coss(yaw);
        if (sCSideButtonYaw == 0) {
            nextYawVel = 0x1000;
            sYawSpeed = 0;
            vec3f_get_dist_and_angle(sMarioCamState->pos, c->pos, &dist, &pitch, &yaw);
        }
        closeToMario |= 1;
    }

    if (-16 < gPlayer1Controller->stickY) {
        c->yaw = yaw;
    }

    calc_y_to_curr_floor(&posHeight, 1, 200, &focHeight, 0.9f, 200);
    vec3f_copy(cPos, c->pos);
    avoidStatus = rotate_camera_around_walls(c, cPos, &avoidYaw, 0x600);
    // If a wall is blocking the view of Mario, then rotate in the calculated direction
    if (avoidStatus == AVOID_STATUS_WALL_COVERING_MARIO) {
        sAvoidYawVel = yaw;
        sStatusFlags |= CAM_FLAG_COLLIDED_WITH_WALL;
        // Rotate to avoid the wall
        approach_s16_asymptotic_bool(&yaw, avoidYaw, 10);
        sAvoidYawVel = (sAvoidYawVel - yaw) / 0x100;
    } else {
        if (gMarioStates[0].forwardVel == 0.f) {
            if (sStatusFlags & CAM_FLAG_COLLIDED_WITH_WALL) {
                if ((yawGoal - yaw) / 0x100 >= 0) {
                    yawDir = -1;
                } else {
                    yawDir = 1;
                }
                if ((sAvoidYawVel > 0 && yawDir > 0) || (sAvoidYawVel < 0 && yawDir < 0)) {
                    yawVel = nextYawVel;
                }
            } else {
                yawVel = nextYawVel;
            }
        } else {
            if (nextYawVel == 0x1000) {
                yawVel = nextYawVel;
            }
            sStatusFlags &= ~CAM_FLAG_COLLIDED_WITH_WALL;
        }

        // If a wall is near the camera, turn twice as fast
        if (avoidStatus != AVOID_STATUS_NONE) {
            yawVel += yawVel;
        }
        // ...Unless the camera already rotated from being close to Mario
        if ((closeToMario & 1) && avoidStatus != AVOID_STATUS_NONE) {
            yawVel = 0;
        }
        if (yawVel != 0 && get_dialog_id() == DIALOG_NONE) {
            camera_approach_s16_symmetric_bool(&yaw, yawGoal, yawVel);
        }
    }

    // Only zoom out if not obstructed by walls and Lakitu hasn't collided with any
    if (avoidStatus == AVOID_STATUS_NONE && !(sStatusFlags & CAM_FLAG_COLLIDED_WITH_WALL)) {
        approach_f32_asymptotic_bool(&dist, zoomDist - 100.f, 0.05f);
    }
    vec3f_set_dist_and_angle(sMarioCamState->pos, cPos, dist, pitch, yaw);
    cPos[1] += posHeight + 125.f;

    // Move the camera away from walls and set the collision flag
    if (collide_with_walls(cPos, 10.f, 80.f) != 0) {
        sStatusFlags |= CAM_FLAG_COLLIDED_WITH_WALL;
    }

    c->focus[0] = sMarioCamState->pos[0];
    c->focus[1] = sMarioCamState->pos[1] + 125.f + focHeight;
    c->focus[2] = sMarioCamState->pos[2];

    marioFloorHeight = 125.f + sMarioGeometry.currFloorHeight;
    marioFloor = sMarioGeometry.currFloor;
    camFloorHeight = find_floor(cPos[0], cPos[1] + 50.f, cPos[2], &cFloor) + 125.f;
    for (scale = 0.1f; scale < 1.f; scale += 0.2f) {
        scale_along_line(tempPos, cPos, sMarioCamState->pos, scale);
        tempFloorHeight = find_floor(tempPos[0], tempPos[1], tempPos[2], &tempFloor) + 125.f;
        if (tempFloor != NULL && tempFloorHeight > marioFloorHeight) {
            marioFloorHeight = tempFloorHeight;
            marioFloor = tempFloor;
        }
    }

    // Lower the camera in Mario mode
    if (sSelectionFlags & CAM_MODE_MARIO_ACTIVE) {
        marioFloorHeight -= 35.f;
        camFloorHeight -= 35.f;
        c->focus[1] -= 25.f;
    }

    // If there's water below the camera, decide whether to keep the camera above the water surface
    waterHeight = find_water_level(cPos[0], cPos[2]);
    if (waterHeight != FLOOR_LOWER_LIMIT) {
        waterHeight += 125.f;
        distFromWater = waterHeight - marioFloorHeight;
        if (!(gCameraMovementFlags & CAM_MOVE_METAL_BELOW_WATER)) {
            if (distFromWater > 800.f && (sMarioCamState->action & ACT_FLAG_METAL_WATER)) {
                gCameraMovementFlags |= CAM_MOVE_METAL_BELOW_WATER;
            }
        } else {
            if (distFromWater < 400.f || !(sMarioCamState->action & ACT_FLAG_METAL_WATER)) {
                gCameraMovementFlags &= ~CAM_MOVE_METAL_BELOW_WATER;
            }
        }
        // If not wearing the metal cap, always stay above
        if (!(gCameraMovementFlags & CAM_MOVE_METAL_BELOW_WATER) && camFloorHeight < waterHeight) {
            camFloorHeight = waterHeight;
        }
    } else {
        gCameraMovementFlags &= ~CAM_MOVE_METAL_BELOW_WATER;
    }

    cPos[1] = camFloorHeight;
    vec3f_copy(tempPos, cPos);
    tempPos[1] -= 125.f;
    if (marioFloor != NULL && camFloorHeight <= marioFloorHeight) {
        avoidStatus = is_range_behind_surface(c->focus, tempPos, marioFloor, 0, SURFACE_NULL);
        if (avoidStatus != AVOID_STATUS_WALL_NEAR_CAMERA && ceilHeight > marioFloorHeight) {
            camFloorHeight = marioFloorHeight;
        }
    }

    posHeight = 0.f;
    if (c->mode == CAMERA_MODE_FREE_ROAM) {
        if (gCameraMovementFlags & CAM_MOVE_ZOOMED_OUT) {
            posHeight = 375.f;
            if (gCurrLevelArea == AREA_SSL_PYRAMID) {
                posHeight /= 2;
            }
        } else {
            posHeight = 100.f;
        }
    }
    if ((gCameraMovementFlags & CAM_MOVE_ZOOMED_OUT) && (sSelectionFlags & CAM_MODE_MARIO_ACTIVE)) {
        posHeight = 610.f;
#ifdef ENABLE_VANILLA_LEVEL_SPECIFIC_CHECKS
        if (gCurrLevelArea == AREA_SSL_PYRAMID || gCurrLevelNum == LEVEL_CASTLE) {
            posHeight /= 2;
        }
#endif
    }

    // Make Lakitu fly above the gas
    gasHeight = find_poison_gas_level(cPos[0], cPos[2]);
    if (gasHeight != FLOOR_LOWER_LIMIT) {
        if ((gasHeight += 130.f) > c->pos[1]) {
            c->pos[1] = gasHeight;
        }
    }

    if (sMarioCamState->action & ACT_FLAG_HANGING || sMarioCamState->action == ACT_RIDING_HOOT) {
        camFloorHeight = sMarioCamState->pos[1] + 400.f;
        if (c->mode == CAMERA_MODE_FREE_ROAM) {
            camFloorHeight -= 100.f;
        }
        ceilHeight = CELL_HEIGHT_LIMIT;
        vec3f_copy(c->focus, sMarioCamState->pos);
    }

    if (sMarioCamState->action & ACT_FLAG_ON_POLE) {
        camFloorHeight = gMarioStates[0].usedObj->oPosY + 125.f;
        if (sMarioCamState->pos[1] - 100.f > camFloorHeight) {
            camFloorHeight = sMarioCamState->pos[1] - 100.f;
        }
        ceilHeight = CELL_HEIGHT_LIMIT;
        vec3f_copy(c->focus, sMarioCamState->pos);
    }
    if (camFloorHeight != FLOOR_LOWER_LIMIT) {
        camFloorHeight += posHeight;
        approach_camera_height(c, camFloorHeight, 20.f);
    }
    c->pos[0] = cPos[0];
    c->pos[2] = cPos[2];
    cPos[0] = gLakituState.goalPos[0];
    cPos[1] = c->pos[1];
    cPos[2] = gLakituState.goalPos[2];
    vec3f_get_dist_and_angle(cPos, c->pos, &dist, &tempPitch, &tempYaw);
    // Prevent the camera from lagging behind too much
    if (dist > 50.f) {
        dist = 50.f;
        vec3f_set_dist_and_angle(cPos, c->pos, dist, tempPitch, tempYaw);
    }
    if (sMarioGeometry.currFloorType != SURFACE_DEATH_PLANE) {
        vec3f_get_dist_and_angle(c->focus, c->pos, &dist, &tempPitch, &tempYaw);
        if (dist > zoomDist) {
            dist = zoomDist;
            vec3f_set_dist_and_angle(c->focus, c->pos, dist, tempPitch, tempYaw);
        }
    }
    if (ceilHeight != CELL_HEIGHT_LIMIT) {
        if (c->pos[1] > (ceilHeight -= 150.f)
            && (avoidStatus = is_range_behind_surface(c->pos, sMarioCamState->pos, ceil, 0, -1)) == 1) {
            c->pos[1] = ceilHeight;
        }
    }
#ifdef ENABLE_VANILLA_LEVEL_SPECIFIC_CHECKS
    if (gCurrLevelArea == AREA_WDW_TOWN) {
        yaw = clamp_positions_and_find_yaw(c->pos, c->focus, 2254.f, -3789.f, 3790.f, -2253.f);
    }
#endif
    return yaw;
}

/**
 * The default camera mode
 * Used by close and free roam modes
 */
void mode_default_camera(struct Camera *c) {
    set_fov_function(CAM_FOV_DEFAULT);
    c->nextYaw = update_default_camera(c);
    pan_ahead_of_player(c);
}
