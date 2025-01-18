#ifndef BUFFERS_H
#define BUFFERS_H

#include <PR/ultratypes.h>

#include "game/save_file.h"
#include "game/game_init.h"
#include "game/main.h"
#include "config.h"
#include "audio/data.h"

extern u8 gAudioHeap[DOUBLE_SIZE_ON_64_BIT(AUDIO_HEAP_SIZE)];

extern u8 gIdleThreadStack[THREAD1_STACK];
extern u8 gThread3Stack[THREAD3_STACK];
extern u8 gThread4Stack[THREAD4_STACK];
extern u8 gThread5Stack[THREAD5_STACK];
#if ENABLE_RUMBLE
extern u8 gThread6Stack[THREAD6_STACK];
#endif

extern u8 gGfxSPTaskYieldBuffer[];

extern struct SaveBuffer gSaveBuffer;

extern u8 gGfxSPTaskStack[];

extern struct GfxPool gGfxPools[2];
#endif // BUFFERS_H
