#include "camera/cutscene_helpers.h"
#include "camera/camera_math.h"
#include "engine/math_util.h"
#include "game/camera.h"

/**
 * Cutscene for the red coin star spawning. Compared to a regular star, this cutscene can warp long
 * distances.
 */

/**
 * Start the red coin star spawning cutscene.
 */
void cutscene_red_coin_star_start(struct Camera *c) {
    object_pos_to_vec3f(sCutsceneVars[1].point, gCutsceneFocus);
    store_info_star(c);
    // Store the default fov for after the cutscene
    sCutsceneVars[2].point[2] = sFOVState.fov;
}

/**
 * Look towards the star's x and z position
 */
void cutscene_red_coin_star_focus_xz(struct Camera *c) {
    approach_f32_asymptotic_bool(&c->focus[0], gCutsceneFocus->oPosX, 0.15f);
    approach_f32_asymptotic_bool(&c->focus[2], gCutsceneFocus->oPosZ, 0.15f);
}

/**
 * Look towards the star's y position. Only active before the camera warp.
 */
void cutscene_red_coin_star_focus_y(struct Camera *c) {
    approach_f32_asymptotic_bool(&c->focus[1], gCutsceneFocus->oPosY, 0.1f);
}

/**
 * Look 80% up towards the star. Only active after the camera warp.
 */
void cutscene_red_coin_star_look_up_at_star(struct Camera *c) {
    c->focus[1] = sCutsceneVars[1].point[1] + (gCutsceneFocus->oPosY - sCutsceneVars[1].point[1]) * 0.8f;
}

/**
 * Warp the camera near the star's spawn point
 */
void cutscene_red_coin_star_warp(struct Camera *c) {
    f32 dist;
    s16 pitch, yaw, posYaw;
    struct Object *obj = gCutsceneFocus;

    vec3f_set(sCutsceneVars[1].point, obj->oHomeX, obj->oHomeY, obj->oHomeZ);
    vec3f_get_dist_and_angle(sCutsceneVars[1].point, c->pos, &dist, &pitch, &yaw);
    posYaw = calculate_yaw(sCutsceneVars[1].point, c->pos);
    yaw = calculate_yaw(sCutsceneVars[1].point, sMarioCamState->pos);

    if (ABS(yaw - posYaw + DEGREES(90)) < ABS(yaw - posYaw - DEGREES(90))) {
        yaw += DEGREES(90);
    } else {
        yaw -= DEGREES(90);
    }

    vec3f_set_dist_and_angle(sCutsceneVars[1].point, c->pos, 400.f, 0x1000, yaw);
    sStatusFlags &= ~CAM_FLAG_SMOOTH_MOVEMENT;
}

/**
 * Zoom out while looking at the star.
 */
void cutscene_red_coin_star_set_fov(UNUSED struct Camera *c) {
    sFOVState.fov = 60.f;
}

void cutscene_red_coin_star(struct Camera *c) {
    sStatusFlags |= CAM_FLAG_SMOOTH_MOVEMENT;
    cutscene_event(cutscene_red_coin_star_start, c, 0, 0);
    cutscene_event(cutscene_red_coin_star_warp, c, 30, 30);
    cutscene_event(cutscene_red_coin_star_focus_xz, c, 0, -1);
    cutscene_event(cutscene_red_coin_star_focus_y, c, 0, 29);
    cutscene_event(cutscene_red_coin_star_look_up_at_star, c, 30, -1);
    cutscene_event(cutscene_red_coin_star_set_fov, c, 30, -1);

    if (gObjCutsceneDone) {
        // Set the timer to CUTSCENE_LOOP, which start the next shot.
        gCutsceneTimer = CUTSCENE_LOOP;
    }
}

/**
 * End the red coin star spawning cutscene
 */
void cutscene_red_coin_star_end(struct Camera *c) {
    retrieve_info_star(c);
    gCutsceneTimer = CUTSCENE_STOP;
    c->cutscene = 0;
    // Restore the default fov
    sFOVState.fov = sCutsceneVars[2].point[2];
}

struct Cutscene sCutsceneRedCoinStarSpawn[] = {
    { cutscene_red_coin_star, CUTSCENE_LOOP },
    { cutscene_red_coin_star_end, 0 }
};
