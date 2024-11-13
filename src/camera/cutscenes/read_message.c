#include "camera/cutscene_helpers.h"
#include "camera/camera_modes.h"
#include "camera/camera_math.h"
#include "engine/math_util.h"
#include "game/camera.h"
#include "game/object_list_processor.h"

/**
 * Cutscene that plays when Mario reads a sign or message.
 */

/**
 * Soften the music, clear cvar0
 *
 * In this cutscene, cvar0.angle[0] is used as a state variable.
 */
void cutscene_read_message_start(struct Camera *c) {
    cutscene_soften_music(c);
    transition_next_state(c, 30);
    reset_pan_distance(c);
    store_info_star(c);

    sCutsceneVars[1].angle[0] = sCUpCameraPitch;
    sCutsceneVars[1].angle[1] = sModeOffsetYaw;
    sCUpCameraPitch = -0x830;
    sModeOffsetYaw = 0;
    sCutsceneVars[0].angle[0] = 0;
}

UNUSED static void unused_cam_to_mario(struct Camera *c) {
    Vec3s dir;

    vec3s_set(dir, 0, sMarioCamState->faceAngle[1], 0);
    offset_rotated_coords(c->pos, sMarioCamState->pos, dir, 0, 100.f, 190.f);
    offset_rotated_coords(c->focus, sMarioCamState->pos, dir, 0, 70.f, -20.f);
}

/**
 * Cutscene that plays when Mario is reading a message (a sign or message on the wall)
 */
void cutscene_read_message(struct Camera *c) {
    cutscene_event(cutscene_read_message_start, c, 0, 0);
    sStatusFlags |= CAM_FLAG_SMOOTH_MOVEMENT;

    switch (sCutsceneVars[0].angle[0]) {
        // Do nothing until message is gone.
        case 0:
            if (get_dialog_id() != DIALOG_NONE) {
                sCutsceneVars[0].angle[0]++;
                set_time_stop_flags(TIME_STOP_ENABLED | TIME_STOP_DIALOG);
            }
            break;
        // Leave the dialog.
        case 1:
            move_mario_head_c_up(c);
            update_c_up(c, c->focus, c->pos);

            // This could cause softlocks. If a message starts one frame after another one closes, the
            // cutscene will never end.
            if (get_dialog_id() == DIALOG_NONE) {
                gCutsceneTimer = CUTSCENE_LOOP;
                retrieve_info_star(c);
                transition_next_state(c, 15);
                sStatusFlags |= CAM_FLAG_UNUSED_CUTSCENE_ACTIVE;
                clear_time_stop_flags(TIME_STOP_ENABLED | TIME_STOP_DIALOG);
                // Retrieve previous state
                sCUpCameraPitch = sCutsceneVars[1].angle[0];
                sModeOffsetYaw = sCutsceneVars[1].angle[1];
                cutscene_unsoften_music(c);
            }
    }
    sStatusFlags |= CAM_FLAG_UNUSED_CUTSCENE_ACTIVE;
}

/**
 * Set CAM_FLAG_UNUSED_CUTSCENE_ACTIVE, which does nothing.
 */
void cutscene_read_message_set_flag(UNUSED struct Camera *c) {
    sStatusFlags |= CAM_FLAG_UNUSED_CUTSCENE_ACTIVE;
}

/**
 * End the message cutscene.
 */
void cutscene_read_message_end(struct Camera *c) {
    sStatusFlags |= CAM_FLAG_UNUSED_CUTSCENE_ACTIVE;
    c->cutscene = 0;
}

struct Cutscene sCutsceneReadMessage[] = {
    { cutscene_read_message, CUTSCENE_LOOP },
    { cutscene_read_message_set_flag, 15 },
    { cutscene_read_message_end, 0 }
};
