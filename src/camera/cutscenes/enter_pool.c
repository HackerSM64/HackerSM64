#include "camera/cutscene_helpers.h"
#include "camera/camera_math.h"
#include "engine/math_util.h"
#include "game/camera.h"

/**
 * Cutscene that plays when Mario enters HMC or CotMC.
 */

void cutscene_enter_pool_start(struct Camera *c) {
    vec3f_copy(sCutsceneVars[3].point, sMarioCamState->pos);

#ifdef ENABLE_VANILLA_LEVEL_SPECIFIC_CHECKS
    if (gCurrLevelNum == LEVEL_CASTLE) { // entering HMC
        vec3f_set(sCutsceneVars[3].point, 2485.f, -1589.f, -2659.f);
    }
    if (gCurrLevelNum == LEVEL_HMC) { // entering CotMC
        vec3f_set(sCutsceneVars[3].point, 3350.f, -4589.f, 4800.f);
    }
#endif

    vec3f_copy(sCutsceneVars[0].point, c->focus);
}

void cutscene_enter_pool_loop(struct Camera *c) {
    cutscene_goto_cvar_pos(c, 1200.f, 0x2000, 0x200, 0);
}

void cutscene_enter_pool(struct Camera *c) {
    cutscene_event(cutscene_enter_pool_start, c, 0, 0);
    cutscene_event(cutscene_enter_pool_loop, c, 0, -1);
    sStatusFlags |= CAM_FLAG_SMOOTH_MOVEMENT;
}

struct Cutscene sCutsceneEnterPool[] = {
    { cutscene_enter_pool, 100 },
    { cutscene_exit_to_castle_grounds_end, 0 }
};
