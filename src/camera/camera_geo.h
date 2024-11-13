#pragma once

#include "types.h"
#include "game/camera.h"

Gfx *geo_camera_fov(s32 callContext, struct GraphNode *g, UNUSED void *context);
Gfx *geo_camera_main(s32 callContext, struct GraphNode *g, void *context);

