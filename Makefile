#
# Makefile for "Vivid Transparency"
# ZONE OF THE ENDERS Toolbox
#

GCC_WIN32 = i686-w64-mingw32-gcc
GXX_WIN32 = i686-w64-mingw32-g++
GCC_WIN64 = x86_64-w64-mingw32-gcc
GXX_WIN64 = x86_64-w64-mingw32-g++
GCC_ARGS  = -Wall -g

#---------------------------------------------------------------------------#

all: \
	archive \
	video \
	texture \
	sound

archive: \
	dat-extract \
	pak-extract

video: \
	pss-demux \
	subtitle-convert

texture: \
	tex-to_image

sound: \
	sdx-extract \
	wvx-extract \
	mdx-splitter \
	efx-splitter \
	mdx-parser

#---------------------------------------------------------------------------#
# Objects
#---------------------------------------------------------------------------#

LODEPNG     = lodepng/lodepng.o
KOJIMASOUND = kojimasound/kojimasound.o

%.o: %.c
	$(CC) $(GCC_ARGS) -o $@ -c $<

#---------------------------------------------------------------------------#
# Targets
#---------------------------------------------------------------------------#

ifeq ($(OS),Windows_NT)
ICONV_STATIC = -Wl,-Bstatic -liconv
else
ICONV_STATIC =
endif

# --- archive ---
dat-extract: dat-extract.c
	$(CC) $(GCC_ARGS) -o $@ $<

pak-extract: pak-extract.c
	$(CC) $(GCC_ARGS) -o $@ $<

# --- video ---
pss-demux: pss-demux.c
	$(CC) $(GCC_ARGS) -o $@ $<

subtitle-convert: subtitle-convert.c
	$(CC) $(GCC_ARGS) -o $@ $< $(ICONV_STATIC)

# --- texture ---
tex-to_image: $(LODEPNG) tex-to_image.c
	$(CC) $(GCC_ARGS) -o $@ $^

# --- sound	 ---
sdx-extract: sdx-extract.c
	$(CC) $(GCC_ARGS) -o $@ $<

wvx-extract: $(KOJIMASOUND) wvx-extract.c
	$(CC) $(GCC_ARGS) -o $@ $^
	
mdx-splitter: $(KOJIMASOUND) mdx-splitter.c
	$(CC) $(GCC_ARGS) -o $@ $^
	
efx-splitter: $(KOJIMASOUND) efx-splitter.c
	$(CC) $(GCC_ARGS) -o $@ $^
	
mdx-parser: $(KOJIMASOUND) mdx-parser.c
	$(CC) $(GCC_ARGS) -o $@ $^

#---------------------------------------------------------------------------#

clean: \
	clean_obj \
	clean_exe

clean_obj:
	-rm *.o
	-rm $(LODEPNG)
	-rm $(KOJIMASOUND)

clean_exe:
	-rm *.exe
	-rm dat-extract
	-rm pak-extract
	-rm pss-demux
	-rm subtitle-convert
	-rm tex-to_image
	-rm sdx-extract
	-rm wvx-extract
	-rm mdx-splitter
	-rm efx-splitter
	-rm mdx-parser
