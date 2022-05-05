##
##	Makefile for "Vivid Transparency"
##	( ZONE OF THE ENDERS Toolbox )
##

###############################################################################

OLEVEL                  ?= 2
GLEVEL                  ?=

CFLAGS                  += -g$(GLEVEL)
CFLAGS                  += -O$(OLEVEL)
CFLAGS                  += -Wall -Wno-comment

CXXFLAGS                := $(CFLAGS)

LDFLAGS                 += -Wl,-Map,$(basename $@).map

###############################################################################
.PHONY: default all

default: all

TARGETS                 :=\
                        dat-extract      \
                        pak-extract      \
                        pss-demux        \
                        subtitle-convert \
                        tex-to_image     \
                        sdx-extract      \
                        wvx-extract      \
                        mdx-splitter     \
                        efx-splitter     \
                        mdx-parser

all: $(TARGETS)

###############################################################################

LODEPNG.O               := lodepng/lodepng.o
KOJIMASOUND.O           := kojimasound/kojimasound.o

%.o: %.c
	$(CC) $(CFLAGS) -o $@ -c $<

#------------------------------------------------------------------------------
# ARCHIVE
#------------------------------------------------------------------------------

dat-extract: dat-extract.c
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

pak-extract: pak-extract.c
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

#------------------------------------------------------------------------------
# STREAM
#------------------------------------------------------------------------------

pss-demux: pss-demux.c
	$(CC) $(CFLAGS) -o $@ $<

subtitle-convert: subtitle-convert.c
ifeq ($(OS),Windows_NT)
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS) -Wl,-Bstatic -liconv
else
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)
endif

#------------------------------------------------------------------------------
# TEXTURE
#------------------------------------------------------------------------------

tex-to_image: $(LODEPNG.O) tex-to_image.c
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

#------------------------------------------------------------------------------
# SOUND
#------------------------------------------------------------------------------

sdx-extract: sdx-extract.c
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

wvx-extract: $(KOJIMASOUND.O) wvx-extract.c
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

mdx-splitter: $(KOJIMASOUND.O) mdx-splitter.c
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

efx-splitter: $(KOJIMASOUND.O) efx-splitter.c
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

mdx-parser: $(KOJIMASOUND.O) mdx-parser.c
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

###############################################################################
.PHONY: clean

clean:
	$(RM) *.o
	$(RM) *.map
	$(RM) $(LODEPNG.O)
	$(RM) $(KOJIMASOUND.O)
	$(RM) $(TARGETS)
