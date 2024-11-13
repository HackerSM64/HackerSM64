# Camera

The camera system in SM64 is notoriously complex, with the original file exceeding 11000 lines of code. HackerSM64 separates the camera code into more relevant and granular files. An index of changes is provided below.

```
src/game
└── camera.c: the default camera modes, as well as CameraTrigger and sZoomOutAreaMasks definitions (as not to interfere with Fast64)
src/camera
├── cutscenes
│   └── {cutscene}.c: the code for a cutscene (or group of scenes), and its cutscene struct definition
├── level_specific
│   └── {level}.c: camera trigger code for a vanilla level, if applicable
├── modes
│   └── {mode}.c: code for a specific camera mode
├── cutscene_helpers.c: cutscene and spline procesing, as well as shared cutscene functions
├── camera_geo.c: code for generating the Camera GraphNode
├── camera_math.c: math routines
└── camera_modes.c: shared functions used by camera modes
```
