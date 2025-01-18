# ![](https://i.imgur.com/CeOukzk.gif) HackerSM64 ![](https://i.imgur.com/s0LUbTo.gif)

**AFTER CLONING THE REPO, CHECK OUT THE `include/config` FOLDER BEFORE ANYTHING ELSE! THERE'S A LOT OF STUFF IN THIS REPO THAT CAN BE TOGGLED THERE.**

HackerSM64 now has a discord server! https://discord.gg/brETAakcXr

This repo requires BOTH a US ROM and a JP ROM in order to build. Place baserom.us.z64 in the repo as usual and ALSO include baserom.jp.z64.

This repo needs gcc in order to be able to build it. To install it, run `sudo apt install gcc-mips-linux-gnu`

This is a fork of the ultrasm64 repo by CrashOveride which includes the following commonly used patches (patches marked with `*` are toggleable in the config files):

**Credits**
- **ArcticJaguar725**: Most audio configuration and layout changes, colored ia4 text, floombas, various bugfixes, and more
- **CowQuack**: Adjustable skybox sizes, area-specific skybox function
- **thecozies**: Water surface types, general maintenance, and time
- **MrComit**: General use object defines, JUMP_KICK_FIX
- **aglab2**: Bugfixes (particularly puppycam), refactor stuff
- **someone2639**: math.s and crash screen disam, stack trace, map packing, shiftable segments 2, S2DEX engine
- **Arthurtilly**: ASCII / UTF-8 support, Multilang, Platform Displacement 2
- **Fazana**: PuppyLib, ucode swapping, audio load time optimisations (with Arctic), general hacker qol improvements, visual debug
- **Reonu**: Starting the project + widescreen, reonucam, various defines for hacker QoL, and a custom Spanish (Spain) translation of the game.
- **JoshDuMan**: Decomp guy, general assistance
- **Arceveti**: Silhouette, shadow optimisation, better hanging, breath meter, 4 controller support
- **axollyon**: Console testing, bugfixes, idea-guying, and had a hand in silhouettes
- **Wiseguy**:  World scale reimplementation, silhouette, graph node optimisations, instant input patch, cake screen fix, segmented code support, and various optimizations/fixes
- **Kaze**: Graph node optimisations, automatic optimal collision distance
- **Pyro Jay**: Texture improvements, repo banner art, some QoL stuff
- **CrashOveride**: creating the [ultrasm64](https://github.com/CrashOveride95/ultrasm64) repo
- **falcobuster**: Original coordinate overflow fix (world scale), ASM version of extended bounds, emulator detector
- **anonymous_moose**: porting falco's extended bounds to decomp
- **tuxlovesyou**: `LOAD_MIO0_TEXTURE` macro and moral support
- **devwizard**: the PJ64 pre-v3.0 detection part of the emulator detector
- **Sauraen**: Creating the F3DEX3 microcode. HackerSM64 implementation by red.

Thanks to Frame#5375 and AloXado320 for also helping with silhouette stuff

**Lighting Engine by Wiseguy**
- Lighting Engine is available on a separate branch ([base/lighting-engine](https://github.com/Reonu/HackerSM64/tree/base/lighting-engine)). Instructions on how to use it are in the readme of that branch.

**Puppycam**
- Puppycam is available on the master branch now, you can toggle it in `config/config_camera.h`. *

**Collision:**
- Slope fix and exposed ceilings fix
- No false ledgegrabs fix *
- Jump kick fix *
- Configurable wallkick angle, in degrees *
- Possibility of disabling BLJs *
- Hanging fix (Mario can grab hangable ceilings from any state, instead of only jump or double jump) *
- Increased maximum pole length (The game will read bparam1 and bparam2 together as a single value, so you can have a very long pole) *
- Platform Displacement 2 by Arthurtilly *
- Water Surface Type patch by thecozies
- Better wall collision with rounded corners by FramePerfection, merged by Cheezepin & Arceveti
- Automatically calculate the optimal collision distance for an object based on its vertices, by Kaze *

**Common Hack Changes:**
- Better extended boundaries by anonymous_moose
- Mario head skip *
- Peach letter cutscene skip *
- Exit course while moving *
- Toggle to disable fall damage and the fall damage sound *
- Nonstop stars *
- Removed course-specific camera processing *
- Ability to set Mario's movement speed when hanging from a ceiling *
- Tighter hanging controls (Mario will face the direction of the analog stick directly while hanging from a ceiling) *
- reonucam3: custom camera by Reonu. This is included as a .patch file in the enhancements folder, you need to apply it if you want this camera.
  This video shows a rundown of the features: https://youtu.be/TQNkznX9Z3k
- Ability to disable Mario getting suck in snow or sand

**Hacker QOL:**
- Global, non-level based, star IDs (off by default) *
- Debug mode: prints mario's coordinates, angle and speed, and a FPS counter.
- Automatic console/emulator detection. If emulator is detected, LODs are disabled. *
- Ability to configure whether there's a 100 coin star at all and how many coins are required to spawn it *
- Ability to easily change the warp that EXIT COURSE takes you to via `config/config_menu.h`, or disable it entirely. *
- 16 bit model IDs by someone2639. This means you can have up to 65536 models (lol). You can set the maximum number of model IDs in `config/config_game.h`.
- Apply_patch.sh improved
- Removed the ifdef hell in `file_select.c` and `ingame_menu.c`
- Added Blake's custom function for object model stuff: `obj_set_model` and `obj_has_model`
- Added function to get the model ID from an object: `obj_get_model_id` (by Arceveti)
- The "far" variable is now u16, allowing you to increase the farclip (the max distance at which geometry is rendered). However, when increasing the farclip, make sure to increase the nearclip by the same ratio, or rendering will break on console and LLE plugins.
- Many general use defines for object struct members, meant for use in custom object behaviors. Check `include/object_fields.h` for more info on this. (By MrComit)
- Included `actors/group0.c` in `behavior_data.c`
- The internal ROM name is now set with a define in `config/config_rom.h` to make it simpler
- There is a `gEmulator` variable to detect console or specific emulators and emulator versions
- Expanded audio heap allows for a larger concurrent note count and the importing of more m64 sequences and sound banks (By ArcticJaguar725) *
- You can set a test level in `config/config_debug.h` in order to boot straight into it, so you can quickly test the level you're working on. *
- Allow all surfaces in the game to have a `force` parameter. Activating this doesn't REQUIRE you to set `force` for every surface: If you don't set, it will default to 0x0000 rather than crashing. Increases RAM usage of collision. *
- The clown font includes the entire English alphabet.
- Colored ia4 text support. Format: `"@XXXXXX[YOUR TEXT]@--------"` (By Arthurtilly)
  - Example Text: `"@FF0000RED @00FF00GREEN @0000FFBLUE @FFFFFFWHITE"`
  - NOTE: Text will need to be recolored each time it scrolls in a dialog box, or the custom color will reset. The text will use gDialogTextAlpha as the alpha value when changing the color.
- Toggle visiblity of collision surfaces and object hitboxes with Visual Surface Debug. `config/config_debug.h` has VISUAL_DEBUG which can be turned on to enable this feature.
- Workaround for infinite death loops caused by using the wrong warp type for death warps. Mario's HP will be restored when being warped to any warp if (and only if) he was warped while dead. *

**Other Bugfixes:**
- Castle music fix (fixes the castle music sometimes triggering after getting a dialog) *
- bparam4 fix (the game no longer uses bparam4 to check if an object is Mario and therefore you can safely use it)
- Instant warp offset fix (makes the instant warp offset work even when warping to a different area) *
- haveyourcake, also known as cake screen fix. Made by Wiseguy and ported/PR'd by Cheezepin
- Tree particle fix (Whether a tree uses snow particles or not is decided via the model IDs instead of the course number) *
- Adjustable world scale. You can change the geometry scaling of your level, which allow large levels to render correctly on console and LLE emulators while not hurting anything on HLE plugins.
- A couple vanilla texture fixes
- Smoke fix (the smoke texture uses the correct texture format)

**Neat Misc. Changes:**
- Instant Input patch by Wiseguy (Removes all input lag caused by plugins supporting framebuffer)
  - This means that you'll have to do your framebuffer effects on buffer 0 for emulator, but NOT for console. You can use the `gEmulator` variable to check for console when doing your framebuffer effects.
- Widescreen (16:9) support toggleable by pressing `L` in the pause menu. *
- S2DEX engine by someone2639! To use it, compile with `make TEXT_ENGINE=s2dex_text_engine` or just set `TEXT_ENGINE` to `s2dex_text_engine` in the makefile.
- ia8 (64x64) coins, the vanilla coin texture is upgraded to accomodate. *
- ia8 (64x64) 30 FPS coins (Textures by InTheBeef, cleaned up by Arceveti). *
- Floombas! Simply just retextured goombas with customizable behaviors (does not overwrite standard goombas). *
- HD texture support for intro splash screen (with floombas if enabled). *
- Mario's silhouette is shown when behind most surfaces (By Frame#5375, Axollyon, AloXado320, Wiseguy, Arceveti) *
- Skybox size modifier. You can have 2x, 3x and 4x size skyboxes (you can select the skybox size in `config/config_graphics.h`.) Please note that this might affect console performance, especially 4x mode. 2x or 3x mode is recommended if aiming for console. By CowQuack *
- You can set the black border size to different values for console and emulator. It's set to 0 by default for both. *
- This repo supports a much better implementation of reverb over vanilla's fake echo reverb. Great for caves or eerie levels, as well as just a better audio experience in general. See `audio/synthesis.c` for more details and configuration info. (By ArcticJaguar725) *
- Fazana's "puppyprint" text engine. *
  - Use `print_small_text` to print normal text. The two last params are aligment and how many characters to print (-1 means PRINT_ALL).
  - Use `render_multi_image` to draw large texture rectangles consisting of multiple images on the screen.
  - More info in `puppyprint.c`
- Wiseguy's Farcall TLB mapping allows to store executable code inside uncompressed segments, that can be loaded and ran as needed, instead of it having to be loaded at all times. See `farcall.h` in the include folder for instructions and details.
- Red Coin Stars now support up to 99 red coins! In addition, multi-area red coin missions can now be created by setting the 2nd behavior paramater of the red coin star to the number of reds required for the star to spawn.
# UltraSM64

- This repo contains a full decompilation of Super Mario 64 (J), (U), (E), and (SH).
- Naming and documentation of the source code and data structures are in progress.
- It has been edited to allow for the usage of the final "N64 OS" library, version ``2.0L``
- Shindou Rumble Pak code is on for all regions.
- Targeting the iQue Player is supported.
- Saving to 32kbyte/256kbit SRAM is supported.
- Newer compression options are supported.
- UNFLoader (flashcart USB library) is supported, allowing for debugging on EverDrive/64Drive.
- It has been patched with someone2639's shiftable segments patch
- Wiseguy's instant input patch has been added to allow for less input lag on emulation (Does not affect console)
  This does mean that any framebuffer effects will have to be done on buffer 0 if targeting emulators
- Automatic console and emulator detection: Use the `gEmulator` variable to wrap your code in an emulator check.
- Separate defines for emulator and console black border height.

Requirements are the same as regular SM64, however a GCC MIPS cross compiler is also required. If you're on Debian-like Linux, you can use the ``gcc-mips-linux-gnu`` package. The toolchain that comes with my SDK is also supported.

## Additional Prerequisites

BinPNG (the CI texture converter) requires some python3 dependencies. Use pip to install them.

``pip install pypng bitstring``

## UNFLoader support

The repository supports UNFLoader for debugging.
To build with UNF, run make with ``UNF=1``.

Further instructions can be found at the [official repository](https://github.com/buu342/N64-UNFLoader)

**NOTE: Closing the UNFLoader window will result in your game eventually hanging due to lacking a USB device to send messages to, so beware of that**

## Multi-Save support
The repository supports SRAM in addition to EEPROM. The standard save data functions are #ifdef'd to accommedate this.
To build with SRAM support, run make with ``SAVETYPE=sram``.

I may attempt FlashRAM in the future.

## Multi-Console support
The repository supports targeting the iQue Player in addition to the N64. The iQue libultra is ***NOT*** compatible with N64 in many ways, so it is currently NOT possible to have one build for both consoles.
To target iQue, run make with the ``CONSOLE=bb`` argument.

## Compression

The repo also supports RNC (Rob Northen Compression). RNC has two methods.

Method 1 is designed to compress as small as possible, while method 2 is designed so that decompression is as fast as possible.

Method 1 is the current default, and is the best all-rounder in terms of speed and ratio.

Both methods are fast. Method 1 has better compression than 2, so I suggest using method 1 if using RNC.

To switch to RNC, run make with either ``COMPRESS=rnc1`` or ``COMPRESS=rnc2``, depending on preferred method.

The repository also supports using DEFLATE compression. This boasts a better compression ratio, but at a slight cost to load times.
On average I'd estimate that the bottleneck on decompression is about 1-2 seconds.

To switch to gzip, run make with the ``COMPRESS=gzip`` argument.

The repo also supports gziping with ``libdeflate-gzip``. This compresses at a slightly better ratio than standard ``gzip``, with no real downside from a decompression standpoint.

To use ``libdeflate-gzip``, first clone the [repo](https://github.com/ebiggers/libdeflate), then `make` and `make install` it.

Then run make for sm64 with ``GZIPVER=libdef`` in addition to ``COMPRESS=gzip``

The repo also supports building a ROM with no compression.
This is not recommended as it increases ROM size significantly, with little point other than load times decreased to almost nothing.
To switch to no compression, run make with the ``COMPRESS=uncomp`` argument.

## FAQ

Q: Why in the hell are you bundling your own build of ``ld``?

A: Newer binutils (Like the one bundled with Ubuntu, 2.34) break linking with libultra builds due to local asm symbols.

This puts me at a crossroads of either touching leaked code and requiring GCC, or just using an older linker that works just fine.

I went with the latter.
Thanks to "someone2639" for this hacky-ass idea

Q: Will this allow me to use FlashRAM/Transfer Pak/microcode swapping/Other Cool N64 Features?

A: Theoretically, all yes.

## Installation help

Go read the original SM64 repo README.md
