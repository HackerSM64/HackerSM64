#include "camera/cutscene_helpers.h"
#include "camera/camera_math.h"
#include "engine/math_util.h"
#include "game/camera.h"

/**
 * Star dance cutscene.
 * For the default dance, the camera moves closer to Mario, then stays in place.
 * For the rotate dance, the camera moves closer and rotates clockwise around Mario.
 */

/**
 * Approach Mario and look up. Since Mario faces the camera when he collects the star, there's no need
 * to worry about the camera's yaw.
 */
void cutscene_dance_move_to_mario(struct Camera *c) {
    s16 pitch, yaw;
    f32 dist;

    vec3f_get_dist_and_angle(sMarioCamState->pos, c->pos, &dist, &pitch, &yaw);
    approach_f32_asymptotic_bool(&dist, 600.f, 0.3f);
    approach_s16_asymptotic_bool(&pitch, 0x1000, 0x10);
    vec3f_set_dist_and_angle(sMarioCamState->pos, c->pos, dist, pitch, yaw);
}

void cutscene_dance_rotate(struct Camera *c) {
    rotate_and_move_vec3f(c->pos, sMarioCamState->pos, 0, 0, 0x200);
}

void cutscene_dance_rotate_move_back(struct Camera *c) {
    rotate_and_move_vec3f(c->pos, sMarioCamState->pos, -15.f, 0, 0);
}

void cutscene_dance_rotate_move_towards_mario(struct Camera *c) {
    rotate_and_move_vec3f(c->pos, sMarioCamState->pos, 20.f, 0, 0);
}

/**
 * Speculated to be dance-related due to its proximity to the other dance functions
 */
UNUSED static void cutscene_dance_unused(UNUSED struct Camera *c) {
}

/**
 * Slowly turn to the point 100 units in front of Mario
 */
void cutscene_dance_default_focus_mario(struct Camera *c) {
    focus_in_front_of_mario(c, -100.f, 0.2f);
}

/**
 * Focus twice as far away as default dance, and move faster.
 */
void cutscene_dance_rotate_focus_mario(struct Camera *c) {
    focus_in_front_of_mario(c, -200.f, 0.03f);
}

void cutscene_dance_shake_fov(UNUSED struct Camera *c) {
    set_fov_shake(0x200, 0x28, 0x8000);
}

/**
 * Handles both the default and rotate dance cutscenes.
 * In the default dance: the camera moves closer to Mario, then stays in place.
 * In the rotate dance: the camera moves closer and rotates clockwise around Mario.
 */
void cutscene_dance_default_rotate(struct Camera *c) {
    sStatusFlags |= CAM_FLAG_SMOOTH_MOVEMENT;
    sYawSpeed = 0;
    set_fov_function(CAM_FOV_DEFAULT);
    cutscene_event(cutscene_dance_default_focus_mario, c, 0, 20);
    cutscene_event(cutscene_dance_move_to_mario, c, 0, 39);
    // Shake the camera on the 4th beat of the music, when Mario gives the peace sign.
    cutscene_event(cutscene_dance_shake_fov, c, 40, 40);

    if (c->cutscene != CUTSCENE_DANCE_DEFAULT) { // CUTSCENE_DANCE_ROTATE
        cutscene_event(cutscene_dance_rotate_focus_mario, c, 75, 102);
        cutscene_event(cutscene_dance_rotate, c, 50, -1);
        // These two functions move the camera away and then towards Mario.
        cutscene_event(cutscene_dance_rotate_move_back, c, 50, 80);
        cutscene_event(cutscene_dance_rotate_move_towards_mario, c, 70, 90);
    } else {
        // secret star, 100 coin star, or bowser red coin star.
        if ((sMarioCamState->action != ACT_STAR_DANCE_NO_EXIT)
            && (sMarioCamState->action != ACT_STAR_DANCE_WATER)
            && (sMarioCamState->action != ACT_STAR_DANCE_EXIT)) {
            gCutsceneTimer = CUTSCENE_STOP;
            c->cutscene = 0;
            transition_next_state(c, 20);
            sStatusFlags |= CAM_FLAG_UNUSED_CUTSCENE_ACTIVE;
        }
    }
}

struct Cutscene sCutsceneDanceDefaultRotate[] = {
    { cutscene_dance_default_rotate, CUTSCENE_LOOP }
};
