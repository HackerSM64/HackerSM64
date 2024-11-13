#include "camera/cutscene_helpers.h"
#include "camera/camera_math.h"
#include "engine/math_util.h"
#include "game/camera.h"

/**
 * Cutscene that plays when Mario dies in quicksand.
 */

/**
 * Copy the camera's focus to cvar0
 */
void cutscene_quicksand_death_start(struct Camera *c) {
    vec3f_copy(sCutsceneVars[0].point, c->focus);
}

/**
 * Fly closer to Mario. In WATER_DEATH, move to Mario's side.
 */
void cutscene_quicksand_death_goto_mario(struct Camera *c) {
    cutscene_goto_cvar_pos(c, 400.f, 0x2800, 0x200, 0);

    if (c->cutscene == CUTSCENE_WATER_DEATH) {
        water_death_move_to_mario_side(c);
    }
}

/**
 * Cutscene that plays when Mario dies in quicksand.
 */
void cutscene_quicksand_death(struct Camera *c) {
    sCutsceneVars[3].point[0] = sMarioCamState->pos[0];
    sCutsceneVars[3].point[1] = sMarioCamState->pos[1] + 20.f;
    sCutsceneVars[3].point[2] = sMarioCamState->pos[2];

    cutscene_event(cutscene_quicksand_death_start, c, 0, 0);
    cutscene_event(cutscene_quicksand_death_goto_mario, c, 0, -1);
    sStatusFlags |= CAM_FLAG_SMOOTH_MOVEMENT;
    set_handheld_shake(HAND_CAM_SHAKE_HIGH);
}

struct Cutscene sCutsceneQuicksandDeath[] = {
    { cutscene_quicksand_death, CUTSCENE_LOOP },
};
