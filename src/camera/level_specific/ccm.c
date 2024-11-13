#include "game/camera.h"

void cam_ccm_enter_slide_shortcut(UNUSED struct Camera *c) {
    sStatusFlags |= CAM_FLAG_CCM_SLIDE_SHORTCUT;
}

void cam_ccm_leave_slide_shortcut(UNUSED struct Camera *c) {
    sStatusFlags &= ~CAM_FLAG_CCM_SLIDE_SHORTCUT;
}
