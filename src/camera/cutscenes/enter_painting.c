#include "camera/cutscene_helpers.h"
#include "camera/camera_math.h"
#include "engine/math_util.h"
#include "engine/surface_collision.h"
#include "game/camera.h"
#include "game/paintings.h"

/**
 * Cutscene that plays when Mario enters a course painting.
 */

/**
 * Plays when Mario enters a painting. The camera flies up to the painting's center, then it slowly
 * zooms in until the star select screen appears.
 */
void cutscene_enter_painting(struct Camera *c) {
    struct Surface *floor, *highFloor;
    Vec3f paintingPos, focus, focusOffset;
    Vec3s paintingAngle;
    f32 floorHeight;

    // Zoom in
    set_fov_function(CAM_FOV_APP_20);
    sStatusFlags |= CAM_FLAG_SMOOTH_MOVEMENT;

    if (gRipplingPainting != NULL) {
        paintingAngle[0] = 0;
        paintingAngle[1] = (s32)((gRipplingPainting->yaw / 360.f) * 65536.f); // convert degrees to IAU
        paintingAngle[2] = 0;

        focusOffset[0] = gRipplingPainting->size / 2;
        focusOffset[1] = focusOffset[0];
        focusOffset[2] = 0;

        paintingPos[0] = gRipplingPainting->posX;
        paintingPos[1] = gRipplingPainting->posY;
        paintingPos[2] = gRipplingPainting->posZ;

        offset_rotated(focus, paintingPos, focusOffset, paintingAngle);
        approach_vec3f_asymptotic(c->focus, focus, 0.1f, 0.1f, 0.1f);
        focusOffset[2] = -(((gRipplingPainting->size * 1000.f) / 2) / 307.f);
        offset_rotated(focus, paintingPos, focusOffset, paintingAngle);
        floorHeight = find_floor(focus[0], focus[1] + 500.f, focus[2], &highFloor) + 125.f;

        if (focus[1] < floorHeight) {
            focus[1] = floorHeight;
        }

        if (c->cutscene == CUTSCENE_ENTER_PAINTING) {
            approach_vec3f_asymptotic(c->pos, focus, 0.2f, 0.1f, 0.2f);
        } else {
            approach_vec3f_asymptotic(c->pos, focus, 0.9f, 0.9f, 0.9f);
        }

        find_floor(sMarioCamState->pos[0], sMarioCamState->pos[1] + 50.f, sMarioCamState->pos[2], &floor);

        if ((floor->type < SURFACE_PAINTING_WOBBLE_A6) || (floor->type > SURFACE_PAINTING_WARP_F9)) {
            c->cutscene = 0;
            gCutsceneTimer = CUTSCENE_STOP;
            sStatusFlags |= CAM_FLAG_SMOOTH_MOVEMENT;
        }
    }
    c->mode = CAMERA_MODE_CLOSE;
}

struct Cutscene sCutsceneEnterPainting[] = {
    { cutscene_enter_painting, CUTSCENE_LOOP }
};
