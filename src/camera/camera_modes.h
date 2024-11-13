#pragma once

#include "config/config_game.h"
#include "game/camera.h"

/**
 * A point in a path used by update_parallel_tracking_camera
 */
struct ParallelTrackingPoint {
    /// Whether this point is the start of a path
    s16 startOfPath;
    /// Point used to define a line segment to follow
    Vec3f pos;
    /// The distance Mario can move along the line before the camera should move
    f32 distThresh;
    /// The percentage that the camera should move from the line to Mario
    f32 zoom;
};


// mode-specific export variables and functions
extern s16 sSpiralStairsYawOffset;
void move_mario_head_c_up(UNUSED struct Camera *c);

void exit_c_up(struct Camera *c);
void radial_camera_input(struct Camera *c);
void radial_camera_move(struct Camera *c);
void radial_camera_input_default(struct Camera *c);
void update_yaw_and_dist_from_c_up(UNUSED struct Camera *c);
void parallel_tracking_init(struct Camera *c, struct ParallelTrackingPoint *path);

void mode_8_directions_camera(struct Camera *c);
void mode_behind_mario_camera(struct Camera *c);
void mode_boss_fight_camera(struct Camera *c);
void mode_c_up_camera(struct Camera *c);
void mode_cannon_camera(struct Camera *c);
void mode_default_camera(struct Camera *c);
void mode_fixed_camera(struct Camera *c);
void mode_lakitu_camera(struct Camera *c);
void mode_mario_camera(struct Camera *c);
void mode_outward_radial_camera(struct Camera *c);
void mode_parallel_tracking_camera(struct Camera *c);
void mode_radial_camera(struct Camera *c);
void mode_slide_camera(struct Camera *c);
void mode_spiral_stairs_camera(struct Camera *c);
void mode_water_surface_camera(struct Camera *c);
s32 mode_behind_mario(struct Camera *c);



void set_camera_height(struct Camera *c, f32 goalHeight);
void handle_c_button_movement(struct Camera *c);
void lakitu_zoom(f32 rangeDist, s16 rangePitch);
s16 look_down_slopes(s16 camYaw);
void pan_ahead_of_player(struct Camera *c);
void focus_on_mario(Vec3f focus, Vec3f pos, f32 posYOff, f32 focYOff, f32 dist, s16 pitch, s16 yaw);
s32 rotate_camera_around_walls(struct Camera *c, Vec3f cPos, s16 *avoidYaw, s16 yawRange);
void calc_y_to_curr_floor(f32 *posOff, f32 posMul, f32 posBound, f32 *focOff, f32 focMul, f32 focBound);
