#include <ultra64.h>

#include "camera/camera_math.h"
#include "engine/math_util.h"
#include "game/camera.h"
#include "game/level_update.h"
#include "game/rendering_graph_node.h"

/**
 * Add a cyclic offset to the camera's field of view based on a cosine wave
 */
static void shake_camera_fov(struct GraphNodePerspective *perspective) {
    if (sFOVState.shakeAmplitude != 0.f) {
        sFOVState.fovOffset = coss(sFOVState.shakePhase) * sFOVState.shakeAmplitude / 0x100;
        sFOVState.shakePhase += sFOVState.shakeSpeed;
        camera_approach_f32_symmetric_bool(&sFOVState.shakeAmplitude, 0.f, sFOVState.decay);
        perspective->fov += sFOVState.fovOffset;
    } else {
        sFOVState.shakePhase = 0;
    }
}

static void set_fov_30(UNUSED struct MarioState *m) {
    sFOVState.fov = 30.f;
}

static void approach_fov_20(UNUSED struct MarioState *m) {
    camera_approach_f32_symmetric_bool(&sFOVState.fov, 20.f, 0.3f);
}

static void set_fov_45(UNUSED struct MarioState *m) {
    sFOVState.fov = 45.f;
}

static void set_fov_29(UNUSED struct MarioState *m) {
    sFOVState.fov = 29.f;
}

static void zoom_fov_30(UNUSED struct MarioState *m) {
    // Pretty sure approach_f32_asymptotic_bool would do a much nicer job here, but you do you,
    // Nintendo.
    camera_approach_f32_symmetric_bool(&sFOVState.fov, 30.f, (30.f - sFOVState.fov) / 60.f);
}

/**
 * This is the default fov function. It makes fov approach 45 degrees, and it handles zooming in when
 * Mario falls a sleep.
 */
static void fov_default(struct MarioState *m) {
    sStatusFlags &= ~CAM_FLAG_SLEEPING;

    if ((m->action == ACT_SLEEPING) || (m->action == ACT_START_SLEEPING)) {
        camera_approach_f32_symmetric_bool(&sFOVState.fov, 30.f, (30.f - sFOVState.fov) / 30.f);
        sStatusFlags |= CAM_FLAG_SLEEPING;
    } else {
        camera_approach_f32_symmetric_bool(&sFOVState.fov, 45.f, (45.f - sFOVState.fov) / 30.f);
        sFOVState.unusedIsSleeping = 0;
    }
    if (m->area->camera->cutscene == CUTSCENE_0F_UNUSED) {
        sFOVState.fov = 45.f;
    }
}

static void approach_fov_30(UNUSED struct MarioState *m) {
    camera_approach_f32_symmetric_bool(&sFOVState.fov, 30.f, 1.f);
}

static void approach_fov_60(UNUSED struct MarioState *m) {
    camera_approach_f32_symmetric_bool(&sFOVState.fov, 60.f, 1.f);
}

static void approach_fov_45(struct MarioState *m) {
    f32 targetFoV = sFOVState.fov;

    if (m->area->camera->mode == CAMERA_MODE_FIXED && m->area->camera->cutscene == 0) {
        targetFoV = 45.f;
    } else {
        targetFoV = 45.f;
    }

    sFOVState.fov = approach_f32(sFOVState.fov, targetFoV, 2.f, 2.f);
}

static void approach_fov_80(UNUSED struct MarioState *m) {
    camera_approach_f32_symmetric_bool(&sFOVState.fov, 80.f, 3.5f);
}

/**
 * Sets the fov in BBH.
 * If there's a cutscene, sets fov to 45. Otherwise sets fov to 60.
 */
static void set_fov_bbh(struct MarioState *m) {
    f32 targetFoV = sFOVState.fov;

    if (m->area->camera->mode == CAMERA_MODE_FIXED && m->area->camera->cutscene == 0) {
        targetFoV = 60.f;
    } else {
        targetFoV = 45.f;
    }

    sFOVState.fov = approach_f32(sFOVState.fov, targetFoV, 2.f, 2.f);
}

/**
 * Sets the field of view for the GraphNodeCamera
 */
Gfx *geo_camera_fov(s32 callContext, struct GraphNode *g, UNUSED void *context) {
    struct GraphNodePerspective *perspective = (struct GraphNodePerspective *) g;
    struct MarioState *marioState = &gMarioStates[0];
    u8 fovFunc = sFOVState.fovFunc;

    if (callContext == GEO_CONTEXT_RENDER) {
        switch (fovFunc) {
            case CAM_FOV_SET_45:
                set_fov_45(marioState);
                break;
            case CAM_FOV_SET_29:
                set_fov_29(marioState);
                break;
            case CAM_FOV_ZOOM_30:
                zoom_fov_30(marioState);
                break;
            case CAM_FOV_DEFAULT:
                fov_default(marioState);
                break;
            case CAM_FOV_BBH:
                set_fov_bbh(marioState);
                break;
            case CAM_FOV_APP_45:
                approach_fov_45(marioState);
                break;
            case CAM_FOV_SET_30:
                set_fov_30(marioState);
                break;
            case CAM_FOV_APP_20:
                approach_fov_20(marioState);
                break;
            case CAM_FOV_APP_80:
                approach_fov_80(marioState);
                break;
            case CAM_FOV_APP_30:
                approach_fov_30(marioState);
                break;
            case CAM_FOV_APP_60:
                approach_fov_60(marioState);
                break;
            default:
                set_fov_45(marioState);
                break;
        }
    }

    perspective->fov = sFOVState.fov;
    shake_camera_fov(perspective);
    return NULL;
}

/**
 * Allocate the GraphNodeCamera's config.camera, and copy `c`'s focus to the Camera's area center point.
 */
static void create_camera(struct GraphNodeCamera *gc, struct AllocOnlyPool *pool) {
#ifdef FORCED_CAMERA_MODE
    gc->config.mode = FORCED_CAMERA_MODE;
#endif
    s16 mode = gc->config.mode;
    struct Camera *c = alloc_only_pool_alloc(pool, sizeof(struct Camera));

    gc->config.camera = c;
    c->mode = mode;
    c->defMode = mode;
    c->cutscene = CUTSCENE_NONE;
    c->doorStatus = DOOR_DEFAULT;
    c->areaCenX = gc->focus[0];
    c->areaCenY = gc->focus[1];
    c->areaCenZ = gc->focus[2];
    c->yaw = 0;
    vec3f_copy(c->pos, gc->pos);
    vec3f_copy(c->focus, gc->focus);
}

/**
 * Zooms out the camera if paused and the level is 'outside', as determined by sZoomOutAreaMasks.
 *
 * Because gCurrLevelArea is assigned gCurrLevelNum * 16 + gCurrentArea->index,
 * dividing by 32 maps 2 levels to one index.
 *
 * areaBit definition:
 * (gCurrLevelArea & 0x10) / 4):
 *      This adds 4 to the shift if the level is an odd multiple of 16
 *
 * ((gCurrLevelArea & 0xF) - 1) & 3):
 *      This isolates the lower 16 'area' bits, subtracts 1 because areas are 1-indexed, and effectively
 *      modulo-4's the result, because each 8-bit mask only has 4 area bits for each level
 */
static void zoom_out_if_paused_and_outside(struct GraphNodeCamera *camera) {
    s16 yaw;
    s32 areaMaskIndex = gCurrLevelArea / 32;
    s32 areaBit = 1 << (((gCurrLevelArea & 0x10) / 4) + (((gCurrLevelArea & 0xF) - 1) & 3));

    if (areaMaskIndex >= LEVEL_MAX / 2) {
        areaMaskIndex = 0;
        areaBit = 0;
    }
    if (gCameraMovementFlags & CAM_MOVE_PAUSE_SCREEN) {
        if (sFramesPaused >= 2) {
            if (sZoomOutAreaMasks[areaMaskIndex] & areaBit) {

                camera->focus[0] = gCamera->areaCenX;
                camera->focus[1] = (sMarioCamState->pos[1] + gCamera->areaCenY) / 2;
                camera->focus[2] = gCamera->areaCenZ;
                vec3f_get_yaw(camera->focus, sMarioCamState->pos, &yaw);
                vec3f_set_dist_and_angle(sMarioCamState->pos, camera->pos, 6000.f, 0x1000, yaw);
#ifdef ENABLE_VANILLA_LEVEL_SPECIFIC_CHECKS
                if (gCurrLevelNum != LEVEL_THI) {
                    find_in_bounds_yaw_wdw_bob_thi(camera->pos, camera->focus, 0);
                }
#endif
            }
        } else {
            sFramesPaused++;
        }
    } else {
        sFramesPaused = 0;
    }
}

/**
 * Copy Lakitu's pos and foc into `gc`
 */
static void update_graph_node_camera(struct GraphNodeCamera *gc) {
    gc->rollScreen = gLakituState.roll;
    vec3f_copy(gc->pos, gLakituState.pos);
    vec3f_copy(gc->focus, gLakituState.focus);
    zoom_out_if_paused_and_outside(gc);
}

Gfx *geo_camera_main(s32 callContext, struct GraphNode *g, void *context) {
    struct GraphNodeCamera *gc = (struct GraphNodeCamera *) g;
    switch (callContext) {
        case GEO_CONTEXT_CREATE:
            create_camera(gc, context);
            break;
        case GEO_CONTEXT_RENDER:
            update_graph_node_camera(gc);
            break;
    }
    return NULL;
}
