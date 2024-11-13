#include "camera/cutscene_helpers.h"
#include "engine/math_util.h"
#include "engine/surface_collision.h"
#include "game/camera.h"

/**
 * Starts the pool entrance cutscene if Mario is not exiting the pool.
 * Used in both the castle and HMC.
 */
void cam_castle_hmc_start_pool_cutscene(struct Camera *c) {
    if ((sMarioCamState->action != ACT_SPECIAL_DEATH_EXIT)
        && (sMarioCamState->action != ACT_SPECIAL_EXIT_AIRBORNE)) {
        start_cutscene(c, CUTSCENE_ENTER_POOL);
    }
}

/**
 * Sets the fixed mode pos offset so that the camera faces the doorway when Mario is near the entrance
 * to the castle lobby
 */
void cam_castle_lobby_entrance(UNUSED struct Camera *c) {
    vec3f_set(sCastleEntranceOffset, -813.f - sFixedModeBasePosition[0],
              378.f - sFixedModeBasePosition[1], 1103.f - sFixedModeBasePosition[2]);
}

/**
 * Make the camera look up the stairs from the 2nd to 3rd floor of the castle
 */
void cam_castle_look_upstairs(struct Camera *c) {
    struct Surface *floor;
    f32 floorHeight = find_floor(c->pos[0], c->pos[1], c->pos[2], &floor);

    // If Mario is on the first few steps, fix the camera pos, making it look up
    if ((sMarioGeometry.currFloorHeight > 1229.f) && (floorHeight < 1229.f)
        && (sCSideButtonYaw == 0)) {
        vec3f_set(c->pos, -227.f, 1425.f, 1533.f);
    }
}

/**
 * Make the camera look down the stairs towards the basement star door
 */
void cam_castle_basement_look_downstairs(struct Camera *c) {
    struct Surface *floor;
    f32 floorHeight = find_floor(c->pos[0], c->pos[1], c->pos[2], &floor);

    // Fix the camera pos, making it look downwards. Only active on the top few steps
    if ((floorHeight > -110.f) && (sCSideButtonYaw == 0)) {
        vec3f_set(c->pos, -980.f, 249.f, -1398.f);
    }
}

/**
 * Enter the fixed-mode castle lobby. A trigger for this is placed in every entrance so that the camera
 * changes to fixed mode.
 */
void cam_castle_enter_lobby(struct Camera *c) {
    if (c->mode != CAMERA_MODE_FIXED) {
        sStatusFlags &= ~CAM_FLAG_SMOOTH_MOVEMENT;
        set_fixed_cam_axis_sa_lobby(c->mode);
        c->mode = CAMERA_MODE_FIXED;
    }
}

/**
 * Starts spiral stairs mode.
 */
void cam_castle_enter_spiral_stairs(struct Camera *c) {
    transition_to_camera_mode(c, CAMERA_MODE_SPIRAL_STAIRS, 20);
}

/**
 * unused, starts close mode if the camera is in spiral stairs mode.
 * This was replaced with cam_castle_close_mode
 */
static UNUSED void cam_castle_leave_spiral_stairs(struct Camera *c) {
    if (c->mode == CAMERA_MODE_SPIRAL_STAIRS) {
        transition_to_camera_mode(c, CAMERA_MODE_CLOSE, 30);
    } else {
        set_camera_mode_close_cam(&c->mode);
    }
}

/**
 * The default mode when outside of the lobby and spiral staircase. A trigger for this is placed at
 * every door leaving the lobby and spiral staircase.
 */
void cam_castle_close_mode(struct Camera *c) {
    set_camera_mode_close_cam(&c->mode);
}

/**
 * Functions the same as cam_castle_close_mode, but sets doorStatus so that the camera will enter
 * fixed-mode when Mario leaves the room.
 */
void cam_castle_leave_lobby_sliding_door(struct Camera *c) {
    cam_castle_close_mode(c);
    c->doorStatus = DOOR_ENTER_LOBBY;
}

/**
 * Just calls cam_castle_enter_lobby
 */
void cam_castle_enter_lobby_sliding_door(struct Camera *c) {
    cam_castle_enter_lobby(c);
}
