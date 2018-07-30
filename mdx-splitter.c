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
	
	if ( argc < 3 ) {
		printf("Not enough args!\nUse: %s MDX-file game [skip empty, see below]\n", argv[0]);
		printf("supported games are:\n%d (MGS1)\n%d (ZoE1 and MGS2HD)\n", mgs1, zoe1);
		printf("To skip empty sequences (which only contain the ending signature), pass 1 after the game\n");
		return 1;
	}
	
	FILE *f, *o;
	char outputname[512] = {0};
	unsigned int game, skip, subsize, numsubs;
	unsigned int curtbl, cursub;
	uint32_t numtables, tableoffset, *subtable;
	uint32_t curtoken;
	game = strtoul(argv[2], NULL, 10);
	if(!(game<NUMWVXGAMES)) {
		printf("Unsupported game %d!\nRun without args to see supported types.\n", game);
		return 1;
	}
	
	if( argc == 4 ) {
		skip = strtoul(argv[3], NULL, 10);
	}
	
	switch(game) {
		case mgs1: {
			subsize = 0x60;
			subtable = malloc(0x60);
			break;
		}
		case zoe1: {
			subsize = 0x80;
			subtable = malloc(0x80);
			break;
		}
	}
	numsubs = subsize / 4;
	
	if( !(f = fopen( argv[1], "rb" ))) {
		printf("Couldnt open file %s\n", argv[1]);
		return 1;
	}
	
	fseek(f, 0, SEEK_SET);
	fread(&numtables, 4, 1, f);
	printf("Number of tables in MDX: %d\n", numtables);
	
	for(curtbl = 0;curtbl < numtables;curtbl++) {
		fseek(f, 4+(4*curtbl), SEEK_SET);
		fread(&tableoffset, 4, 1, f);
		printf("Table %d - Offset: %08x\n", curtbl, tableoffset);
		fseek(f, tableoffset, SEEK_SET);
		fread(subtable, subsize, 1, f);
		for(cursub = 0;cursub < numsubs;cursub++) {
			sprintf(outputname, "tbl%05d-sub%05d.bin", curtbl, cursub);
			curtoken = 0;
			if( !(o = fopen( outputname, "wb" ))) {
				printf("Couldnt open file %s\n", outputname);
				return 1;
			}
			printf("Table %d - Subsequence %02d - Offset: %08x\n", curtbl, cursub, subtable[cursub]);
			fseek(f, subtable[cursub], SEEK_SET);
			while(curtoken != 0xFFFE0000) {
				fread(&curtoken, 4, 1, f);
				fwrite(&curtoken, 4, 1, o);
			}
			if((ftell(o) == 4) && skip) {
				printf("Empty sequence! Skipping...\n");
				fclose(o);
				remove(outputname);
			}
			else fclose(o);
		}
	}
	
	printf("Done.\n");
	
	fclose(f);
	return 0;
}
