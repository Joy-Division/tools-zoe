/*
	written by Missingno_force a.k.a. Missingmew
	Copyright (c) 2014-2018
	see LICENSE for details
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "kojimasound/kojimasound.h"

int main( int argc, char **argv ) {
	
	if ( argc < 2 ) {
		printf("Not enough args!\nUse: %s EFX-file\n", argv[0]);
		printf("supported games are:\nMGS1\nZoE1 and MGS2HD\n");
		printf("The game will be automatically detected based on filesize!\n");
		return 1;
	}
	
	FILE *f, *o;
	char outputname[512] = {0};
	unsigned char *dumpblock = NULL;
	unsigned int numentries, sizelimit = 0x2000, size, game, blocksize, sequenceblockoffset;
	unsigned int i, j;
	efxFlags minihead;
	
	uint32_t sequenceoffset;
	uint32_t curtoken;
	
	if( !(f = fopen( argv[1], "rb" ))) {
		printf("Couldnt open file %s\n", argv[1]);
		return 1;
	}
	
	fseek(f, 0, SEEK_END);
	size = ftell(f);
	fseek(f, 0, SEEK_SET);
	if(size < sizelimit) {
		game = mgs1;
		numentries = 0x80;
		sequenceblockoffset = 0;
		printf("Game based on filesize is MGS1\n");
	}
	else {
		game = zoe1;
		numentries = 0x100;
		sequenceblockoffset = 0x800;
		printf("Game based on filesize is ZoE1 or MGS2HD\n");
	}
	
	if(game == zoe1) {
		printf("Dumping block 0 of efx (found in games after MGS1)\n");
		sprintf(outputname, "block0-unknown.bin");
		if( !(o = fopen( outputname, "wb" ))) {
			printf("Couldnt open file %s\n", outputname);
			return 1;
		}
		dumpblock = malloc(0x800);
		fread(dumpblock, 0x800, 1, f);
		fwrite(dumpblock, 0x800, 1, o);
		fclose(o);
		free(dumpblock);
	}
	
	printf("Dumping block 1 of efx (offset block)\n");
	sprintf(outputname, "block1-offsets.bin");
	if( !(o = fopen( outputname, "wb" ))) {
		printf("Couldnt open file %s\n", outputname);
		return 1;
	}
	dumpblock = malloc(0x800);
	fread(dumpblock, 0x800, 1, f);
	fwrite(dumpblock, 0x800, 1, o);
	fclose(o);
	free(dumpblock);
	
	printf("Dumping block 2 of efx (sequence block)\n");
	sprintf(outputname, "block2-sequences.bin");
	if( !(o = fopen( outputname, "wb" ))) {
		printf("Couldnt open file %s\n", outputname);
		return 1;
	}
	if(game == mgs1) blocksize = size-0x800;
	else if(game == zoe1) blocksize = size-0x1000;
	dumpblock = malloc(blocksize);
	fread(dumpblock, blocksize, 1, f);
	fwrite(dumpblock, blocksize, 1, o);
	fclose(o);
	free(dumpblock);
	
	
	for(i = 0; i < numentries; i++) {
		fseek(f, sequenceblockoffset + (0x10*i), SEEK_SET);
		fread(&minihead, 4, 1, f);
		printf("Minihead: %02x %02x %02x %02x\n", minihead.unknown1, minihead.numSequences, minihead.unknown2, minihead.unknown3);
		printf("Sequences in entry %02x: %d\n", i, minihead.numSequences);
		if(minihead.numSequences) {
			for(j = 0; j < minihead.numSequences; j++) {
				curtoken = 0;
				fseek(f, sequenceblockoffset + (0x10*i) + 4 + (4*j), SEEK_SET);
				fread(&sequenceoffset, 4, 1, f);
				if(sequenceoffset == 0xFFFFFFFF) {
					printf("Invalid sequence offset!\nEntry %02x - Sequence %d\n", i, j);
					return 1;
				}
				printf("Entry %02x - Sequence %d - Offset %08x\n", i, j, sequenceoffset);
				fseek(f, sequenceblockoffset+(numentries*0x10)+sequenceoffset, SEEK_SET);
				sprintf(outputname, "ent%03d-seq%d.bin", i, j);
				if( !(o = fopen( outputname, "wb" ))) {
					printf("Couldnt open file %s\n", outputname);
					return 1;
				}
				while(curtoken != 0xFFFE0000) {
					fread(&curtoken, 4, 1, f);
					fwrite(&curtoken, 4, 1, o);
				}
				fclose(o);
			}
		}
	}
	
	printf("Done.\n");
	
	fclose(f);
	return 0;
}
