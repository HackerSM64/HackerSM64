#include <ultra64.h>

#include "config.h"
#include "framebuffers.h"

// 0x70800 bytes
ALIGNED16 RGBA16 gFramebuffers[3][SCREEN_WIDTH * SCREEN_HEIGHT];
