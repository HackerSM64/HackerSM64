#include "camera/cutscene_helpers.h"
#include "camera/camera_math.h"
#include "engine/math_util.h"
#include "engine/surface_collision.h"
#include "game/camera.h"

/**
 * Cutscene that plays when Mario enters the cannon and it rises out of the hole.
 */

/**
 * End the cutscene, starting cannon mode.
 */
void cutscene_enter_cannon_end(struct Camera *c) {
    sStatusFlags &= ~CAM_FLAG_SMOOTH_MOVEMENT;
    sStatusFlags |= CAM_FLAG_BLOCK_SMOOTH_MOVEMENT;
    c->mode = CAMERA_MODE_INSIDE_CANNON;
    c->cutscene = CUTSCENE_NONE;
    sCannonYOffset = 800.f;
}

/**
 * Rotate around the cannon as it rises out of the hole.
 */
void cutscene_enter_cannon_raise(struct Camera *c) {
    struct Object *obj;
    f32 floorHeight;
    struct Surface *floor;
    Vec3f cannonFocus;
    Vec3s cannonAngle;

    // Shake the camera when the cannon is fully raised
    cutscene_event(cutscene_shake_explosion, c, 70, 70);
    sStatusFlags |= CAM_FLAG_SMOOTH_MOVEMENT;
    camera_approach_s16_symmetric_bool(&sCutsceneVars[1].angle[0], 0, 0x80);
    camera_approach_s16_symmetric_bool(&sCutsceneVars[2].angle[0], 0, 0x80);
    // Move the camera around the cannon, gradually rotating and moving closer
    vec3f_set_dist_and_angle(sCutsceneVars[0].point, c->pos, sCutsceneVars[1].point[2], sCutsceneVars[1].angle[0],
                             sCutsceneVars[1].angle[1]);
    sCutsceneVars[1].point[2] = approach_f32(sCutsceneVars[1].point[2], 400.f, 5.f, 5.f);
    sCutsceneVars[1].angle[1] += 0x40;
    sCutsceneVars[3].point[1] += 2.f;
    c->pos[1] += sCutsceneVars[3].point[1];

    if ((obj = sMarioCamState->usedObj) != NULL) {
        sCutsceneVars[0].point[1] = obj->oPosY;
        cannonAngle[0] = obj->oMoveAnglePitch;
        cannonAngle[1] = obj->oMoveAngleYaw;
        cannonAngle[2] = obj->oMoveAngleRoll;
        c->focus[0] = obj->oPosX;
        c->focus[1] = obj->oPosY;
        c->focus[2] = obj->oPosZ;
        cannonFocus[0] = 0.f;
        cannonFocus[1] = 100.f;
        cannonFocus[2] = 0.f;
        offset_rotated(c->focus, c->focus, cannonFocus, cannonAngle);
    }

    floorHeight = find_floor(c->pos[0], c->pos[1] + 500.f, c->pos[2], &floor) + 100.f;

    if (c->pos[1] < floorHeight) {
        c->pos[1] = floorHeight;
    }
}

/**
 * Start the cannon entering cutscene
 */
void cutscene_enter_cannon_start(struct Camera *c) {
    struct Object *obj;

    sStatusFlags |= CAM_FLAG_SMOOTH_MOVEMENT;
    sMarioCamState->cameraEvent = 0;

    // Store the cannon's position and angle in cvar0
    if ((obj = sMarioCamState->usedObj) != NULL) {
        sCutsceneVars[0].point[0] = obj->oPosX;
        sCutsceneVars[0].point[1] = obj->oPosY;
        sCutsceneVars[0].point[2] = obj->oPosZ;
        sCutsceneVars[0].angle[0] = obj->oMoveAnglePitch;
        sCutsceneVars[0].angle[1] = obj->oMoveAngleYaw;
        sCutsceneVars[0].angle[2] = obj->oMoveAngleRoll;
    }

    // Store the camera's polar offset from the cannon in cvar1
    vec3f_get_dist_and_angle(sCutsceneVars[0].point, c->pos, &sCutsceneVars[1].point[2],
                             &sCutsceneVars[1].angle[0], &sCutsceneVars[1].angle[1]);
    sCutsceneVars[3].point[1] = 0.f;
    //! cvar4 is unused in this cutscene
    sCutsceneVars[4].point[1] = 0.f;
}

struct Cutscene sCutsceneEnterCannon[] = {
    { cutscene_enter_cannon_start, 1 },
    { cutscene_enter_cannon_raise, 121 },
    { cutscene_enter_cannon_end, 0 }
};
