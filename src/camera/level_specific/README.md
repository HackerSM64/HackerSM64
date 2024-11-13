# Level-Specific Camera Processing

Some vanilla levels in SM64 extend the camera functionality by use of volumes. When Mario enters these areas, a function is called that can change the camera mode, correct camera movement through tight spaces, etc. If you aren't making a vanilla edit, these functions at best litter your game, and at worst make camera.c unreadable.

While it would make sense to also place camera trigger definitions here (as is done with cutscenes), this would require a change to Fast64.
