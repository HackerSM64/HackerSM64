#include "camera/cutscene_helpers.h"
#include "camera/camera_math.h"
#include "engine/math_util.h"
#include "game/camera.h"

/**
 * Cutscene that plays when a star spawns from ie a box or after a boss fight.
 */

void cutscene_star_spawn_store_info(struct Camera *c) {
    store_info_star(c);
}

/**
 * Focus on the top of the star.
 */
void cutscene_star_spawn_focus_star(struct Camera *c) {
    Vec3f starPos;

    if (gCutsceneFocus != NULL) {
        object_pos_to_vec3f(starPos, gCutsceneFocus);
        starPos[1] += gCutsceneFocus->hitboxHeight;
        approach_vec3f_asymptotic(c->focus, starPos, 0.1f, 0.1f, 0.1f);
    }
}

/**
 * Use boss fight mode's update function to move the focus back.
 */
void cutscene_star_spawn_update_boss_fight(struct Camera *c) {
    Vec3f pos, focus;

    update_boss_fight_camera(c, focus, pos);
    approach_vec3f_asymptotic(c->focus, focus, 0.2f, 0.2f, 0.2f);
    approach_vec3f_asymptotic(c->pos,     pos, 0.2f, 0.2f, 0.2f);
}

/**
 * Fly back to the camera's previous pos and focus.
 */
void cutscene_star_spawn_fly_back(struct Camera *c) {
    retrieve_info_star(c);
    transition_next_state(c, 15);
}

/**
 * Plays when a star spawns (ie from a box).
 */
void cutscene_star_spawn(struct Camera *c) {
    cutscene_event(cutscene_star_spawn_store_info, c, 0, 0);
    cutscene_event(cutscene_star_spawn_focus_star, c, 0, -1);
    sStatusFlags |= CAM_FLAG_SMOOTH_MOVEMENT;

    if (gObjCutsceneDone) {
        // Set the timer to CUTSCENE_LOOP, which start the next shot.
        gCutsceneTimer = CUTSCENE_LOOP;
    }
}

/**
 * Move the camera back to Mario.
 */
void cutscene_star_spawn_back(struct Camera *c) {
    if ((c->mode == CAMERA_MODE_BOSS_FIGHT) && (set_cam_angle(0) == CAM_ANGLE_LAKITU)) {
        cutscene_event(cutscene_star_spawn_update_boss_fight, c, 0, -1);
    } else {
        cutscene_event(cutscene_star_spawn_fly_back, c, 0, 0);
    }

    sStatusFlags |= CAM_FLAG_SMOOTH_MOVEMENT;
    sStatusFlags |= CAM_FLAG_UNUSED_CUTSCENE_ACTIVE;
}

void cutscene_star_spawn_end(struct Camera *c) {
    sStatusFlags |= CAM_FLAG_SMOOTH_MOVEMENT;
    gCutsceneTimer = CUTSCENE_STOP;
    c->cutscene = 0;
}

struct Cutscene sCutsceneStarSpawn[] = {
    { cutscene_star_spawn, CUTSCENE_LOOP },
    { cutscene_star_spawn_back, 15 },
    { cutscene_star_spawn_end, 0 }
};
