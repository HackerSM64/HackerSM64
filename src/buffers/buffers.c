#include <ultra64.h>

#include "buffers.h"
#include "audio/data.h"
#include "config.h"
#include "audio/synthesis.h"

ALIGNED16 u8 gAudioHeap[DOUBLE_SIZE_ON_64_BIT(AUDIO_HEAP_SIZE)];

ALIGNED8 u8 gIdleThreadStack[THREAD1_STACK];
ALIGNED8 u8 gThread3Stack[THREAD3_STACK];
ALIGNED8 u8 gThread4Stack[THREAD4_STACK];
ALIGNED8 u8 gThread5Stack[THREAD5_STACK];
#if ENABLE_RUMBLE
ALIGNED8 u8 gThread6Stack[THREAD6_STACK];
#endif
// 0x400 bytes
__attribute__((aligned(32))) u8 gGfxSPTaskStack[SP_DRAM_STACK_SIZE8];
__attribute__((aligned(32))) u8 gGfxSPTaskYieldBuffer[OS_YIELD_DATA_SIZE];
// 0x200 bytes
ALIGNED8 struct SaveBuffer gSaveBuffer;
// 0x190a0 bytes
struct GfxPool gGfxPools[2];
