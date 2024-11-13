#include "camera/cutscene_helpers.h"
#include "camera/camera_math.h"
#include "engine/math_util.h"
#include "game/camera.h"


void cutscene_unused_start(UNUSED struct Camera *c) {
}

void cutscene_unused_loop(UNUSED struct Camera *c) {
}

struct Cutscene sCutsceneUnused[] = {
    { cutscene_unused_start, 1 },
    { cutscene_unused_loop, CUTSCENE_LOOP }
};
