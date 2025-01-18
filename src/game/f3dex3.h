#pragma once

// F3DEX3 profiler
#ifdef DEBUG_F3DEX3_PROFILER
typedef struct {  /* Default performance counters, if no CFG_PROFILING_* is enabled */
    /* Number of vertices processed by the RSP */
    u16 vertexCount;
    /* Number of tris actually drawn, after clipping and all types of culling */
    u16 rdpOutTriCount;
    /* Number of tris which processing started on the RSP (before clipping / culling) */
    u32 rspInTriCount:18;
    /* Number of fill rects and tex rects drawn */
    u32 rectCount:14;
    /* Number of cycles the RSP was stalled because the RDP FIFO was full */
    u32 stallRDPFifoFullCycles;
    /* Unused, zero */
    u32 dummy;
} F3DEX3ProfilingDefault;

typedef struct {  /* Counters for CFG_PROFILING_A */
    /* Number of cycles the RSP spent processing vertex commands, including vertex DMAs */
    u32 vertexProcCycles;
    /* Number of display list commands fetched from DRAM, >= dlCommandCount */
    u16 fetchedDLCommandCount;
    /* Number of display list commands executed */
    u16 dlCommandCount;
    /* Number of cycles the RSP was stalled because the RDP FIFO was full */
    u32 stallRDPFifoFullCycles;
    /* Number of cycles the RSP spent processing triangle commands, NOT including buffer flushes (i.e. FIFO full) */
    u32 triProcCycles;
} F3DEX3ProfilingA;

typedef struct {  /* Counters for CFG_PROFILING_B */
    /* Number of vertices processed by the RSP */
    u16 vertexCount;
    /* Number of vertices processed which had lighting enabled */
    u16 litVertexCount;
    /* Number of tris culled by the occlusion plane */
    u32 occlusionPlaneCullCount:18;
    /* Number of RSP/input triangles which got clipped */
    u32 clippedTriCount:14;
    /* Number of times any microcode overlay was loaded */
    u32 allOverlayLoadCount:18;
    /* Number of times overlay 2 (lighting) was loaded */
    u32 lightingOverlayLoadCount:14;
    /* Number of times overlay 3 (clipping) was loaded */
    u32 clippingOverlayLoadCount:18;
    /* Number of times overlay 4 (mIT matrix, matrix multiply, etc.) was loaded */
    u32 miscOverlayLoadCount:14;
} F3DEX3ProfilingB;

typedef struct {  /* Counters for CFG_PROFILING_C */
    /* Total cycles F3DEX3 believes it was running, not including SPLoadUcode */
    u32 ex3UcodeCycles;
    /* The "GCLK is alive" bit of the RDP status is sampled once every time a
    display list command is started. This counts the number of times that bit
    was 1. Divide by dlCommandCount to get an approximate measurement of the
    percentage of time the RDP was doing useful work, as opposed to waiting
    for framebuffer / Z buffer memory transactions to complete. */
    u16 commandsSampledGclkActive;
    /* Number of display list commands executed */
    u16 dlCommandCount;
    /* Number of commands sent to the RDP except for triangle commands */
    u32 smallRDPCommandCount:18;
    /* Number of matrix loads, of any type */
    u32 matrixCount:14;
    /* Number of cycles the RSP was stalled waiting for any DMAs: vertex loads,
    matrix loads, copying command buffers to the RDP FIFO, overlay loads, etc. */
    u32 stallDMACycles;
} F3DEX3ProfilingC;

typedef struct {
    union {
        F3DEX3ProfilingDefault def;
        F3DEX3ProfilingA a;
        F3DEX3ProfilingB b;
        F3DEX3ProfilingC c;
        u64 dummy_alignment[2];
    };
    u32 taskdataptr; /* Not a perf counter, can ignore */
    u32 ucode; /* Not a perf counter, can ignore */
} F3DEX3YieldDataFooter;

extern u32 gF3DEX3ProfilerPage;
extern volatile F3DEX3YieldDataFooter gRSPProfilingResults;

void extract_f3dex3_profiler_data();
void query_f3dex3_profiler();
void draw_f3dex3_profiler();
#endif