#include "camera/cutscene_helpers.h"
#include "camera/camera_math.h"
#include "engine/math_util.h"
#include "game/camera.h"

/**
 * Unused cutscene for when the pyramid explodes.
 */

/**
 * Store the camera focus in cvar1.
 * Store the area's center position (which happens to be the pyramid, in SSL) in cvar3.
 */
void cutscene_pyramid_top_explode_start(struct Camera *c) {
    reset_pan_distance(c);
    store_info_cannon(c);

    vec3f_copy(sCutsceneVars[1].point, c->focus);
    vec3f_set(sCutsceneVars[3].point, c->areaCenX, 1280.f, c->areaCenZ);
}

/**
 * Zoom in on the pyramid.
 */
void cutscene_pyramid_top_explode_zoom_in(UNUSED struct Camera *c) {
    set_fov_function(CAM_FOV_APP_30);
}

/**
 * Look at the pyramid top.
 */
void cutscene_pyramid_top_explode_focus(struct Camera *c) {
    approach_vec3f_asymptotic(c->focus, sCutsceneVars[3].point, 0.02f, 0.02f, 0.02f);
    sStatusFlags |= CAM_FLAG_SMOOTH_MOVEMENT;
}

/**
 * Store the old pos and focus, then warp to the pyramid top.
 */
void cutscene_pyramid_top_explode_warp(struct Camera *c) {
    s16 pitch, yaw;
    f32 dist;

    set_fov_function(CAM_FOV_DEFAULT);
    sFOVState.fov = 45.f;

    vec3f_copy(sCutsceneVars[4].point, c->pos);
    vec3f_copy(sCutsceneVars[5].point, c->focus);
    vec3f_copy(c->focus, sCutsceneVars[3].point);

    vec3f_get_dist_and_angle(sCutsceneVars[3].point, sMarioCamState[0].pos, &dist, &pitch, &yaw);
    vec3f_set_dist_and_angle(sCutsceneVars[3].point, c->pos, 2000.f, 0, yaw);
    c->pos[1] += 500.f;
}

/**
 * Close up view of the spinning pyramid top as it rises.
 */
void cutscene_pyramid_top_explode_closeup(struct Camera *c) {
    s16 pitch, yaw;
    f32 dist;

    vec3f_get_dist_and_angle(sCutsceneVars[3].point, c->pos, &dist, &pitch, &yaw);
    approach_f32_asymptotic_bool(&dist, 2000.f, 0.1f);
    vec3f_set_dist_and_angle(sCutsceneVars[3].point, c->pos, dist, pitch, yaw);

    c->focus[1] += 4.f;
    c->pos[1] -= 5.f;
    sFOVState.fov = 45.f;
    set_handheld_shake(HAND_CAM_SHAKE_CUTSCENE);
}

/**
 * Shake the camera during the closeup.
 */
void cutscene_pyramid_top_explode_cam_shake(UNUSED struct Camera *c) {
    set_environmental_camera_shake(SHAKE_ENV_PYRAMID_EXPLODE);
}

/**
 * Warp back to the old position, and start a heavy camera shake.
 */
void cutscene_pyramid_top_explode_warp_back(struct Camera *c) {
    vec3f_copy(c->pos, sCutsceneVars[4].point);
    vec3f_copy(c->focus, sCutsceneVars[5].point);
    set_environmental_camera_shake(SHAKE_ENV_BOWSER_JUMP);
}

/**
 * An unused cutscene for when the pyramid explodes.
 */
void cutscene_pyramid_top_explode(struct Camera *c) {
    cutscene_event(cutscene_pyramid_top_explode_start, c, 0, 0);
    cutscene_event(cutscene_pyramid_top_explode_focus, c, 0, 30);
    cutscene_event(cutscene_pyramid_top_explode_warp, c, 31, 31);
    cutscene_event(cutscene_pyramid_top_explode_closeup, c, 31, 139);
    cutscene_event(cutscene_pyramid_top_explode_zoom_in, c, 23, 23);
    cutscene_event(cutscene_pyramid_top_explode_warp_back, c, 140, 140);
    cutscene_event(cutscene_pyramid_top_explode_cam_shake, c, 31, 139);
}

/**
 * End the pyramid top explosion cutscene.
 */
void cutscene_pyramid_top_explode_end(struct Camera *c) {
    cutscene_stop_dialog(c);
    stop_cutscene_and_retrieve_stored_info(c);
    // Move the camera back to Mario
    transition_next_state(c, 30);
}

struct Cutscene sCutscenePyramidTopExplode[] = {
    { cutscene_mario_dialog_look_front, CUTSCENE_LOOP },
    { cutscene_pyramid_top_explode, 150 },
    { cutscene_pyramid_top_explode_end, 0 }
};
