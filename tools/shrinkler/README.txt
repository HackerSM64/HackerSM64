
Shrinkler executable file compressor for Amiga by Blueberry

Designed for maximum compression of Amiga 64k and 4k intros, and
everything in between.

Executables for different platforms are available in their respective
subdirectories. The output executables are compatible with all Amiga CPUs
and kickstarts.

Run with no arguments for a list of options. For the options controlling
compression efficiency, higher values generally result in better
compression, at the cost of higher time and/or memory requirements.

Data decompression source code for 68000 is included. A version for Z80 is
here: http://www.cpcwiki.eu/forum/programming/shrinkler-z80-decrunch-routine
A version for 6502 is here: https://github.com/atari8xxl/unShrinkler


History:

2022-02-22:  Version 4.7. Faster and with new features:
             Now built with Amiga-GCC. 15% faster and no longer needs ixemul.
             Option to disable parity context, for use with 8-bit data.
             Option to support commandline arguments. Works with all headers.
             Data sizes are no longer rounded up to a multiple of 4 bytes.
             Option to write header in front of data file with sizes and flags.
             Utility code to load and decompress a data file with header.
             Upped default compression preset to -3.

2020-02-22:  Version 4.6. Better support for large files:
             Faster suffix array construction (the pause before each hunk).
             Fixed verify error when compressed size is above roughly 2MB.
             Increased max number of reference edges to 100000000.
             Support HUNK_RELOC32SHORT and HUNK_DREL32 relocation hunks.
             Fixed broken output formatting when the first hunk is skipped.
             Included link to Z80 decompressor in README.

2018-01-03:  Version 4.5. Fixes and convenience features:
             Crunched programs no longer depend on undocumented A3 contents.
             This fixes programs with icon crashing when run from Workbench.
             Quick -1, ..., -9 options for compression presets.
             Option to process hunks of executable without crunching.
             Support empty hunks (padded to 4 bytes).
             Print minimum safety margin for overlapped data decrunching.

2015-01-18:  Version 4.4. Optimizations galore:
             New match finder based on a suffix array.
             New reference edge map based on a cuckoo hash table.
             Pre-compute number encoding sizes for faster estimation.
             Recycle references edges to save alloc/dealloc overhead.
             Updated defaults to take advantage of speed increase.
             Data file compression mode with decompression source.
             Fixed broken progress output for big files.
             Do not crash if text file could not be opened.

2015-01-05:  Version 4.3. Minor fixes:
             Usage information adjusted to fit within 77 columns.
             References discarded metric computed properly.
             First progress step is at 0.1% rather than 1.0%.
             Option to omit progress output (for non-ANSI consoles).
             Source changes for easier compilation with MSVC.

2014-12-16:  Version 4.2. For memory-efficient decrunching:
             Option to overlap compressed and decompressed data.
             Print memory overhead during and after decrunching.
             Verifier accepts partially filled hunks.

2014-02-08:  Version 4.1. Bug fixes and new features:
             Fixed some bugs in the range coder.
             Fixed handling of very small first hunk.
             Added internal verifier to check correctness of output.
             Print helpful text when encountering an internal error.
             Better error message when running out of memory.
             Set output file to be executable.
             New options to print text from an argument or file.
             New option to flash a hardware register during decrunching.

2014-01-05:  Version 4.0. First public release with new name.

1999 - 2012: Various public and internal versions.


Source code available from https://github.com/askeksa/Shrinkler


For questions and comments, visit the ADA forum at

http://ada.untergrund.net/?p=boardthread&id=264

or the Pouet forum at

http://www.pouet.net/prod.php?which=64851

or write to blueberry at loonies dot dk.
