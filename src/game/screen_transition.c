#include <PR/ultratypes.h>
#include <PR/gbi.h>

#include "area.h"
#include "camera.h"
#include "config/config_rom.h"
#include "engine/graph_node.h"
#include "engine/math_util.h"
#include "game/game_init.h"
#include "game/ingame_menu.h"
#include "geo_misc.h"
#include "gfx_dimensions.h"
#include "memory.h"
#include "screen_transition.h"
#include "segment2.h"
#include "sm64.h"
#include "types.h"

u8 sTransitionFadeTimer = 0;
u16 sTransitionTextureAngle = 0;

void *sTextureTransitionID[] = {
    texture_transition_star_half,
    texture_transition_circle_half,
    texture_transition_mario,
    texture_transition_bowser_half,
};


s32 set_and_reset_transition_fade_timer(u8 transTime) {
    sTransitionFadeTimer++;

    if (sTransitionFadeTimer >= transTime) {
        sTransitionFadeTimer = 0;
        sTransitionTextureAngle = 0;
        return TRUE;
    }

    return FALSE;
}

void make_tex_transition_vertex(Vtx *verts, s32 n, f32 centerTransX, f32 centerTransY,
                                f32 vertX, f32 vertY, s16 tx, s16 ty) {
    // Rotate around the center
    s16 angle = sTransitionTextureAngle;
    f32 x = vertX * coss(angle) - vertY * sins(angle) + centerTransX;
    f32 y = vertX * sins(angle) + vertY * coss(angle) + centerTransY;

    s16 roundedX = roundf(x);
    s16 roundedY = roundf(y);

    make_simple_vertex(verts, n, roundedX, roundedY, -1, tx * 32, ty * 32);
}

#define SOLID_COL_RADIUS 2000
void make_tex_transition_vertices(Vtx *verts, f32 centerTransX, f32 centerTransY, f32 texTransRadius, 
                                  s8 transTexType) {
    
    s16 leftUV, rightUV, downUV, upUV;

    if (transTexType == TRANS_TYPE_MIRROR) {
        leftUV = -31;
        rightUV = 31;
        downUV = 0;
        upUV = 63;
    }
    else {
        leftUV = 0;
        rightUV = 63;
        downUV = 0;
        upUV = 63;
    }

    // Shape texture
    make_tex_transition_vertex(verts, 0, centerTransX, centerTransY, -texTransRadius, -texTransRadius, leftUV, upUV);
    make_tex_transition_vertex(verts, 1, centerTransX, centerTransY, texTransRadius, -texTransRadius, rightUV, upUV);
    make_tex_transition_vertex(verts, 2, centerTransX, centerTransY, texTransRadius, texTransRadius, rightUV, downUV);
    make_tex_transition_vertex(verts, 3, centerTransX, centerTransY, -texTransRadius, texTransRadius, leftUV, downUV);

    // Solid color
    make_tex_transition_vertex(verts, 4, centerTransX, centerTransY, -SOLID_COL_RADIUS, -SOLID_COL_RADIUS, 0, 0);
    make_tex_transition_vertex(verts, 5, centerTransX, centerTransY, SOLID_COL_RADIUS, -SOLID_COL_RADIUS, 0, 0);
    make_tex_transition_vertex(verts, 6, centerTransX, centerTransY, SOLID_COL_RADIUS, SOLID_COL_RADIUS, 0, 0);
    make_tex_transition_vertex(verts, 7, centerTransX, centerTransY, -SOLID_COL_RADIUS, SOLID_COL_RADIUS, 0, 0);
}

f32 calc_tex_transition_radius(u8 transTime, struct WarpTransitionData *transData) {
    f32 amount = (f32) sTransitionFadeTimer / (f32) (transTime - 1);

#ifdef POLISHED_TRANSITIONS
    return smoothstep(transData->startTexRadius, transData->endTexRadius, amount);
#else
    return lerpf(transData->startTexRadius, transData->endTexRadius, amount);
#endif

}

f32 center_tex_transition_x(struct WarpTransitionData *transData, f32 posDistance, u16 texTransDir) {
    f32 x = transData->startTexX + coss(texTransDir) * posDistance;

    return x;
}

f32 center_tex_transition_y(struct WarpTransitionData *transData, f32 posDistance, u16 texTransDir) {
    f32 y = transData->startTexY + sins(texTransDir) * posDistance;

    return y;
}

f32 calc_tex_transition_pos_distance(s8 transTime, struct WarpTransitionData *transData) {
    f32 startX = transData->startTexX;
    f32 startY = transData->startTexY;
    f32 endX = transData->endTexX;
    f32 endY = transData->endTexY;

    f32 distance = sqrtf(sqr(startX - endX) + sqr(startY - endY));

    f32 amount = (f32) sTransitionFadeTimer / (f32)(transTime - 1);

    return distance * amount;
}

u16 calc_tex_transition_direction(struct WarpTransitionData *transData) {
    f32 x = transData->endTexX - transData->startTexX;
    f32 y = transData->endTexY - transData->startTexY;

    return atan2s(x, y);
}

/*
 * Called during render_screen_transition.
 * Handles shape transitions (such as the star, circle and Mario and bowser´s heads).
 */
s32 render_textured_transition(u8 transTime, struct WarpTransitionData *transData, s8 texID, s8 transTexType) {
    u16 texTransDir = calc_tex_transition_direction(transData);
    f32 posDistance = calc_tex_transition_pos_distance(transTime, transData);
    f32 centerTransX = center_tex_transition_x(transData, posDistance, texTransDir);
    f32 centerTransY = center_tex_transition_y(transData, posDistance, texTransDir);

    f32 texTransRadius = calc_tex_transition_radius(transTime, transData);
    Vtx *verts = alloc_display_list(8 * sizeof(Vtx));

    if (verts != NULL) {
        Gfx *tempGfxHead = gDisplayListHead;

        make_tex_transition_vertices(verts, centerTransX, centerTransY, texTransRadius, transTexType);
        gSPDisplayList(tempGfxHead++, dl_proj_mtx_fullscreen);

        u8 r = transData->red;
        u8 g = transData->green;
        u8 b = transData->blue;
        gDPSetPrimColor(tempGfxHead++, 0, 0, r, g, b, 255);

        gDPSetCombineMode(tempGfxHead++, G_CC_PRIMITIVE, G_CC_PRIMITIVE);
        gDPSetRenderMode(tempGfxHead++, G_RM_AA_OPA_SURF, G_RM_AA_OPA_SURF2);

        gSPVertex(tempGfxHead++, VIRTUAL_TO_PHYSICAL(verts), 8, 0);
        gSPDisplayList(tempGfxHead++, dl_transition_draw_filled_region);
        gDPPipeSync(tempGfxHead++);

        gDPSetCombineLERP(tempGfxHead++, 0, 0, 0, PRIMITIVE, 0, 0, 0, TEXEL0, 
                                              0, 0, 0, PRIMITIVE, 0, 0, 0, TEXEL0);

        gDPSetRenderMode(tempGfxHead++, G_RM_AA_XLU_SURF, G_RM_AA_XLU_SURF2);
        gDPSetTextureFilter(tempGfxHead++, G_TF_BILERP);

        switch (transTexType) {
            case TRANS_TYPE_MIRROR:
                gDPLoadTextureBlock(tempGfxHead++, sTextureTransitionID[texID], G_IM_FMT_IA, G_IM_SIZ_8b, 32, 64, 0,
                    G_TX_WRAP | G_TX_MIRROR, G_TX_WRAP | G_TX_MIRROR, 5, 6, G_TX_NOLOD, G_TX_NOLOD);
                break;
            case TRANS_TYPE_CLAMP:
                gDPLoadTextureBlock(tempGfxHead++, sTextureTransitionID[texID], G_IM_FMT_IA, G_IM_SIZ_8b, 64, 64, 0,
                    G_TX_CLAMP, G_TX_CLAMP, 6, 6, G_TX_NOLOD, G_TX_NOLOD);
                break;
        }
        gSPTexture(tempGfxHead++, 0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON);
        gSPVertex(tempGfxHead++, VIRTUAL_TO_PHYSICAL(verts), 4, 0);
        gSPDisplayList(tempGfxHead++, dl_draw_quad_verts_0123);
        gSPTexture(tempGfxHead++, 0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF);
        gSPDisplayList(tempGfxHead++, dl_screen_transition_end);
        sTransitionTextureAngle += transData->angleSpeed;

        gDisplayListHead = tempGfxHead;
    }

#if defined(F3DEX_GBI_3) && defined(F3DEX3_FB_MEMCLEAR) // if anyone know why i have to do this please explain
    if (sTransitionFadeTimer + 1 >= transTime && transData->endTexRadius < transData->startTexRadius) {
        gSPMemset(gDisplayListHead++, gPhysicalFramebuffers[0], GPACK_RGBA5551(transData->red, transData->green, transData->blue, 1), SCREEN_WIDTH * SCREEN_HEIGHT * 2);
        gSPMemset(gDisplayListHead++, gPhysicalFramebuffers[1], GPACK_RGBA5551(transData->red, transData->green, transData->blue, 1), SCREEN_WIDTH * SCREEN_HEIGHT * 2);
        gSPMemset(gDisplayListHead++, gPhysicalFramebuffers[2], GPACK_RGBA5551(transData->red, transData->green, transData->blue, 1), SCREEN_WIDTH * SCREEN_HEIGHT * 2);
    }
#endif

    return set_and_reset_transition_fade_timer(transTime);
}

Vtx *vertex_transition_color() {
    Vtx *verts = alloc_display_list(4 * sizeof(Vtx));

    if (verts != NULL) {
        make_simple_vertex(verts, 0, GFX_DIMENSIONS_FROM_LEFT_EDGE(0), 0, -1, 0, 0);
        make_simple_vertex(verts, 1, GFX_DIMENSIONS_FROM_RIGHT_EDGE(0), 0, -1, 0, 0);
        make_simple_vertex(verts, 2, GFX_DIMENSIONS_FROM_RIGHT_EDGE(0), SCREEN_HEIGHT, -1, 0, 0);
        make_simple_vertex(verts, 3, GFX_DIMENSIONS_FROM_LEFT_EDGE(0), SCREEN_HEIGHT, -1, 0, 0);
    }
    return verts;
}

s32 dl_transition_color(u8 transTime, struct WarpTransitionData *transData, u8 alpha) {
    Vtx *verts = vertex_transition_color();

    if (verts != NULL) {
        Gfx *tempGfxHead = gDisplayListHead;

        u8 r = transData->red;
        u8 g = transData->green;
        u8 b = transData->blue;
        gDPSetPrimColor(tempGfxHead++, 0, 0, r, g, b, alpha);

        gSPDisplayList(tempGfxHead++, dl_proj_mtx_fullscreen);
        gDPSetCombineMode(tempGfxHead++, G_CC_PRIMITIVE, G_CC_PRIMITIVE);
        gDPSetRenderMode(tempGfxHead++, G_RM_AA_XLU_SURF, G_RM_AA_XLU_SURF2);
        gSPVertex(tempGfxHead++, VIRTUAL_TO_PHYSICAL(verts), 4, 0);
        gSPDisplayList(tempGfxHead++, dl_draw_quad_verts_0123);
        gSPDisplayList(tempGfxHead++, dl_screen_transition_end);

        gDisplayListHead = tempGfxHead;
    }
    return set_and_reset_transition_fade_timer(transTime);
}

u8 set_transition_color_fade_alpha(s8 fadeType, u8 transTime) {
    u8 time = 0;
    f32 amount = (f32) sTransitionFadeTimer / (f32) (transTime - 1);
    switch (fadeType) {
        case COLOR_TRANS_FADE_INTO_COLOR:
            time = lerpf(0.f, 255.0f, amount);
            break;
        case COLOR_TRANS_FADE_FROM_COLOR:
            time = lerpf(255.0f, 0.f, amount);
            break;
    }
    return roundf(time);
}

s32 render_fade_transition_from_color(u8 transTime, struct WarpTransitionData *transData) {
    u8 alpha = set_transition_color_fade_alpha(COLOR_TRANS_FADE_FROM_COLOR, transTime);
    return dl_transition_color(transTime, transData, alpha);
}

s32 render_fade_transition_into_color(u8 transTime, struct WarpTransitionData *transData) {
    u8 alpha = set_transition_color_fade_alpha(COLOR_TRANS_FADE_INTO_COLOR, transTime);

#if defined(F3DEX_GBI_3) && defined(F3DEX3_FB_MEMCLEAR) // if anyone know why i have to do this please explain (ditto)
    if (alpha == 255) {
        gSPMemset(gDisplayListHead++, gPhysicalFramebuffers[0], GPACK_RGBA5551(transData->red, transData->green, transData->blue, 1), SCREEN_WIDTH * SCREEN_HEIGHT * 2);
        gSPMemset(gDisplayListHead++, gPhysicalFramebuffers[1], GPACK_RGBA5551(transData->red, transData->green, transData->blue, 1), SCREEN_WIDTH * SCREEN_HEIGHT * 2);
        gSPMemset(gDisplayListHead++, gPhysicalFramebuffers[2], GPACK_RGBA5551(transData->red, transData->green, transData->blue, 1), SCREEN_WIDTH * SCREEN_HEIGHT * 2);
    }
#endif

    return dl_transition_color(transTime, transData, alpha);
}

/*
 * Called during rendering if gWarpTransition.isActive is on.
 * Handles solid colour transitions and shape transitions 
 * (such as the star, circle and Mario and bowser´s heads).
 */
s32 render_screen_transition(s8 transType, u8 transTime, struct WarpTransitionData *transData) {
    switch (transType) {
        case WARP_TRANSITION_FADE_FROM_COLOR:
            return render_fade_transition_from_color(transTime, transData);
            break;
        case WARP_TRANSITION_FADE_INTO_COLOR:
            return render_fade_transition_into_color(transTime, transData);
            break;
        
        case WARP_TRANSITION_FADE_FROM_STAR:
        case WARP_TRANSITION_FADE_INTO_STAR:
            return render_textured_transition(transTime, transData, TEX_TRANS_STAR, TRANS_TYPE_MIRROR);
            break;
        
        case WARP_TRANSITION_FADE_FROM_CIRCLE:
        case WARP_TRANSITION_FADE_INTO_CIRCLE:
            return render_textured_transition(transTime, transData, TEX_TRANS_CIRCLE, TRANS_TYPE_MIRROR);
            break;
        
        case WARP_TRANSITION_FADE_FROM_MARIO:
        case WARP_TRANSITION_FADE_INTO_MARIO:
            return render_textured_transition(transTime, transData, TEX_TRANS_MARIO, TRANS_TYPE_CLAMP);
            break;
        
        case WARP_TRANSITION_FADE_FROM_BOWSER:
        case WARP_TRANSITION_FADE_INTO_BOWSER:
            return render_textured_transition(transTime, transData, TEX_TRANS_BOWSER, TRANS_TYPE_MIRROR);
            break;
    }

    return FALSE;
}


Gfx *render_cannon_circle_base(void) {
#ifdef WIDESCREEN
    Vtx *verts = alloc_display_list(8 * sizeof(Vtx));
    Gfx *dlist = alloc_display_list(20 * sizeof(Gfx));
#else
    Vtx *verts = alloc_display_list(4 * sizeof(Vtx));
    Gfx *dlist = alloc_display_list(16 * sizeof(Gfx));
#endif
    Gfx *g = dlist;

    if (verts != NULL && dlist != NULL) {
        gDPSetPrimColor(gDisplayListHead++, 0, 0, 0, 0, 0, 255);

        make_simple_vertex(verts, 0,            0,             0, -1, -1152, 1824);
        make_simple_vertex(verts, 1, SCREEN_WIDTH,             0, -1,  1152, 1824);
        make_simple_vertex(verts, 2, SCREEN_WIDTH, SCREEN_HEIGHT, -1,  1152,  192);
        make_simple_vertex(verts, 3,            0, SCREEN_HEIGHT, -1, -1152,  192);

#ifdef WIDESCREEN
        // Render black rectangles outside the 4:3 area.
        make_simple_vertex(verts, 4, GFX_DIMENSIONS_FROM_LEFT_EDGE(0), 0, -1, 0, 0, 0);
        make_simple_vertex(verts, 5, GFX_DIMENSIONS_FROM_RIGHT_EDGE(0), 0, -1, 0, 0, 0);
        make_simple_vertex(verts, 6, GFX_DIMENSIONS_FROM_RIGHT_EDGE(0), SCREEN_HEIGHT, -1, 0, 0);
        make_simple_vertex(verts, 7, GFX_DIMENSIONS_FROM_LEFT_EDGE(0), SCREEN_HEIGHT, -1, 0, 0);
#endif

        gSPDisplayList(g++, dl_proj_mtx_fullscreen);
        gDPSetCombineMode(g++, G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM);
        gDPSetTextureFilter(g++, G_TF_BILERP);
        gDPLoadTextureBlock(g++, sTextureTransitionID[TEX_TRANS_CIRCLE], G_IM_FMT_IA, G_IM_SIZ_8b, 32, 64, 0,
            G_TX_WRAP | G_TX_MIRROR, G_TX_WRAP | G_TX_MIRROR, 5, 6, G_TX_NOLOD, G_TX_NOLOD);
        gSPTexture(g++, 0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON);
        gSPVertex(g++, VIRTUAL_TO_PHYSICAL(verts), 4, 0);
        gSPDisplayList(g++, dl_draw_quad_verts_0123);
        gSPTexture(g++, 0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF);
#ifdef WIDESCREEN
        gDPSetCombineMode(g++, G_CC_SHADE, G_CC_SHADE);
        gSPVertex(g++, VIRTUAL_TO_PHYSICAL(verts + 4), 4, 4);
        gSP2Triangles(g++, 4, 0, 3, 0x0, 4, 3, 7, 0x0);
        gSP2Triangles(g++, 1, 5, 6, 0x0, 1, 6, 2, 0x0);
#endif
        gSPDisplayList(g++, dl_screen_transition_end);
        gSPEndDisplayList(g);
    } else {
        return NULL;
    }
    return dlist;
}

Gfx *geo_cannon_circle_base(s32 callContext, struct GraphNode *node, UNUSED Mat4 mtx) {
    struct GraphNodeGenerated *graphNode = (struct GraphNodeGenerated *) node;
    Gfx *dlist = NULL;

    if (callContext == GEO_CONTEXT_RENDER && gCurrentArea != NULL
        && gCurrentArea->camera->mode == CAMERA_MODE_INSIDE_CANNON) {
        SET_GRAPH_NODE_LAYER(graphNode->fnNode.node.flags, LAYER_TRANSPARENT);
#ifndef L3DEX2_ALONE
        dlist = render_cannon_circle_base();
#endif
    }
    return dlist;
}
