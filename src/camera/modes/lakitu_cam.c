#include "camera/camera_math.h"
#include "camera/camera_modes.h"
#include "engine/math_util.h"
#include "game/camera.h"

/**
 * The mode used by close and free roam
 */
void mode_lakitu_camera(struct Camera *c) {
    gCameraZoomDist = 800.f;
    mode_default_camera(c);
}
