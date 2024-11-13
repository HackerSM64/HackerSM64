#include "engine/math_util.h"
#include "game/camera.h"

/**
 * Warps the camera underneath the floor, used in HMC to move under the elevator platforms
 */
void move_camera_through_floor_while_descending(struct Camera *c, f32 height) {
    if ((sMarioGeometry.currFloorHeight < height - 100.f)
        && (sMarioGeometry.prevFloorHeight > sMarioGeometry.currFloorHeight)) {
        c->pos[1] = height - 400.f;
        gLakituState.curPos[1] = height - 400.f;
        gLakituState.goalPos[1] = height - 400.f;
    }
}

void cam_hmc_enter_maze(struct Camera *c) {
    s16 pitch, yaw;
    f32 dist;

    if (c->pos[1] > -102.f) {
        vec3f_get_dist_and_angle(c->focus, gLakituState.goalPos, &dist, &pitch, &yaw);
        vec3f_set_dist_and_angle(c->focus, gLakituState.goalPos, 300.f, pitch, yaw);
        gLakituState.goalPos[1] = -800.f;
        c->pos[1] = gLakituState.goalPos[1];
        gLakituState.curPos[1] = gLakituState.goalPos[1];
        sStatusFlags &= ~CAM_FLAG_SMOOTH_MOVEMENT;
    }
}

void cam_hmc_elevator_black_hole(struct Camera *c) {
    move_camera_through_floor_while_descending(c, 1536.f);
}

void cam_hmc_elevator_maze_emergency_exit(struct Camera *c) {
    move_camera_through_floor_while_descending(c, 2355.f);
}

void cam_hmc_elevator_lake(struct Camera *c) {
    move_camera_through_floor_while_descending(c, 1843.f);
}

void cam_hmc_elevator_maze(struct Camera *c) {
    move_camera_through_floor_while_descending(c, 1843.f);
}
