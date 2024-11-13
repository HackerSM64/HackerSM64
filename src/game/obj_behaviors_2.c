#include <PR/ultratypes.h>

#include "sm64.h"
#include "actors/common0.h"
#include "actors/group11.h"
#include "actors/group17.h"
#include "audio/external.h"
#include "behavior_actions.h"
#include "behavior_data.h"
#include "camera.h"
#include "camera/cutscene_helpers.h"
#include "dialog_ids.h"
#include "engine/behavior_script.h"
#include "engine/math_util.h"
#include "engine/surface_collision.h"
#include "engine/surface_load.h"
#include "game_init.h"
#include "geo_misc.h"
#include "ingame_menu.h"
#include "interaction.h"
#include "level_table.h"
#include "level_update.h"
#include "levels/bitdw/header.h"
#include "levels/bitfs/header.h"
#include "levels/bits/header.h"
#include "levels/bob/header.h"
#include "levels/ccm/header.h"
#include "levels/hmc/header.h"
#include "levels/jrb/header.h"
#include "levels/lll/header.h"
#include "levels/rr/header.h"
#include "levels/ssl/header.h"
#include "levels/thi/header.h"
#include "levels/ttc/header.h"
#include "levels/vcutm/header.h"
#include "mario.h"
#include "mario_actions_cutscene.h"
#include "memory.h"
#include "obj_behaviors.h"
#include "obj_behaviors_2.h"
#include "object_constants.h"
#include "object_helpers.h"
#include "object_list_processor.h"
#include "platform_displacement.h"
#include "rendering_graph_node.h"
#include "save_file.h"
#include "seq_ids.h"
#include "spawn_sound.h"

//! TODO: remove static

enum ObjPositionOperation {
    POS_OP_SAVE_POSITION,
    POS_OP_COMPUTE_VELOCITY,
    POS_OP_RESTORE_POSITION
};

Vec3f sObjSavedPos;

void wiggler_jumped_on_attack_handler(void);
void huge_goomba_weakly_attacked(void);

static s32 obj_is_rendering_enabled(void) {
    if (o->header.gfx.node.flags & GRAPH_RENDER_ACTIVE) {
        return TRUE;
    } else {
        return FALSE;
    }
}

static s16 obj_get_pitch_from_vel(void) {
    return -atan2s(o->oForwardVel, o->oVelY);
}

/**
 * Show dialog proposing a race.
 * If the player accepts the race, then leave time stop enabled and Mario in the
 * text action so that the racing object can wait before starting the race.
 * If the player declines the race, then disable time stop and allow Mario to
 * move again.
 */
static s32 obj_update_race_proposition_dialog(s16 dialogID) {
    s32 dialogResponse = cur_obj_update_dialog_with_cutscene(MARIO_DIALOG_LOOK_UP,
        (DIALOG_FLAG_TURN_TO_MARIO | DIALOG_FLAG_TIME_STOP_ENABLED), CUTSCENE_RACE_DIALOG, dialogID);

    if (dialogResponse == DIALOG_RESPONSE_NO) {
        set_mario_npc_dialog(MARIO_DIALOG_STOP);
        disable_time_stop_including_mario();
    }

    return dialogResponse;
}

static void obj_set_dist_from_home(f32 distFromHome) {
    o->oPosX = o->oHomeX + distFromHome * coss(o->oMoveAngleYaw);
    o->oPosZ = o->oHomeZ + distFromHome * sins(o->oMoveAngleYaw);
}

static s32 obj_is_near_to_and_facing_mario(f32 maxDist, s16 maxAngleDiff) {
    if (o->oDistanceToMario < maxDist
        && abs_angle_diff(o->oMoveAngleYaw, o->oAngleToMario) < maxAngleDiff) {
        return TRUE;
    }
    return FALSE;
}

static void obj_perform_position_op(s32 op) {
    switch (op) {
        case POS_OP_SAVE_POSITION:    vec3f_copy(sObjSavedPos, &o->oPosVec); break;
        case POS_OP_COMPUTE_VELOCITY: vec3f_diff(&o->oVelVec, &o->oPosVec, sObjSavedPos); break;
        case POS_OP_RESTORE_POSITION: vec3f_copy(&o->oPosVec, sObjSavedPos); break;
    }
}

static void cur_obj_spin_all_dimensions(f32 pitchSpeed, f32 rollSpeed) {
    f32 pitch, yaw, roll;
    f32 c, s;
    f32 px, pz, ny, nz, nx;

    if (o->oForwardVel == 0.0f) {
        roll = yaw = pitch = 0.0f;

        if (o->oMoveFlags & OBJ_MOVE_IN_AIR) {
            yaw = 50.0f;
        } else {
            if (o->oFaceAnglePitch < 0x0) {
                pitch = -pitchSpeed;
            } else if (o->oFaceAnglePitch > 0x0) {
                pitch =  pitchSpeed;
            }

            if (o->oFaceAngleRoll < 0x0) {
                roll = -rollSpeed;
            } else if (o->oFaceAngleRoll > 0x0) {
                roll = rollSpeed;
            }
        }

        c = coss(o->oFaceAnglePitch);
        s = sins(o->oFaceAnglePitch);
        nz = pitch * c + yaw * s;
        ny = yaw * c - pitch * s;

        c = coss(o->oFaceAngleRoll);
        s = sins(o->oFaceAngleRoll);
        nx = roll * c + ny * s;
        ny = ny * c - roll * s;

        c = coss(o->oFaceAngleYaw);
        s = sins(o->oFaceAngleYaw);
        px = nx * c - nz * s;
        nz = nz * c + nx * s;

        nx = roll * c - pitch * s;
        pz = pitch * c + roll * s;

        o->oPosX = o->oHomeX - nx + px;
        o->oGraphYOffset = yaw - ny;
        o->oPosZ = o->oHomeZ + pz - nz;
    }
}

static void obj_rotate_yaw_and_bounce_off_walls(s16 targetYaw, s16 turnAmount) {
    if (o->oMoveFlags & OBJ_MOVE_HIT_WALL) {
        targetYaw = cur_obj_reflect_move_angle_off_wall();
    }
    cur_obj_rotate_yaw_toward(targetYaw, turnAmount);
}

static s16 obj_get_pitch_to_home(f32 latDistToHome) {
    return atan2s(latDistToHome, o->oPosY - o->oHomeY);
}

static void obj_compute_vel_from_move_pitch(f32 speed) {
    o->oForwardVel = speed * coss(o->oMoveAnglePitch);
    o->oVelY = speed * -sins(o->oMoveAnglePitch);
}

static s32 clamp_s16(s16 *value, s16 minimum, s16 maximum) {
    if (*value <= minimum) {
        *value = minimum;
    } else if (*value >= maximum) {
        *value = maximum;
    } else {
        return FALSE;
    }

    return TRUE;
}

static s32 clamp_f32(f32 *value, f32 minimum, f32 maximum) {
    if (*value <= minimum) {
        *value = minimum;
    } else if (*value >= maximum) {
        *value = maximum;
    } else {
        return FALSE;
    }

    return TRUE;
}

static void cur_obj_init_anim_extend(s32 animIndex) {
    cur_obj_init_animation_with_sound(animIndex);
    cur_obj_extend_animation_if_at_end();
}

static s32 cur_obj_init_anim_and_check_if_end(s32 animIndex) {
    cur_obj_init_animation_with_sound(animIndex);
    return cur_obj_check_if_near_animation_end();
}

static s32 cur_obj_init_anim_check_frame(s32 animIndex, s32 frame) {
    cur_obj_init_animation_with_sound(animIndex);
    return cur_obj_check_anim_frame(frame);
}

static s32 cur_obj_set_anim_if_at_end(s32 animIndex) {
    if (cur_obj_check_if_at_animation_end()) {
        cur_obj_init_animation_with_sound(animIndex);
        return TRUE;
    }
    return FALSE;
}

static s32 cur_obj_play_sound_at_anim_range(s8 startFrame1, s8 startFrame2, u32 sound) {
    s32 rangeLength = o->header.gfx.animInfo.animAccel / 0x10000;

    if (rangeLength <= 0) {
        rangeLength = 1;
    }

    if (cur_obj_check_anim_frame_in_range(startFrame1, rangeLength) || cur_obj_check_anim_frame_in_range(startFrame2, rangeLength)) {
        cur_obj_play_sound_2(sound);
        return TRUE;
    }

    return FALSE;
}

static s16 obj_turn_pitch_toward_mario(f32 targetOffsetY, s16 turnAmount) {
    s16 targetPitch;

    o->oPosY -= targetOffsetY;
    targetPitch = obj_turn_toward_object(o, gMarioObject, O_MOVE_ANGLE_PITCH_INDEX, turnAmount);
    o->oPosY += targetOffsetY;

    return targetPitch;
}

static s32 approach_f32_ptr(f32 *px, f32 target, f32 delta) {
    if (*px > target) {
        delta = -delta;
    }

    *px += delta;

    if ((*px - target) * delta >= 0) {
        *px = target;
        return TRUE;
    }
    return FALSE;
}

static s32 obj_forward_vel_approach(f32 target, f32 delta) {
    return approach_f32_ptr(&o->oForwardVel, target, delta);
}

static s32 obj_y_vel_approach(f32 target, f32 delta) {
    return approach_f32_ptr(&o->oVelY, target, delta);
}

static s32 obj_move_pitch_approach(s16 target, s16 delta) {
    o->oMoveAnglePitch = approach_s16_symmetric(o->oMoveAnglePitch, target, delta);

    if ((s16) o->oMoveAnglePitch == target) {
        return TRUE;
    }

    return FALSE;
}

static s32 obj_face_pitch_approach(s16 targetPitch, s16 deltaPitch) {
    o->oFaceAnglePitch = approach_s16_symmetric(o->oFaceAnglePitch, targetPitch, deltaPitch);

    if ((s16) o->oFaceAnglePitch == targetPitch) {
        return TRUE;
    }

    return FALSE;
}

static s32 obj_face_yaw_approach(s16 targetYaw, s16 deltaYaw) {
    o->oFaceAngleYaw = approach_s16_symmetric(o->oFaceAngleYaw, targetYaw, deltaYaw);

    if ((s16) o->oFaceAngleYaw == targetYaw) {
        return TRUE;
    }

    return FALSE;
}

static s32 obj_face_roll_approach(s16 targetRoll, s16 deltaRoll) {
    o->oFaceAngleRoll = approach_s16_symmetric(o->oFaceAngleRoll, targetRoll, deltaRoll);

    if ((s16) o->oFaceAngleRoll == targetRoll) {
        return TRUE;
    }

    return FALSE;
}

static s32 obj_smooth_turn(s16 *angleVel, s32 *angle, s16 targetAngle, f32 targetSpeedProportion,
                           s16 accel, s16 minSpeed, s16 maxSpeed) {
    s16 currentSpeed;
    s16 currentAngle = (s16)(*angle);

    *angleVel = approach_s16_symmetric(*angleVel, (targetAngle - currentAngle) * targetSpeedProportion, accel);

    currentSpeed = abss(*angleVel);
    clamp_s16(&currentSpeed, minSpeed, maxSpeed);

    *angle = approach_angle(*angle, targetAngle, currentSpeed);
    return (s16)(*angle) == targetAngle;
}

static void obj_roll_to_match_yaw_turn(s16 targetYaw, s16 maxRoll, s16 rollSpeed) {
    s16 targetRoll = o->oMoveAngleYaw - targetYaw;
    clamp_s16(&targetRoll, -maxRoll, maxRoll);
    obj_face_roll_approach(targetRoll, rollSpeed);
}

static s16 random_linear_offset(s16 base, s16 range) {
    return base + (s16)(range * random_float());
}

static s16 random_mod_offset(s16 base, s16 step, s16 mod) {
    return base + step * (random_u16() % mod);
}

static s16 obj_random_fixed_turn(s16 delta) {
    return o->oMoveAngleYaw + (s16) random_sign() * delta;
}

/**
 * Begin by increasing the object's scale by *scaleVel, and slowly decreasing
 * scaleVel. Once the object starts to shrink, wait a bit, and then begin to
 * scale the object toward endScale. The first time it reaches below
 * shootFireScale during this time, return 1.
 * Return -1 once it's reached endScale.
 */
static s32 obj_grow_then_shrink(f32 *scaleVel, f32 shootFireScale, f32 endScale) {
    if (o->oTimer < 2) {
        o->header.gfx.scale[0] += *scaleVel;

        if ((*scaleVel -= 0.01f) > -0.03f) {
            o->oTimer = 0;
        }
    } else if (o->oTimer > 10) {
        if (approach_f32_ptr(&o->header.gfx.scale[0], endScale, 0.05f)) {
            return -1;
        } else if (*scaleVel != 0.0f && o->header.gfx.scale[0] < shootFireScale) {
            *scaleVel = 0.0f;
            return 1;
        }
    }

    return 0;
}

static s32 oscillate_toward(s32 *value, f32 *vel, s32 target, f32 velCloseToZero, f32 accel, f32 slowdown) {
    s32 startValue = *value;
    *value += (s32) *vel;

    if (*value == target
        || ((*value - target) * (startValue - target) < 0 && *vel > -velCloseToZero
            && *vel < velCloseToZero)) {
        *value = target;
        *vel = 0.0f;
        return TRUE;
    } else {
        if (*value >= target) {
            accel = -accel;
        }
        if (*vel * accel < 0.0f) {
            accel *= slowdown;
        }

        *vel += accel;
    }

    return FALSE;
}

static void obj_update_blinking(s32 *blinkTimer, s16 baseCycleLength, s16 cycleLengthRange,
                                s16 blinkLength) {
    if (*blinkTimer != 0) {
        (*blinkTimer)--;
    } else {
        *blinkTimer = random_linear_offset(baseCycleLength, cycleLengthRange);
    }

    if (*blinkTimer > blinkLength) {
        o->oAnimState = OBJ_BLINKING_ANIM_STATE_EYES_OPEN;
    } else {
        o->oAnimState = OBJ_BLINKING_ANIM_STATE_EYES_CLOSED;
    }
}

static s32 obj_resolve_object_collisions(s32 *targetYaw) {
    struct Object *otherObject;
    f32 dx, dz;
    s16 angle;
    f32 radius, otherRadius, relativeRadius;

    if (o->numCollidedObjs != 0) {
        s32 i;
        for (i = 0; i < o->numCollidedObjs; i++) {
            otherObject = o->collidedObjs[i];
            if (otherObject == gMarioObject) continue;
            if (otherObject->oInteractType & INTERACT_MASK_NO_OBJ_COLLISIONS) continue;

            dx = o->oPosX - otherObject->oPosX;
            dz = o->oPosZ - otherObject->oPosZ;

            radius = o->hurtboxRadius > 0 ? o->hurtboxRadius : o->hitboxRadius;
            otherRadius = otherObject->hurtboxRadius > 0 ? otherObject->hurtboxRadius : otherObject->hitboxRadius;
            relativeRadius = radius + otherRadius;

            if ((sqr(dx) + sqr(dz)) > sqr(relativeRadius)) continue;
            angle    = atan2s(dz, dx);
            o->oPosX = otherObject->oPosX + (relativeRadius * sins(angle));
            o->oPosZ = otherObject->oPosZ + (relativeRadius * coss(angle));

            if (targetYaw != NULL && abs_angle_diff(o->oMoveAngleYaw, angle) < 0x4000) {
                *targetYaw = (s16)(angle - o->oMoveAngleYaw + angle + 0x8000);
                return TRUE;
            }
        }
    }

    return FALSE;
}

static s32 obj_bounce_off_walls_edges_objects(s32 *targetYaw) {
    if (o->oMoveFlags & OBJ_MOVE_HIT_WALL) {
        *targetYaw = cur_obj_reflect_move_angle_off_wall();
    } else if (o->oMoveFlags & OBJ_MOVE_HIT_EDGE) {
        *targetYaw = (s16)(o->oMoveAngleYaw + 0x8000);
    } else if (!obj_resolve_object_collisions(targetYaw)) {
        return FALSE;
    }

    return TRUE;
}

static s32 obj_resolve_collisions_and_turn(s16 targetYaw, s16 turnSpeed) {
    obj_resolve_object_collisions(NULL);

    if (cur_obj_rotate_yaw_toward(targetYaw, turnSpeed)) {
        return FALSE;
    } else {
        return TRUE;
    }
}

static void obj_die_if_health_non_positive(void) {
    if (o->oHealth <= 0) {
        if (o->oDeathSound == 0) {
            spawn_mist_particles_with_sound(SOUND_OBJ_DEFAULT_DEATH);
        } else if (o->oDeathSound > 0) {
            spawn_mist_particles_with_sound(o->oDeathSound);
        } else {
            spawn_mist_particles();
        }

        if ((s32)o->oNumLootCoins < 0) {
            spawn_object(o, MODEL_BLUE_COIN, bhvMrIBlueCoin);
        } else {
            obj_spawn_loot_yellow_coins(o, o->oNumLootCoins, 20.0f);
        }
        // This doesn't do anything
        obj_spawn_loot_yellow_coins(o, o->oNumLootCoins, 20.0f);

        if (o->oHealth < 0) {
            cur_obj_hide();
            cur_obj_become_intangible();
        } else {
            obj_mark_for_deletion(o);
        }
    }
}

UNUSED static void obj_unused_die(void) {
    o->oHealth = 0;
    obj_die_if_health_non_positive();
}

static void obj_set_knockback_action(s32 attackType) {
    switch (attackType) {
        case ATTACK_KICK_OR_TRIP:
        case ATTACK_FAST_ATTACK:
            o->oAction = OBJ_ACT_VERTICAL_KNOCKBACK;
            o->oForwardVel = 20.0f;
            o->oVelY = 50.0f;
            break;

        default:
            o->oAction = OBJ_ACT_HORIZONTAL_KNOCKBACK;
            o->oForwardVel = 50.0f;
            o->oVelY = 30.0f;
            break;
    }

    o->oFlags &= ~OBJ_FLAG_SET_FACE_YAW_TO_MOVE_YAW;
    o->oMoveAngleYaw = obj_angle_to_object(gMarioObject, o);
}

static void obj_set_squished_action(void) {
    cur_obj_play_sound_2(SOUND_OBJ_STOMPED);
    o->oAction = OBJ_ACT_SQUISHED;
}

static s32 obj_die_if_above_lava_and_health_non_positive(void) {
    if (o->oMoveFlags & OBJ_MOVE_MASK_IN_WATER) {
        if (o->oGravity + o->oBuoyancy > 0.0f
            || find_water_level(o->oPosX, o->oPosZ) - o->oPosY < 10.0f) {
            return FALSE;
        }
        obj_die_if_health_non_positive();
        return TRUE;

    } else if (o->oMoveFlags & OBJ_MOVE_ABOVE_LAVA) {
        if (o->oMoveFlags & (OBJ_MOVE_ON_GROUND | OBJ_MOVE_LANDED)) {
            obj_die_if_health_non_positive();
            return TRUE;
        }
    }

    if (o->oMoveFlags & OBJ_MOVE_ENTERED_WATER) {
        if (o->oWallHitboxRadius < 200.0f) {
            cur_obj_play_sound_2(SOUND_OBJ_DIVING_INTO_WATER);
        } else {
            cur_obj_play_sound_2(SOUND_OBJ_DIVING_IN_WATER);
        }
    }

    return FALSE;
}

static s32 obj_handle_attacks(struct ObjectHitbox *hitbox, s32 attackedMarioAction,
                              u8 *attackHandlers) {
    s32 attackType;

    obj_set_hitbox(o, hitbox);

    //! Die immediately if above lava
    if (obj_die_if_above_lava_and_health_non_positive()) {
        return ATTACK_HANDLER_DIE_IF_HEALTH_NON_POSITIVE;
    } else if (o->oInteractStatus & INT_STATUS_INTERACTED) {
        if (o->oInteractStatus & INT_STATUS_ATTACKED_MARIO) {
            if (o->oAction != attackedMarioAction) {
                o->oAction = attackedMarioAction;
                o->oTimer = 0;
            }
        } else {
            attackType = o->oInteractStatus & INT_STATUS_ATTACK_MASK;

            switch (attackHandlers[attackType - 1]) {
                case ATTACK_HANDLER_NOP:
                    break;

                case ATTACK_HANDLER_DIE_IF_HEALTH_NON_POSITIVE:
                    obj_die_if_health_non_positive();
                    break;

                case ATTACK_HANDLER_KNOCKBACK:
                    obj_set_knockback_action(attackType);
                    break;

                case ATTACK_HANDLER_SQUISHED:
                    obj_set_squished_action();
                    break;

                case ATTACK_HANDLER_SPECIAL_KOOPA_LOSE_SHELL:
                    shelled_koopa_attack_handler(attackType);
                    break;

                case ATTACK_HANDLER_SET_SPEED_TO_ZERO:
                    obj_set_speed_to_zero();
                    break;

                case ATTACK_HANDLER_SPECIAL_WIGGLER_JUMPED_ON:
                    wiggler_jumped_on_attack_handler();
                    break;

                case ATTACK_HANDLER_SPECIAL_HUGE_GOOMBA_WEAKLY_ATTACKED:
                    huge_goomba_weakly_attacked();
                    break;

                case ATTACK_HANDLER_SQUISHED_WITH_BLUE_COIN:
                    o->oNumLootCoins = -1;
                    obj_set_squished_action();
                    break;
            }

            o->oInteractStatus = INT_STATUS_NONE;
            return attackType;
        }
    }

    o->oInteractStatus = INT_STATUS_NONE;
    return ATTACK_HANDLER_NOP;
}

static void obj_act_knockback(UNUSED f32 baseScale) {
    cur_obj_update_floor_and_walls();

    if (o->header.gfx.animInfo.curAnim != NULL) {
        cur_obj_extend_animation_if_at_end();
    }

    //! Dies immediately if above lava
    if ((o->oMoveFlags
         & (OBJ_MOVE_MASK_ON_GROUND | OBJ_MOVE_MASK_IN_WATER | OBJ_MOVE_HIT_WALL | OBJ_MOVE_ABOVE_LAVA))
        || (o->oAction == OBJ_ACT_VERTICAL_KNOCKBACK && o->oTimer >= 9)) {
        obj_die_if_health_non_positive();
    }

    cur_obj_move_standard(-78);
}

static void obj_act_squished(f32 baseScale) {
    f32 targetScaleY = baseScale * 0.3f;

    cur_obj_update_floor_and_walls();

    if (o->header.gfx.animInfo.curAnim != NULL) {
        cur_obj_extend_animation_if_at_end();
    }

    if (approach_f32_ptr(&o->header.gfx.scale[1], targetScaleY, baseScale * 0.14f)) {
        o->header.gfx.scale[0] = o->header.gfx.scale[2] = baseScale * 2.0f - o->header.gfx.scale[1];

        if (o->oTimer >= 16) {
            obj_die_if_health_non_positive();
        }
    }

    o->oForwardVel = 0.0f;
    cur_obj_move_standard(-78);
}

static s32 obj_update_standard_actions(f32 scale) {
    if (o->oAction < 100) {
        return TRUE;
    } else {
        cur_obj_become_intangible();

        switch (o->oAction) {
            case OBJ_ACT_HORIZONTAL_KNOCKBACK:
            case OBJ_ACT_VERTICAL_KNOCKBACK:
                obj_act_knockback(scale);
                break;

            case OBJ_ACT_SQUISHED:
                obj_act_squished(scale);
                break;
        }

        return FALSE;
    }
}

static s32 obj_check_attacks(struct ObjectHitbox *hitbox, s32 attackedMarioAction) {
    s32 attackType;

    obj_set_hitbox(o, hitbox);

    //! Dies immediately if above lava
    if (obj_die_if_above_lava_and_health_non_positive()) {
        return ATTACK_HANDLER_DIE_IF_HEALTH_NON_POSITIVE;
    } else if (o->oInteractStatus & INT_STATUS_INTERACTED) {
        if (o->oInteractStatus & INT_STATUS_ATTACKED_MARIO) {
            if (o->oAction != attackedMarioAction) {
                o->oAction = attackedMarioAction;
                o->oTimer = 0;
            }
        } else {
            attackType = o->oInteractStatus & INT_STATUS_ATTACK_MASK;
            obj_die_if_health_non_positive();
            o->oInteractStatus = INT_STATUS_NONE;
            return attackType;
        }
    }

    o->oInteractStatus = INT_STATUS_NONE;
    return ATTACK_HANDLER_NOP;
}

static s32 obj_move_for_one_second(s32 endAction) {
    cur_obj_update_floor_and_walls();
    cur_obj_extend_animation_if_at_end();

    if (o->oTimer > 30) {
        o->oAction = endAction;
        return TRUE;
    }

    cur_obj_move_standard(-78);
    return FALSE;
}

/**
 * If we are far from home (> threshold away), then set oAngleToMario to the
 * angle to home and oDistanceToMario to 25000.
 * If we are close to home, but Mario is far from us (> threshold away), then
 * keep oAngleToMario the same and set oDistanceToMario to 20000.
 * If we are close to both home and Mario, then keep both oAngleToMario and
 * oDistanceToMario the same.
 *
 * The point of this function is to avoid having to write extra code to get
 * the object to return to home. When Mario is far away and the object is far
 * from home, it could theoretically re-use the "approach Mario" logic to approach
 * its home instead.
 * However, most objects that use this function handle the far-from-home case
 * separately anyway.
 * This function causes seemingly erroneous behavior in some objects that try to
 * attack Mario (e.g. fly guy shooting fire or lunging), especially when combined
 * with partial updates.
 */
static void treat_far_home_as_mario(f32 threshold) {
    Vec3f d;
    vec3f_diff(d, &o->oHomeVec, &o->oPosVec);

    if (vec3_sumsq(d) > sqr(threshold)) {
        o->oAngleToMario = atan2s(d[2], d[0]);
        o->oDistanceToMario = 25000.0f;
    } else {
        if (!gMarioObject) {
            o->oDistanceToMario = 20000.0f;
            return;
        }

        vec3f_diff(d, &o->oHomeVec, &gMarioObject->oPosVec);
        if (vec3_sumsq(d) > sqr(threshold)) {
            o->oDistanceToMario = 20000.0f;
        }
    }
}

#include "behaviors/koopa.inc.c" // TODO: Text arg field name
#include "behaviors/pokey.inc.c"
#include "behaviors/swoop.inc.c"
#include "behaviors/fly_guy.inc.c"
#include "behaviors/goomba.inc.c"
#include "behaviors/chain_chomp.inc.c" // TODO: chain_chomp_sub_act_lunge documentation
#include "behaviors/wiggler.inc.c"     // TODO
#include "behaviors/spiny.inc.c"
#include "behaviors/enemy_lakitu.inc.c" // TODO
#include "behaviors/cloud.inc.c"
#include "behaviors/camera_lakitu.inc.c" // TODO: 104 label, follow cam documentation
#include "behaviors/monty_mole.inc.c"    // TODO
#include "behaviors/platform_on_track.inc.c"
#include "behaviors/seesaw_platform.inc.c"
#include "behaviors/ferris_wheel.inc.c"
#include "behaviors/water_bomb.inc.c" // TODO: Shadow position
#include "behaviors/ttc_rotating_solid.inc.c"
#include "behaviors/ttc_pendulum.inc.c"
#include "behaviors/ttc_treadmill.inc.c" // TODO
#include "behaviors/ttc_moving_bar.inc.c"
#include "behaviors/ttc_cog.inc.c"
#include "behaviors/ttc_pit_block.inc.c"
#include "behaviors/ttc_elevator.inc.c"
#include "behaviors/ttc_2d_rotator.inc.c"
#include "behaviors/ttc_spinner.inc.c"
#include "behaviors/mr_blizzard.inc.c"
#include "behaviors/sliding_platform_2.inc.c"
#include "behaviors/rotating_octagonal_plat.inc.c"
#include "behaviors/animated_floor_switch.inc.c"
#include "behaviors/activated_bf_plat.inc.c"
#include "behaviors/recovery_heart.inc.c"
#include "behaviors/water_bomb_cannon.inc.c"
#include "behaviors/unagi.inc.c"
#include "behaviors/dorrie.inc.c"
#include "behaviors/haunted_chair.inc.c"
#include "behaviors/mad_piano.inc.c"
#include "behaviors/flying_bookend_switch.inc.c"

/**
 * Used by bowser, fly guy, piranha plant, and fire spitters.
 */
void obj_spit_fire(s16 relativePosX, s16 relativePosY, s16 relativePosZ, f32 scale, ModelID32 model,
                   f32 startSpeed, f32 endSpeed, s16 movePitch) {
    struct Object *obj = spawn_object_relative_with_scale(MOVING_FLAME_BP_MOVE, relativePosX, relativePosY, relativePosZ,
                                                           scale, o, model, bhvMovingFlame);

    if (obj != NULL) {
        obj->oSmallPiranhaFlameStartSpeed = startSpeed;
        obj->oSmallPiranhaFlameEndSpeed = endSpeed;
        obj->oSmallPiranhaFlameModel = model;
        obj->oMoveAnglePitch = movePitch;
    }
}

#include "behaviors/fire_piranha_plant.inc.c"
#include "behaviors/fire_spitter.inc.c"
#include "behaviors/flame.inc.c"
#include "behaviors/snufit.inc.c"
#include "behaviors/horizontal_grindel.inc.c"
#include "behaviors/eyerok.inc.c"
#include "behaviors/klepto.inc.c"
#include "behaviors/bird.inc.c"
#include "behaviors/racing_penguin.inc.c"
#include "behaviors/coffin.inc.c"
#include "behaviors/clam.inc.c"
#include "behaviors/skeeter.inc.c"
#include "behaviors/swing_platform.inc.c"
#include "behaviors/donut_platform.inc.c"
#include "behaviors/ddd_pole.inc.c"
#include "behaviors/reds_star_marker.inc.c"
#include "behaviors/triplet_butterfly.inc.c"
#include "behaviors/bubba.inc.c"
