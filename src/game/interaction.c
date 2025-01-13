#include <PR/ultratypes.h>

#include "area.h"
#include "actors/common1.h"
#include "audio/external.h"
#include "behavior_actions.h"
#include "behavior_data.h"
#include "camera.h"
#include "course_table.h"
#include "dialog_ids.h"
#include "engine/math_util.h"
#include "engine/surface_collision.h"
#include "game_init.h"
#include "interaction.h"
#include "level_update.h"
#include "mario.h"
#include "mario_step.h"
#include "memory.h"
#include "obj_behaviors.h"
#include "object_helpers.h"
#include "save_file.h"
#include "seq_ids.h"
#include "sm64.h"
#include "sound_init.h"
#include "rumble_init.h"
#include "config.h"

u8  sDelayInvincTimer;
s16 sInvulnerable;
u32 interact_coin          (struct MarioState *m, u32 interactType, struct Object *obj);
u32 interact_water_ring    (struct MarioState *m, u32 interactType, struct Object *obj);
u32 interact_star_or_key   (struct MarioState *m, u32 interactType, struct Object *obj);
u32 interact_bbh_entrance  (struct MarioState *m, u32 interactType, struct Object *obj);
u32 interact_warp          (struct MarioState *m, u32 interactType, struct Object *obj);
u32 interact_warp_door     (struct MarioState *m, u32 interactType, struct Object *obj);
u32 interact_door          (struct MarioState *m, u32 interactType, struct Object *obj);
u32 interact_cannon_base   (struct MarioState *m, u32 interactType, struct Object *obj);
u32 interact_igloo_barrier (struct MarioState *m, u32 interactType, struct Object *obj);
u32 interact_tornado       (struct MarioState *m, u32 interactType, struct Object *obj);
u32 interact_whirlpool     (struct MarioState *m, u32 interactType, struct Object *obj);
u32 interact_strong_wind   (struct MarioState *m, u32 interactType, struct Object *obj);
u32 interact_flame         (struct MarioState *m, u32 interactType, struct Object *obj);
u32 interact_snufit_bullet (struct MarioState *m, u32 interactType, struct Object *obj);
u32 interact_clam_or_bubba (struct MarioState *m, u32 interactType, struct Object *obj);
u32 interact_bully         (struct MarioState *m, u32 interactType, struct Object *obj);
u32 interact_shock         (struct MarioState *m, u32 interactType, struct Object *obj);
u32 interact_mr_blizzard   (struct MarioState *m, u32 interactType, struct Object *obj);
u32 interact_hit_from_below(struct MarioState *m, u32 interactType, struct Object *obj);
u32 interact_bounce_top    (struct MarioState *m, u32 interactType, struct Object *obj);
u32 interact_spiny_walking (struct MarioState *m, u32 interactType, struct Object *obj);
u32 interact_damage        (struct MarioState *m, u32 interactType, struct Object *obj);
u32 interact_breakable     (struct MarioState *m, u32 interactType, struct Object *obj);
u32 interact_koopa_shell   (struct MarioState *m, u32 interactType, struct Object *obj);
u32 interact_pole          (struct MarioState *m, u32 interactType, struct Object *obj);
u32 interact_hoot          (struct MarioState *m, u32 interactType, struct Object *obj);
u32 interact_cap           (struct MarioState *m, u32 interactType, struct Object *obj);
u32 interact_grabbable     (struct MarioState *m, u32 interactType, struct Object *obj);
u32 interact_text          (struct MarioState *m, u32 interactType, struct Object *obj);

struct InteractionHandler {
    u32 interactType;
    u32 (*handler)(struct MarioState *m, u32 interactType, struct Object *obj);
};

static struct InteractionHandler sInteractionHandlers[] = {
    { INTERACT_COIN,           interact_coin },
    { INTERACT_WATER_RING,     interact_water_ring },
    { INTERACT_STAR_OR_KEY,    interact_star_or_key },
    { INTERACT_BBH_ENTRANCE,   interact_bbh_entrance },
    { INTERACT_WARP,           interact_warp },
    { INTERACT_WARP_DOOR,      interact_warp_door },
    { INTERACT_DOOR,           interact_door },
    { INTERACT_CANNON_BASE,    interact_cannon_base },
    { INTERACT_IGLOO_BARRIER,  interact_igloo_barrier },
    { INTERACT_TORNADO,        interact_tornado },
    { INTERACT_WHIRLPOOL,      interact_whirlpool },
    { INTERACT_STRONG_WIND,    interact_strong_wind },
    { INTERACT_FLAME,          interact_flame },
    { INTERACT_SNUFIT_BULLET,  interact_snufit_bullet },
    { INTERACT_CLAM_OR_BUBBA,  interact_clam_or_bubba },
    { INTERACT_BULLY,          interact_bully },
    { INTERACT_SHOCK,          interact_shock },
    { INTERACT_BOUNCE_TOP2,    interact_bounce_top },
    { INTERACT_MR_BLIZZARD,    interact_mr_blizzard },
    { INTERACT_HIT_FROM_BELOW, interact_hit_from_below },
    { INTERACT_BOUNCE_TOP,     interact_bounce_top },
    { INTERACT_DAMAGE,         interact_damage },
    { INTERACT_POLE,           interact_pole },
    { INTERACT_HOOT,           interact_hoot },
    { INTERACT_BREAKABLE,      interact_breakable },
    { INTERACT_KOOPA,          interact_bounce_top },
    { INTERACT_KOOPA_SHELL,    interact_koopa_shell },
    { INTERACT_SPINY_WALKING,  interact_spiny_walking },
    { INTERACT_CAP,            interact_cap },
    { INTERACT_GRABBABLE,      interact_grabbable },
    { INTERACT_TEXT,           interact_text },
};

static u32 sForwardKnockbackActions[][3] = {
//    Soft                        Normal                 Hard
    { ACT_SOFT_FORWARD_GROUND_KB, ACT_FORWARD_GROUND_KB, ACT_HARD_FORWARD_GROUND_KB }, // Ground
    { ACT_FORWARD_AIR_KB,         ACT_FORWARD_AIR_KB,    ACT_HARD_FORWARD_AIR_KB    }, // Air
    { ACT_FORWARD_WATER_KB,       ACT_FORWARD_WATER_KB,  ACT_FORWARD_WATER_KB       }, // Water
};

static u32 sBackwardKnockbackActions[][3] = {
//    Soft                         Normal                  Hard
    { ACT_SOFT_BACKWARD_GROUND_KB, ACT_BACKWARD_GROUND_KB, ACT_HARD_BACKWARD_GROUND_KB }, // Ground
    { ACT_BACKWARD_AIR_KB,         ACT_BACKWARD_AIR_KB,    ACT_HARD_BACKWARD_AIR_KB    }, // Air
    { ACT_BACKWARD_WATER_KB,       ACT_BACKWARD_WATER_KB,  ACT_BACKWARD_WATER_KB       }, // Water
};

static u8 sDisplayingDoorText = FALSE;
static u8 sJustTeleported = FALSE;
static u8 sPssSlideStarted = FALSE;

/**
 * Returns the type of cap Mario is wearing.
 */
u32 get_mario_cap_flag(struct Object *capObject) {
    const BehaviorScript *script = virtual_to_segmented(SEGMENT_BEHAVIOR_DATA, capObject->behavior);

    if (script == bhvNormalCap) {
        return MARIO_NORMAL_CAP;
    } else if (script == bhvMetalCap) {
        return MARIO_METAL_CAP;
    } else if (script == bhvWingCap) {
        return MARIO_WING_CAP;
    } else if (script == bhvVanishCap) {
        return MARIO_VANISH_CAP;
    }

    return 0;
}

/**
 * Returns true if the passed in object has a moving angle yaw
 * in the angular range given towards Mario.
 */
u32 object_facing_mario(struct MarioState *m, struct Object *obj, s16 angleRange) {
    f32 dx = m->pos[0] - obj->oPosX;
    f32 dz = m->pos[2] - obj->oPosZ;

    s16 angleToMario = atan2s(dz, dx);
    s16 dAngle = angleToMario - obj->oMoveAngleYaw;

    if (-angleRange <= dAngle && dAngle <= angleRange) {
        return TRUE;
    }

    return FALSE;
}

s16 mario_obj_angle_to_object(struct MarioState *m, struct Object *obj) {
    f32 dx = obj->oPosX - m->pos[0];
    f32 dz = obj->oPosZ - m->pos[2];

    return atan2s(dz, dx);
}

/**
 * Determines Mario's interaction with a given object depending on their proximity,
 * action, speed, and position.
 */
u32 determine_interaction(struct MarioState *m, struct Object *obj) {
    u32 interaction = 0;
    u32 action = m->action;

    if (action & ACT_FLAG_ATTACKING) {
        if (action == ACT_PUNCHING || action == ACT_MOVE_PUNCHING || action == ACT_JUMP_KICK) {
            s16 dYawToObject = mario_obj_angle_to_object(m, obj) - m->faceAngle[1];

            if (m->flags & MARIO_PUNCHING) {
                // 120 degrees total, or 60 each way
                if (-0x2AAA <= dYawToObject && dYawToObject <= 0x2AAA) {
                    interaction = INT_PUNCH;
                }
            }
            if (m->flags & MARIO_KICKING) {
                // 120 degrees total, or 60 each way
                if (-0x2AAA <= dYawToObject && dYawToObject <= 0x2AAA) {
                    interaction = INT_KICK;
                }
            }
            if (m->flags & MARIO_TRIPPING) {
                // 180 degrees total, or 90 each way
                if (-0x4000 <= dYawToObject && dYawToObject <= 0x4000) {
                    interaction = INT_TRIP;
                }
            }
        } else if (action == ACT_GROUND_POUND || action == ACT_TWIRLING) {
            if (m->vel[1] < 0.0f) {
                interaction = INT_GROUND_POUND_OR_TWIRL;
            }
        } else if (action == ACT_GROUND_POUND_LAND || action == ACT_TWIRL_LAND) {
            // Neither ground pounding nor twirling change Mario's vertical speed on landing.,
            // so the speed check is nearly always true (perhaps not if you land while going upwards?)
            // Additionally, actionState it set on each first thing in their action, so this is
            // only true prior to the very first frame (i.e. active 1 frame prior to it run).
            if (m->vel[1] < 0.0f && m->actionState == 0) {
                interaction = INT_GROUND_POUND_OR_TWIRL;
            }
        } else if (action == ACT_SLIDE_KICK || action == ACT_SLIDE_KICK_SLIDE) {
            interaction = INT_SLIDE_KICK;
        } else if (action & ACT_FLAG_RIDING_SHELL) {
            interaction = INT_FAST_ATTACK_OR_SHELL;
        } else if (m->forwardVel <= -26.0f || 26.0f <= m->forwardVel) {
            interaction = INT_FAST_ATTACK_OR_SHELL;
        }
    }

    // Prior to this, the interaction type could be overwritten. This requires, however,
    // that the interaction not be set prior. This specifically overrides turning a ground
    // pound into just a bounce.
    if ((interaction == INT_NONE) && (action & ACT_FLAG_AIR)) {
        if (m->vel[1] < 0.0f) {
            if (m->pos[1] > obj->oPosY) {
                interaction = INT_HIT_FROM_ABOVE;
            }
        } else {
            if (m->pos[1] < obj->oPosY) {
                interaction = INT_HIT_FROM_BELOW;
            }
        }
    }

    return interaction;
}

/**
 * Sets the interaction types for INT_STATUS_INTERACTED, INT_STATUS_WAS_ATTACKED
 */
u32 attack_object(struct Object *obj, s32 interaction) {
    u32 attackType = ATTACK_NONE;

    switch (interaction) {
        case INT_GROUND_POUND_OR_TWIRL:
            attackType = ATTACK_GROUND_POUND_OR_TWIRL;
            break;
        case INT_PUNCH:
            attackType = ATTACK_PUNCH;
            break;
        case INT_KICK:
        case INT_TRIP:
            attackType = ATTACK_KICK_OR_TRIP;
            break;
        case INT_SLIDE_KICK:
        case INT_FAST_ATTACK_OR_SHELL:
            attackType = ATTACK_FAST_ATTACK;
            break;
        case INT_HIT_FROM_ABOVE:
            attackType = ATTACK_FROM_ABOVE;
            break;
        case INT_HIT_FROM_BELOW:
            attackType = ATTACK_FROM_BELOW;
            break;
    }

    obj->oInteractStatus = attackType + (INT_STATUS_INTERACTED | INT_STATUS_WAS_ATTACKED);
    return attackType;
}

void mario_stop_riding_object(struct MarioState *m) {
    if (m->riddenObj != NULL) {
        m->riddenObj->oInteractStatus = INT_STATUS_STOP_RIDING;
        stop_shell_music();
        m->riddenObj = NULL;
    }
}

void mario_grab_used_object(struct MarioState *m) {
    if (m->heldObj == NULL) {
        m->heldObj = m->usedObj;
        obj_set_held_state(m->heldObj, bhvCarrySomethingHeld);
    }
}

void mario_drop_held_object(struct MarioState *m) {
    if (m->heldObj != NULL) {
        if (m->heldObj->behavior == segmented_to_virtual(bhvKoopaShellUnderwater)) {
            stop_shell_music();
        }

        obj_set_held_state(m->heldObj, bhvCarrySomethingDropped);

        // ! When dropping an object instead of throwing it, it will be put at Mario's
        // y-positon instead of the HOLP's y-position. This fact is often exploited when
        // cloning objects.
        m->heldObj->oPosX = m->marioBodyState->heldObjLastPosition[0];
        // m->heldObj->oPosY = m->marioBodyState->heldObjLastPosition[1];
        m->heldObj->oPosY = m->pos[1];
        m->heldObj->oPosZ = m->marioBodyState->heldObjLastPosition[2];

        m->heldObj->oMoveAngleYaw = m->faceAngle[1];

        m->heldObj = NULL;
    }
}

void mario_throw_held_object(struct MarioState *m) {
    if (m->heldObj != NULL) {
        if (m->heldObj->behavior == segmented_to_virtual(bhvKoopaShellUnderwater)) {
            stop_shell_music();
        }

        obj_set_held_state(m->heldObj, bhvCarrySomethingThrown);

        m->heldObj->oPosX = m->marioBodyState->heldObjLastPosition[0] + 32.0f * sins(m->faceAngle[1]);
        m->heldObj->oPosY = m->marioBodyState->heldObjLastPosition[1];
        m->heldObj->oPosZ = m->marioBodyState->heldObjLastPosition[2] + 32.0f * coss(m->faceAngle[1]);

        m->heldObj->oMoveAngleYaw = m->faceAngle[1];

        m->heldObj = NULL;
    }
}

void mario_stop_riding_and_holding(struct MarioState *m) {
    mario_drop_held_object(m);
    mario_stop_riding_object(m);

    if (m->action == ACT_RIDING_HOOT) {
        m->usedObj->oInteractStatus = FALSE;
        m->usedObj->oHootMarioReleaseTime = gGlobalTimer;
    }
}

u32 does_mario_have_normal_cap_on_head(struct MarioState *m) {
    return (m->flags & (MARIO_CAPS | MARIO_CAP_ON_HEAD)) == (MARIO_NORMAL_CAP | MARIO_CAP_ON_HEAD);
}

#ifdef PREVENT_CAP_LOSS
UNUSED void mario_blow_off_cap(UNUSED struct MarioState *m, UNUSED f32 capSpeed) {
}

Bool32 mario_lose_cap_to_enemy(UNUSED u32 enemyType) {
    return FALSE;
}
#else
void mario_blow_off_cap(struct MarioState *m, f32 capSpeed) {
    struct Object *capObject;

    if (does_mario_have_normal_cap_on_head(m)) {
        save_file_set_cap_pos(m->pos[0], m->pos[1], m->pos[2]);

        m->flags &= ~(MARIO_NORMAL_CAP | MARIO_CAP_ON_HEAD);

        capObject = spawn_object(m->marioObj, MODEL_MARIOS_CAP, bhvNormalCap);

        capObject->oPosY += (m->action & ACT_FLAG_SHORT_HITBOX) ? 120.0f : 180.0f;
        capObject->oForwardVel = capSpeed;
        capObject->oMoveAngleYaw = (s16)(m->faceAngle[1] + 0x400);

        if (m->forwardVel < 0.0f) {
            capObject->oMoveAngleYaw = (s16)(capObject->oMoveAngleYaw + 0x8000);
        }
    }
}

Bool32 mario_lose_cap_to_enemy(u32 enemyType) {
    if (does_mario_have_normal_cap_on_head(gMarioState)) {
        save_file_set_flags(enemyType == 1 ? SAVE_FLAG_CAP_ON_KLEPTO : SAVE_FLAG_CAP_ON_UKIKI);
        gMarioState->flags &= ~(MARIO_NORMAL_CAP | MARIO_CAP_ON_HEAD);
        return TRUE;
    }

    return FALSE;
}
#endif

void mario_retrieve_cap(void) {
    mario_drop_held_object(gMarioState);
    save_file_clear_flags(SAVE_FLAG_CAP_ON_KLEPTO | SAVE_FLAG_CAP_ON_UKIKI);
    gMarioState->flags &= ~MARIO_CAP_ON_HEAD;
    gMarioState->flags |= MARIO_NORMAL_CAP | MARIO_CAP_IN_HAND;
}

u32 able_to_grab_object(struct MarioState *m, UNUSED struct Object *obj) {
    u32 action = m->action;

    if (action == ACT_DIVE_SLIDE || action == ACT_DIVE) {
        if (!(obj->oInteractionSubtype & INT_SUBTYPE_GRABS_MARIO)) {
            return TRUE;
        }
    } else if (action == ACT_PUNCHING || action == ACT_MOVE_PUNCHING) {
        if (m->actionArg < 2) {
            return TRUE;
        }
    }

    return FALSE;
}

struct Object *mario_get_collided_object(struct MarioState *m, u32 interactType) {
    s32 i;
    struct Object *object;

    for (i = 0; i < m->marioObj->numCollidedObjs; i++) {
        object = m->marioObj->collidedObjs[i];

        if (object->oInteractType == interactType) {
            return object;
        }
    }

    return NULL;
}

u32 mario_check_object_grab(struct MarioState *m) {
    u32 result = FALSE;
    const BehaviorScript *script;

    if (m->input & INPUT_INTERACT_OBJ_GRABBABLE) {
        script = virtual_to_segmented(SEGMENT_BEHAVIOR_DATA, m->interactObj->behavior);

        if (script == bhvBowser) {
            s16 facingDYaw = m->faceAngle[1] - m->interactObj->oMoveAngleYaw;
            if (facingDYaw >= -0x5555 && facingDYaw <= 0x5555) {
                m->faceAngle[1] = m->interactObj->oMoveAngleYaw;
                m->usedObj = m->interactObj;
                m->pos[1] = m->floorHeight;
                m->vel[1] = 0.0f;
                result = set_mario_action(m, ACT_PICKING_UP_BOWSER, 0);
            }
        } else {
            s16 facingDYaw = mario_obj_angle_to_object(m, m->interactObj) - m->faceAngle[1];
            if (facingDYaw >= -0x2AAA && facingDYaw <= 0x2AAA) {
                m->usedObj = m->interactObj;

                if (!(m->action & ACT_FLAG_AIR)) {
                    set_mario_action(
                        m, (m->action & ACT_FLAG_DIVING) ? ACT_DIVE_PICKING_UP : ACT_PICKING_UP, 0);
                }

                result = TRUE;
            }
        }
    }

    return result;
}

u32 bully_knock_back_mario(struct MarioState *mario) {
    struct BullyCollisionData marioData;
    struct BullyCollisionData bullyData;
    s16 newMarioYaw;
    s16 newBullyYaw;
    s16 marioDYaw;

    u32 bonkAction = 0;

    struct Object *bully = mario->interactObj;

    //! Conversion ratios multiply to more than 1 (could allow unbounded speed
    // with bonk cancel - but this isn't important for regular bully battery)
    f32 bullyToMarioRatio = bully->hitboxRadius * 3 / 53;
    f32 marioToBullyRatio = 53.0f / bully->hitboxRadius;

    init_bully_collision_data(&marioData, mario->pos[0], mario->pos[2], mario->forwardVel,
                              mario->faceAngle[1], bullyToMarioRatio, 52.0f);

    init_bully_collision_data(&bullyData, bully->oPosX, bully->oPosZ, bully->oForwardVel,
                              bully->oMoveAngleYaw, marioToBullyRatio, bully->hitboxRadius + 2.0f);

    if (mario->forwardVel != 0.0f) {
        transfer_bully_speed(&marioData, &bullyData);
    } else {
        transfer_bully_speed(&bullyData, &marioData);
    }

    newMarioYaw = atan2s(marioData.velZ, marioData.velX);
    newBullyYaw = atan2s(bullyData.velZ, bullyData.velX);

    marioDYaw = newMarioYaw - mario->faceAngle[1];

    mario->faceAngle[1] = newMarioYaw;
    mario->forwardVel = sqrtf(sqr(marioData.velX) + sqr(marioData.velZ));
    mario->pos[0] = marioData.posX;
    mario->pos[2] = marioData.posZ;

    bully->oMoveAngleYaw = newBullyYaw;
    bully->oForwardVel = sqrtf(sqr(bullyData.velX) + sqr(bullyData.velZ));
    bully->oPosX = bullyData.posX;
    bully->oPosZ = bullyData.posZ;

    if (marioDYaw < -0x4000 || marioDYaw > 0x4000) {
        mario->faceAngle[1] += 0x8000;
        mario->forwardVel *= -1.0f;

        if (mario->action & ACT_FLAG_AIR) {
            bonkAction = ACT_BACKWARD_AIR_KB;
        } else {
            bonkAction = ACT_SOFT_BACKWARD_GROUND_KB;
        }
    } else {
        if (mario->action & ACT_FLAG_AIR) {
            bonkAction = ACT_FORWARD_AIR_KB;
        } else {
            bonkAction = ACT_SOFT_FORWARD_GROUND_KB;
        }
    }

    return bonkAction;
}

void bounce_off_object(struct MarioState *m, struct Object *obj, f32 velY) {
    m->pos[1] = obj->oPosY + obj->hitboxHeight;
    m->vel[1] = velY;

    m->flags &= ~MARIO_JUMPING;

    play_sound(SOUND_ACTION_BOUNCE_OFF_OBJECT, m->marioObj->header.gfx.cameraToObject);
}

void hit_object_from_below(struct MarioState *m, UNUSED struct Object *obj) {
    m->vel[1] = 0.0f;
    set_camera_shake_from_hit(SHAKE_HIT_FROM_BELOW);
}

UNUSED static u32 unused_determine_knockback_action(struct MarioState *m) {
    u32 bonkAction;
    s16 angleToObject = mario_obj_angle_to_object(m, m->interactObj);
    s16 facingDYaw = angleToObject - m->faceAngle[1];

    if (m->forwardVel < 16.0f) {
        m->forwardVel = 16.0f;
    }

    m->faceAngle[1] = angleToObject;

    if (facingDYaw >= -0x4000 && facingDYaw <= 0x4000) {
        m->forwardVel *= -1.0f;
        if (m->action & (ACT_FLAG_AIR | ACT_FLAG_ON_POLE | ACT_FLAG_HANGING)) {
            bonkAction = ACT_BACKWARD_AIR_KB;
        } else {
            bonkAction = ACT_SOFT_BACKWARD_GROUND_KB;
        }
    } else {
        m->faceAngle[1] += 0x8000;
        if (m->action & (ACT_FLAG_AIR | ACT_FLAG_ON_POLE | ACT_FLAG_HANGING)) {
            bonkAction = ACT_FORWARD_AIR_KB;
        } else {
            bonkAction = ACT_SOFT_FORWARD_GROUND_KB;
        }
    }

    return bonkAction;
}

u32 determine_knockback_action(struct MarioState *m, UNUSED s32 arg) {
    u32 bonkAction;

    s16 terrainIndex = 0; // 1 = air, 2 = water, 0 = default
    s16 strengthIndex = 0;

    s16 angleToObject = mario_obj_angle_to_object(m, m->interactObj);
    s16 facingDYaw = angleToObject - m->faceAngle[1];
    s16 remainingHealth = m->health - 0x40 * m->hurtCounter;

    if (m->action & (ACT_FLAG_SWIMMING | ACT_FLAG_METAL_WATER)) {
        terrainIndex = 2;
    } else if (m->action & (ACT_FLAG_AIR | ACT_FLAG_ON_POLE | ACT_FLAG_HANGING)) {
        terrainIndex = 1;
    }

    if (remainingHealth < 0x100) {
        strengthIndex = 2;
    } else if (m->interactObj->oDamageOrCoinValue >= 4) {
        strengthIndex = 2;
    } else if (m->interactObj->oDamageOrCoinValue >= 2) {
        strengthIndex = 1;
    }

    m->faceAngle[1] = angleToObject;

    if (terrainIndex == 2) {
        if (m->forwardVel < 28.0f) {
            mario_set_forward_vel(m, 28.0f);
        }

        if (m->pos[1] >= m->interactObj->oPosY) {
            if (m->vel[1] < 20.0f) {
                m->vel[1] = 20.0f;
            }
        } else {
            if (m->vel[1] > 0.0f) {
                m->vel[1] = 0.0f;
            }
        }
    } else {
        if (m->forwardVel < 16.0f) {
            mario_set_forward_vel(m, 16.0f);
        }
    }

    if (-0x4000 <= facingDYaw && facingDYaw <= 0x4000) {
        m->forwardVel *= -1.0f;
        bonkAction = sBackwardKnockbackActions[terrainIndex][strengthIndex];
    } else {
        m->faceAngle[1] += 0x8000;
        bonkAction = sForwardKnockbackActions[terrainIndex][strengthIndex];
    }

    return bonkAction;
}

void push_mario_out_of_object(struct MarioState *m, struct Object *obj, f32 padding) {
    f32 minDistance = obj->hitboxRadius + m->marioObj->hitboxRadius + padding;

    f32 offsetX = m->pos[0] - obj->oPosX;
    f32 offsetZ = m->pos[2] - obj->oPosZ;
    f32 distanceSquared = (sqr(offsetX) + sqr(offsetZ));

    if (distanceSquared < sqr(minDistance)) {
        struct Surface *floor;
        s16 pushAngle;
        f32 newMarioX;
        f32 newMarioZ;

        if (distanceSquared == 0.0f) {
            pushAngle = m->faceAngle[1];
        } else {
            pushAngle = atan2s(offsetZ, offsetX);
        }

        newMarioX = obj->oPosX + minDistance * sins(pushAngle);
        newMarioZ = obj->oPosZ + minDistance * coss(pushAngle);

        f32_find_wall_collision(&newMarioX, &m->pos[1], &newMarioZ, 60.0f, 50.0f);

        f32 floorHeight = find_floor(newMarioX, m->pos[1], newMarioZ, &floor);
        if (floor != NULL) {
            m->pos[0] = newMarioX;
            m->pos[2] = newMarioZ;
            set_mario_floor(m, floor, floorHeight); // Prevent oob death
        }
    }
}

void bounce_back_from_attack(struct MarioState *m, u32 interaction) {
    if (interaction & (INT_PUNCH | INT_KICK | INT_TRIP)) {
        if (m->action == ACT_PUNCHING) {
            m->action = ACT_MOVE_PUNCHING;
        }

        if (m->action & ACT_FLAG_AIR) {
            mario_set_forward_vel(m, -16.0f);
        } else {
            mario_set_forward_vel(m, -48.0f);
        }

        set_camera_shake_from_hit(SHAKE_ATTACK);
        m->particleFlags |= PARTICLE_TRIANGLE;
    }

    if (interaction & (INT_PUNCH | INT_KICK | INT_TRIP | INT_FAST_ATTACK_OR_SHELL)) {
        play_sound(SOUND_ACTION_HIT_2, m->marioObj->header.gfx.cameraToObject);
    }
}

u32 should_push_or_pull_door(struct MarioState *m, struct Object *obj) {
    f32 dx = obj->oPosX - m->pos[0];
    f32 dz = obj->oPosZ - m->pos[2];

    s16 dYaw = abs_angle_diff(obj->oMoveAngleYaw, atan2s(dz, dx));

    return (dYaw <= 0x4000) ? WARP_FLAG_DOOR_PULLED : WARP_FLAG_DOOR_FLIP_MARIO;
}

u32 take_damage_from_interact_object(struct MarioState *m) {
    s32 shake;
    s32 damage = m->interactObj->oDamageOrCoinValue;

    if (damage >= 4) {
        shake = SHAKE_LARGE_DAMAGE;
    } else if (damage >= 2) {
        shake = SHAKE_MED_DAMAGE;
    } else {
        shake = SHAKE_SMALL_DAMAGE;
    }

    if (!(m->flags & MARIO_CAP_ON_HEAD)) {
        damage += (damage + 1) / 2;
    }

    if (m->flags & MARIO_METAL_CAP) {
        damage = 0;
    }

    m->hurtCounter += 4 * damage;

#if ENABLE_RUMBLE
    queue_rumble_data(5, 80);
#endif
    set_camera_shake_from_hit(shake);

    return damage;
}

u32 take_damage_and_knock_back(struct MarioState *m, struct Object *obj) {
    u32 damage;

    if (!sInvulnerable && !(m->flags & MARIO_VANISH_CAP)
        && !(obj->oInteractionSubtype & INT_SUBTYPE_DELAY_INVINCIBILITY)) {
        obj->oInteractStatus = INT_STATUS_INTERACTED | INT_STATUS_ATTACKED_MARIO;
        m->interactObj = obj;

        damage = take_damage_from_interact_object(m);

        if (obj->oInteractionSubtype & INT_SUBTYPE_BIG_KNOCKBACK) {
            m->forwardVel = 40.0f;
        }

        if (obj->oDamageOrCoinValue > 0) {
            play_sound(SOUND_MARIO_ATTACKED, m->marioObj->header.gfx.cameraToObject);
        }

        update_mario_sound_and_camera(m);
        return drop_and_set_mario_action(m, determine_knockback_action(m, obj->oDamageOrCoinValue), damage);
    }

    return FALSE;
}

void reset_mario_pitch(struct MarioState *m) {
    if (m->action == ACT_WATER_JUMP || m->action == ACT_SHOT_FROM_CANNON || m->action == ACT_FLYING) {
        set_camera_mode(m->area->camera, m->area->camera->defMode, 1);
        m->faceAngle[0] = 0;
    }
}

u32 interact_coin(struct MarioState *m, UNUSED u32 interactType, struct Object *obj) {
    m->numCoins += obj->oDamageOrCoinValue;
    m->healCounter += 4 * obj->oDamageOrCoinValue;
#ifdef BREATH_METER
    m->breathCounter += (4 * obj->oDamageOrCoinValue);
#endif
    obj->oInteractStatus = INT_STATUS_INTERACTED;

#ifdef X_COIN_STAR
    if (COURSE_IS_MAIN_COURSE(gCurrCourseNum) && m->numCoins - obj->oDamageOrCoinValue < X_COIN_STAR
        && m->numCoins >= X_COIN_STAR && !g100CoinStarSpawned) {
        bhv_spawn_star_no_level_exit(STAR_BP_ACT_100_COINS);
        g100CoinStarSpawned = TRUE;
    }
#endif
#if ENABLE_RUMBLE
    if (obj->oDamageOrCoinValue >= 2) {
        queue_rumble_data(5, 80);
    }
#endif

    return FALSE;
}

u32 interact_water_ring(struct MarioState *m, UNUSED u32 interactType, struct Object *obj) {
#ifdef BREATH_METER
    m->breathCounter += 4 * obj->oDamageOrCoinValue;
#else
    m->healCounter += 4 * obj->oDamageOrCoinValue;
#endif
    obj->oInteractStatus = INT_STATUS_INTERACTED;
    return FALSE;
}

u32 interact_star_or_key(struct MarioState *m, UNUSED u32 interactType, struct Object *obj) {
    u32 starIndex;
    u32 starGrabAction = ACT_STAR_DANCE_EXIT;
#ifdef NON_STOP_STARS
 #ifdef KEYS_EXIT_LEVEL
    u32 noExit = !obj_has_model(obj, MODEL_BOWSER_KEY);
 #else
    u32 noExit = TRUE;
 #endif
#else // !NON_STOP_STARS
    u32 noExit = (obj->oInteractionSubtype & INT_SUBTYPE_NO_EXIT) != 0;
#endif // !NON_STOP_STARS
    u32 grandStar = (obj->oInteractionSubtype & INT_SUBTYPE_GRAND_STAR) != 0;

    if (m->health >= 0x100) {
        mario_stop_riding_and_holding(m);
#if ENABLE_RUMBLE
        queue_rumble_data(5, 80);
#endif

#ifdef POWER_STARS_HEAL
        m->hurtCounter = 0;
        m->healCounter = 31;
 #ifdef BREATH_METER
        m->breathCounter = 31;
 #endif
        if (!noExit) {
#else // !POWER_STARS_HEAL
        if (!noExit) {
            m->hurtCounter = 0;
            m->healCounter = 0;
 #ifdef BREATH_METER
            m->breathCounter = 0;
 #endif
#endif // !POWER_STARS_HEAL
            if (m->capTimer > 1) {
                m->capTimer = 1;
            }
        }

        if (noExit) {
            starGrabAction = ACT_STAR_DANCE_NO_EXIT;
        }

        if (m->action & ACT_FLAG_SWIMMING) {
            starGrabAction = ACT_STAR_DANCE_WATER;
        }

        if (m->action & ACT_FLAG_METAL_WATER) {
            starGrabAction = ACT_STAR_DANCE_WATER;
        }

        if (m->action & ACT_FLAG_AIR) {
            starGrabAction = ACT_FALL_AFTER_STAR_GRAB;
        }

        spawn_object(obj, MODEL_NONE, bhvStarKeyCollectionPuffSpawner);

        obj->oInteractStatus = INT_STATUS_INTERACTED;
        m->interactObj       = obj;
        m->usedObj           = obj;

        gStarModelLastCollected = obj_get_model_id(obj);

#ifdef GLOBAL_STAR_IDS
        starIndex = (obj->oBehParams >> 24) & 0xFF;
#else
        starIndex = (obj->oBehParams >> 24) & 0x1F;
#endif
        save_file_collect_star_or_key(m->numCoins, starIndex);

        m->numStars =
            save_file_get_total_star_count(gCurrSaveFileNum - 1, COURSE_MIN - 1, COURSE_MAX - 1);

        if (!noExit) {
            drop_queued_background_music();
            fadeout_level_music(126);
        }

        play_sound(SOUND_MENU_STAR_SOUND, m->marioObj->header.gfx.cameraToObject);
        update_mario_sound_and_camera(m);

        if (grandStar) {
            return set_mario_action(m, ACT_JUMBO_STAR_CUTSCENE, 0);
        }

        return set_mario_action(m, starGrabAction, noExit + 2 * grandStar);
    }

    return FALSE;
}

u32 interact_bbh_entrance(struct MarioState *m, UNUSED u32 interactType, struct Object *obj) {
    if (m->action != ACT_BBH_ENTER_SPIN && m->action != ACT_BBH_ENTER_JUMP) {
        mario_stop_riding_and_holding(m);

        obj->oInteractStatus = INT_STATUS_INTERACTED;
        m->interactObj       = obj;
        m->usedObj           = obj;

        if (m->action & ACT_FLAG_AIR) {
            return set_mario_action(m, ACT_BBH_ENTER_SPIN, 0);
        }

        return set_mario_action(m, ACT_BBH_ENTER_JUMP, 0);
    }

    return FALSE;
}

u32 interact_warp(struct MarioState *m, UNUSED u32 interactType, struct Object *obj) {
    u32 action;

    if (obj->oInteractionSubtype & INT_SUBTYPE_FADING_WARP) {
        action = m->action;

        if (action == ACT_TELEPORT_FADE_IN) {
            sJustTeleported = TRUE;

        } else if (!sJustTeleported) {
            if (action == ACT_IDLE || action == ACT_PANTING || action == ACT_STANDING_AGAINST_WALL || action == ACT_CROUCHING) {
                m->interactObj = obj;
                m->usedObj     = obj;

                sJustTeleported = TRUE;
                return set_mario_action(m, ACT_TELEPORT_FADE_OUT, 0);
            }
        }
    } else {
        if (m->action != ACT_EMERGE_FROM_PIPE) {
            obj->oInteractStatus = INT_STATUS_INTERACTED;
            m->interactObj       = obj;
            m->usedObj           = obj;

#if ENABLE_RUMBLE
            if (obj->collisionData == segmented_to_virtual(warp_pipe_seg3_collision_03009AC8)) {
                play_sound(SOUND_MENU_ENTER_PIPE, m->marioObj->header.gfx.cameraToObject);
                queue_rumble_data(15, 80);
            } else {
                play_sound(SOUND_MENU_ENTER_HOLE, m->marioObj->header.gfx.cameraToObject);
                queue_rumble_data(12, 80);
            }
#else
            play_sound(obj->collisionData == segmented_to_virtual(warp_pipe_seg3_collision_03009AC8)
                           ? SOUND_MENU_ENTER_PIPE
                           : SOUND_MENU_ENTER_HOLE,
                       m->marioObj->header.gfx.cameraToObject);
#endif

            mario_stop_riding_object(m);
            return set_mario_action(m, ACT_DISAPPEARED, (WARP_OP_WARP_OBJECT << 16) + 2);
        }
    }

    return FALSE;
}

u32 interact_warp_door(struct MarioState *m, UNUSED u32 interactType, struct Object *obj) {
    u32 doorAction = ACT_UNINITIALIZED;
#ifndef UNLOCK_ALL
    u32 saveFlags = save_file_get_flags();
    s16 warpDoorId = (obj->oBehParams >> 24);
#endif

    if (m->action == ACT_WALKING || m->action == ACT_DECELERATING) {
#ifndef UNLOCK_ALL
        if (warpDoorId == 1 && !(saveFlags & SAVE_FLAG_UNLOCKED_UPSTAIRS_DOOR)) {
            if (!(saveFlags & SAVE_FLAG_HAVE_KEY_2)) {
                if (!sDisplayingDoorText) {
                    set_mario_action(m, ACT_READING_AUTOMATIC_DIALOG,
                                     (saveFlags & SAVE_FLAG_HAVE_KEY_1) ? DIALOG_023 : DIALOG_022);
                }
                sDisplayingDoorText = TRUE;

                return FALSE;
            }

            doorAction = ACT_UNLOCKING_KEY_DOOR;
        }

        if (warpDoorId == 2 && !(saveFlags & SAVE_FLAG_UNLOCKED_BASEMENT_DOOR)) {
            if (!(saveFlags & SAVE_FLAG_HAVE_KEY_1)) {
                if (!sDisplayingDoorText) {
                    // Moat door skip was intended confirmed
                    set_mario_action(m, ACT_READING_AUTOMATIC_DIALOG,
                                     (saveFlags & SAVE_FLAG_HAVE_KEY_2) ? DIALOG_023 : DIALOG_022);
                }
                sDisplayingDoorText = TRUE;

                return FALSE;
            }

            doorAction = ACT_UNLOCKING_KEY_DOOR;
        }
#endif

        if (m->action == ACT_WALKING || m->action == ACT_DECELERATING) {
            u32 actionArg = should_push_or_pull_door(m, obj) + WARP_FLAG_DOOR_IS_WARP;

            if (doorAction == 0) {
                if (actionArg & WARP_FLAG_DOOR_PULLED) {
                    doorAction = ACT_PULLING_DOOR;
                } else {
                    doorAction = ACT_PUSHING_DOOR;
                }
            }

            m->interactObj = obj;
            m->usedObj = obj;
            return set_mario_action(m, doorAction, actionArg);
        }
    }

    return FALSE;
}

u32 get_door_save_file_flag(struct Object *door) {
    u32 saveFileFlag = 0;
    s16 requiredNumStars = door->oBehParams >> 24;

    s16 isCcmDoor = door->oPosX < 0.0f;
    s16 isPssDoor = door->oPosY > 500.0f;

    switch (requiredNumStars) {
        case  1: saveFileFlag = (isPssDoor ? SAVE_FLAG_UNLOCKED_PSS_DOOR : SAVE_FLAG_UNLOCKED_WF_DOOR ); break;
        case  3: saveFileFlag = (isCcmDoor ? SAVE_FLAG_UNLOCKED_CCM_DOOR : SAVE_FLAG_UNLOCKED_JRB_DOOR); break;
        case  8: saveFileFlag = SAVE_FLAG_UNLOCKED_BITDW_DOOR;                                           break;
        case 30: saveFileFlag = SAVE_FLAG_UNLOCKED_BITFS_DOOR;                                           break;
        case 50: saveFileFlag = SAVE_FLAG_UNLOCKED_50_STAR_DOOR;                                         break;
    }

    return saveFileFlag;
}

u32 interact_door(struct MarioState *m, UNUSED u32 interactType, struct Object *obj) {
    s16 requiredNumStars = (obj->oBehParams >> 24);
#ifndef UNLOCK_ALL
    s16 numStars = save_file_get_total_star_count(gCurrSaveFileNum - 1, COURSE_MIN - 1, COURSE_MAX - 1);
#endif
    if (m->action == ACT_WALKING || m->action == ACT_DECELERATING) {
#ifndef UNLOCK_ALL
        if (numStars >= requiredNumStars) {
#endif
            u32 actionArg = should_push_or_pull_door(m, obj);
            u32 enterDoorAction;

            if (actionArg & WARP_FLAG_DOOR_PULLED) {
                enterDoorAction = ACT_PULLING_DOOR;
            } else {
                enterDoorAction = ACT_PUSHING_DOOR;
            }

#ifndef UNLOCK_ALL
            u32 doorSaveFileFlag = get_door_save_file_flag(obj);
#endif
            m->interactObj = obj;
            m->usedObj     = obj;

            if (obj->oInteractionSubtype & INT_SUBTYPE_STAR_DOOR) {
                enterDoorAction = ACT_ENTERING_STAR_DOOR;
            }

#ifndef UNLOCK_ALL
            if (doorSaveFileFlag != 0 && !(save_file_get_flags() & doorSaveFileFlag)) {
                enterDoorAction = ACT_UNLOCKING_STAR_DOOR;
            }
#endif
            return set_mario_action(m, enterDoorAction, actionArg);
#ifndef UNLOCK_ALL
        } else if (!sDisplayingDoorText) {
            u32 text = DIALOG_022 << 16;

            switch (requiredNumStars) {
                case  1: text = DIALOG_024 << 16; break;
                case  3: text = DIALOG_025 << 16; break;
                case  8: text = DIALOG_026 << 16; break;
                case 30: text = DIALOG_027 << 16; break;
                case 50: text = DIALOG_028 << 16; break;
                case 70: text = DIALOG_029 << 16; break;
            }

            text += requiredNumStars - numStars;

            sDisplayingDoorText = TRUE;
            return set_mario_action(m, ACT_READING_AUTOMATIC_DIALOG, text);
        }
#endif
    } else if (m->action == ACT_IDLE && sDisplayingDoorText == TRUE && requiredNumStars == 70) {
        m->interactObj = obj;
        m->usedObj     = obj;
        return set_mario_action(m, ACT_ENTERING_STAR_DOOR, should_push_or_pull_door(m, obj));
    }

    return FALSE;
}

u32 interact_cannon_base(struct MarioState *m, UNUSED u32 interactType, struct Object *obj) {
    if (m->action != ACT_IN_CANNON) {
        mario_stop_riding_and_holding(m);
        obj->oInteractStatus = INT_STATUS_INTERACTED;
        m->interactObj       = obj;
        m->usedObj           = obj;
        return set_mario_action(m, ACT_IN_CANNON, 0);
    }

    return FALSE;
}

u32 interact_igloo_barrier(struct MarioState *m, UNUSED u32 interactType, struct Object *obj) {
    //! Sets used object without changing action (LOTS of interesting glitches,
    // but unfortunately the igloo barrier is the only object with this interaction
    // type)
    m->interactObj = obj;
    m->usedObj     = obj;
    push_mario_out_of_object(m, obj, 5.0f);
    return FALSE;
}

u32 interact_tornado(struct MarioState *m, UNUSED u32 interactType, struct Object *obj) {
    struct Object *marioObj = m->marioObj;

    if (m->action != ACT_TORNADO_TWIRLING && m->action != ACT_SQUISHED) {
        mario_stop_riding_and_holding(m);
        mario_set_forward_vel(m, 0.0f);
        update_mario_sound_and_camera(m);

        obj->oInteractStatus = INT_STATUS_INTERACTED;
        m->interactObj       = obj;
        m->usedObj           = obj;

        marioObj->oMarioTornadoYawVel = 0x400;
        marioObj->oMarioTornadoPosY = m->pos[1] - obj->oPosY;

        play_sound(SOUND_MARIO_WAAAOOOW, m->marioObj->header.gfx.cameraToObject);
#if ENABLE_RUMBLE
        queue_rumble_data(30, 60);
#endif
        return set_mario_action(m, ACT_TORNADO_TWIRLING, m->action == ACT_TWIRLING);
    }

    return FALSE;
}

u32 interact_whirlpool(struct MarioState *m, UNUSED u32 interactType, struct Object *obj) {
    struct Object *marioObj = m->marioObj;

    if (m->action != ACT_CAUGHT_IN_WHIRLPOOL) {
        mario_stop_riding_and_holding(m);
        obj->oInteractStatus = INT_STATUS_INTERACTED;
        m->interactObj       = obj;
        m->usedObj           = obj;

        m->forwardVel = 0.0f;

        marioObj->oMarioWhirlpoolPosY = m->pos[1] - obj->oPosY;

        play_sound(SOUND_MARIO_WAAAOOOW, m->marioObj->header.gfx.cameraToObject);
#if ENABLE_RUMBLE
        queue_rumble_data(30, 60);
#endif
        return set_mario_action(m, ACT_CAUGHT_IN_WHIRLPOOL, 0);
    }

    return FALSE;
}

u32 interact_strong_wind(struct MarioState *m, UNUSED u32 interactType, struct Object *obj) {
    if (m->action != ACT_GETTING_BLOWN) {
        mario_stop_riding_and_holding(m);
        obj->oInteractStatus = INT_STATUS_INTERACTED;
        m->interactObj = obj;
        m->usedObj     = obj;

        m->faceAngle[1] = obj->oMoveAngleYaw + 0x8000;
        m->windGravity = 0.4f;
        m->forwardVel = -24.0f;
        m->vel[1] = 12.0f;

        play_sound(SOUND_MARIO_WAAAOOOW, m->marioObj->header.gfx.cameraToObject);
        update_mario_sound_and_camera(m);
        return set_mario_action(m, ACT_GETTING_BLOWN, 0);
    }

    return FALSE;
}

u32 interact_flame(struct MarioState *m, UNUSED u32 interactType, struct Object *obj) {
    u32 burningAction = ACT_BURNING_JUMP;

    if (!sInvulnerable && !(m->flags & MARIO_METAL_CAP) && !(m->flags & MARIO_VANISH_CAP)
        && !(obj->oInteractionSubtype & INT_SUBTYPE_DELAY_INVINCIBILITY)) {
#if ENABLE_RUMBLE
        queue_rumble_data(5, 80);
#endif
        obj->oInteractStatus = INT_STATUS_INTERACTED;
        m->interactObj       = obj;

        if ((m->action & (ACT_FLAG_SWIMMING | ACT_FLAG_METAL_WATER))
            || m->waterLevel - m->pos[1] > 50.0f) {
            play_sound(SOUND_GENERAL_FLAME_OUT, m->marioObj->header.gfx.cameraToObject);
        } else {
            m->marioObj->oMarioBurnTimer = 0;
            update_mario_sound_and_camera(m);
            play_sound(SOUND_MARIO_ON_FIRE, m->marioObj->header.gfx.cameraToObject);

            if ((m->action & ACT_FLAG_AIR) && m->vel[1] <= 0.0f) {
                burningAction = ACT_BURNING_FALL;
            }

            return drop_and_set_mario_action(m, burningAction, 1);
        }
    }

    return FALSE;
}

u32 interact_snufit_bullet(struct MarioState *m, UNUSED u32 interactType, struct Object *obj) {
    if (!sInvulnerable && !(m->flags & MARIO_VANISH_CAP)) {
        if (m->flags & MARIO_METAL_CAP) {
            obj->oInteractStatus = INT_STATUS_INTERACTED | INT_STATUS_WAS_ATTACKED;
            play_sound(SOUND_ACTION_SNUFFIT_BULLET_HIT_METAL, m->marioObj->header.gfx.cameraToObject);
        } else {
            obj->oInteractStatus = INT_STATUS_INTERACTED | INT_STATUS_ATTACKED_MARIO;
            m->interactObj = obj;
            take_damage_from_interact_object(m);

            play_sound(SOUND_MARIO_ATTACKED, m->marioObj->header.gfx.cameraToObject);
            update_mario_sound_and_camera(m);

            return drop_and_set_mario_action(m, determine_knockback_action(m, obj->oDamageOrCoinValue),
                                             obj->oDamageOrCoinValue);
        }
    }

    if (!(obj->oInteractionSubtype & INT_SUBTYPE_DELAY_INVINCIBILITY)) {
        sDelayInvincTimer = TRUE;
    }

    return FALSE;
}

u32 interact_clam_or_bubba(struct MarioState *m, UNUSED u32 interactType, struct Object *obj) {
    if (obj->oInteractionSubtype & INT_SUBTYPE_EATS_MARIO) {
        obj->oInteractStatus = INT_STATUS_INTERACTED;
        m->interactObj = obj;
        return set_mario_action(m, ACT_EATEN_BY_BUBBA, 0);
    } else if (take_damage_and_knock_back(m, obj)) {
        return TRUE;
    }

    if (!(obj->oInteractionSubtype & INT_SUBTYPE_DELAY_INVINCIBILITY)) {
        sDelayInvincTimer = TRUE;
    }

    return TRUE;
}

u32 interact_bully(struct MarioState *m, UNUSED u32 interactType, struct Object *obj) {
    u32 interaction;
    if (m->flags & MARIO_METAL_CAP) {
        interaction = INT_FAST_ATTACK_OR_SHELL;
    } else {
        interaction = determine_interaction(m, obj);
    }

    m->interactObj = obj;

    if (interaction & INT_ATTACK_NOT_FROM_BELOW) {
#if ENABLE_RUMBLE
        queue_rumble_data(5, 80);
#endif
        push_mario_out_of_object(m, obj, 5.0f);

        m->forwardVel = -16.0f;
        obj->oMoveAngleYaw = m->faceAngle[1];
        obj->oForwardVel = 3392.0f / obj->hitboxRadius;

        attack_object(obj, interaction);
        bounce_back_from_attack(m, interaction);
        return TRUE;
    } else if (!sInvulnerable && !(m->flags & MARIO_VANISH_CAP)
             && !(obj->oInteractionSubtype & INT_SUBTYPE_DELAY_INVINCIBILITY)) {
        obj->oInteractStatus = INT_STATUS_INTERACTED;
        m->invincTimer = 2;

        update_mario_sound_and_camera(m);
        play_sound(SOUND_MARIO_EEUH, m->marioObj->header.gfx.cameraToObject);
        play_sound(SOUND_OBJ_BULLY_METAL, m->marioObj->header.gfx.cameraToObject);

        push_mario_out_of_object(m, obj, 5.0f);
        drop_and_set_mario_action(m, bully_knock_back_mario(m), 0);
#if ENABLE_RUMBLE
        queue_rumble_data(5, 80);
#endif
        return TRUE;
    }

    return FALSE;
}

u32 interact_shock(struct MarioState *m, UNUSED u32 interactType, struct Object *obj) {
    if (!sInvulnerable && !(m->flags & MARIO_VANISH_CAP)
        && !(obj->oInteractionSubtype & INT_SUBTYPE_DELAY_INVINCIBILITY)) {
        u32 actionArg = (m->action & (ACT_FLAG_AIR | ACT_FLAG_ON_POLE | ACT_FLAG_HANGING)) == 0;

        obj->oInteractStatus = INT_STATUS_INTERACTED | INT_STATUS_ATTACKED_MARIO;
        m->interactObj = obj;

        take_damage_from_interact_object(m);
        play_sound(SOUND_MARIO_ATTACKED, m->marioObj->header.gfx.cameraToObject);
#if ENABLE_RUMBLE
        queue_rumble_data(70, 60);
#endif

        if (m->action & (ACT_FLAG_SWIMMING | ACT_FLAG_METAL_WATER)) {
            return drop_and_set_mario_action(m, ACT_WATER_SHOCKED, 0);
        } else {
            update_mario_sound_and_camera(m);
            return drop_and_set_mario_action(m, ACT_SHOCKED, actionArg);
        }
    }

    if (!(obj->oInteractionSubtype & INT_SUBTYPE_DELAY_INVINCIBILITY)) {
        sDelayInvincTimer = TRUE;
    }

    return FALSE;
}

UNUSED static u32 interact_stub(UNUSED struct MarioState *m, UNUSED u32 interactType, struct Object *obj) {
    if (!(obj->oInteractionSubtype & INT_SUBTYPE_DELAY_INVINCIBILITY)) {
        sDelayInvincTimer = TRUE;
    }
    return FALSE;
}

u32 interact_mr_blizzard(struct MarioState *m, UNUSED u32 interactType, struct Object *obj) {
    if (take_damage_and_knock_back(m, obj)) {
        return TRUE;
    }

    if (!(obj->oInteractionSubtype & INT_SUBTYPE_DELAY_INVINCIBILITY)) {
        sDelayInvincTimer = TRUE;
    }

    return FALSE;
}

u32 interact_hit_from_below(struct MarioState *m, UNUSED u32 interactType, struct Object *obj) {
    u32 interaction;
    if (m->flags & MARIO_METAL_CAP) {
        interaction = INT_FAST_ATTACK_OR_SHELL;
    } else {
        interaction = determine_interaction(m, obj);
    }

    if (interaction & INT_ANY_ATTACK) {
#if ENABLE_RUMBLE
        queue_rumble_data(5, 80);
#endif
        attack_object(obj, interaction);
        bounce_back_from_attack(m, interaction);

        if (interaction & INT_HIT_FROM_BELOW) {
            hit_object_from_below(m, obj);
        }

        if (interaction & INT_HIT_FROM_ABOVE) {
            if (obj->oInteractionSubtype & INT_SUBTYPE_TWIRL_BOUNCE) {
                bounce_off_object(m, obj, 80.0f);
                reset_mario_pitch(m);
                play_sound(SOUND_MARIO_TWIRL_BOUNCE, m->marioObj->header.gfx.cameraToObject);
                return drop_and_set_mario_action(m, ACT_TWIRLING, 0);
            } else {
                bounce_off_object(m, obj, 30.0f);
            }
        }
    } else if (take_damage_and_knock_back(m, obj)) {
        return TRUE;
    }

    if (!(obj->oInteractionSubtype & INT_SUBTYPE_DELAY_INVINCIBILITY)) {
        sDelayInvincTimer = TRUE;
    }

    return FALSE;
}

u32 interact_bounce_top(struct MarioState *m, UNUSED u32 interactType, struct Object *obj) {
    u32 interaction;
    if (m->flags & MARIO_METAL_CAP) {
        interaction = INT_FAST_ATTACK_OR_SHELL;
    } else {
        interaction = determine_interaction(m, obj);
    }

    if (interaction & INT_ATTACK_NOT_FROM_BELOW) {
#if ENABLE_RUMBLE
        queue_rumble_data(5, 80);
#endif
        attack_object(obj, interaction);
        bounce_back_from_attack(m, interaction);

        if (interaction & INT_HIT_FROM_ABOVE) {
            if (obj->oInteractionSubtype & INT_SUBTYPE_TWIRL_BOUNCE) {
                bounce_off_object(m, obj, 80.0f);
                reset_mario_pitch(m);
                play_sound(SOUND_MARIO_TWIRL_BOUNCE, m->marioObj->header.gfx.cameraToObject);
                return drop_and_set_mario_action(m, ACT_TWIRLING, 0);
            } else {
                bounce_off_object(m, obj, 30.0f);
            }
        }
    } else if (take_damage_and_knock_back(m, obj)) {
        return TRUE;
    }

    if (!(obj->oInteractionSubtype & INT_SUBTYPE_DELAY_INVINCIBILITY)) {
        sDelayInvincTimer = TRUE;
    }

    return FALSE;
}

u32 interact_spiny_walking(struct MarioState *m, UNUSED u32 interactType, struct Object *obj) {
    u32 interaction = determine_interaction(m, obj);

    if (interaction & INT_PUNCH) {
        obj->oInteractStatus = INT_STATUS_INTERACTED | INT_STATUS_WAS_ATTACKED | ATTACK_PUNCH;
        bounce_back_from_attack(m, interaction);
    } else if (take_damage_and_knock_back(m, obj)) {
        return TRUE;
    }

    if (!(obj->oInteractionSubtype & INT_SUBTYPE_DELAY_INVINCIBILITY)) {
        sDelayInvincTimer = TRUE;
    }

    return FALSE;
}

u32 interact_damage(struct MarioState *m, UNUSED u32 interactType, struct Object *obj) {
    if (take_damage_and_knock_back(m, obj)) {
        return TRUE;
    }

    if (!(obj->oInteractionSubtype & INT_SUBTYPE_DELAY_INVINCIBILITY)) {
        sDelayInvincTimer = TRUE;
    }

    return FALSE;
}

u32 interact_breakable(struct MarioState *m, UNUSED u32 interactType, struct Object *obj) {
    u32 interaction = determine_interaction(m, obj);

    if (interaction & INT_ATTACK_NOT_WEAK_FROM_ABOVE) {
        attack_object(obj, interaction);
        bounce_back_from_attack(m, interaction);

        m->interactObj = obj;

        switch (interaction) {
            case INT_HIT_FROM_ABOVE:
                bounce_off_object(m, obj, 30.0f); //! Not in the 0x8F mask
                break;

            case INT_HIT_FROM_BELOW:
                hit_object_from_below(m, obj);
                break;
        }

        return TRUE;
    }

    return FALSE;
}

u32 interact_koopa_shell(struct MarioState *m, UNUSED u32 interactType, struct Object *obj) {
    if (!(m->action & ACT_FLAG_RIDING_SHELL)) {
        u32 interaction = determine_interaction(m, obj);

        if (interaction == INT_HIT_FROM_ABOVE || m->action == ACT_WALKING
            || m->action == ACT_HOLD_WALKING) {
            m->interactObj = obj;
            m->usedObj = obj;
            m->riddenObj = obj;

            attack_object(obj, interaction);
            update_mario_sound_and_camera(m);
            play_shell_music();
            mario_drop_held_object(m);

            //! Puts Mario in ground action even when in air, making it easy to
            // escape air actions into crouch slide (shell cancel)
            return set_mario_action(m, ACT_RIDING_SHELL_GROUND, 0);
        }

        push_mario_out_of_object(m, obj, 2.0f);
    }

    return FALSE;
}

u32 check_object_grab_mario(struct MarioState *m, UNUSED u32 interactType, struct Object *obj) {
    if ((!(m->action & (ACT_FLAG_AIR | ACT_FLAG_INVULNERABLE | ACT_FLAG_ATTACKING)) || !sInvulnerable)
        && (obj->oInteractionSubtype & INT_SUBTYPE_GRABS_MARIO)) {
        if (object_facing_mario(m, obj, DEGREES(60))) {
            mario_stop_riding_and_holding(m);
            obj->oInteractStatus = INT_STATUS_INTERACTED | INT_STATUS_GRABBED_MARIO;

            m->faceAngle[1] = obj->oMoveAngleYaw;
            m->interactObj  = obj;
            m->usedObj      = obj;

            update_mario_sound_and_camera(m);
            play_sound(SOUND_MARIO_OOOF, m->marioObj->header.gfx.cameraToObject);
#if ENABLE_RUMBLE
            queue_rumble_data(5, 80);
#endif
            return set_mario_action(m, ACT_GRABBED, 0);
        }
    }

    push_mario_out_of_object(m, obj, -5.0f);
    return FALSE;
}

u32 interact_pole(struct MarioState *m, UNUSED u32 interactType, struct Object *obj) {
    s32 actionId = m->action & ACT_ID_MASK;
    if (actionId >= ACT_GROUP_AIRBORNE && actionId < (ACT_HOLD_JUMP & ACT_ID_MASK)) {
        if (!(m->prevAction & ACT_FLAG_ON_POLE) || m->usedObj != obj) {
#if defined(VERSION_SH) || defined(SHINDOU_POLES)
            f32 velConv = m->forwardVel; // conserve the velocity.
            struct Object *marioObj = m->marioObj;
            u32 lowSpeed;
#else
            u32 lowSpeed = (m->forwardVel <= 10.0f);
            struct Object *marioObj = m->marioObj;
#endif

            mario_stop_riding_and_holding(m);

#if defined(VERSION_SH) || defined(SHINDOU_POLES)
            lowSpeed = (velConv <= 10.0f);
#endif

            m->interactObj = obj;
            m->usedObj = obj;
            m->vel[1] = 0.0f;
            m->forwardVel = 0.0f;

            // Pole fix
            // If mario is beneath the pole, clamp mario's position to the down-offset of the pole (bottom)
            marioObj->oMarioPolePos = ((m->pos[1] - obj->oPosY) < 0) ? -obj->hitboxDownOffset : (m->pos[1] - obj->oPosY);

            if (lowSpeed) {
                return set_mario_action(m, ACT_GRAB_POLE_SLOW, 0);
            }

#if defined(VERSION_SH) || defined(SHINDOU_POLES)
            m->angleVel[1] = (s32)(velConv * 0x100 + 0x1000);
#else
            m->angleVel[1] = 0x1000;
#endif
            reset_mario_pitch(m);
#if ENABLE_RUMBLE
            queue_rumble_data(5, 80);
#endif
            return set_mario_action(m, ACT_GRAB_POLE_FAST, 0);
        }
    }

    return FALSE;
}

u32 interact_hoot(struct MarioState *m, UNUSED u32 interactType, struct Object *obj) {
    s32 actionId = m->action & ACT_ID_MASK;

    //! Can pause to advance the global timer without falling too far, allowing
    // you to regrab after letting go.
    if (
        actionId >= (ACT_JUMP & ACT_ID_MASK)
        && actionId <  (ACT_SHOT_FROM_CANNON & ACT_ID_MASK)
        && gGlobalTimer - m->usedObj->oHootMarioReleaseTime > 30
    ) {
        mario_stop_riding_and_holding(m);

        obj->oInteractStatus = TRUE; //! Note: Not a flag, treated as a TRUE/FALSE statement
        m->interactObj = obj;
        m->usedObj     = obj;

#if ENABLE_RUMBLE
        queue_rumble_data(5, 80);
#endif
        update_mario_sound_and_camera(m);
        return set_mario_action(m, ACT_RIDING_HOOT, 0);
    }

    return FALSE;
}

u32 interact_cap(struct MarioState *m, UNUSED u32 interactType, struct Object *obj) {
    u32 capFlag = get_mario_cap_flag(obj);
    u16 capMusic = 0;
    u16 capTime = 0;

    if (m->action != ACT_GETTING_BLOWN && capFlag != 0) {
        m->interactObj = obj;
        obj->oInteractStatus = INT_STATUS_INTERACTED;

        m->flags &= ~MARIO_CAP_ON_HEAD & ~MARIO_CAP_IN_HAND;
        m->flags |= capFlag;

        switch (capFlag) {
            case MARIO_VANISH_CAP: capTime =  600; capMusic = SEQUENCE_ARGS(4, SEQ_EVENT_POWERUP  ); break;
            case MARIO_METAL_CAP:  capTime =  600; capMusic = SEQUENCE_ARGS(4, SEQ_EVENT_METAL_CAP); break;
            case MARIO_WING_CAP:   capTime = 1800; capMusic = SEQUENCE_ARGS(4, SEQ_EVENT_POWERUP  ); break;
        }

        if (capTime > m->capTimer) {
            m->capTimer = capTime;
        }

        if ((m->action & ACT_FLAG_IDLE) || m->action == ACT_WALKING) {
            m->flags |= MARIO_CAP_IN_HAND;
            set_mario_action(m, ACT_PUTTING_ON_CAP, 0);
        } else {
            m->flags |= MARIO_CAP_ON_HEAD;
        }

        play_sound(SOUND_MENU_STAR_SOUND, m->marioObj->header.gfx.cameraToObject);
        play_sound(SOUND_MARIO_HERE_WE_GO, m->marioObj->header.gfx.cameraToObject);

        if (capMusic != 0) {
            play_cap_music(capMusic);
        }

        return TRUE;
    }

    return FALSE;
}

u32 interact_grabbable(struct MarioState *m, u32 interactType, struct Object *obj) {
    const BehaviorScript *script = virtual_to_segmented(SEGMENT_BEHAVIOR_DATA, obj->behavior);

    if (obj->oInteractionSubtype & INT_SUBTYPE_KICKABLE) {
        u32 interaction = determine_interaction(m, obj);
        if (interaction & (INT_KICK | INT_TRIP)) {
            attack_object(obj, interaction);
            bounce_back_from_attack(m, interaction);
            return FALSE;
        }
    }

    if ((obj->oInteractionSubtype & INT_SUBTYPE_GRABS_MARIO)) {
        if (check_object_grab_mario(m, interactType, obj)) {
            return TRUE;
        }
    }

    if (able_to_grab_object(m, obj)) {
        if (!(obj->oInteractionSubtype & INT_SUBTYPE_NOT_GRABBABLE)) {
            m->interactObj = obj;
            m->input |= INPUT_INTERACT_OBJ_GRABBABLE;
            return TRUE;
        }
    }

    if (script != bhvBowser) {
        push_mario_out_of_object(m, obj, -5.0f);
    }

    return FALSE;
}

u32 mario_can_talk(struct MarioState *m, u32 arg) {
#ifdef EASIER_DIALOG_TRIGGER
    if (m->action & (
        ACT_FLAG_AIR
        | ACT_FLAG_SHORT_HITBOX
        | ACT_FLAG_RIDING_SHELL
        | ACT_FLAG_BUTT_OR_STOMACH_SLIDE
        | ACT_FLAG_DIVING
        | ACT_FLAG_ON_POLE
        | ACT_FLAG_HANGING
        | ACT_FLAG_INTANGIBLE)
    ) {
         return FALSE;
    }

    if (m->action == ACT_FIRST_PERSON) {
        return FALSE;
    }

    if (m->action & (
        ACT_FLAG_IDLE
        | ACT_FLAG_STATIONARY
        | ACT_FLAG_ALLOW_FIRST_PERSON
    )) {
        return TRUE;
    }

    if (
        m->action == ACT_WALKING
        || m->action == ACT_DECELERATING
        || m->action == ACT_BRAKING
        || m->action == ACT_BRAKING_STOP) {
#else
    if (m->action & ACT_FLAG_IDLE) {
        return TRUE;
    }

    if (m->action == ACT_WALKING) {
#endif
        if (arg) {
            return TRUE;
        }

        s16 animID = m->marioObj->header.gfx.animInfo.animID;

        if (animID == MARIO_ANIM_SIDESTEP_RIGHT || animID == MARIO_ANIM_SIDESTEP_LEFT || animID == MARIO_ANIM_PUSHING) {
            return TRUE;
        }
    }

    return FALSE;
}

#define READ_MASK (INPUT_A_PRESSED | INPUT_B_PRESSED)
#define SIGN_RANGE DEGREES(90)

u32 check_read_sign(struct MarioState *m, struct Object *obj) {
#ifdef EASIER_DIALOG_TRIGGER
    s16 facingDYaw = (s16)(obj->oMoveAngleYaw + 0x8000) - m->faceAngle[1];
    if (
        mario_can_talk(m, TRUE)
        && object_facing_mario(m, obj, SIGN_RANGE)
        && (facingDYaw >= -SIGN_RANGE)
        && (facingDYaw <=  SIGN_RANGE)
        && abs_angle_diff(mario_obj_angle_to_object(m, obj), m->faceAngle[1]) <= SIGN_RANGE
    ) {
#ifdef DIALOG_INDICATOR
        struct Object *orangeNumber;
        if (obj->behavior == segmented_to_virtual(bhvSignOnWall)) {
            orangeNumber = spawn_object_relative(ORANGE_NUMBER_A, 0, 180, 32, obj, MODEL_NUMBER, bhvOrangeNumber);
        } else {
            orangeNumber = spawn_object_relative(ORANGE_NUMBER_A, 0, 160,  8, obj, MODEL_NUMBER, bhvOrangeNumber);
        }
        orangeNumber->oHomeX = orangeNumber->oPosX;
        orangeNumber->oHomeZ = orangeNumber->oPosZ;
#endif
        if (m->input & READ_MASK) {
#else
    if ((m->input & READ_MASK) && mario_can_talk(m, 0) && object_facing_mario(m, obj, SIGN_RANGE)) {
        s16 facingDYaw = (s16)(obj->oMoveAngleYaw + 0x8000) - m->faceAngle[1];
        if (facingDYaw >= -SIGN_RANGE && facingDYaw <= SIGN_RANGE) {
#endif
            f32 targetX = obj->oPosX + 105.0f * sins(obj->oMoveAngleYaw);
            f32 targetZ = obj->oPosZ + 105.0f * coss(obj->oMoveAngleYaw);

            m->marioObj->oMarioReadingSignDYaw = facingDYaw;
            m->marioObj->oMarioReadingSignDPosX = targetX - m->pos[0];
            m->marioObj->oMarioReadingSignDPosZ = targetZ - m->pos[2];

            m->interactObj = obj;
            m->usedObj     = obj;
            return set_mario_action(m, ACT_READING_SIGN, 0);
        }
    }

    return FALSE;
}

u32 check_npc_talk(struct MarioState *m, struct Object *obj) {
#ifdef EASIER_DIALOG_TRIGGER
    if (
        mario_can_talk(m, TRUE)
        && abs_angle_diff(mario_obj_angle_to_object(m, obj), m->faceAngle[1]) <= SIGN_RANGE
    ) {
#ifdef DIALOG_INDICATOR
        struct Object *orangeNumber;
        if (obj->behavior == segmented_to_virtual(bhvYoshi)) {
            orangeNumber = spawn_object_relative(ORANGE_NUMBER_A, 0, 256, 64, obj, MODEL_NUMBER, bhvOrangeNumber);
        } else {
            orangeNumber = spawn_object_relative(ORANGE_NUMBER_A, 0, 160,  0, obj, MODEL_NUMBER, bhvOrangeNumber);
        }
        orangeNumber->oHomeX = orangeNumber->oPosX;
        orangeNumber->oHomeZ = orangeNumber->oPosZ;
#endif
        if (m->input & READ_MASK) {
#else
    if ((m->input & READ_MASK) && mario_can_talk(m, 1)) {
        s16 facingDYaw = mario_obj_angle_to_object(m, obj) - m->faceAngle[1];
        if (facingDYaw >= -SIGN_RANGE && facingDYaw <= SIGN_RANGE) {
#endif
            obj->oInteractStatus = INT_STATUS_INTERACTED;

            m->interactObj = obj;
            m->usedObj     = obj;

            push_mario_out_of_object(m, obj, -10.0f);
            return set_mario_action(m, ACT_WAITING_FOR_DIALOG, 0);
        }
    }

    push_mario_out_of_object(m, obj, -10.0f);
    return FALSE;
}

u32 interact_text(struct MarioState *m, UNUSED u32 interactType, struct Object *obj) {
    u32 interact = FALSE;

    if (obj->oInteractionSubtype & INT_SUBTYPE_SIGN) {
        interact = check_read_sign(m, obj);
    } else if (obj->oInteractionSubtype & INT_SUBTYPE_NPC) {
        interact = check_npc_talk(m, obj);
    } else {
        push_mario_out_of_object(m, obj, 2.0f);
    }

    return interact;
}

void check_kick_or_punch_wall(struct MarioState *m) {
    if (m->flags & (MARIO_PUNCHING | MARIO_KICKING | MARIO_TRIPPING)) {
        struct WallCollisionData detector;
        detector.x = m->pos[0] + 50.0f * sins(m->faceAngle[1]);
        detector.z = m->pos[2] + 50.0f * coss(m->faceAngle[1]);
        detector.y = m->pos[1];
        detector.offsetY = 80.0f;
        detector.radius = 5.0f;

        if (find_wall_collisions(&detector) > 0) {
            if (m->action != ACT_MOVE_PUNCHING || m->forwardVel >= 0.0f) {
                if (m->action == ACT_PUNCHING) {
                    m->action = ACT_MOVE_PUNCHING;
                }

                mario_set_forward_vel(m, -48.0f);
                play_sound(SOUND_ACTION_HIT_2, m->marioObj->header.gfx.cameraToObject);
                m->particleFlags |= PARTICLE_TRIANGLE;
            } else if (m->action & ACT_FLAG_AIR) {
                mario_set_forward_vel(m, -16.0f);
                play_sound(SOUND_ACTION_HIT_2, m->marioObj->header.gfx.cameraToObject);
                m->particleFlags |= PARTICLE_TRIANGLE;
            }
        }
    }
}

void mario_process_interactions(struct MarioState *m) {
    sDelayInvincTimer = FALSE;
    sInvulnerable = (m->action & ACT_FLAG_INVULNERABLE) || m->invincTimer != 0;

    if (!(m->action & ACT_FLAG_INTANGIBLE) && m->collidedObjInteractTypes != 0) {
        s32 i;
        for (i = 0; i < ARRAY_COUNT(sInteractionHandlers); i++) {
            u32 interactType = sInteractionHandlers[i].interactType;
            if (m->collidedObjInteractTypes & interactType) {
                struct Object *object = mario_get_collided_object(m, interactType);

                m->collidedObjInteractTypes &= ~interactType;

                if (!(object->oInteractStatus & INT_STATUS_INTERACTED)) {
                    if (sInteractionHandlers[i].handler(m, interactType, object)) {
                        break;
                    }
                }
            }
        }
    }

    if (m->invincTimer > 0 && !sDelayInvincTimer) {
        m->invincTimer--;
    }

    //! If the kick/punch flags are set and an object collision changes Mario's
    // action, he will get the kick/punch wall speed anyway.
    check_kick_or_punch_wall(m);
    m->flags &= ~MARIO_PUNCHING & ~MARIO_KICKING & ~MARIO_TRIPPING;

    if (!(m->marioObj->collidedObjInteractTypes & (INTERACT_WARP_DOOR | INTERACT_DOOR))) {
        sDisplayingDoorText = FALSE;
    }
    if (!(m->marioObj->collidedObjInteractTypes & INTERACT_WARP)) {
        sJustTeleported = FALSE;
    }
}

void check_death_barrier(struct MarioState *m) {
    if (m->pos[1] < m->floorHeight + 2048.0f) {
        if (level_trigger_warp(m, WARP_OP_WARP_FLOOR) == 20 && !(m->flags & MARIO_FALL_SOUND_PLAYED)) {
            play_sound(SOUND_MARIO_WAAAOOOW, m->marioObj->header.gfx.cameraToObject);
        }
    }
}

void check_lava_boost(struct MarioState *m) {
    if (!(m->action & ACT_FLAG_RIDING_SHELL) && m->pos[1] < m->floorHeight + 10.0f) {
        if (!(m->flags & MARIO_METAL_CAP)) {
            m->hurtCounter += (m->flags & MARIO_CAP_ON_HEAD) ? 12 : 18;
        }

        update_mario_sound_and_camera(m);
        drop_and_set_mario_action(m, ACT_LAVA_BOOST, 0);
    }
}

void pss_begin_slide(UNUSED struct MarioState *m) {
    if (!(gHudDisplay.flags & HUD_DISPLAY_FLAG_TIMER)) {
        level_control_timer(TIMER_CONTROL_SHOW);
        level_control_timer(TIMER_CONTROL_START);
        sPssSlideStarted = TRUE;
    }
}

void pss_end_slide(struct MarioState *m) {
    //! This flag isn't set on death or level entry, allowing double star spawn
    if (sPssSlideStarted) {
        u16 slideTime = level_control_timer(TIMER_CONTROL_STOP);
        if (slideTime < 630) {
            m->marioObj->oBehParams = (1 << 24);
            spawn_default_star(-6358.0f, -4300.0f, 4700.0f);
        }
        sPssSlideStarted = FALSE;
    }
}

void mario_handle_special_floors(struct MarioState *m) {
    if ((m->action & ACT_GROUP_MASK) == ACT_GROUP_CUTSCENE) {
        return;
    }

    if (m->floor != NULL) {
        s32 floorType = m->floor->type;

        switch (floorType) {
            case SURFACE_DEATH_PLANE:
            case SURFACE_VERTICAL_WIND:
                check_death_barrier(m);
                break;

            case SURFACE_WARP:
                level_trigger_warp(m, WARP_OP_WARP_FLOOR);
                break;

            case SURFACE_TIMER_START:
                pss_begin_slide(m);
                break;

            case SURFACE_TIMER_END:
                pss_end_slide(m);
                break;
        }

        if (!(m->action & (ACT_FLAG_AIR | ACT_FLAG_SWIMMING))) {
            if (floorType == SURFACE_BURNING) {
                check_lava_boost(m);
            }
        }
    }
}
