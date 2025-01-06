OUTPUT_ARCH (mips)

#define LINKER /* Removes externs from preprocessed script */
#define TARGET_N64
#include "include/segments.h"
#include "include/config.h"
#undef LINKER

#define BEGIN_SEG(name, addr) \
    _##name##SegmentStart = ADDR(.name); \
    _##name##SegmentRomStart = __romPos; \
    .name addr : AT(__romPos)

#define END_SEG(name) \
    _##name##SegmentEnd = ADDR(.name) + SIZEOF(.name); \
    _##name##SegmentRomEnd = __romPos + SIZEOF(.name); \
    __romPos += SIZEOF(.name);

#define BEGIN_NOLOAD(name) \
    _##name##SegmentBssStart = ADDR(.name.noload); \
    .name.noload (NOLOAD) :

#define END_NOLOAD(name) \
    _##name##SegmentBssEnd = ADDR(.name.noload) + SIZEOF(.name.noload); \
    _##name##SegmentBssSize = SIZEOF(.name.noload);

#define YAY0_SEG(name, segAddr) \
   BEGIN_SEG(name##_yay0, segAddr) \
   { \
      KEEP(BUILD_DIR/bin/name.szp.o(.data*)); \
      . = ALIGN(0x10); \
   } \
   END_SEG(name##_yay0) \
   _##name##_mio0SegmentRomStart = _##name##_yay0SegmentRomStart; \
   _##name##_mio0SegmentRomEnd   = _##name##_yay0SegmentRomEnd;

#define YAY0_EU_SEG(name, segAddr) \
   BEGIN_SEG(name##_yay0, segAddr) \
   { \
      KEEP(BUILD_DIR/bin/eu/name.szp.o(.data*)); \
      . = ALIGN(0x10); \
   } \
   END_SEG(name##_yay0) \
   _##name##_mio0SegmentRomStart = _##name##_yay0SegmentRomStart; \
   _##name##_mio0SegmentRomEnd   = _##name##_yay0SegmentRomEnd;

#define STANDARD_LEVEL(name) \
   BEGIN_SEG(name##_segment_7, 0x07000000) \
   { \
      KEEP(BUILD_DIR/levels/name/leveldata.szp.o(.data*)); \
      . = ALIGN(0x10); \
   } \
   END_SEG(name##_segment_7) \
   BEGIN_SEG(name, 0x0E000000) \
   { \
      KEEP(BUILD_DIR/levels/name/script.o(.*data*)); \
      KEEP(BUILD_DIR/levels/name/script.o(.text*)); \
      KEEP(BUILD_DIR/levels/name/script.o(.rodata*)); \
      KEEP(BUILD_DIR/levels/name/geo.o(.*data*)); \
      KEEP(BUILD_DIR/levels/name/geo.o(.text*)); \
      KEEP(BUILD_DIR/levels/name/geo.o(.rodata*)); \
   } \
   END_SEG(name) \
   BEGIN_NOLOAD(name) \
   { \
      KEEP(BUILD_DIR/levels/name/script.o(.bss*)); \
      KEEP(BUILD_DIR/levels/name/geo.o(.bss*)); \
   } \
   END_NOLOAD(name)

#define STANDARD_OBJECTS(name, segAddr, geoAddr) \
   BEGIN_SEG(name##_yay0, segAddr) \
   { \
      KEEP(BUILD_DIR/actors/name.szp.o(.data*)); \
      . = ALIGN(0x10); \
   } \
   END_SEG(name##_yay0) \
   BEGIN_SEG(name##_geo, geoAddr) \
   { \
      KEEP(BUILD_DIR/actors/name##_geo.o(.*data*)); \
      KEEP(BUILD_DIR/actors/name##_geo.o(.rodata*)); \
      KEEP(BUILD_DIR/actors/name##_geo.o(.text*)); \
   } \
   END_SEG(name##_geo) \
   BEGIN_NOLOAD(name##_geo) \
   { \
      KEEP(BUILD_DIR/actors/name##_geo.o(.bss*)); \
   } \
   END_NOLOAD(name##_geo) \
   _##name##_mio0SegmentRomStart = _##name##_yay0SegmentRomStart; \
   _##name##_mio0SegmentRomEnd = _##name##_yay0SegmentRomEnd;

SECTIONS
{
   __romPos = 0;

   BEGIN_SEG(boot, 0x04000000)
   {
      KEEP(BUILD_DIR/asm/rom_header.o(.text*));
      KEEP(BUILD_DIR/asm/ipl3.o(.text*));
   }
   END_SEG(boot)

   . = (RAM_START + 0x400);
   BEGIN_NOLOAD(zbuffer)
   {
      BUILD_DIR/src/buffers/zbuffer.o(.bss*);
   }
   END_NOLOAD(zbuffer)

   . = _zbufferSegmentBssEnd;
   BEGIN_NOLOAD(buffers)
   {
      BUILD_DIR/src/buffers/buffers.o(.bss*);
      BUILD_DIR/src/audio/globals_start.o(.bss*);
      BUILD_DIR/src/audio/synthesis.o(.bss*);
      BUILD_DIR/src/audio/heap.o(.bss*);
      BUILD_DIR/src/audio/load.o(.bss*);
      BUILD_DIR/src/audio/data.o(.bss*);
      BUILD_DIR/src/audio*.o(.bss*);
      . = ALIGN(0x10);
      BUILD_DIR/src/buffers/gfx_output_buffer.o(.bss*);
   }
   END_NOLOAD(buffers)

   . = _buffersSegmentBssEnd;

#ifdef HVQM
   BEGIN_NOLOAD(hvqmwork)
   {
      BUILD_DIR/src/buffers/hvqmwork.o(.bss*);
   }
   END_NOLOAD(hvqmwork)
   BEGIN_NOLOAD(adpcmbuf)
   {
      BUILD_DIR/src/buffers/adpcmbuf.o(.bss*);
   }
   END_NOLOAD(adpcmbuf)
   BEGIN_NOLOAD(hvqbuf)
   {
      BUILD_DIR/src/buffers/hvqbuf.o(.bss*);
   }
   END_NOLOAD(hvqbuf)

   . = _hvqbufSegmentBssEnd;
#endif

/* hardcoded symbols to satisfy preliminary link for map parser */
#ifndef DEBUG_MAP_STACKTRACE
      _mapDataSegmentRomStart = 0;
      gMapEntries   = 0;
      gMapEntrySize = 0;
      gMapStrings   = 0;
#endif

   BEGIN_SEG(main, .) SUBALIGN(16)
   {
      KEEP(BUILD_DIR/asm/entry.o(.text*));
      KEEP(BUILD_DIR/asm/n64_assert.o(.text*));
#ifdef EEP
      KEEP(BUILD_DIR/asm/vc_bin.o(.text*));
#endif
      KEEP(BUILD_DIR/asm/emux.o(.text*));
      KEEP(BUILD_DIR/asm/pj64_get_count_factor_asm.o(.text*));
      KEEP(BUILD_DIR/asm/round.o(.text*));
      KEEP(BUILD_DIR/asm/fcr31.o(.text*));

      BUILD_DIR/src/boot*.o(.text*);
      BUILD_DIR/src/hvqm*.o(.text*);
      BUILD_DIR/src/usb*.o(.text*);
      BUILD_DIR/src/audio*.o(.text*);
#ifdef S2DEX_TEXT_ENGINE
      BUILD_DIR/src/s2d_engine*.o(.text*);
#endif
#ifdef LIBPL
      BUILD_DIR/lib/libpl*.o(.text*);
#endif
      *ULTRALIB.a:*.o(.text*);
      */libnustd.a:*.o(.text*);
      *libgcc.a:*.o(.text*);
      */libz.a:*.o(.text*);
      */libhvqm2.a:*.o(.text*);
      lib/rspboot.o(.text*);
#include "rsptext.inc.ld"
      BUILD_DIR/lib/aspMain.o(.text*);
      lib/PR/audio/n_aspMain.o(.text*);
      lib/PR/hvqm/hvqm2sp1.o(.text*);
      _mainSegmentTextEnd = .;

      /* data */
      BUILD_DIR/asm/n64_assert.o(.*data*);
      BUILD_DIR/src/boot*.o(.*data*);
      BUILD_DIR/src/audio*.o(.*data*);
#ifdef S2DEX_TEXT_ENGINE
      BUILD_DIR/src/s2d_engine*.o(.*data*);
#endif
#ifdef LIBPL
      BUILD_DIR/lib/libpl*.o(.*data*);
#endif
      *ULTRALIB.a:*.o(.data*);
      */libhvqm2.a:*.o(.data*);
      */libz.a:*.o(.data*);
#include "rspdata.inc.ld"
      BUILD_DIR/lib/aspMain.o(.data*);
      lib/PR/audio/n_aspMain.o(.data*);
      lib/PR/hvqm/hvqm2sp1.o(.data*);

      /* rodata */
      BUILD_DIR/src/boot*.o(.rodata*);
      BUILD_DIR/src/usb*.o(.rodata*);
      BUILD_DIR/src/audio*.o(.rodata*);
#ifdef S2DEX_TEXT_ENGINE
      BUILD_DIR/src/s2d_engine*.o(.rodata*);
#endif
#ifdef LIBPL
      BUILD_DIR/lib/libpl*.o(.rodata*);
#endif
      *ULTRALIB.a:*.o(.rodata*);
      *libgcc.a:*.o(.rodata*);
      */libz.a:*.o(.rodata*);

      lib/PR/hvqm/hvqm2sp1.o(.rodata*);
   }
   END_SEG(main)
   BEGIN_NOLOAD(main)
   {
      BUILD_DIR/src/boot*.o(.*bss*);
      BUILD_DIR/src/hvqm*.o(.*bss*);
      BUILD_DIR/src/usb*.o(.*bss*);
      BUILD_DIR/src/audio*.o(.*bss*);
#ifdef S2DEX_TEXT_ENGINE
      BUILD_DIR/src/s2d_engine*.o(.*bss*);
#endif
#ifdef LIBPL
      BUILD_DIR/lib/libpl*.o(.*bss*);
#endif
      *ULTRALIB.a:*.o(COMMON);
      *ULTRALIB.a:*.o(.scommon);
      *ULTRALIB.a:*.o(.*bss*);
      */libhvqm2.a:*.o(.bss*);
      */libz.a:*.o(.bss*);
      . = ALIGN(0x8);
   }
   END_NOLOAD(main)

   . = _mainSegmentBssEnd;

   BEGIN_SEG(engine, .)
   {
      BUILD_DIR/src/game*.o(.text*);
      BUILD_DIR/src/game/behavior_actions.o(.text*);
      BUILD_DIR/src/game/obj_behaviors_2.o(.text*);
      BUILD_DIR/src/game/obj_behaviors.o(.text*);
      BUILD_DIR/src/game/object_helpers.o(.text*);
      BUILD_DIR/src/game/rendering_graph_node.o(.text*);
      BUILD_DIR/src/engine/math_util.o(.text*);
      BUILD_DIR/src/engine/surface_collision.o(.text*);
      BUILD_DIR/src/engine/surface_load.o(.text*);
      BUILD_DIR/src/engine/graph_node.o(.text*);
      BUILD_DIR/src/engine*.o(.text*);
      _engineSegmentTextEnd = .;
      /* data */
      BUILD_DIR/src/game*.o(.data*);
      BUILD_DIR/src/engine*.o(.data*);
      BUILD_DIR/src/usb*.o(.data*);
      /* sdata */
      BUILD_DIR/src/game*.o(.sdata*);
      BUILD_DIR/src/engine*.o(.sdata*);
      BUILD_DIR/src/usb*.o(.data*);
      /* rodata */
      BUILD_DIR/src/game*.o(.rodata*);
      BUILD_DIR/src/engine*.o(.rodata*);
      BUILD_DIR/src/usb*.o(.rodata*);
      . = ALIGN(0x10);
   }
   END_SEG(engine)
   BEGIN_NOLOAD(engine)
   {
      BUILD_DIR/src/game*.o(.*bss*);
      BUILD_DIR/src/engine*.o(.bss*);
      . = ALIGN(0x40);
   }
   END_NOLOAD(engine)

   . = _engineSegmentBssEnd;
   BEGIN_NOLOAD(framebuffers)
   {
      BUILD_DIR/src/buffers/framebuffers.o(.bss*);
   }
   END_NOLOAD(framebuffers)

   __mainPoolStart = .;
   __mainPoolSize  = (RAM_END - .);

   __expansionRamStart = (RAM_START + (RAM_1MB * 4));
#ifndef USE_EXT_RAM
   #ifndef PRELIMINARY
         ASSERT((. <= __expansionRamStart), "Error: RDRAM expanded into Expansion RAM, despite Expansion RAM not being defined.")
   #endif
#endif

   BEGIN_SEG(entry, 0x10000000)
   {
      BUILD_DIR/levels/entry.o(.data*);
   }
   END_SEG(entry)

   /* load YAY0 and level data */
   YAY0_SEG(segment2, 0x02000000)
#ifdef VERSION_EU
   YAY0_EU_SEG(translation_en, 0x19000000)
   YAY0_EU_SEG(translation_fr, 0x19000000)
   YAY0_EU_SEG(translation_de, 0x19000000)
#endif

   /* mario actor group */
   STANDARD_OBJECTS(group0,  0x04000000, 0x17000000)

   /* load the other actor groups */
   STANDARD_OBJECTS(group1,  0x05000000, 0x0C000000)
   STANDARD_OBJECTS(group2,  0x05000000, 0x0C000000)
   STANDARD_OBJECTS(group3,  0x05000000, 0x0C000000)
   STANDARD_OBJECTS(group4,  0x05000000, 0x0C000000)
   STANDARD_OBJECTS(group5,  0x05000000, 0x0C000000)
   STANDARD_OBJECTS(group6,  0x05000000, 0x0C000000)
   STANDARD_OBJECTS(group7,  0x05000000, 0x0C000000)
   STANDARD_OBJECTS(group8,  0x05000000, 0x0C000000)
   STANDARD_OBJECTS(group9,  0x05000000, 0x0C000000)
   STANDARD_OBJECTS(group10, 0x05000000, 0x0C000000)
   STANDARD_OBJECTS(group11, 0x05000000, 0x0C000000)
   STANDARD_OBJECTS(group12, 0x06000000, 0x0D000000)
   STANDARD_OBJECTS(group13, 0x06000000, 0x0D000000)
   STANDARD_OBJECTS(group14, 0x06000000, 0x0D000000)
   STANDARD_OBJECTS(group15, 0x06000000, 0x0D000000)
   STANDARD_OBJECTS(group16, 0x06000000, 0x0D000000)
   STANDARD_OBJECTS(group17, 0x06000000, 0x0D000000)

   /* load the common actor groups */
   STANDARD_OBJECTS(common0, 0x08000000, 0x0F000000)
   STANDARD_OBJECTS(common1, 0x03000000, 0x16000000)

   /* use segmented addressing for behaviors */
   BEGIN_SEG(behavior, 0x13000000)
   {
      KEEP(BUILD_DIR/data/behavior_data.o(.data*));
      KEEP(BUILD_DIR/data/behavior_data.o(.rodata*));
      KEEP(BUILD_DIR/data/behavior_data.o(.text*));
   }
   END_SEG(behavior)
   BEGIN_NOLOAD(behavior)
   {
      KEEP(BUILD_DIR/data/behavior_data.o(.bss*));
   }
   END_NOLOAD(behavior)


   /* 0x8016F000 21D7D0-255EC0 [386F0] */
   BEGIN_SEG(goddard, (RAM_END - GODDARD_SIZE))
   {
      KEEP(BUILD_DIR/src/menu*.o(.text*));
      _goddardSegmentTextEnd = .;
      KEEP(BUILD_DIR/src/menu*.o(.data*));
      KEEP(BUILD_DIR/src/menu*.o(.rodata*));
#ifdef KEEP_MARIO_HEAD
      KEEP(BUILD_DIR/libgoddard.a:*.o(.text*));
      KEEP(BUILD_DIR/libgoddard.a:gd_main.o(.data*));
      KEEP(BUILD_DIR/libgoddard.a:draw_objects.o(.data*));
      KEEP(BUILD_DIR/libgoddard.a:objects.o(.data*));
      KEEP(BUILD_DIR/libgoddard.a:particles.o(.data*));
      KEEP(BUILD_DIR/libgoddard.a:dynlist_proc.o(.data*));
      KEEP(BUILD_DIR/libgoddard.a:debug_utils.o(.data*));
      KEEP(BUILD_DIR/libgoddard.a:joints.o(.data*));
      KEEP(BUILD_DIR/libgoddard.a:shape_helper.o(.data*));
      KEEP(BUILD_DIR/libgoddard.a:renderer.o(.data*));
      KEEP(BUILD_DIR/libgoddard.a:gd_main.o(.rodata*));
      KEEP(BUILD_DIR/libgoddard.a:gd_memory.o(.rodata*));
      KEEP(BUILD_DIR/libgoddard.a:draw_objects.o(.rodata*));
      KEEP(BUILD_DIR/libgoddard.a:objects.o(.rodata*));
      KEEP(BUILD_DIR/libgoddard.a:skin_movement.o(.rodata*));
      KEEP(BUILD_DIR/libgoddard.a:particles.o(.rodata*));
      KEEP(BUILD_DIR/libgoddard.a:dynlist_proc.o(.rodata*));
      KEEP(BUILD_DIR/libgoddard.a:old_menu.o(.rodata*));
      KEEP(BUILD_DIR/libgoddard.a:debug_utils.o(.rodata*));
      KEEP(BUILD_DIR/libgoddard.a:joints.o(.rodata*));
      KEEP(BUILD_DIR/libgoddard.a:skin.o(.rodata*));
      KEEP(BUILD_DIR/libgoddard.a:gd_math.o(.rodata*));
      KEEP(BUILD_DIR/libgoddard.a:shape_helper.o(.rodata*));
      KEEP(BUILD_DIR/libgoddard.a:renderer.o(.rodata*));
#endif
      . = ALIGN(16);
   }
   END_SEG(goddard)
   BEGIN_NOLOAD(goddard)
   {
      KEEP(BUILD_DIR/src/menu*.o(.bss*));
#ifdef KEEP_MARIO_HEAD
      KEEP(BUILD_DIR/libgoddard.a:*.o(.bss*));
#endif
      . = ALIGN(16);
   }
   END_NOLOAD(goddard)

#ifdef PRELIMINARY
      GODDARD_SIZE = SIZEOF(.goddard) + SIZEOF(.goddard.noload);
#else
      ASSERT((. <= (SEG_POOL_START + POOL_SIZE)), "Error: extended past pool end.")
#endif

   /* 0x268020 0x268020-0 [0] */
   BEGIN_SEG(intro, 0x14000000)
   {
      KEEP(BUILD_DIR/levels/intro/script.o(.*data));
      KEEP(BUILD_DIR/levels/intro/script.o(.rodata*));
      KEEP(BUILD_DIR/levels/intro/script.o(.text*));
      KEEP(BUILD_DIR/levels/intro/geo.o(.data*));
   }
   END_SEG(intro)
   BEGIN_NOLOAD(intro)
   {
      KEEP(BUILD_DIR/levels/intro/script.o(.bss*));
   }
   END_NOLOAD(intro)
   BEGIN_SEG(intro_segment_7, 0x07000000)
   {
      KEEP(BUILD_DIR/levels/intro/leveldata.szp.o(.data*));
      . = ALIGN(0x10);
   }
   END_SEG(intro_segment_7)

   YAY0_SEG(debug_level_select, 0x07000000)
   YAY0_SEG(title_screen_bg,    0x0A000000)

#ifdef KEEP_MARIO_HEAD
   BEGIN_SEG(gd_dynlists, 0x04000000)
   {
      BUILD_DIR/libgoddard.a:dynlist_test_cube.o(.data*);
      BUILD_DIR/libgoddard.a:dynlist_unused.o(.data*);
      BUILD_DIR/libgoddard.a:dynlist_mario_face.o(.data*);
      BUILD_DIR/libgoddard.a:dynlists_mario_eyes.o(.data*);
      BUILD_DIR/libgoddard.a:dynlists_mario_eyebrows_mustache.o(.data*);
      BUILD_DIR/libgoddard.a:dynlist_mario_master.o(.data*);
      BUILD_DIR/libgoddard.a:anim_mario_mustache_right.o(.data*);
      BUILD_DIR/libgoddard.a:anim_mario_mustache_left.o(.data*);
      BUILD_DIR/libgoddard.a:anim_mario_lips_1.o(.data*);
      BUILD_DIR/libgoddard.a:anim_mario_lips_2.o(.data*);
      BUILD_DIR/libgoddard.a:anim_mario_eyebrows_1.o(.data*);
      BUILD_DIR/libgoddard.a:anim_group_1.o(.data*);
      BUILD_DIR/libgoddard.a:anim_group_2.o(.data*);
      BUILD_DIR/libgoddard.a:dynlist_test_cube.o(.rodata*);
      BUILD_DIR/libgoddard.a:dynlist_unused.o(.rodata*);
      BUILD_DIR/libgoddard.a:*.o(.data*);
      BUILD_DIR/libgoddard.a:*.o(.rodata*);
   }
   END_SEG(gd_dynlists)
#endif

   gMainLevels = __romPos;

   BEGIN_SEG(menu, 0x14000000)
   {
      KEEP(BUILD_DIR/levels/menu/script.o(.data*));
      KEEP(BUILD_DIR/levels/menu/geo.o(.data*));
   }
   END_SEG(menu)
   BEGIN_SEG(menu_segment_7, 0x07000000)
   {
      KEEP(BUILD_DIR/levels/menu/leveldata.szp.o(.data*));
      . = ALIGN(0x10);
   }
   END_SEG(menu_segment_7)

   BEGIN_SEG(scripts, 0x15000000)
   {
      BUILD_DIR/levels/scripts.o(.data*);
   }
   END_SEG(scripts)

   YAY0_SEG(water_skybox,       0x0A000000)
   YAY0_SEG(ccm_skybox,         0x0A000000)
   YAY0_SEG(clouds_skybox,      0x0A000000)
   YAY0_SEG(bitfs_skybox,       0x0A000000)
   YAY0_SEG(wdw_skybox,         0x0A000000)
   YAY0_SEG(cloud_floor_skybox, 0x0A000000)
   YAY0_SEG(ssl_skybox,         0x0A000000)
   YAY0_SEG(bbh_skybox,         0x0A000000)
   YAY0_SEG(bidw_skybox,        0x0A000000)
   YAY0_SEG(bits_skybox,        0x0A000000)

   // Texture bins
   YAY0_SEG(fire,     0x09000000)
   YAY0_SEG(spooky,   0x09000000)
   YAY0_SEG(generic,  0x09000000)
   YAY0_SEG(water,    0x09000000)
   YAY0_SEG(sky,      0x09000000)
   YAY0_SEG(snow,     0x09000000)
   YAY0_SEG(cave,     0x09000000)
   YAY0_SEG(machine,  0x09000000)
   YAY0_SEG(mountain, 0x09000000)
   YAY0_SEG(grass,    0x09000000)
   YAY0_SEG(outside,  0x09000000)
   YAY0_SEG(inside,   0x09000000)
   YAY0_SEG(effect,   0x0B000000)

#define STUB_LEVEL(_0, _1, _2, _3, _4, _5, _6, _7, _8)
#define DEFINE_LEVEL(_0, _1, _2, folder, _4, _5, _6, _7, _8, _9, _10) STANDARD_LEVEL(folder)

   #include "levels/level_defines.h"

#undef STUB_LEVEL
#undef DEFINE_LEVEL

   /* 4E9FA0-? [?] */
   BEGIN_SEG(assets, __romPos)
   {
      KEEP(BUILD_DIR/assets/mario_anim_data.o(.data*));
      KEEP(BUILD_DIR/assets/mario_anim_data.o(.rodata*));
      KEEP(BUILD_DIR/assets/demo_data.o(.data*));
      KEEP(BUILD_DIR/sound/sound_data.o(.data*));
   }
   END_SEG(assets)
#ifdef HVQM
   BEGIN_SEG(capcom, __romPos) SUBALIGN(2)
   {
      KEEP(BUILD_DIR/data/capcom.o(.data*));
   }
	END_SEG(capcom)
#endif

#ifdef DEBUG_MAP_STACKTRACE
   BEGIN_SEG(mapData, (RAM_END - 0x00100000)) {
      KEEP(BUILD_DIR/asm/debug/map.o(.data*));
   }
   END_SEG(mapData)
#endif
   /* DWARF debug sections.
      Symbols in the DWARF debugging sections are relative to the beginning
      of the section so we begin them at 0.  */
   /* DWARF 1 */
   .debug          0 : { *(.debug) }
   .line           0 : { *(.line) }
   /* GNU DWARF 1 extensions */
   .debug_srcinfo  0 : { *(.debug_srcinfo) }
   .debug_sfnames  0 : { *(.debug_sfnames) }
   /* DWARF 1.1 and DWARF 2 */
   .debug_aranges  0 : { *(.debug_aranges) }
   .debug_pubnames 0 : { *(.debug_pubnames) }
   /* DWARF 2 */
   .debug_info     0 : { *(.debug_info .gnu.linkonce.wi.*) }
   .debug_abbrev   0 : { *(.debug_abbrev) }
   .debug_line     0 : { *(.debug_line .debug_line.* .debug_line_end ) }
   .debug_frame    0 : { *(.debug_frame) }
   .debug_str      0 : { *(.debug_str) }
   .debug_loc      0 : { *(.debug_loc) }
   .debug_macinfo  0 : { *(.debug_macinfo) }
   /* SGI/MIPS DWARF 2 extensions */
   .debug_weaknames 0 : { *(.debug_weaknames) }
   .debug_funcnames 0 : { *(.debug_funcnames) }
   .debug_typenames 0 : { *(.debug_typenames) }
   .debug_varnames  0 : { *(.debug_varnames) }
   /* DWARF 3 */
   .debug_pubtypes 0 : { *(.debug_pubtypes) }
   .debug_ranges   0 : { *(.debug_ranges) }
   /* DWARF Extension.  */
   .debug_macro    0 : { *(.debug_macro) }
   .gnu.attributes 0 : { KEEP (*(.gnu.attributes)) }
   /* Discard everything not specifically mentioned above. */
   /DISCARD/ :
   {
      *(*);
   }
}
