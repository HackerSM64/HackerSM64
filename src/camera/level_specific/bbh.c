#include "engine/math_util.h"
#include "camera/camera_modes.h"
#include "game/camera.h"

void cam_bbh_room_6(struct Camera *c) {
    /**
     * Table that dictates camera movement in bookend room.
     * Due to only the X being varied in the table, this only moves along the X axis linearly.
     * Third entry is seemingly unused.
     */
    static struct ParallelTrackingPoint sBBHLibraryParTrackPath[] = {
        { 1, { -929.0f, 1619.0f, -1490.0f }, 50.0f, 0.0f },
        { 0, { -2118.0f, 1619.0f, -1490.0f }, 50.0f, 0.0f },
        { 0, { 0.0f, 0.0f, 0.0f }, 0.0f, 0.0f },
    };

    parallel_tracking_init(c, sBBHLibraryParTrackPath);
}

void cam_bbh_fall_off_roof(struct Camera *c) {
    set_camera_mode_close_cam(&c->mode);
}

void cam_bbh_fall_into_pool(struct Camera *c) {
    Vec3f dir;
    set_camera_mode_close_cam(&c->mode);
    vec3f_set(dir, 0.f, 0.f, 300.f);
    offset_rotated(gLakituState.goalPos, sMarioCamState->pos, dir, sMarioCamState->faceAngle);
    gLakituState.goalPos[1] = -2300.f;
    vec3f_copy(c->pos, gLakituState.goalPos);
    sStatusFlags &= ~CAM_FLAG_SMOOTH_MOVEMENT;
}

void cam_bbh_room_1(struct Camera *c) {
    set_camera_mode_fixed(c, 956, 440, 1994);
}

void cam_bbh_leave_front_door(struct Camera *c) {
    c->doorStatus = DOOR_LEAVING_SPECIAL;
    cam_bbh_room_1(c);
}

void cam_bbh_room_2_lower(struct Camera *c) {
    set_camera_mode_fixed(c, 2591, 400, 1284);
}

void cam_bbh_room_4(struct Camera *c) {
    set_camera_mode_fixed(c, 3529, 340, -1384);
}

void cam_bbh_room_8(struct Camera *c) {
    set_camera_mode_fixed(c, -500, 740, -1306);
}

/**
 * In BBH's room 5's library (the first floor room with the vanish cap/boo painting)
 * set the camera mode to fixed and position to (-2172, 200, 675)
 */
void cam_bbh_room_5_library(struct Camera *c) {
    set_camera_mode_fixed(c, -2172, 200, 675);
}

/**
 * In BBH's room 5 (the first floor room with the vanish cap/boo painting)
 * set the camera mode to to the hidden room's position
 * if coming from the library.
 */
void cam_bbh_room_5_library_to_hidden_transition(struct Camera *c) {
    if (set_camera_mode_fixed(c, -2172, 200, 675) == 1) {
        transition_next_state(c, 20);
    }
}

void cam_bbh_room_5_hidden_to_library_transition(struct Camera *c) {
    if (set_camera_mode_fixed(c, -1542, 320, -307) == 1) {
        transition_next_state(c, 20);
    }
}

void cam_bbh_room_5_hidden(struct Camera *c) {
    c->doorStatus = DOOR_LEAVING_SPECIAL;
    set_camera_mode_fixed(c, -1542, 320, -307);
}

void cam_bbh_room_3(struct Camera *c) {
    set_camera_mode_fixed(c, -1893, 320, 2327);
}

void cam_bbh_room_7_mr_i(struct Camera *c) {
    set_camera_mode_fixed(c, 1371, 360, -1302);
}

void cam_bbh_room_7_mr_i_to_coffins_transition(struct Camera *c) {
    if (set_camera_mode_fixed(c, 1371, 360, -1302) == 1) {
        transition_next_state(c, 20);
    }
}

void cam_bbh_room_7_coffins_to_mr_i_transition(struct Camera *c) {
    if (set_camera_mode_fixed(c, 2115, 260, -772) == 1) {
        transition_next_state(c, 20);
    }
}

void cam_bbh_elevator_room_lower(struct Camera *c) {
    c->doorStatus = DOOR_LEAVING_SPECIAL;
    set_camera_mode_close_cam(&c->mode);
}

void cam_bbh_room_0_back_entrance(struct Camera *c) {
    set_camera_mode_close_cam(&c->mode);
}

void cam_bbh_elevator(struct Camera *c) {
    if (c->mode == CAMERA_MODE_FIXED) {
        set_camera_mode_close_cam(&c->mode);
        c->pos[1] = -405.f;
        gLakituState.goalPos[1] = -405.f;
    }
}

void cam_bbh_room_12_upper(struct Camera *c) {
    c->doorStatus = DOOR_LEAVING_SPECIAL;
    set_camera_mode_fixed(c, -2932, 296, 4429);
}

void cam_bbh_enter_front_door(struct Camera *c) {
    set_camera_mode_close_cam(&c->mode);
}

void cam_bbh_room_2_library(struct Camera *c) {
    set_camera_mode_fixed(c, 3493, 440, 617);
}

void cam_bbh_room_2_library_to_trapdoor_transition(struct Camera *c) {
    if (set_camera_mode_fixed(c, 3493, 440, 617) == 1) {
        transition_next_state(c, 20);
    }
}

void cam_bbh_room_2_trapdoor(struct Camera *c) {
    set_camera_mode_fixed(c, 3502, 440, 1217);
}

void cam_bbh_room_2_trapdoor_transition(struct Camera *c) {
    if (set_camera_mode_fixed(c, 3502, 440, 1217) == 1) {
        transition_next_state(c, 20);
    }
}

void cam_bbh_room_9_attic(struct Camera *c) {
    set_camera_mode_fixed(c, -670, 460, 372);
}

void cam_bbh_room_9_attic_transition(struct Camera *c) {
    if (set_camera_mode_fixed(c, -670, 460, 372) == 1) {
        transition_next_state(c, 20);
    }
}

void cam_bbh_room_9_mr_i_transition(struct Camera *c) {
    if (set_camera_mode_fixed(c, 131, 380, -263) == 1) {
        transition_next_state(c, 20);
    }
}

void cam_bbh_room_13_balcony(struct Camera *c) {
    set_camera_mode_fixed(c, 210, 420, 3109);
}

void cam_bbh_room_0(struct Camera *c) {
    c->doorStatus = DOOR_LEAVING_SPECIAL;
    set_camera_mode_fixed(c, -204, 807, 204);
}
