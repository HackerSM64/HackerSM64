apultra -- a new, opensource optimal compressor for the aPLib format
====================================================================

apultra is a command-line tool and a library that compresses bitstreams in the aPLib format. 

The tool produces files that are 5 to 7% smaller on average than appack, the aPLib compressor. Unlike the similar [cap](https://github.com/svendahl/cap) compressor, apultra can compress files larger than 64K.

apultra is written in portable C. It is fully open-source under a liberal license. You can continue to use the regular aPLib decompression libraries for your target environment. You can do whatever you like with it.

    Example compression with vmlinux-5.3.0-1-amd64

    original       27923676 (100,00%)
    appack         7370129 (26,39%)
    gzip 1.8       7166179 (25,66%)
    apultra 1.4.1  6910729 (24,75%)


The output is fully compatible with the original [aPLib](http://ibsensoftware.com/products_aPLib.html) by Jørgen Ibsen.

Inspirations:

 * [cap](https://github.com/svendahl/cap) by Sven-Åke Dahl. 
 * [Charles Bloom](http://cbloomrants.blogspot.com/)'s compression blog. 
 * [LZ4](https://github.com/lz4/lz4) by Yann Collet. 
 * spke for help and support

Some projects that use apultra for compression:
 * [Hyperdrive](https://www.usebox.net/jjm/hyperdrive/), a new, excellent shoot'em up for the Amstrad CPC 464/6128/GX4000, in cartridge format, by usebox.net.
 * [Brick Rick](https://www.usebox.net/jjm/brick-rick/), a new game for the Amstrad CPC 464/6128 by usebox.net. A physical copy can be ordered from [Polyplay](https://www.polyplay.xyz/navi.php?suche=Brick+Rick&lang=eng)
 * [Brick Rick: Graveyard Shift](https://www.usebox.net/jjm/graveyard-shift/), a similar new game for the ZX Spectrum 128K by usebox.net. Get it on tape from [TFW8b.com](https://www.thefuturewas8bit.com/cas019.html)
 * [Kitsune's Curse](https://www.usebox.net/jjm/kitsunes-curse/), another new title for the CPC line by usebox.net.
 * [Sgt. Helmet's Training Day](https://www.mojontwins.com/juegos_mojonos/sgt-helmet-training-day-2020-cpc/), a new game for the Amstrad CPC by the Mojon Twins (using their MK1 engine).
 * [Prince Dastan - Sokoban Within](https://www.pouet.net/prod.php?which=87382), a CPCRetroDev 2020 game for the Amstrad CPC by Euphoria Design 
 * [Petris](https://github.com/bbbbbr/Petris), a homebrew game for the Gameboy.
 * [Mr Palot](https://github.com/graelx/mrpalot), a ZX Spectrum game made with the Mojon Twins MK1 engine.
 * [rasm](https://github.com/EdouardBERGE/rasm), a popular Z80 assembler, features built-in support for apultra-compressed data sections.

Also of interest:
 * [oapack](https://gitlab.com/eugene77/oapack) by Eugene Larchenko, a brute-force (exhaustive) optimal packer for the aPLib format. 
 * [Streamed 8088 decompressor](https://hg.ulukai.org/ecm/inicomp/file/4c6ae7774f3a/apl.asm) for aPLib by C. Masloch
 * [Gameboy decompressor](https://github.com/untoxa/UnaPACK.GBZ80) by untoxa

License:

* The apultra code is available under the Zlib license.
* The match finder (matchfinder.c) is available under the CC0 license due to using portions of code from Eric Bigger's Wimlib in the suffix array-based matchfinder.
