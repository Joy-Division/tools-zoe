# VividTransparency
Factory

Tools for working with files from Zone of the Enders

---------

# Building and debugging

Building should only require a reasonably recent c compiler (e.g. GCC) and make, any required libraries are currently bundled and may be exchanged for submodules in the future (if possible).

The following libraries are currently bundled with the project and will be exchanged for git submodules in the future:
- [lodepng](https://github.com/lvandeve/lodepng)

As long as the code is still included in this repository, see the individual libraries folder for licensing of it

For building on windows, slight changes to the Makefile may be required (like changing the extensions from .elf to .exe), the code of all tools should already be compatible

Useful things for debugging are:
- gdb
- valgrind

Additionally for comparison with actual game output:
- recordings of gameplay 
- working emulator setupto play the game on your system

**Note:** Code quality and style may vary wildly between different code files. Feel free to propose a style to be used (but do not count on it).
Minimal error checking is in place, but assume that any unexpected input will result in your system eating your cat.

# Overview of included tools

## dat-extract
Unpacks the ZOE.DAT file found in the root of the disc to allow easy access to its individual files (for the TGS2000 demo, see dat-extract_tgs).

## dat-extract_tgs
Unpacks the ZOE.DAT file found in the TGS2000 demo of Zone of the Enders, lightly modified from dat-extract to account for different offsets found in the file (each offset is increased by 0x18 sectors).

## dat-patch_tgs
Modifies the offsets of the TGS2000 demo to remove the 0x18 increment (**Warning:** this actively modifies the passed file, ensure you have backups!).

## efx-splitter
Dumps the sections found in efx files produced by sdx-extract and splits up the sequence block into individual sequences.

## mdx-parser
Walks through a mdx file produced by sdx-extract and prints the encountered events. Does not produce any output besides this.

## mdx-splitter
Dumps the individual tracks found in mdx files produced by sdx-extract.

## pak-extract
Unpacks various pak files found inside the unpacked ZOE.DAT.

## pss-demux
Demuxes the pss files found inside the unpacked ZOE.DAT, outputs m2v video, vag audio, dmx polygon demo data and subtitles if found (see subtitle-convert to produce readable output).

## sdx-extract
Dumps the various kinds of data found in sdx files found in the unpacked ZOE.DAT, outputs wvx wave archives, efx and mdx sequences.

## subtitle-convert
Converts the subtitles from pss-demux into ready-to-use srt files. Japanese subtitles are converted to UTF-8 (from EUC-JP) while any other language uses Windows CP-1252 (as this code was originally written on a Windows based system).

## tex-to_image
Converts the tex files found inside the unpacked ZOE.DAT into either PNG or raw RGBA images (see below for limitations).

## wvx-extract
Splits the wvx files produced by sdx-extract into ready to play vag files. Note that some files will throw errors or lock up players due to only containing the EOF marker.

# Current limitations

## mdx-parser
All events are currently interpreted in the same way as MGS1, resulting in possible inaccuracies and leaving a lot of unknown events

## tex-to_image
The tex files contain bitmaps which can contain multiple subtextures, each with their own palette. Currently, for each subtexture the containing bitmap is output completely while applying the subtextures palette.

# License
Unless noted otherwise in a source file, see [LICENSE](LICENSE) for details. For bundled libraries, see above.
