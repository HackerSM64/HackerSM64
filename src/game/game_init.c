#include <ultra64.h>

#include "PR/gbi.h"
#include "config/config_rom.h"
#include "sm64.h"
#include "gfx_dimensions.h"
#include "audio/external.h"
#include "buffers/buffers.h"
#include "buffers/gfx_output_buffer.h"
#include "buffers/framebuffers.h"
#include "buffers/zbuffer.h"
#include "engine/level_script.h"
#include "engine/math_util.h"
#include "game_init.h"
#include "main.h"
#include "memory.h"
#include "save_file.h"
#include "seq_ids.h"
#include "sound_init.h"
#include "print.h"
#include "segment2.h"
#include "segment_symbols.h"
#include "rumble_init.h"
#ifdef SRAM
#include "sram.h"
#endif
#ifdef F3DEX_GBI_3
#include "f3dex3.h"
#endif
#include "puppyprint.h"
#include "puppycam2.h"
#include "debug_box.h"
#include "vc_ultra.h"
#include "profiling.h"
#include "emutest.h"

// Emulators that the Instant Input patch should not be applied to
#define INSTANT_INPUT_BLACKLIST (EMU_CONSOLE | EMU_WIIVC | EMU_ARES | EMU_SIMPLE64 | EMU_CEN64 | DISABLE_INSTANT_INPUT)

// Gfx handlers
struct SPTask *gGfxSPTask;
Gfx *gDisplayListHead;
u8 *gGfxPoolEnd;
struct GfxPool *gGfxPool;

// OS Controllers
struct Controller gControllers[MAXCONTROLLERS];
OSContStatus gControllerStatuses[MAXCONTROLLERS];
OSContPadEx gControllerPads[MAXCONTROLLERS];
u8 gControllerBits = 0b0000;
u8 gBorderHeight;
#ifdef VANILLA_STYLE_CUSTOM_DEBUG
u8 gCustomDebugMode;
#endif
#ifdef EEP
s8 gEepromProbe;
#endif
#ifdef SRAM
s8 gSramProbe;
#endif
OSMesgQueue gGameVblankQueue;
OSMesgQueue gGfxVblankQueue;
OSMesg gGameMesgBuf[1];
OSMesg gGfxMesgBuf[1];

// Vblank Handler
struct VblankHandler gGameVblankHandler;

// Buffers
uintptr_t gPhysicalFramebuffers[3];
uintptr_t gPhysicalZBuffer;

// Mario Anims and Demo allocation
void *gMarioAnimsMemAlloc;
void *gDemoInputsMemAlloc;
struct DmaHandlerList gMarioAnimsBuf;
struct DmaHandlerList gDemoInputsBuf;

// General timer that runs as the game starts
u32 gGlobalTimer = 0;
u8 *gAreaSkyboxStart[AREA_COUNT];
u8 *gAreaSkyboxEnd[AREA_COUNT];

// Framebuffer rendering values (max 3)
u16 sRenderedFramebuffer = 0;
u16 sRenderingFramebuffer = 0;

// Goddard Vblank Function Caller
void (*gGoddardVblankCallback)(void) = NULL;

// Defined player slots. Anything above MAX_NUM_PLAYERS should not be used.
struct Controller* const gPlayer1Controller = &gControllers[0];
struct Controller* const gPlayer2Controller = &gControllers[1];
struct Controller* const gPlayer3Controller = &gControllers[2];
struct Controller* const gPlayer4Controller = &gControllers[3];

// Title Screen Demo Handler
struct DemoInput *gCurrDemoInput = NULL;
u16 gDemoInputListID = 0;
struct DemoInput gRecordedDemoInput = { 0 };

// Display
// ----------------------------------------------------------------------------------------------------

/**
 * Sets the initial RDP (Reality Display Processor) rendering settings.
 */
const Gfx init_rdp[] = {
    gsDPPipeSync(),
    gsDPPipelineMode(G_PM_NPRIMITIVE),

    gsDPSetScissor(G_SC_NON_INTERLACE, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),

    gsDPSetTextureLOD(G_TL_TILE),
    gsDPSetTextureLUT(G_TT_NONE),
    gsDPSetTextureDetail( G_TD_CLAMP),
    gsDPSetTexturePersp(G_TP_PERSP),
    gsDPSetTextureFilter( G_TF_BILERP),
    gsDPSetTextureConvert(G_TC_FILT),

    gsDPSetCombineKey(G_CK_NONE),
    gsDPSetAlphaCompare(G_AC_NONE),
    gsDPSetRenderMode(G_RM_OPA_SURF, G_RM_OPA_SURF2),
    gsDPSetColorDither(G_CD_MAGICSQ),
    gsDPSetCycleType(G_CYC_FILL),
    gsDPSetAlphaDither(G_AD_PATTERN),
    gsSPEndDisplayList(),
};

/**
 * Sets the initial RSP (Reality Signal Processor) settings.
 */
const Gfx init_rsp[] = {
    gsDPPipeSync(),
    gsSPClearGeometryMode(G_CULL_FRONT | G_FOG | G_LIGHTING | G_TEXTURE_GEN | G_TEXTURE_GEN_LINEAR | G_LOD),
    gsSPSetGeometryMode(G_SHADE | G_SHADING_SMOOTH | G_CULL_BACK | G_LIGHTING),
    gsSPTexture(0, 0, 0, G_TX_RENDERTILE, G_OFF),
#ifdef USE_FRUSTRATIO2
    gsSPClipRatio(FRUSTRATIO_2),
#else
    gsSPClipRatio(FRUSTRATIO_1),
#endif
    gsSPEndDisplayList(),
};

/**
 * Initialize the z buffer for the current frame.
 */
void init_z_buffer(s32 resetZB) {
    Gfx *tempGfxHead = gDisplayListHead;

    gDPPipeSync(tempGfxHead++);
    gDPSetDepthSource(tempGfxHead++, G_ZS_PIXEL);
    gDPSetDepthImage(tempGfxHead++, gPhysicalZBuffer);

    if (!resetZB) {
        return;
    }


#if defined(F3DEX_GBI_3) && defined(F3DEX3_FB_MEMCLEAR)
    gSPMemset(tempGfxHead++, gPhysicalZBuffer, GPACK_ZDZ(G_MAXFBZ, 0), SCREEN_WIDTH * SCREEN_HEIGHT * 2);
#else
    gDPSetColorImage(tempGfxHead++, G_IM_FMT_RGBA, G_IM_SIZ_16b, SCREEN_WIDTH, gPhysicalZBuffer);
    gDPSetFillColor(tempGfxHead++,
                    GPACK_ZDZ(G_MAXFBZ, 0) << 16 | GPACK_ZDZ(G_MAXFBZ, 0));

    gDPFillRectangle(tempGfxHead++, 0, gBorderHeight, SCREEN_WIDTH - 1,
                     SCREEN_HEIGHT - 1 - gBorderHeight);
#endif
    gDisplayListHead = tempGfxHead;
}

/**
 * Tells the RDP which of the three framebuffers it shall draw to.
 */
void select_framebuffer(void) {
    Gfx *tempGfxHead = gDisplayListHead;

    gDPPipeSync(tempGfxHead++);

    gDPSetCycleType(tempGfxHead++, G_CYC_1CYCLE);
    gDPSetColorImage(tempGfxHead++, G_IM_FMT_RGBA, G_IM_SIZ_16b, SCREEN_WIDTH,
                     gPhysicalFramebuffers[sRenderingFramebuffer]);
    gDPSetScissor(tempGfxHead++, G_SC_NON_INTERLACE, 0, gBorderHeight, SCREEN_WIDTH,
                  SCREEN_HEIGHT - gBorderHeight);

    gDisplayListHead = tempGfxHead;
}

/**
 * Clear the framebuffer and fill it with a 32-bit color.
 * Information about the color argument: https://jrra.zone/n64/doc/n64man/gdp/gDPSetFillColor.htm
 */
void clear_framebuffer(s32 color) {
    Gfx *tempGfxHead = gDisplayListHead;
#if defined(F3DEX_GBI_3) && defined(F3DEX3_FB_MEMCLEAR)
    gSPMemset(tempGfxHead++, gPhysicalFramebuffers[sRenderingFramebuffer], color, SCREEN_WIDTH * SCREEN_HEIGHT * 2);
#else
    gDPPipeSync(tempGfxHead++);

    gDPSetRenderMode(tempGfxHead++, G_RM_OPA_SURF, G_RM_OPA_SURF2);
    gDPSetCycleType(tempGfxHead++, G_CYC_FILL);

    gDPSetFillColor(tempGfxHead++, color);
    gDPFillRectangle(tempGfxHead++,
                     GFX_DIMENSIONS_RECT_FROM_LEFT_EDGE(0), gBorderHeight,
                     GFX_DIMENSIONS_RECT_FROM_RIGHT_EDGE(0) - 1, SCREEN_HEIGHT - gBorderHeight - 1);

    gDPPipeSync(tempGfxHead++);

    gDPSetCycleType(tempGfxHead++, G_CYC_1CYCLE);

    gDisplayListHead = tempGfxHead;
#endif
}

/**
 * Resets the viewport, readying it for the final image.
 */
void clear_viewport(Vp *viewport, s32 color) {
    s16 vpUlx = (viewport->vp.vtrans[0] - viewport->vp.vscale[0]) / 4 + 1;
    s16 vpUly = (viewport->vp.vtrans[1] - viewport->vp.vscale[1]) / 4 + 1;
    s16 vpLrx = (viewport->vp.vtrans[0] + viewport->vp.vscale[0]) / 4 - 2;
    s16 vpLry = (viewport->vp.vtrans[1] + viewport->vp.vscale[1]) / 4 - 2;

#ifdef WIDESCREEN
    vpUlx = GFX_DIMENSIONS_RECT_FROM_LEFT_EDGE(vpUlx);
    vpLrx = GFX_DIMENSIONS_RECT_FROM_RIGHT_EDGE(SCREEN_WIDTH - vpLrx);
#endif

    Gfx *tempGfxHead = gDisplayListHead;

    gDPPipeSync(tempGfxHead++);

    gDPSetRenderMode(tempGfxHead++, G_RM_OPA_SURF, G_RM_OPA_SURF2);
    gDPSetCycleType(tempGfxHead++, G_CYC_FILL);

    gDPSetFillColor(tempGfxHead++, color);
    gDPFillRectangle(tempGfxHead++, vpUlx, vpUly, vpLrx, vpLry);

    gDPPipeSync(tempGfxHead++);

    gDPSetCycleType(tempGfxHead++, G_CYC_1CYCLE);

    gDisplayListHead = tempGfxHead;
}

/**
 * Draw the horizontal screen borders.
 */
void draw_screen_borders(void) {
    Gfx *tempGfxHead = gDisplayListHead;

    gDPPipeSync(tempGfxHead++);

    gDPSetScissor(tempGfxHead++, G_SC_NON_INTERLACE, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
    gDPSetRenderMode(tempGfxHead++, G_RM_OPA_SURF, G_RM_OPA_SURF2);
    gDPSetCycleType(tempGfxHead++, G_CYC_FILL);

    gDPSetFillColor(tempGfxHead++, GPACK_RGBA5551(0, 0, 0, 0) << 16 | GPACK_RGBA5551(0, 0, 0, 0));

    if (gBorderHeight) {
        gDPFillRectangle(tempGfxHead++, GFX_DIMENSIONS_RECT_FROM_LEFT_EDGE(0), 0,
                        GFX_DIMENSIONS_RECT_FROM_RIGHT_EDGE(0) - 1, gBorderHeight - 1);
        gDPFillRectangle(tempGfxHead++,
                        GFX_DIMENSIONS_RECT_FROM_LEFT_EDGE(0), SCREEN_HEIGHT - gBorderHeight,
                        GFX_DIMENSIONS_RECT_FROM_RIGHT_EDGE(0) - 1, SCREEN_HEIGHT - 1);
    }

    gDisplayListHead = tempGfxHead;
}

/**
 * Defines the viewport scissoring rectangle.
 * Scissoring: https://jrra.zone/n64/doc/pro-man/pro12/12-03.htm#01
 */
void make_viewport_clip_rect(Vp *viewport) {
    s16 vpUlx = (viewport->vp.vtrans[0] - viewport->vp.vscale[0]) / 4 + 1;
    s16 vpPly = (viewport->vp.vtrans[1] - viewport->vp.vscale[1]) / 4 + 1;
    s16 vpLrx = (viewport->vp.vtrans[0] + viewport->vp.vscale[0]) / 4 - 1;
    s16 vpLry = (viewport->vp.vtrans[1] + viewport->vp.vscale[1]) / 4 - 1;

    gDPSetScissor(gDisplayListHead++, G_SC_NON_INTERLACE, vpUlx, vpPly, vpLrx, vpLry);
}

/**
 * Initializes the Fast3D OSTask structure.
 * If you plan on using gSPLoadUcode, make sure to add OS_TASK_LOADABLE to the flags member.
 */
void create_gfx_task_structure(void) {
    s32 entries = gDisplayListHead - gGfxPool->buffer;

    gGfxSPTask->msgqueue = &gGfxVblankQueue;
    gGfxSPTask->msg = (OSMesg) 2;
    gGfxSPTask->task.t.type = M_GFXTASK;
    gGfxSPTask->task.t.ucode_boot = rspbootTextStart;
    gGfxSPTask->task.t.ucode_boot_size = ((u8 *) rspbootTextEnd - (u8 *) rspbootTextStart);
    gGfxSPTask->task.t.flags = (OS_TASK_LOADABLE | OS_TASK_DP_WAIT);

#if defined(F3DEX_GBI_3)
    #if F3DEX_VERSION == 3 // Standard F3DEX3
        #if defined(DEBUG_F3DEX3_PROFILER)
            switch (gF3DEX3ProfilerPage) {
                case 4: GRUCODE_TASK(F3DEX3_BrW_PC); break;
                case 3: GRUCODE_TASK(F3DEX3_BrW_PB); break;
                case 2: GRUCODE_TASK(F3DEX3_BrW_PA); break;
                default: case 1: GRUCODE_TASK(F3DEX3_BrW); break;
            }
        #else
            GRUCODE_TASK(F3DEX3_BrW);
        #endif
    #elif F3DEX_VERSION == 4 // F3DEX3 LVP
        #if defined(DEBUG_F3DEX3_PROFILER)
            switch (gF3DEX3ProfilerPage) {
                case 4: GRUCODE_TASK(F3DEX3_BrW_LVP_PC); break;
                case 3: GRUCODE_TASK(F3DEX3_BrW_LVP_PB); break;
                case 2: GRUCODE_TASK(F3DEX3_BrW_LVP_PA); break;
                default: case 1: GRUCODE_TASK(F3DEX3_BrW_LVP); break;
            }
        #else
            GRUCODE_TASK(F3DEX3_BrW_LVP);
        #endif
    #else
        #error "Invalid F3DEX3 selection."
    #endif
#elif defined(F3DEX_GBI_2)
    GRUCODE_TASK(F3DZEX2_NoN_fifo)
#elif defined(F3DEX_GBI)
    GRUCODE_TASK(F3DEX_NoN_fifo);
#else
    #error "Invalid microcode selected."
#endif

    gGfxSPTask->task.t.dram_stack = (u64 *) gGfxSPTaskStack;
    gGfxSPTask->task.t.dram_stack_size = SP_DRAM_STACK_SIZE8;
    gGfxSPTask->task.t.output_buff = gGfxSPTaskOutputBuffer;
    gGfxSPTask->task.t.output_buff_size =
        (u64 *)((u8 *) gGfxSPTaskOutputBuffer + sizeof(gGfxSPTaskOutputBuffer));
    gGfxSPTask->task.t.data_ptr = (u64 *) &gGfxPool->buffer;
    gGfxSPTask->task.t.data_size = entries * sizeof(Gfx);
    gGfxSPTask->task.t.yield_data_ptr = (u64 *) gGfxSPTaskYieldBuffer;
    gGfxSPTask->task.t.yield_data_size = OS_YIELD_DATA_SIZE;
}

/**
 * Set default RCP (Reality Co-Processor) settings.
 */
void init_rcp(s32 resetZB) {
    move_segment_table_to_dmem();
    gSPDisplayList(gDisplayListHead++, init_rdp);
    gSPDisplayList(gDisplayListHead++, init_rsp);
    init_z_buffer(resetZB);
    select_framebuffer();
}

/**
 * End the master display list and initialize the graphics task structure for the next frame to be rendered.
 */
void end_master_display_list(void) {
    draw_screen_borders();

    gDPFullSync(gDisplayListHead++);
    gSPEndDisplayList(gDisplayListHead++);

    create_gfx_task_structure();
}

/**
 * Draw the bars that appear when the N64 is soft reset.
 */
void draw_reset_bars(void) {
    s32 width, height;
    s32 fbNum;
    u64 *fbPtr;

    if (gResetTimer != 0 && gNmiResetBarsTimer < 15) {
        if (sRenderedFramebuffer == 0) {
            fbNum = 2;
        } else {
            fbNum = sRenderedFramebuffer - 1;
        }

        fbPtr = (u64 *) PHYSICAL_TO_VIRTUAL(gPhysicalFramebuffers[fbNum]);
        fbPtr += gNmiResetBarsTimer++ * (SCREEN_WIDTH / 4);

        for (width = 0; width < ((SCREEN_HEIGHT / 16) + 1); width++) {
            for (height = 0; height < (SCREEN_WIDTH / 4); height++) {
                *fbPtr++ = 0;
            }
            fbPtr += ((SCREEN_WIDTH / 4) * 14);
        }
    }

    osWritebackDCacheAll();
    osRecvMesg(&gGameVblankQueue, &gMainReceivedMesg, OS_MESG_BLOCK);
    osRecvMesg(&gGameVblankQueue, &gMainReceivedMesg, OS_MESG_BLOCK);
}

/**
 * Initial settings for the first rendered frame.
 */
void render_init(void) {
#ifdef DEBUG_FORCE_CRASH_ON_BOOT
    FORCE_CRASH
#endif
    gGfxPool = &gGfxPools[0];
    set_segment_base_addr(SEGMENT_RENDER, gGfxPool->buffer);
    gGfxSPTask = &gGfxPool->spTask;
    gDisplayListHead = gGfxPool->buffer;
    gGfxPoolEnd = (u8 *)(gGfxPool->buffer + GFX_POOL_SIZE);
    init_rcp(CLEAR_ZBUFFER);
    clear_framebuffer(0x0001);
    end_master_display_list();
    exec_display_list(&gGfxPool->spTask);

    // Skip incrementing the initial framebuffer index on emulators so that they display immediately as the Gfx task finishes
    // VC probably emulates osViSwapBuffer accurately so instant patch breaks VC compatibility
    // Currently, Ares and Simple64 have issues with single buffering so disable it there as well.
    if (gEmulator & INSTANT_INPUT_BLACKLIST) {
        sRenderingFramebuffer++;
    }
    gGlobalTimer++;
}

/**
 * Selects the location of the F3D output buffer (gDisplayListHead).
 */
void select_gfx_pool(void) {
    gGfxPool = &gGfxPools[gGlobalTimer % ARRAY_COUNT(gGfxPools)];
    set_segment_base_addr(SEGMENT_RENDER, gGfxPool->buffer);
    gGfxSPTask = &gGfxPool->spTask;
    gDisplayListHead = gGfxPool->buffer;
    gGfxPoolEnd = (u8 *) (gGfxPool->buffer + GFX_POOL_SIZE);
}

/**
 * This function:
 * - Sends the current master display list out to be rendered.
 * - Tells the VI which color framebuffer to be displayed.
 * - Yields to the VI framerate twice, locking the game at 30 FPS.
 * - Selects which framebuffer will be rendered and displayed to next time.
 */
void display_and_vsync(void) {
    osRecvMesg(&gGfxVblankQueue, &gMainReceivedMesg, OS_MESG_BLOCK);
    if (gGoddardVblankCallback != NULL) {
        gGoddardVblankCallback();
        gGoddardVblankCallback = NULL;
    }
    exec_display_list(&gGfxPool->spTask);
#ifndef UNLOCK_FPS
    osRecvMesg(&gGameVblankQueue, &gMainReceivedMesg, OS_MESG_BLOCK);
#endif
    osViSwapBuffer((void *) PHYSICAL_TO_VIRTUAL(gPhysicalFramebuffers[sRenderedFramebuffer]));
#ifndef UNLOCK_FPS
    osRecvMesg(&gGameVblankQueue, &gMainReceivedMesg, OS_MESG_BLOCK);
#endif
    // Skip swapping buffers on inaccurate emulators other than VC so that they display immediately as the Gfx task finishes
    if (gEmulator & INSTANT_INPUT_BLACKLIST) {
        if (++sRenderedFramebuffer == 3) {
            sRenderedFramebuffer = 0;
        }
        if (++sRenderingFramebuffer == 3) {
            sRenderingFramebuffer = 0;
        }
    }
    gGlobalTimer++;
}

#if !defined(DISABLE_DEMO) && defined(KEEP_MARIO_HEAD)
// this function records distinct inputs over a 255-frame interval to RAM locations and was likely
// used to record the demo sequences seen in the final game. This function is unused.
UNUSED static void record_demo(void) {
    // record the player's button mask and current rawStickX and rawStickY.
    u8 buttonMask =
        ((gPlayer1Controller->buttonDown & (A_BUTTON | B_BUTTON | Z_TRIG | START_BUTTON)) >> 8)
        | (gPlayer1Controller->buttonDown & (U_CBUTTONS | D_CBUTTONS | L_CBUTTONS | R_CBUTTONS));
    s8 rawStickX = gPlayer1Controller->rawStickX;
    s8 rawStickY = gPlayer1Controller->rawStickY;

    // If the stick is in deadzone, set its value to 0 to
    // nullify the effects. We do not record deadzone inputs.
    if (rawStickX > -8 && rawStickX < 8) {
        rawStickX = 0;
    }

    if (rawStickY > -8 && rawStickY < 8) {
        rawStickY = 0;
    }

    // Rrecord the distinct input and timer so long as they are unique.
    // If the timer hits 0xFF, reset the timer for the next demo input.
    if (gRecordedDemoInput.timer == 0xFF || buttonMask != gRecordedDemoInput.buttonMask
        || rawStickX != gRecordedDemoInput.rawStickX || rawStickY != gRecordedDemoInput.rawStickY) {
        gRecordedDemoInput.timer = 0;
        gRecordedDemoInput.buttonMask = buttonMask;
        gRecordedDemoInput.rawStickX = rawStickX;
        gRecordedDemoInput.rawStickY = rawStickY;
    }
    gRecordedDemoInput.timer++;
}

/**
 * If a demo sequence exists, this will run the demo input list until it is complete.
 */
void run_demo_inputs(void) {
    // Eliminate the unused bits.
    gPlayer1Controller->controllerData->button &= VALID_BUTTONS;

    // Check if a demo inputs list exists and if so,
    // run the active demo input list.
    if (gCurrDemoInput != NULL) {
        // The timer variable being 0 at the current input means the demo is over.
        // Set the button to the END_DEMO mask to end the demo.
        if (gCurrDemoInput->timer == 0) {
            gPlayer1Controller->controllerData->stick_x = 0;
            gPlayer1Controller->controllerData->stick_y = 0;
            gPlayer1Controller->controllerData->button = END_DEMO;
        } else {
            // Backup the start button if it is pressed, since we don't want the
            // demo input to override the mask where start may have been pressed.
            u16 startPushed = (gPlayer1Controller->controllerData->button & START_BUTTON);

            // Perform the demo inputs by assigning the current button mask and the stick inputs.
            gPlayer1Controller->controllerData->stick_x = gCurrDemoInput->rawStickX;
            gPlayer1Controller->controllerData->stick_y = gCurrDemoInput->rawStickY;

            // To assign the demo input, the button information is stored in
            // an 8-bit mask rather than a 16-bit mask. this is because only
            // A, B, Z, Start, and the C-Buttons are used in a demo, as bits
            // in that order. In order to assign the mask, we need to take the
            // upper 4 bits (A, B, Z, and Start) and shift then left by 8 to
            // match the correct input mask. We then add this to the masked
            // lower 4 bits to get the correct button mask.
            gPlayer1Controller->controllerData->button =
                ((gCurrDemoInput->buttonMask & 0xF0) << 8) + ((gCurrDemoInput->buttonMask & 0xF));

            // If start was pushed, put it into the demo sequence being input to end the demo.
            gPlayer1Controller->controllerData->button |= startPushed;

            // Run the current demo input's timer down. if it hits 0, advance the demo input list.
            if (--gCurrDemoInput->timer == 0) {
                gCurrDemoInput++;
            }
        }
    }
}

#endif

/**
 * Take the updated controller struct and calculate the new x, y, and distance floats.
 */
void adjust_analog_stick(struct Controller *controller) {
    // Reset the controller's x and y floats.
    controller->stickX = 0;
    controller->stickY = 0;

    // Modulate the rawStickX and rawStickY to be the new f32 values by adding/subtracting 6.
    if (controller->rawStickX <= -8) {
        controller->stickX = controller->rawStickX + 6;
    }

    if (controller->rawStickX >= 8) {
        controller->stickX = controller->rawStickX - 6;
    }

    if (controller->rawStickY <= -8) {
        controller->stickY = controller->rawStickY + 6;
    }

    if (controller->rawStickY >= 8) {
        controller->stickY = controller->rawStickY - 6;
    }

    // Calculate f32 magnitude from the center by vector length.
    controller->stickMag =
        sqrtf(controller->stickX * controller->stickX + controller->stickY * controller->stickY);

    // Magnitude cannot exceed 64.0f: if it does, modify the values
    // appropriately to flatten the values down to the allowed maximum value.
    if (controller->stickMag > 64) {
        controller->stickX *= 64 / controller->stickMag;
        controller->stickY *= 64 / controller->stickMag;
        controller->stickMag = 64;
    }
}

/**
 * Update the controller struct with available inputs if present.
 */
void read_controller_inputs(s32 threadID) {
    // If any controllers are plugged in, update the controller information.
    if (gControllerBits) {
        if (threadID == THREAD_5_GAME_LOOP) {
            osRecvMesg(&gSIEventMesgQueue, &gMainReceivedMesg, OS_MESG_BLOCK);
        }
        osContGetReadDataEx(gControllerPads);
#if ENABLE_RUMBLE
        release_rumble_pak_control();
#endif
    }
#if !defined(DISABLE_DEMO) && defined(KEEP_MARIO_HEAD)
    run_demo_inputs();
#endif

    for (s32 cont = 0; cont < MAX_NUM_PLAYERS; cont++) {
        struct Controller* controller = &gControllers[cont];
        OSContPadEx* controllerData = controller->controllerData;

        // if we're receiving inputs, update the controller struct with the new button info.
        if (controller->controllerData != NULL) {
            // HackerSM64: Swaps Z and L, only on console, and only when playing with a GameCube controller.
            if ((controller->statusData->type & CONT_CONSOLE_MASK) == CONT_CONSOLE_GCN) {
                u32 oldButton = controllerData->button;
                u32 newButton = oldButton & ~(Z_TRIG | L_TRIG);
                if (oldButton & Z_TRIG) {
                    newButton |= L_TRIG;
                }
                if (controllerData->l_trig > 85) { // How far the player has to press the L trigger for it to be considered a Z press. 64 is about 25%. 127 would be about 50%.
                    newButton |= Z_TRIG;
                }
                controllerData->button = newButton;
            }
            controller->rawStickX = controllerData->stick_x;
            controller->rawStickY = controllerData->stick_y;
            controller->buttonPressed  = (~controller->buttonDown & controllerData->button);
            controller->buttonReleased = (~controllerData->button & controller->buttonDown);
            // 0.5x A presses are a good meme
            controller->buttonDown = controllerData->button;
            adjust_analog_stick(controller);
        } else { // otherwise, if the controllerData is NULL, 0 out all of the inputs.
            controller->rawStickX      = 0;
            controller->rawStickY      = 0;
            controller->buttonPressed  = 0x0000;
            controller->buttonReleased = 0x0000;
            controller->buttonDown     = 0x0000;
            controller->stickX         = 0.0f;
            controller->stickY         = 0.0f;
            controller->stickMag       = 0.0f;
        }
    }
}

/**
 * @brief Links a controller struct to the appropriate status and pad.
 *
 * @param[out] controller The controller to link.
 * @param[in ] port The port to get the data from.
 */
static void assign_controller_data_to_port(struct Controller* controller, int port) {
    controller->statusData = &gControllerStatuses[port];
    controller->controllerData = &gControllerPads[port];
    controller->port = port;
}

/**
 * Initialize the controller structs to point at the OSCont information.
 */
void init_controllers(void) {
    int port, cont = 0;
    int lastUsedPort = -1;

    // Set controller 1 to point to the set of status/pads for input 1 and
    // init the controllers.
    assign_controller_data_to_port(&gControllers[0], 0);
    osContInit(&gSIEventMesgQueue, &gControllerBits, gControllerStatuses);

#ifdef EEP
    // strangely enough, the EEPROM probe for save data is done in this function.
    // save pak detection?
    gEepromProbe = (gEmulator & EMU_WIIVC)
                 ? osEepromProbeVC(&gSIEventMesgQueue)
                 : osEepromProbe  (&gSIEventMesgQueue);
#endif
#ifdef SRAM
    gSramProbe = nuPiInitSram();
#endif

    // Loop over the 4 ports and link the controller structs to the appropriate status and pad.
    for (port = 0; port < MAXCONTROLLERS; port++) {
        if (cont >= MAX_NUM_PLAYERS) {
            break;
        }

        // Is controller plugged in?
        if (gControllerBits & (1 << port)) {
            // The game allows you to have just 1 controller plugged
            // into any port in order to play the game. this was probably
            // so if any of the ports didn't work, you can have controllers
            // plugged into any of them and it will work.
            assign_controller_data_to_port(&gControllers[cont], port);

            lastUsedPort = port;

            cont++;
        }
    }

#if (MAX_NUM_PLAYERS >= 2)
    //! Some flashcarts (eg. ED64p) don't let you start a ROM with a GameCube controller in port 1,
    //   so if port 1 is an N64 controller and port 2 is a GC controller, swap them.
    if (
        (gEmulator & EMU_CONSOLE) &&
        ((gControllerBits & 0b11) == 0b11) && // Only swap if the first two ports both have controllers plugged in.
        ((gControllerStatuses[0].type & CONT_CONSOLE_MASK) == CONT_CONSOLE_N64) && // If the 1st port's controller is N64.
        ((gControllerStatuses[1].type & CONT_CONSOLE_MASK) == CONT_CONSOLE_GCN)    // If the 2nd port's controller is GCN.
    ) {
        struct Controller temp = gControllers[0];
        gControllers[0] = gControllers[1];
        gControllers[1] = temp;
    }
#endif

    // Disable the ports after the last used one.
    osContSetCh(lastUsedPort + 1);
}

// Game thread core
// ----------------------------------------------------------------------------------------------------

/**
 * Setup main segments and framebuffers.
 */
void setup_game_memory(void) {
    // Setup general Segment 0
    set_segment_base_addr(SEGMENT_MAIN, (void *)RAM_START);
    // Create Mesg Queues
    osCreateMesgQueue(&gGfxVblankQueue, gGfxMesgBuf, ARRAY_COUNT(gGfxMesgBuf));
    osCreateMesgQueue(&gGameVblankQueue, gGameMesgBuf, ARRAY_COUNT(gGameMesgBuf));
    // Setup z buffer and framebuffer
    gPhysicalZBuffer = VIRTUAL_TO_PHYSICAL(gZBuffer);
    gPhysicalFramebuffers[0] = VIRTUAL_TO_PHYSICAL(gFramebuffer0);
    gPhysicalFramebuffers[1] = VIRTUAL_TO_PHYSICAL(gFramebuffer1);
    gPhysicalFramebuffers[2] = VIRTUAL_TO_PHYSICAL(gFramebuffer2);
    // Setup Mario Animations
    gMarioAnimsMemAlloc = main_pool_alloc(MARIO_ANIMS_POOL_SIZE, MEMORY_POOL_LEFT);
    set_segment_base_addr(SEGMENT_MARIO_ANIMS, (void *) gMarioAnimsMemAlloc);
    setup_dma_table_list(&gMarioAnimsBuf, gMarioAnims, gMarioAnimsMemAlloc);
#ifdef PUPPYPRINT_DEBUG
    set_segment_memory_printout(SEGMENT_MARIO_ANIMS, MARIO_ANIMS_POOL_SIZE);
    set_segment_memory_printout(SEGMENT_DEMO_INPUTS, DEMO_INPUTS_POOL_SIZE);
#endif
    // Setup Demo Inputs List
    gDemoInputsMemAlloc = main_pool_alloc(DEMO_INPUTS_POOL_SIZE, MEMORY_POOL_LEFT);
    set_segment_base_addr(SEGMENT_DEMO_INPUTS, (void *) gDemoInputsMemAlloc);
    setup_dma_table_list(&gDemoInputsBuf, gDemoInputs, gDemoInputsMemAlloc);
    // Setup Level Script Entry
    load_segment(SEGMENT_LEVEL_ENTRY, _entrySegmentRomStart, _entrySegmentRomEnd, MEMORY_POOL_LEFT, NULL, NULL);
    // Setup Segment 2 (Fonts, Text, etc)
    load_segment_decompress(SEGMENT_SEGMENT2, _segment2_mio0SegmentRomStart, _segment2_mio0SegmentRomEnd);
}

/**
 * Main game loop thread. Runs forever as long as the game continues.
 */
void thread5_game_loop(UNUSED void *arg) {
    setup_game_memory();
#if ENABLE_RUMBLE
    init_rumble_pak_scheduler_queue();
#endif
    init_controllers();
#if ENABLE_RUMBLE
    create_thread_6();
#endif
    save_file_load_all();
#ifdef PUPPYCAM
    puppycam_boot();
#endif

    set_vblank_handler(2, &gGameVblankHandler, &gGameVblankQueue, (OSMesg) 1);

    // Point address to the entry point into the level script data.
    struct LevelCommand *addr = segmented_to_virtual(level_script_entry);

    play_music(SEQ_PLAYER_SFX, SEQUENCE_ARGS(0, SEQ_SOUND_PLAYER), 0);
    set_sound_mode(save_file_get_sound_mode());
#ifdef WIDE
    gConfig.widescreen = save_file_get_widescreen_mode();
#endif
    render_init();

    while (TRUE) {
        profiler_frame_setup();
        // If the reset timer is active, run the process to reset the game.
        if (gResetTimer != 0) {
            draw_reset_bars();
            continue;
        }
#ifdef PUPPYPRINT_DEBUG
    bzero(&gPuppyCallCounter, sizeof(gPuppyCallCounter));
#endif
        // If any controllers are plugged in, start read the data for when
        // read_controller_inputs is called later.
        if (gControllerBits) {
#if ENABLE_RUMBLE
            block_until_rumble_pak_free();
#endif
            osContStartReadDataEx(&gSIEventMesgQueue);
        }

        audio_game_loop_tick();
        select_gfx_pool();
        read_controller_inputs(THREAD_5_GAME_LOOP);
#ifdef DEBUG_F3DEX3_PROFILER
        query_f3dex3_profiler();
#endif
        profiler_update(PROFILER_TIME_CONTROLLERS, 0);
        profiler_collision_reset();
        addr = level_script_execute(addr);
        profiler_collision_completed();
#if !defined(PUPPYPRINT_DEBUG) && defined(VISUAL_DEBUG)
        debug_box_input();
#endif
#ifdef PUPPYPRINT_DEBUG
        puppyprint_profiler_process();
#endif

        display_and_vsync();
#ifdef VANILLA_DEBUG
        // when debug info is enabled, print the "BUF %d" information.
        if (gShowDebugText) {
            // subtract the end of the gfx pool with the display list to obtain the
            // amount of free space remaining.
            print_text_fmt_int(180, 20, "BUF %d", gGfxPoolEnd - (u8 *) gDisplayListHead);
        }
#endif
    }
}
