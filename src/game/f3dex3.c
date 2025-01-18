#include <ultra64.h>
#ifdef F3DEX_GBI_3
#include "game_init.h"
#ifdef DEBUG_F3DEX3_PROFILER
#include "buffers/buffers.h"
#include "puppyprint.h"
#include "fasttext.h"
#endif
#include "f3dex3.h"

#ifdef DEBUG_F3DEX3_PROFILER
u32 gF3DEX3ProfilerPage = 0;

volatile F3DEX3YieldDataFooter gRSPProfilingResults;

/**
 * Extracts the profiling information from the F3DEX3 graphics task yield data.
 */

void extract_f3dex3_profiler_data() {
    F3DEX3YieldDataFooter* footer = (F3DEX3YieldDataFooter*)((u8*)gGfxSPTaskYieldBuffer +
                                    OS_YIELD_DATA_SIZE - sizeof(F3DEX3YieldDataFooter));
    osInvalDCache(footer, sizeof(F3DEX3YieldDataFooter));
    bcopy(footer, &gRSPProfilingResults, sizeof(F3DEX3YieldDataFooter));
}

/**
 * Queries user input for the F3DEX3 profiler. See config/config_debug.h for more information.
 */

void query_f3dex3_profiler() {
    if (gPlayer1Controller->buttonPressed & L_TRIG && gPlayer1Controller->buttonPressed & D_CBUTTONS) {
        if (++gF3DEX3ProfilerPage > 4) {
            gF3DEX3ProfilerPage = 0;
        }
        gPlayer1Controller->buttonPressed &= ~D_CBUTTONS;
    }
}

void draw_f3dex3_profiler() {
    if (!gF3DEX3ProfilerPage) {
        return;
    }
    char profilerBuffers[9][32];

    sprintf(profilerBuffers[8], "Page %u of 4", gF3DEX3ProfilerPage);

    switch (gF3DEX3ProfilerPage) {
        case 4:
            sprintf(profilerBuffers[0], "Grucode cycles: %u", gRSPProfilingResults.c.ex3UcodeCycles);
            sprintf(profilerBuffers[1], "GLCK alive: %u", gRSPProfilingResults.c.commandsSampledGclkActive);
            sprintf(profilerBuffers[2], "RSP commands: %u", gRSPProfilingResults.c.dlCommandCount);
            sprintf(profilerBuffers[3], "Small RDP commands: %u", gRSPProfilingResults.c.smallRDPCommandCount);
            sprintf(profilerBuffers[4], "Matrix count: %u", gRSPProfilingResults.c.matrixCount);
            sprintf(profilerBuffers[5], "DMA stall cycles: %u", gRSPProfilingResults.c.stallDMACycles);
            sprintf(profilerBuffers[6], "Cycles / command: %f01", (f32) gRSPProfilingResults.c.ex3UcodeCycles / (f32) gRSPProfilingResults.c.dlCommandCount);
            sprintf(profilerBuffers[7], "");
        break;
        case 3:
            sprintf(profilerBuffers[0], "Vertex: %u", gRSPProfilingResults.b.vertexCount);
            sprintf(profilerBuffers[1], "Lit vertex: %u", gRSPProfilingResults.b.litVertexCount);
            sprintf(profilerBuffers[2], "Occluded triangles: %u", gRSPProfilingResults.b.occlusionPlaneCullCount);
            sprintf(profilerBuffers[3], "Clipped triangles: %u", gRSPProfilingResults.b.clippedTriCount);
            sprintf(profilerBuffers[4], "Overlay loads: %u", gRSPProfilingResults.b.allOverlayLoadCount);
            sprintf(profilerBuffers[5], "Lighting overlay loads: %u", gRSPProfilingResults.b.lightingOverlayLoadCount);
            sprintf(profilerBuffers[6], "Clipping overlay loads: %u", gRSPProfilingResults.b.clippingOverlayLoadCount);
            sprintf(profilerBuffers[7], "Miscallaneous overlay loads: %u", gRSPProfilingResults.b.miscOverlayLoadCount);
        break;
        case 2:
            sprintf(profilerBuffers[0], "Vertex cycles: %u", gRSPProfilingResults.a.vertexProcCycles);
            sprintf(profilerBuffers[1], "RSP fetched commands: %u", gRSPProfilingResults.a.fetchedDLCommandCount);
            sprintf(profilerBuffers[2], "RSP commands: %u", gRSPProfilingResults.a.dlCommandCount);
            sprintf(profilerBuffers[3], "Triangle cycles: %u", gRSPProfilingResults.a.triProcCycles);
            sprintf(profilerBuffers[4], "RSP FIFO stall: %u", gRSPProfilingResults.a.stallRDPFifoFullCycles);
            sprintf(profilerBuffers[5], "");
            sprintf(profilerBuffers[6], "");
            sprintf(profilerBuffers[7], "");
        break;
        case 1:
            sprintf(profilerBuffers[0], "Vertex: %u", gRSPProfilingResults.def.vertexCount);
            sprintf(profilerBuffers[1], "RDP triangles: %u", gRSPProfilingResults.def.rdpOutTriCount);
            sprintf(profilerBuffers[2], "RSP triangles: %u", gRSPProfilingResults.def.rspInTriCount);
            sprintf(profilerBuffers[3], "Rectangles: %u", gRSPProfilingResults.def.rectCount);
            sprintf(profilerBuffers[4], "RSP FIFO stall: %u", gRSPProfilingResults.def.stallRDPFifoFullCycles);
            sprintf(profilerBuffers[5], "");
            sprintf(profilerBuffers[6], "");
            sprintf(profilerBuffers[7], "");
        break;
    }

    prepare_blank_box();
    render_blank_box(32, 32, SCREEN_WIDTH - 32, SCREEN_HEIGHT - 32, 0, 0, 0, 168);
    finish_blank_box();

        Gfx* dlHead = gDisplayListHead;
        gDPPipeSync(dlHead++);
        gDPSetCycleType(dlHead++, G_CYC_1CYCLE);
        gDPSetRenderMode(dlHead++, G_RM_TEX_EDGE, G_RM_TEX_EDGE2);
        gDPSetTexturePersp(dlHead++, G_TP_NONE);
        gDPSetTextureFilter(dlHead++, G_TF_POINT);
        gDPSetTextureLUT(dlHead++, G_TT_NONE);
        drawSmallStringCol(&dlHead, 48, 48, profilerBuffers[0], 255, 255, 255);
        drawSmallStringCol(&dlHead, 48, 66, profilerBuffers[1], 255, 255, 255);
        drawSmallStringCol(&dlHead, 48, 84, profilerBuffers[2], 255, 255, 255);
        drawSmallStringCol(&dlHead, 48, 102, profilerBuffers[3], 255, 255, 255);
        drawSmallStringCol(&dlHead, 48, 120, profilerBuffers[4], 255, 255, 255);
        drawSmallStringCol(&dlHead, 48, 138, profilerBuffers[5], 255, 255, 255);
        drawSmallStringCol(&dlHead, 48, 156, profilerBuffers[6], 255, 255, 255);
        drawSmallStringCol(&dlHead, 48, 174, profilerBuffers[7], 255, 255, 255);
        drawSmallStringCol(&dlHead, 196, 186, profilerBuffers[8], 255, 255, 255);
        gDisplayListHead = dlHead;

}
#endif // DEBUG_F3DEX3_PROFILER
#endif // F3DEX_GBI_3
