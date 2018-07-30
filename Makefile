LODEPNG = lodepng/lodepng.o
KOJIMASOUND = kojimasound/kojimasound.o


all: archive video texture sound test
archive: dat-extract.elf dat-extract_tgs.elf pak-extract.elf
video: pss-demux.elf subtitle-convert.elf
texture: tex-to_image.elf
sound: sdx-extract.elf wvx-extract.elf mdx-splitter.elf efx-splitter.elf mdx-parser.elf
test: dat-patch_tgs.elf

tex-to_image.elf: $(LODEPNG) tex-to_image.c
	$(CC) -Wall -g -o $@ $^
	
wvx-extract.elf: $(KOJIMASOUND) wvx-extract.c
	$(CC) -Wall -g -o $@ $^
	
mdx-splitter.elf: $(KOJIMASOUND) mdx-splitter.c
	$(CC) -Wall -g -o $@ $^
	
efx-splitter.elf: $(KOJIMASOUND) efx-splitter.c
	$(CC) -Wall -g -o $@ $^
	
mdx-parser.elf: $(KOJIMASOUND) mdx-parser.c
	$(CC) -Wall -g -o $@ $^
	
%.elf: %.c
	$(CC) -Wall -g -o $@ $<
	
%.o: %.c
	$(CC) -c -Wall -g -o $@ $<
	
clean:
	-rm lodepng/lodepng.o kojimasound/kojimasound.o *.elf
