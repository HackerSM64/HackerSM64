#include "camera/cutscene_helpers.h"
#include "camera/camera_math.h"
#include "engine/math_util.h"
#include "engine/surface_collision.h"
#include "game/camera.h"

/**
 * Set cvar7 to Mario's pos and faceAngle
 * Set cvar6 to the focus offset from Mario.
 * set cvar5 to the pos offset from Mario. (This is always overwritten)
 */
void cutscene_non_painting_death_start(UNUSED struct Camera *c) {
    vec3f_copy(sCutsceneVars[7].point, sMarioCamState->pos);
    vec3s_copy(sCutsceneVars[7].angle, sMarioCamState->faceAngle);
    vec3f_set(sCutsceneVars[6].point, -42.f, 350.f, 727.f);
    // This is always overwritten, except in the unused cutscene_exit_bowser_death()
    vec3f_set(sCutsceneVars[5].point, 107.f, 226.f, 1187.f);
}

/**
 * Set the camera pos depending on which level Mario exited.
 */
void cutscene_non_painting_set_cam_pos(struct Camera *c) {
    struct Surface *floor;

    switch (gPrevLevel) {
        case LEVEL_HMC:
            vec3f_set(c->pos, 3465.f, -1008.f, -2961.f);
            break;

        case LEVEL_COTMC:
            vec3f_set(c->pos, 3465.f, -1008.f, -2961.f);
            break;

        case LEVEL_RR:
            vec3f_set(c->pos, -3741.f, 3151.f, 6065.f);
            break;

        case LEVEL_WMOTR:
            vec3f_set(c->pos, 1972.f, 3230.f, 5891.f);
            break;

        default:
            offset_rotated(c->pos, sCutsceneVars[7].point, sCutsceneVars[5].point, sCutsceneVars[7].angle);
            c->pos[1] = find_floor(c->pos[0], c->pos[1] + 1000.f, c->pos[2], &floor) + 125.f;
            break;
    }
}

/**
 * Update the camera focus depending on which level Mario exited.
 */
void cutscene_non_painting_set_cam_focus(struct Camera *c) {
    offset_rotated(c->focus, sCutsceneVars[7].point, sCutsceneVars[6].point, sCutsceneVars[7].angle);

    if ((gPrevLevel == LEVEL_COTMC) || (gPrevLevel == LEVEL_HMC) || (gPrevLevel == LEVEL_RR)
        || (gPrevLevel == LEVEL_WMOTR)) {
        c->focus[0] = c->pos[0] + (sMarioCamState->pos[0] - c->pos[0]) * 0.7f;
        c->focus[1] = c->pos[1] + (sMarioCamState->pos[1] - c->pos[1]) * 0.4f;
        c->focus[2] = c->pos[2] + (sMarioCamState->pos[2] - c->pos[2]) * 0.7f;
    } else {
        c->focus[1] = c->pos[1] + (sMarioCamState->pos[1] - c->pos[1]) * 0.2f;
    }
}

/**
 * End a non-painting exit cutscene. Used by BBH and bowser courses.
 */
void cutscene_non_painting_end(struct Camera *c) {
    c->cutscene = 0;

#ifdef USE_COURSE_DEFAULT_MODE
    c->mode = c->defMode;
#else
    if (c->defMode == CAMERA_MODE_CLOSE) {
        c->mode = CAMERA_MODE_CLOSE;
    } else {
        c->mode = CAMERA_MODE_FREE_ROAM;
    }
#endif

    sStatusFlags |= CAM_FLAG_UNUSED_CUTSCENE_ACTIVE;
    sStatusFlags |= CAM_FLAG_SMOOTH_MOVEMENT;
    transition_next_state(c, 60);
    cutscene_update_camera_yaw(c);
}

/**
 * Warp the camera to Mario, then use his faceAngle to calculate the right relative position.
 *
 * cvar0.point is Mario's position
 * cvar0.angle is Mario's faceAngle
 *
 * cvar1 is the camera's position relative to Mario
 * cvar2 is the camera's focus relative to Mario
 */
void cutscene_exit_painting_start(struct Camera *c) {
    struct Surface *floor;
    f32 floorHeight;

    vec3f_set(sCutsceneVars[2].point, 258.f, -352.f, 1189.f);
    vec3f_set(sCutsceneVars[1].point, 65.f, -155.f, 444.f);

    if (gPrevLevel == LEVEL_TTM) {
        sCutsceneVars[1].point[1] = 0.f;
        sCutsceneVars[1].point[2] = 0.f;
    }
    vec3f_copy(sCutsceneVars[0].point, sMarioCamState->pos);
    sCutsceneVars[0].angle[0] = 0;
    sCutsceneVars[0].angle[1] = sMarioCamState->faceAngle[1];
    sCutsceneVars[0].angle[2] = 0;
    offset_rotated(c->focus, sCutsceneVars[0].point, sCutsceneVars[1].point, sCutsceneVars[0].angle);
    offset_rotated(c->pos, sCutsceneVars[0].point, sCutsceneVars[2].point, sCutsceneVars[0].angle);
    floorHeight = find_floor(c->pos[0], c->pos[1] + 10.f, c->pos[2], &floor);

    if (floorHeight != FLOOR_LOWER_LIMIT) {
        if (c->pos[1] < (floorHeight += 60.f)) {
            c->pos[1] = floorHeight;
        }
    }
}

/**
 * Decrease cvar2's x and z offset, moving closer to Mario.
 */
void cutscene_exit_painting_move_to_mario(struct Camera *c) {
    Vec3f pos;

    //! Tricky math: Since offset_rotated() flips Z offsets, you'd expect a positive Z offset to move
    //! the camera into the wall. However, Mario's faceAngle always points into the painting, so a
    //! positive Z offset moves the camera "behind" Mario, away from the painting.
    //!
    //! In the success cutscene, when Mario jumps out face-first, only his gfx angle is updated. His
    //! actual face angle isn't updated until after the cutscene.
    approach_f32_asymptotic_bool(&sCutsceneVars[2].point[0], 178.f, 0.05f);
    approach_f32_asymptotic_bool(&sCutsceneVars[2].point[2], 889.f, 0.05f);
    offset_rotated(pos, sCutsceneVars[0].point, sCutsceneVars[2].point, sCutsceneVars[0].angle);
    c->pos[0] = pos[0];
    c->pos[2] = pos[2];
}

/**
 * Move the camera down to the floor Mario lands on.
 */
void cutscene_exit_painting_move_to_floor(struct Camera *c) {
    struct Surface *floor;
    Vec3f floorHeight;

    vec3f_copy(floorHeight, sMarioCamState->pos);
    floorHeight[1] = find_floor(sMarioCamState->pos[0], sMarioCamState->pos[1] + 10.f, sMarioCamState->pos[2], &floor);

    if (floor != NULL) {
        floorHeight[1] = floorHeight[1] + (sMarioCamState->pos[1] - floorHeight[1]) * 0.7f + 125.f;
        approach_vec3f_asymptotic(c->focus, floorHeight, 0.2f, 0.2f, 0.2f);

        if (floorHeight[1] < c->pos[1]) {
            approach_f32_asymptotic_bool(&c->pos[1], floorHeight[1], 0.05f);
        }
    }
}

/**
 * Cutscene played when Mario leaves a painting, either due to death or collecting a star.
 */
void cutscene_exit_painting(struct Camera *c) {
    cutscene_event(cutscene_exit_painting_start, c, 0, 0);
    cutscene_event(cutscene_exit_painting_move_to_mario, c, 5, -1);
    cutscene_event(cutscene_exit_painting_move_to_floor, c, 5, -1);

    //! Hardcoded position. TTM's painting is close to an opposite wall, so just fix the pos.
    if (gPrevLevel == LEVEL_TTM) {
        vec3f_set(c->pos, -296.f, 1261.f, 3521.f);
    }

    cutscene_update_camera_yaw(c);
}

/**
 * Give control back to the player.
 */
void cutscene_exit_painting_end(struct Camera *c) {
#ifdef USE_COURSE_DEFAULT_MODE
    c->mode = c->defMode;
#else
    c->mode = CAMERA_MODE_CLOSE;
#endif
    c->cutscene = CUTSCENE_NONE;
    gCutsceneTimer = CUTSCENE_STOP;
    sStatusFlags |= CAM_FLAG_SMOOTH_MOVEMENT;
    sStatusFlags &= ~CAM_FLAG_BLOCK_SMOOTH_MOVEMENT;
    cutscene_update_camera_yaw(c);
}

/**
 * This cutscene is the same as non_painting_death, but the camera is closer to Mario and lower.
 * Because it it doesn't call cutscene_non_painting_death_override_offset, the value from
 * cutscene_non_painting_death_start is used.
 *
 * This cutscene is unused, dying in bowser's arena spawns Mario near the warp pipe, not back in the
 * hub.
 */
void cutscene_exit_bowser_death(struct Camera *c) {
    cutscene_event(cutscene_non_painting_death_start, c, 0, 0);
    cutscene_event(cutscene_non_painting_set_cam_pos, c, 0, -1);
    cutscene_event(cutscene_non_painting_set_cam_focus, c, 0, -1);
}

/**
 * Set cvars:
 * cvar7 is Mario's pos and angle
 * cvar6 is the focus offset
 * cvar5 is the position offset
 */
void cutscene_exit_succ_start(UNUSED struct Camera *c) {
    vec3f_copy(sCutsceneVars[7].point, sMarioCamState->pos);
    vec3s_copy(sCutsceneVars[7].angle, sMarioCamState->faceAngle);
    vec3f_set(sCutsceneVars[6].point, 6.f, 363.f, 543.f);
    vec3f_set(sCutsceneVars[5].point, 137.f, 226.f, 995.f);
}

/**
 * Focus slightly left of Mario. Perhaps to keep the bowser painting in view?
 */
void cutscene_exit_bowser_succ_focus_left(UNUSED struct Camera *c) {
    approach_f32_asymptotic_bool(&sCutsceneVars[6].point[0], -24.f, 0.05f);
}

/**
 * Instead of focusing on the key, just start a pitch shake. Clever!
 * The shake lasts 32 frames.
 */
void cutscene_exit_bowser_key_toss_shake(struct Camera *c) {
    //! Unnecessary check.
    if (c->cutscene == CUTSCENE_EXIT_BOWSER_SUCC) {
        set_camera_pitch_shake(0x800, 0x40, 0x800);
    }
}

/**
 * Start a camera shake when Mario lands on the ground.
 */
void cutscene_exit_succ_shake_landing(UNUSED struct Camera *c) {
    set_environmental_camera_shake(SHAKE_ENV_EXPLOSION);
}

/**
 * Cutscene that plays when Mario beats bowser and exits the level.
 */
void cutscene_exit_bowser_succ(struct Camera *c) {
    cutscene_event(cutscene_exit_succ_start, c, 0, 0);
    cutscene_event(cutscene_non_painting_set_cam_pos, c, 0, -1);
    cutscene_event(cutscene_exit_bowser_succ_focus_left, c, 18, -1);
    cutscene_event(cutscene_non_painting_set_cam_focus, c, 0, -1);
    cutscene_event(cutscene_exit_bowser_key_toss_shake, c, 125, 125);
    cutscene_event(cutscene_exit_succ_shake_landing, c, 41, 41);
}

/**
 * Override the position offset.
 */
void cutscene_exit_non_painting_succ_override_cvar(UNUSED struct Camera *c) {
    vec3f_set(sCutsceneVars[5].point, 137.f, 246.f, 1115.f);
}

/**
 * Cutscene that plays when Mario collects a star and leaves a non-painting course, like HMC or BBH.
 */
void cutscene_exit_non_painting_succ(struct Camera *c) {
    cutscene_event(cutscene_exit_succ_start, c, 0, 0);
    cutscene_event(cutscene_exit_non_painting_succ_override_cvar, c, 0, 0);
    cutscene_event(cutscene_non_painting_set_cam_pos, c, 0, -1);
    cutscene_event(cutscene_exit_bowser_succ_focus_left, c, 18, -1);
    cutscene_event(cutscene_non_painting_set_cam_focus, c, 0, -1);
    cutscene_event(cutscene_exit_succ_shake_landing, c, 41, 41);
    cutscene_update_camera_yaw(c);
}

/**
 * Set the offset from Mario depending on the course Mario exited.
 * This overrides cutscene_non_painting_death_start()
 */
void cutscene_non_painting_death_override_offset(UNUSED struct Camera *c) {
    switch (gPrevLevel) {
        case LEVEL_HMC:
            vec3f_set(sCutsceneVars[5].point, 187.f, 369.f, -197.f);
            break;
        case LEVEL_COTMC:
            vec3f_set(sCutsceneVars[5].point, 187.f, 369.f, -197.f);
            break;
        default:
            vec3f_set(sCutsceneVars[5].point, 107.f, 246.f, 1307.f);
            break;
    }
}

/**
 * Cutscene played when Mario dies in a non-painting course, like HMC or BBH.
 */
void cutscene_non_painting_death(struct Camera *c) {
    cutscene_event(cutscene_non_painting_death_start, c, 0, 0);
    cutscene_event(cutscene_non_painting_death_override_offset, c, 0, 0);
    cutscene_event(cutscene_non_painting_set_cam_pos, c, 0, -1);
    cutscene_event(cutscene_non_painting_set_cam_focus, c, 0, -1);
    sStatusFlags |= CAM_FLAG_UNUSED_CUTSCENE_ACTIVE;
}

void cutscene_exit_waterfall_warp(struct Camera *c) {
    //! hardcoded position
    vec3f_set(c->pos, -3899.f, 39.f, -5671.f);
}

/**
 * Look at Mario, used by cutscenes that play when Mario exits a course to castle grounds.
 */
void cutscene_exit_to_castle_grounds_focus_mario(struct Camera *c) {
    vec3f_copy(c->focus, sMarioCamState->pos);
    c->focus[1] = c->pos[1] + (sMarioCamState->pos[1] + 125.f - c->pos[1]) * 0.5f;
    approach_vec3f_asymptotic(c->focus, sMarioCamState->pos, 0.05f, 0.4f, 0.05f);
}

/**
 * Cutscene that plays when Mario leaves CotMC through the waterfall.
 */
void cutscene_exit_waterfall(struct Camera *c) {
    cutscene_event(cutscene_exit_waterfall_warp, c, 0, 0);
    cutscene_event(cutscene_exit_to_castle_grounds_focus_mario, c, 0, -1);
    cutscene_update_camera_yaw(c);
}

void cutscene_exit_fall_to_castle_grounds_warp(struct Camera *c) {
    //! hardcoded position
    vec3f_set(c->pos, 5830.f, 32.f, 3985.f);
}

/**
 * Cutscene that plays when Mario falls from WMOTR.
 */
void cutscene_exit_fall_to_castle_grounds(struct Camera *c) {
    cutscene_event(cutscene_exit_fall_to_castle_grounds_warp, c, 0, 0);
    cutscene_event(cutscene_exit_to_castle_grounds_focus_mario, c, 0, -1);
    cutscene_update_camera_yaw(c);
}

/**
 * Unused. Warp the camera to Mario.
 */
void cutscene_unused_exit_start(struct Camera *c) {
    Vec3f offset;
    Vec3s marioAngle;

    vec3f_set(offset, 200.f, 300.f, 200.f);
    vec3s_set(marioAngle, 0, sMarioCamState->faceAngle[1], 0);
    offset_rotated(c->pos, sMarioCamState->pos, offset, marioAngle);
    set_focus_rel_mario(c, 0.f, 125.f, 0.f, 0);
}

/**
 * Unused. Focus on Mario as he exits.
 */
void cutscene_unused_exit_focus_mario(struct Camera *c) {
    Vec3f focus;

    vec3f_set(focus, sMarioCamState->pos[0], sMarioCamState->pos[1] + 125.f, sMarioCamState->pos[2]);
    set_focus_rel_mario(c, 0.f, 125.f, 0.f, 0);
    approach_vec3f_asymptotic(c->focus, focus, 0.02f, 0.001f, 0.02f);
    cutscene_update_camera_yaw(c);
}

/**
 * Cutscene that plays when Mario dies and warps back to the castle.
 */
struct Cutscene sCutsceneDeathExit[] = {
    { cutscene_exit_painting, 118 },
    { cutscene_exit_painting_end, 0 }
};

/**
 * Unused cutscene for when Mario dies in bowser's arena. Instead, Mario just respawns at the warp pipe.
 */
struct Cutscene sCutsceneExitBowserDeath[] = {
    { cutscene_exit_bowser_death, 120 },
    { cutscene_non_painting_end, 0 }
};

/**
 * Cutscene that plays when Mario exits bowser's arena after getting the key.
 */
struct Cutscene sCutsceneExitBowserSuccess[] = {
    { cutscene_exit_bowser_succ, 190 },
    { cutscene_non_painting_end, 0 }
};

/**
 * Cutscene that plays when Mario warps to the castle after collecting a star.
 */
struct Cutscene sCutsceneExitPaintingSuccess[] = {
    { cutscene_exit_painting, 180 },
    { cutscene_exit_painting_end, 0 }
};

/**
 * Cutscene that plays when Mario exits a non-painting course, like HMC.
 */
struct Cutscene sCutsceneExitSpecialSuccess[] = {
    { cutscene_exit_non_painting_succ, 163 },
    { cutscene_non_painting_end, 0 }
};

/**
 * Cutscene that plays when Mario enters the castle grounds after leaving CotMC through the waterfall.
 */
struct Cutscene sCutsceneExitWaterfall[] = {
    { cutscene_exit_waterfall, 52 },
    { cutscene_exit_to_castle_grounds_end, 0 }
};

/**
 * Cutscene that plays when Mario falls from WMOTR.
 */
struct Cutscene sCutsceneFallToCastleGrounds[] = {
    { cutscene_exit_fall_to_castle_grounds, 73 },
    { cutscene_exit_to_castle_grounds_end, 0 }
};

/**
 * Cutscene that plays when Mario exits from dying in a non-painting course, like HMC.
 */
struct Cutscene sCutsceneNonPaintingDeath[] = {
    { cutscene_non_painting_death, 120 },
    { cutscene_non_painting_end, 0 }
};

struct Cutscene sCutsceneUnusedExit[] = {
    { cutscene_unused_exit_start, 1 },
    { cutscene_unused_exit_focus_mario, 60 },
    { cutscene_exit_painting_end, 0 }
};
