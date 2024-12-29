
/**
 * Behavior for bhvHiddenBlueCoin and bhvBlueCoinSwitch.
 * bhvHiddenBlueCoin are the stationary blue coins that appear when
 * you press a blue coin switch (a.k.a. bhvBlueCoinSwitch).
 */

/**
 * Update function for bhvHiddenBlueCoin.
 */
void bhv_hidden_blue_coin_loop(void) {
    struct Object *blueCoinSwitch;

    switch (o->oAction) {
        case HIDDEN_BLUE_COIN_ACT_INACTIVE:
            // Become invisible and intangible
            cur_obj_disable_rendering();
            cur_obj_become_intangible();

            // Set action to HIDDEN_BLUE_COIN_ACT_WAITING after the blue coin switch is found.
            o->oHiddenBlueCoinSwitch = cur_obj_nearest_object_with_behavior(bhvBlueCoinSwitch);

            if (o->oHiddenBlueCoinSwitch != NULL) {
                o->oAction = HIDDEN_BLUE_COIN_ACT_WAITING;
            }

            break;

        case HIDDEN_BLUE_COIN_ACT_WAITING:
            // Wait until the blue coin switch starts ticking to activate.
            blueCoinSwitch = o->oHiddenBlueCoinSwitch;

            if (blueCoinSwitch->oAction == BLUE_COIN_SWITCH_ACT_TICKING) {
                o->oAction = HIDDEN_BLUE_COIN_ACT_ACTIVE;
            }

#ifdef BLUE_COIN_SWITCH_PREVIEW
            if (gMarioObject->platform == blueCoinSwitch) {
                cur_obj_enable_rendering();
                o->oOpacity = 159;
            } else {
                cur_obj_disable_rendering();
            }
#endif

            break;

        case HIDDEN_BLUE_COIN_ACT_ACTIVE:
            // Become tangible
            cur_obj_enable_rendering();
            cur_obj_become_tangible();
            o->oOpacity = 255;

            // After 200 frames of waiting and 20 2-frame blinks (for 240 frames total),
            // delete the object.
            if (cur_obj_wait_then_blink(200, 20)) {
#ifdef BLUE_COIN_SWITCH_RETRY
                o->oAction = HIDDEN_BLUE_COIN_ACT_INACTIVE;
                o->header.gfx.node.flags &= ~GRAPH_RENDER_INVISIBLE;
#else
                obj_mark_for_deletion(o);
#endif
            }

            break;
    }

    // Delete the coin once collected
    if (o->oInteractStatus & INT_STATUS_INTERACTED) {
        spawn_object(o, MODEL_SPARKLES, bhvCoinSparklesSpawner);
        obj_mark_for_deletion(o);
    }

    o->oInteractStatus = INT_STATUS_NONE;
}

/**
 * Update function for bhvBlueCoinSwitch.
 */
void bhv_blue_coin_switch_loop(void) {
    // The switch's model is 1/3 size.
    cur_obj_scale(3.0f);

    switch (o->oAction) {
        case BLUE_COIN_SWITCH_ACT_IDLE:
            // If Mario is on the switch and has ground-pounded,
            // recede and get ready to start ticking.
            if (gMarioObject->platform == o) {
                if (gMarioStates[0].action == ACT_GROUND_POUND_LAND) {
                    // Set to BLUE_COIN_SWITCH_ACT_RECEDING
                    o->oAction = BLUE_COIN_SWITCH_ACT_RECEDING;
#ifdef BLUE_COIN_SWITCH_RETRY
                    // Recede at a rate of 16 units/frame.
                    o->oVelY = -16.0f;
#else
                    // Recede at a rate of 20 units/frame.
                    o->oVelY = -20.0f;
#endif
                    // Set gravity to 0 so it doesn't accelerate when receding.
                    o->oGravity = 0.0f;

                    cur_obj_play_sound_2(SOUND_GENERAL_SWITCH_DOOR_OPEN);
                }
            }

            // Have collision
            load_object_collision_model();

            break;

        case BLUE_COIN_SWITCH_ACT_RECEDING:
            // Recede for 6 frames before going invisible and ticking.
            // This is probably an off-by-one error, since the switch is 100 units tall
            // and recedes at 20 units/frame, which means it will fully recede after 5 frames.
#ifdef BLUE_COIN_SWITCH_RETRY
            if (o->oTimer > 3) {
#else
            if (o->oTimer > 5) {
                cur_obj_hide();
#endif
                // Set to BLUE_COIN_SWITCH_ACT_TICKING
                o->oAction = BLUE_COIN_SWITCH_ACT_TICKING;
#ifdef BLUE_COIN_SWITCH_RETRY
                // ???
                o->oVelY    = 0.0f;
                o->oGravity = 0.0f;
#else
                o->oPosY = gMarioObject->oPosY - 40.0f;
#endif

                // Spawn particles. There's a function that calls this same function
                // with the same arguments, spawn_mist_particles, why didn't they just call that?
                spawn_mist_particles_variable(0, 0, 46.0f);
            } else {
                // Have collision while receding
                load_object_collision_model();
                // Recede
                cur_obj_move_using_fvel_and_gravity();
            }

            break;

        case BLUE_COIN_SWITCH_ACT_TICKING:
            // Tick faster when the blue coins start blinking
            if (o->oTimer < 200) {
                play_sound(SOUND_GENERAL2_SWITCH_TICK_FAST, gGlobalSoundSource);
            } else {
                play_sound(SOUND_GENERAL2_SWITCH_TICK_SLOW, gGlobalSoundSource);
            }
#ifdef BLUE_COIN_SWITCH_RETRY
            if (cur_obj_nearest_object_with_behavior(bhvHiddenBlueCoin) == NULL) {
                spawn_mist_particles_variable(0, 0, 46.0f);
                obj_mark_for_deletion(o);
            // Set to BLUE_COIN_SWITCH_ACT_EXTENDING after the coins unload after the 240-frame timer expires.
            } else if (o->oTimer > 240) {
                o->oAction  = BLUE_COIN_SWITCH_ACT_EXTENDING;
                o->oVelY    = 16.0f;
                o->oGravity =  0.0f;
            }
            load_object_collision_model();
            break;
        case BLUE_COIN_SWITCH_ACT_EXTENDING:
            if (cur_obj_nearest_object_with_behavior(bhvHiddenBlueCoin) == NULL) {
                // This code can be executed if the last blue coin is collected at the very end of the timer.
                spawn_mist_particles_variable(0, 0, 46.0f);
                obj_mark_for_deletion(o);
            } else if (o->oTimer > 3) {
                // Set to BLUE_COIN_SWITCH_ACT_IDLE
                o->oAction = BLUE_COIN_SWITCH_ACT_IDLE;
            } else {
                // Have collision while extending
                load_object_collision_model();
                // Extend
                cur_obj_move_using_fvel_and_gravity();
            }
#else
            // Delete the switch (which stops the sound) after the last coin is collected,
            // or after the coins unload after the 240-frame timer expires.
            if ((cur_obj_nearest_object_with_behavior(bhvHiddenBlueCoin) == NULL) || (o->oTimer > 240)) {
                obj_mark_for_deletion(o);
            }
#endif
            break;
    }
}

#ifdef BLUE_COIN_SWITCH_PREVIEW
Gfx *geo_switch_blue_coin_transparency(s32 callContext, struct GraphNode *node, UNUSED void *context) {
    if (callContext == GEO_CONTEXT_RENDER) {
        struct Object *obj = gCurGraphNodeObjectNode;
        struct GraphNodeSwitchCase *switchCase = (struct GraphNodeSwitchCase *) node;
        if (gCurGraphNodeHeldObject != NULL) {
            obj = gCurGraphNodeHeldObject->objNode;
        }

        // Only allow transparency for hidden blue coins, since other behaviors don't set oOpacity to 255
        if (obj->behavior == segmented_to_virtual(bhvHiddenBlueCoin) && obj->oOpacity != 255) {
            switchCase->selectedCase = 1; // transparent
        } else {
            switchCase->selectedCase = 0; // alpha
        }
    }

    return NULL;
}

Gfx *geo_update_blue_coin_transparency(s32 callContext, struct GraphNode *node, UNUSED void *context) {
    Gfx *dlStart = NULL;

    if (callContext == GEO_CONTEXT_RENDER) {
        struct Object *obj = gCurGraphNodeObjectNode;
        struct GraphNodeGenerated *currentGraphNode = (struct GraphNodeGenerated *) node;
        if (gCurGraphNodeHeldObject != NULL) {
            obj = gCurGraphNodeHeldObject->objNode;
        }

        SET_GRAPH_NODE_LAYER(currentGraphNode->fnNode.node.flags, LAYER_TRANSPARENT);

        dlStart = alloc_display_list(sizeof(Gfx) * 2);
        if (dlStart) {
            Gfx *dlHead = dlStart;
            gDPSetEnvColor(dlHead++, 255, 255, 255, obj->oOpacity);
            gSPEndDisplayList(dlHead);
        }
    }

    return dlStart;
}
#endif
