#include "camera/cutscene_helpers.h"
#include "camera/camera_math.h"
#include "engine/math_util.h"
#include "game/camera.h"

/**
 * Cutscene that plays when Mario dies while standing, or from electrocution.
 */

void cutscene_death_standing_start(struct Camera *c) {
    vec3f_copy(sCutsceneVars[0].point, c->focus);
    vec3f_copy(sCutsceneVars[3].point, sMarioCamState->pos);
    sCutsceneVars[3].point[1] += 70.f;
}

/**
 * Fly to Mario and turn on handheld shake.
 */
void cutscene_death_standing_goto_mario(struct Camera *c) {
    cutscene_goto_cvar_pos(c, 400.f, 0x1000, 0x300, 0);
    set_handheld_shake(HAND_CAM_SHAKE_HIGH);
}

/**
 * Cutscene that plays when Mario dies while standing.
 */
void cutscene_death_standing(struct Camera *c) {
    cutscene_event(cutscene_death_standing_start, c, 0, 0);
    cutscene_event(cutscene_death_standing_goto_mario, c, 0, -1);
    sStatusFlags |= CAM_FLAG_SMOOTH_MOVEMENT;
}

struct Cutscene sCutsceneStandingDeath[] = {
    { cutscene_death_standing, CUTSCENE_LOOP }
};
