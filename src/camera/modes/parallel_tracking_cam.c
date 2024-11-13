#include "camera/camera_math.h"
#include "camera/camera_modes.h"
#include "engine/math_util.h"
#include "game/camera.h"

/**
 * The index into the current parallel tracking path
 */
static u32 sParTrackIndex;

/**
 * The current list of ParallelTrackingPoints used in update_parallel_tracking_camera()
 */
static struct ParallelTrackingPoint *sParTrackPath;

/**
 * On the first frame after the camera changes to a different parallel tracking path, this stores the
 * displacement between the camera's calculated new position and its previous positions
 *
 * This transition offset is then used to smoothly interpolate the camera's position between the two
 * paths
 */
static struct CameraStoredInfo sParTrackTransOff;

/**
 * Start parallel tracking mode using the path `path`
 */
void parallel_tracking_init(struct Camera *c, struct ParallelTrackingPoint *path) {
    if (c->mode != CAMERA_MODE_PARALLEL_TRACKING) {
        sParTrackPath = path;
        sParTrackIndex = 0;
        sParTrackTransOff.pos[0] = 0.f;
        sParTrackTransOff.pos[1] = 0.f;
        sParTrackTransOff.pos[2] = 0.f;
        // Place the camera in the middle of the path
        c->pos[0] = (sParTrackPath[0].pos[0] + sParTrackPath[1].pos[0]) / 2;
        c->pos[1] = (sParTrackPath[0].pos[1] + sParTrackPath[1].pos[1]) / 2;
        c->pos[2] = (sParTrackPath[0].pos[2] + sParTrackPath[1].pos[2]) / 2;
        sStatusFlags &= ~CAM_FLAG_SMOOTH_MOVEMENT;
        c->mode = CAMERA_MODE_PARALLEL_TRACKING;
    }
}

/**
 * Move the camera in parallel tracking mode
 *
 * Uses the line between the next two points in sParTrackPath
 * The camera can move forward/back and side to side, but it will face perpendicular to that line
 *
 * Although, annoyingly, it's not truly parallel, the function returns the yaw from the camera to Mario,
 * so Mario will run slightly towards the camera.
 */
s32 update_parallel_tracking_camera(struct Camera *c, Vec3f focus, Vec3f pos) {
    Vec3f path[2];
    Vec3f parMidPoint;
    Vec3f marioOffset;
    Vec3f camOffset;
    /// Adjusts the focus to look where Mario is facing. Unused since marioOffset is copied to focus
    Vec3f focOffset = { 0 };
    s16 pathPitch;
    s16 pathYaw;
    f32 camParDist;
    f32 pathLength;
    f32 parScale = 0.5f;
    f32 marioFloorDist;
    Vec3f marioPos;
    Vec3s pathAngle;
    // Variables for changing to the next/prev path in the list
    Vec3f oldPos;
    Vec3f prevPathPos;
    Vec3f nextPathPos;
    f32 distToNext, distToPrev;
    s16 prevPitch, nextPitch;
    s16 prevYaw, nextYaw;

    // Store camera pos, for changing between paths
    vec3f_copy(oldPos, pos);

    vec3f_copy(path[0], sParTrackPath[sParTrackIndex].pos);
    vec3f_copy(path[1], sParTrackPath[sParTrackIndex + 1].pos);

    f32 distThresh = sParTrackPath[sParTrackIndex].distThresh;
    f32 zoom = sParTrackPath[sParTrackIndex].zoom;
    calc_y_to_curr_floor(&marioFloorDist, 1.f, 200.f, &marioFloorDist, 0.9f, 200.f);

    marioPos[0] = sMarioCamState->pos[0];
    // Mario's y pos + ~Mario's height + Mario's height above the floor
    marioPos[1] = sMarioCamState->pos[1] + 150.f + marioFloorDist;
    marioPos[2] = sMarioCamState->pos[2];

    // Calculate middle of the path (parScale is 0.5f)
    parMidPoint[0] = path[0][0] + (path[1][0] - path[0][0]) * parScale;
    parMidPoint[1] = path[0][1] + (path[1][1] - path[0][1]) * parScale;
    parMidPoint[2] = path[0][2] + (path[1][2] - path[0][2]) * parScale;

    // Get direction of path
    vec3f_get_dist_and_angle(path[0], path[1], &pathLength, &pathPitch, &pathYaw);

    marioOffset[0] = marioPos[0] - parMidPoint[0];
    marioOffset[1] = marioPos[1] - parMidPoint[1];
    marioOffset[2] = marioPos[2] - parMidPoint[2];

    // Make marioOffset point from the midpoint -> the start of the path
    // Rotating by -yaw then -pitch moves the hor dist from the midpoint into marioOffset's z coordinate
    // marioOffset[0] = the (perpendicular) horizontal distance from the path
    // marioOffset[1] = the vertical distance from the path
    // marioOffset[2] = the (parallel) horizontal distance from the path's midpoint
    pathYaw = -pathYaw;
    rotate_in_xz(marioOffset, marioOffset, pathYaw);
    pathYaw = -pathYaw;
    pathPitch = -pathPitch;
    rotate_in_yz(marioOffset, marioOffset, pathPitch);
    pathPitch = -pathPitch;
    focOffset[2] = marioOffset[2];

    // Repeat above calcs with camOffset
    camOffset[0] = pos[0] - parMidPoint[0];
    camOffset[1] = pos[1] - parMidPoint[1];
    camOffset[2] = pos[2] - parMidPoint[2];
    pathYaw = -pathYaw;
    rotate_in_xz(camOffset, camOffset, pathYaw);
    pathYaw = -pathYaw;
    pathPitch = -pathPitch;
    rotate_in_yz(camOffset, camOffset, pathPitch);
    pathPitch = -pathPitch;

    // If Mario is distThresh units away from the camera along the path, move the camera
    //! When distThresh != 0, it causes Mario to move slightly towards the camera when running sideways
    //! Set each ParallelTrackingPoint's distThresh to 0 to make Mario truly run parallel to the path
    if (marioOffset[2] > camOffset[2]) {
        if (marioOffset[2] - camOffset[2] > distThresh) {
            camOffset[2] = marioOffset[2] - distThresh;
        }
    } else {
        if (marioOffset[2] - camOffset[2] < -distThresh) {
            camOffset[2] = marioOffset[2] + distThresh;
        }
    }

    // If zoom != 0.0, the camera will move zoom% closer to Mario
    marioOffset[0] = -marioOffset[0] * zoom;
    marioOffset[1] = marioOffset[1] * zoom;
    marioOffset[2] = camOffset[2];

    // make marioOffset[2] == distance from the start of the path
    marioOffset[2] = pathLength / 2 - marioOffset[2];

    pathAngle[0] = pathPitch;
    pathAngle[1] = pathYaw + DEGREES(180);
    pathAngle[2] = 0;

    // Rotate the offset in the direction of the path again
    offset_rotated(pos, path[0], marioOffset, pathAngle);
    vec3f_get_dist_and_angle(path[0], c->pos, &camParDist, &pathPitch, &pathYaw);

    // Adjust the focus. Does nothing, focus is set to Mario at the end
    focOffset[2] = pathLength / 2 - focOffset[2];
    offset_rotated(c->focus, path[0], focOffset, pathAngle);

    // Changing paths, update the stored position offset
    if (sStatusFlags & CAM_FLAG_CHANGED_PARTRACK_INDEX) {
        sStatusFlags &= ~CAM_FLAG_CHANGED_PARTRACK_INDEX;
        sParTrackTransOff.pos[0] = oldPos[0] - c->pos[0];
        sParTrackTransOff.pos[1] = oldPos[1] - c->pos[1];
        sParTrackTransOff.pos[2] = oldPos[2] - c->pos[2];
    }
    // Slowly transition to the next path
    approach_f32_asymptotic_bool(&sParTrackTransOff.pos[0], 0.f, 0.025f);
    approach_f32_asymptotic_bool(&sParTrackTransOff.pos[1], 0.f, 0.025f);
    approach_f32_asymptotic_bool(&sParTrackTransOff.pos[2], 0.f, 0.025f);
    vec3f_add(c->pos, sParTrackTransOff.pos);

    // Check if the camera should go to the next path
    if (sParTrackPath[sParTrackIndex + 1].startOfPath != 0) {
        // get Mario's distance to the next path
        calculate_angles(sParTrackPath[sParTrackIndex + 1].pos, sParTrackPath[sParTrackIndex + 2].pos, &nextPitch, &nextYaw);
        vec3f_set_dist_and_angle(sParTrackPath[sParTrackIndex + 1].pos, nextPathPos, 400.f, nextPitch, nextYaw);
        distToPrev = calc_abs_dist_squared(marioPos, nextPathPos);

        // get Mario's distance to the previous path
        calculate_angles(sParTrackPath[sParTrackIndex + 1].pos, sParTrackPath[sParTrackIndex].pos, &prevPitch, &prevYaw);
        vec3f_set_dist_and_angle(sParTrackPath[sParTrackIndex + 1].pos, prevPathPos, 400.f, prevPitch, prevYaw);
        distToNext = calc_abs_dist_squared(marioPos, prevPathPos);
        if (distToPrev < distToNext) {
            sParTrackIndex++;
            sStatusFlags |= CAM_FLAG_CHANGED_PARTRACK_INDEX;
        }
    }

    // Check if the camera should go to the previous path
    if (sParTrackIndex != 0) {
        // get Mario's distance to the next path
        calculate_angles((*(sParTrackPath + sParTrackIndex)).pos, (*(sParTrackPath + sParTrackIndex + 1)).pos, &nextPitch, &nextYaw);
        vec3f_set_dist_and_angle(sParTrackPath[sParTrackIndex].pos, nextPathPos, 700.f, nextPitch, nextYaw);
        distToPrev = calc_abs_dist_squared(marioPos, nextPathPos);

        // get Mario's distance to the previous path
        calculate_angles((*(sParTrackPath + sParTrackIndex)).pos, (*(sParTrackPath + sParTrackIndex - 1)).pos, &prevPitch, &prevYaw);
        vec3f_set_dist_and_angle(sParTrackPath[sParTrackIndex].pos, prevPathPos, 700.f, prevPitch, prevYaw);
        distToNext = calc_abs_dist_squared(marioPos, prevPathPos);
        if (distToPrev > distToNext) {
            sParTrackIndex--;
            sStatusFlags |= CAM_FLAG_CHANGED_PARTRACK_INDEX;
        }
    }

    // Update the camera focus and return the camera's yaw
    vec3f_copy(focus, marioPos);
    vec3f_get_dist_and_angle(focus, pos, &camParDist, &pathPitch, &pathYaw);
    return pathYaw;
}

/**
 * Parallel tracking mode, the camera faces perpendicular to a line defined by sParTrackPath
 *
 * @see update_parallel_tracking_camera
 */
void mode_parallel_tracking_camera(struct Camera *c) {
    radial_camera_input(c);
    set_fov_function(CAM_FOV_DEFAULT);
    c->nextYaw = update_parallel_tracking_camera(c, c->focus, c->pos);
}

