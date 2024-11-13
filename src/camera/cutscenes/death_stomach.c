#include "camera/cutscene_helpers.h"
#include "camera/camera_math.h"
#include "engine/math_util.h"
#include "game/camera.h"

/**
 * Cutscene that plays when Mario dies on his stomach.
 */

void cutscene_death_stomach_start(struct Camera *c) {
    Vec3f offset = { 0, 40.f, -60.f };

    offset_rotated(sCutsceneVars[3].point, sMarioCamState->pos, offset, sMarioCamState->faceAngle);
    vec3f_copy(sCutsceneVars[0].point, c->focus);
}

void cutscene_death_stomach_goto_mario(struct Camera *c) {
    cutscene_goto_cvar_pos(c, 400.f, 0x1800, 0, -0x400);
}

/**
 * Cutscene that plays when Mario dies on his stomach.
 */
void cutscene_death_stomach(struct Camera *c) {
    cutscene_event(cutscene_death_stomach_start, c, 0, 0);
    cutscene_event(cutscene_death_stomach_goto_mario, c, 0, -1);
    sStatusFlags |= CAM_FLAG_SMOOTH_MOVEMENT;
    set_handheld_shake(HAND_CAM_SHAKE_CUTSCENE);
}

struct Cutscene sCutsceneDeathStomach[] = {
    { cutscene_death_stomach, CUTSCENE_LOOP }
};
