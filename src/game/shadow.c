#include <PR/ultratypes.h>
#include <PR/gbi.h>

#include "engine/math_util.h"
#include "engine/surface_collision.h"
#include "behavior_data.h"
#include "geo_misc.h"
#include "level_table.h"
#include "memory.h"
#include "level_update.h"
#include "object_list_processor.h"
#include "rendering_graph_node.h"
#include "segment2.h"
#include "shadow.h"
#include "sm64.h"

/**
 * @file shadow.c
 * This file implements a self-contained subsystem used to draw shadows.
 */

/**
 * An array consisting of all the hardcoded rectangle shadows in the game.
 */
static ShadowRectangle sShadowRectangles[2] = {
    { 7.2f, 4.6f, TRUE }, // Spindel
    { 4.0f, 3.6f, TRUE }, // Whomp
};

struct Shadow gCurrShadow;
struct Shadow *s = &gCurrShadow;

/**
 * Shrink a shadow when its parent object is further from the floor, given the
 * initial size of the shadow and the current distance.
 */
f32 scale_shadow_with_distance(f32 initial, f32 distFromFloor) {
    if (distFromFloor <= 0.0f) {
        return initial;
    } else if (distFromFloor >= 600.0f) {
        return initial * 0.5f;
    } else {
        return initial * (1.0f - ((distFromFloor * 0.5f) / 600.0f));
    }
}

/**
 * Dim a shadow when its parent object is further from the ground.
 */
s32 dim_shadow_with_distance(u8 solidity, f32 distFromFloor) {
    if (solidity < 121) {
        return solidity;
    } else if (distFromFloor <= 0.0f) {
        return solidity;
    } else if (distFromFloor >= 600.0f) {
        return 120;
    } else {
        return (((120 - solidity) * distFromFloor) / 600.0f) + (f32) solidity;
    }
}

/**
 * Initialize a shadow. Return 0 on success, 1 on failure.
 *
 * @param pos Position of the parent object (not the shadow)
 * @param shadowScale Diameter of the shadow
 * @param overwriteSolidity Flag for whether the existing shadow solidity should
 *                          be dimmed based on its distance to the floor
 */
s32 init_shadow(f32 distToShadow, s16 shadowScale, s8 shadowType, u8 overwriteSolidity) {
    f32 baseScale;

    if (shadowType != SHADOW_SQUARE_PERMANENT) {
        // Set solidity and scale based on distance.

        if (overwriteSolidity) {
            s->solidity = dim_shadow_with_distance(overwriteSolidity, distToShadow);
        }

        baseScale = scale_shadow_with_distance(shadowScale, distToShadow);
    } else {
        s->solidity = overwriteSolidity;
        baseScale = shadowScale;
    }
    vec3f_set(s->scale, baseScale, baseScale, baseScale);

    return !(s->solidity);
}

/**
 * Linearly interpolate a shadow's solidity between zero and finalSolidity
 * depending on curr's relation to start and end.
 */
void linearly_interpolate_solidity_positive(u8 finalSolidity, s16 curr, s16 start,
                                            s16 end) {
    if (curr >= 0 && curr < start) {
        s->solidity = 0;
    } else if (end < curr) {
        s->solidity = finalSolidity;
    } else {
        s->solidity = (f32) finalSolidity * (curr - start) / (end - start);
    }
}

/**
 * Linearly interpolate a shadow's solidity between initialSolidity and zero
 * depending on curr's relation to start and end. Note that if curr < start,
 * the solidity will be zero.
 */
void linearly_interpolate_solidity_negative(u8 initialSolidity, s16 curr, s16 start,
                                            s16 end) {
    // The curr < start case is not handled. Thus, if start != 0, this function
    // will have the surprising behavior of hiding the shadow until start.
    // This is not necessarily a bug, since this function is only used once,
    // with start == 0.
    if (curr >= start && end >= curr) {
        s->solidity = ((f32) initialSolidity * (1.0f - (f32)(curr - start) / (end - start)));
    } else {
        s->solidity = 0;
    }
}

/**
 * Change a shadow's solidity based on the player's current animation frame.
 */
s32 correct_shadow_solidity_for_animations(u8 initialSolidity) {
    s16 animFrame = gMarioObject->header.gfx.animInfo.animFrame;
    switch (gMarioObject->header.gfx.animInfo.animID) {
        case MARIO_ANIM_IDLE_ON_LEDGE:
            return SHADOW_SOLIDITY_NO_SHADOW;
        case MARIO_ANIM_FAST_LEDGE_GRAB:
            linearly_interpolate_solidity_positive(initialSolidity, animFrame,  5, 14);
            return SHADOW_SOILDITY_ALREADY_SET;
        case MARIO_ANIM_SLOW_LEDGE_GRAB:
            linearly_interpolate_solidity_positive(initialSolidity, animFrame, 21, 33);
            return SHADOW_SOILDITY_ALREADY_SET;
        case MARIO_ANIM_CLIMB_DOWN_LEDGE:
            linearly_interpolate_solidity_negative(initialSolidity, animFrame,  0,  5);
            return SHADOW_SOILDITY_ALREADY_SET;
        default:
            return SHADOW_SOLIDITY_NOT_YET_SET;
    }
}

#ifdef ENABLE_VANILLA_LEVEL_SPECIFIC_CHECKS
/**
 * Slightly change the height of a shadow in levels with lava.
 */
void correct_lava_shadow_height(f32 *floorHeight) {
    if (gCurrLevelNum == LEVEL_BITFS) {
        if (*floorHeight < -3000.0f) {
            *floorHeight = -3062.0f;
            s->isDecal = FALSE;
        } else if (*floorHeight > 3400.0f) {
            *floorHeight = 3492.0f;
            s->isDecal = FALSE;
        }
    } else if (gCurrLevelNum == LEVEL_LLL
               && gCurrAreaIndex == 1) {
        *floorHeight = 5.0f;
        s->isDecal = FALSE;
    }
}
#endif

/**
 * Add a shadow to the given display list.
 * shadowType 0 uses a circle texture, the rest use a square texture.
 * Uses environment alpha for shadow solidity.
 */
static void add_shadow_to_display_list(Gfx *displayListHead) {
    gDPSetPrimColor(displayListHead++, 0, 0, 255, 255, 255, s->solidity);
    gSPDisplayList(displayListHead++, dl_shadow_tris);
    gSPEndDisplayList(displayListHead);
}

//! TODO:
//      - Breakout create_shadow_below_xyz into multiple functions
/**
 * Create a shadow at the absolute position given, with the given parameters.
 * Return a pointer to the display list representing the shadow.
 */
Gfx *create_shadow_below_xyz(Vec3f pos, s16 shadowScale, u8 shadowSolidity, s8 shadowType, s8 shifted) {
    struct Object *obj = gCurGraphNodeObjectNode;
    // Check if the object exists.
    if (obj == NULL) {
        return NULL;
    }

    // The floor underneath the object.
    struct Surface *floor = NULL;
    // The y-position of the floor (or water or lava) underneath the object.
    f32 floorHeight = FLOOR_LOWER_LIMIT_MISC;
    f32 x = pos[0];
    f32 y = pos[1];
    f32 z = pos[2];
    s8 isPlayer   = (obj == gMarioObject);
    s8 notHeldObj = (gCurGraphNodeHeldObject == NULL);

    // Attempt to use existing floors before finding a new one.
    if (notHeldObj && isPlayer && gMarioState->floor) {
        // The object is Mario and has a referenced floor.
        floor       = gMarioState->floor;
        floorHeight = gMarioState->floorHeight;
    } else if (notHeldObj && (gCurGraphNodeObject != &gMirrorMario) && obj->oFloor) {
        // The object is not Mario but has a referenced floor.
        //! Some objects only get their oFloor from bhv_init_room, which skips dynamic floors.
        floor       = obj->oFloor;
        floorHeight = obj->oFloorHeight;
    } else {
        // The object has no referenced floor, so find a new one.
        // gCollisionFlags |= COLLISION_FLAG_RETURN_FIRST;
        floorHeight = find_floor(x, y, z, &floor);

        // No shadow if the position is OOB.
        if (floor == NULL) {
            return NULL;
        }

        // Skip shifting the shadow height later, since the find_floor call above uses the already shifted position.
        shifted = FALSE;
    }

    // The shadow is a decal by default.
    s->isDecal = TRUE;

    // Check for water under the shadow.
    struct Surface *waterFloor = NULL;
    f32 waterLevel = find_water_level_and_floor(x, y, z, &waterFloor);

    // Whether the floor is an environment box rather than an actual surface.
    s32 isEnvBox = FALSE;

    if (waterLevel > FLOOR_LOWER_LIMIT_MISC
        && y >= waterLevel
        && floorHeight <= waterLevel) {
        // Skip shifting the shadow height later, since the find_water_level_and_floor call above uses the already shifted position.
        shifted = FALSE;

        // If there is water under the shadow, put the shadow on the water.
        floorHeight = waterLevel;

        // Don't use the decal layer, since water is transparent.
        s->isDecal = FALSE;

        // Check whether the water is an environment box or a water surface.
        if (waterFloor != NULL) { // Water surfaces:
            floor = waterFloor;
        } else { // Environment boxes:
            isEnvBox = TRUE;
        }
    } else { // Normal surfaces:
        TerrainData type = floor->type;
        if (type == SURFACE_ICE) {
            // Ice floors are usually transparent.
            s->isDecal = FALSE;
#ifdef ENABLE_VANILLA_LEVEL_SPECIFIC_CHECKS
        } else if (type == SURFACE_BURNING) {
            // Set the shadow height to the lava height in specific areas.
            correct_lava_shadow_height(&floorHeight);
#endif
        } else if (floor->object != NULL
                   && floor->object->behavior == segmented_to_virtual(bhvPlatformOnTrack)
                   && floor->object->oPlatformOnTrackType == PLATFORM_ON_TRACK_TYPE_CARPET) {
            // Raise the shadow 5 units so the shadow doesn't clip into the flying carpet.
            floorHeight += 5;
            // The flying carpet is transparent.
            s->isDecal = FALSE;
        }
    }

    f32 nx, ny, nz;
    if (isEnvBox) {
        // Assume the floor is flat.
        nx = 0.0f;
        ny = 1.0f;
        nz = 0.0f;
    } else {
        // Read the floor's normals.
        nx = floor->normal.x;
        ny = floor->normal.y;
        nz = floor->normal.z;

        // No shadow if the y-normal is negative (an unexpected result).
        if (ny <= 0.0f) {
            return NULL;
        }

        // If the animation changes the shadow position, move its height to the new position.
        if (shifted) {
            floorHeight = -((x * nx) + (z * nz) + floor->originOffset) / ny;
        }
    }

    // No shadow if the floor is lower than expected possible,
    if (floorHeight < FLOOR_LOWER_LIMIT_MISC) {
        return NULL;
    }

    // Get the vertical distance to the shadow, now that the final shadow height is set.
    f32 distToShadow = (y - floorHeight);

    // No shadow if the object is below it.
    if (distToShadow < -80.0f) {
        return NULL;
    }

    // No shadow if the non-Mario object is too high.
    if (!isPlayer && distToShadow > 1024.0f) {
        return NULL;
    }

    vec3f_set(s->floorNormal, nx, ny, nz);

    if (isPlayer) {
        // Set the shadow solidity manually for certain Mario animations.
        s32 solidityAction = correct_shadow_solidity_for_animations(shadowSolidity);
        switch (solidityAction) {
            case SHADOW_SOLIDITY_NO_SHADOW:
                return NULL;
            case SHADOW_SOILDITY_ALREADY_SET:
                if (init_shadow(distToShadow, shadowScale, shadowType, /* overwriteSolidity */ 0)) {
                    return NULL;
                }
                break;
            case SHADOW_SOLIDITY_NOT_YET_SET:
                if (init_shadow(distToShadow, shadowScale, shadowType, shadowSolidity)) {
                    return NULL;
                }
                break;
            default:
                return NULL;
        }
    } else {
        if (init_shadow(distToShadow, shadowScale, shadowType, shadowSolidity)) {
            return NULL;
        }

        // Get the scaling modifiers for rectangular shadows (Whomp and Spindel).
        if (shadowType >= SHADOW_RECTANGLE_HARDCODED_OFFSET) {
            s8 idx = shadowType - SHADOW_RECTANGLE_HARDCODED_OFFSET;
            s->scale[0] *= sShadowRectangles[idx].scaleX;
            s->scale[2] *= sShadowRectangles[idx].scaleZ;
            if (sShadowRectangles[idx].scaleWithDistance) {
                scale_shadow_with_distance(s->scale[0], distToShadow);
                scale_shadow_with_distance(s->scale[2], distToShadow);
            }
        }
    }

    Gfx *displayList = alloc_display_list(3 * sizeof(Gfx));

    if (displayList == NULL) {
        return NULL;
    }

    // Generate the shadow display list with type and solidity.
    add_shadow_to_display_list(displayList);

    // Move the shadow position to the floor height.
    pos[1] = floorHeight;

    return displayList;
}
