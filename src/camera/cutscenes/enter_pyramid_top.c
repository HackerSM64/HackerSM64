#include "camera/cutscene_helpers.h"
#include "camera/camera_math.h"
#include "engine/math_util.h"
#include "game/camera.h"

/**
 * Cutscene that plays when Mario enters the pyramid through the hole at the top.
 */

/**
 * Store the camera focus in cvar0, and store the top of the pyramid in cvar3.
 */
void cutscene_enter_pyramid_top_start(struct Camera *c) {
    vec3f_copy(sCutsceneVars[0].point, c->focus);
    vec3f_set(sCutsceneVars[3].point, c->areaCenX, 1280.f, c->areaCenZ);
}

/**
 * Cutscene that plays when Mario enters the top of the pyramid.
 */
void cutscene_enter_pyramid_top(struct Camera *c) {
    cutscene_event(cutscene_enter_pyramid_top_start, c, 0, 0);
    // Move to cvar3
    cutscene_goto_cvar_pos(c, 200.f, 0x3000, 0, 0);
    sStatusFlags |= CAM_FLAG_SMOOTH_MOVEMENT;
    set_handheld_shake(HAND_CAM_SHAKE_CUTSCENE);

    if (sMarioCamState->pos[1] > 1250.f) {
        // End the cutscene early if Mario ledge-grabbed.
        // This only works because of the janky way that ledge-grabbing is implemented.
        cutscene_exit_to_castle_grounds_end(c);
    }
}

struct Cutscene sCutsceneEnterPyramidTop[] = {
    { cutscene_enter_pyramid_top, 90 },
    { cutscene_exit_to_castle_grounds_end, 0 }
};
