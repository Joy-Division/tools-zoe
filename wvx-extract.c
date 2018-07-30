/*
	written by Missingno_force a.k.a. Missingmew
	Copyright (c) 2014-2018
	see LICENSE for details
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "kojimasound/kojimasound.h"

#define BLOCKSIZE 0x800

#ifdef _WIN32
#include <direct.h>
#define createDirectory(dirname) mkdir(dirname)
#else
#include <sys/stat.h>
#include <sys/types.h>
#define createDirectory(dirname) mkdir(dirname, 0777)
#endif

int main( int argc, char **argv ) {
	
	char dirname[512] = {0};
	unsigned char temp[4];
	unsigned int numSamples, tableSize, game;
	FILE *f;
	
	if( argc < 3 ) {
		printf("Not enough args!\nUse: %s WVX-file game\n", argv[0]);
		printf("supported games are:\n%d (MGS1)\n%d (ZoE1 and MGS2HD)\n", mgs1, zoe1);
		return 1;
	}
	
	game = strtoul(argv[2], NULL, 10);
	if(!(game<NUMWVXGAMES)) {
		printf("Unsupported game %d!\nRun without args to see supported types.\n", game);
		return 1;
	}
	
	if( !(f = fopen( argv[1], "rb" ))) {
		printf("Couldnt open file %s\n", argv[1]);
		return 1;
	}
	
	sprintf( dirname, "%s-Samples", argv[1] );
	createDirectory(dirname);
	fseek( f, 4, SEEK_SET );
	fread( temp, 4, 1, f );
	tableSize = temp[0] << 24 | temp[1] << 16 | temp[2] << 8 | temp[3];
	numSamples = tableSize/0x10;
	printf( "%d samples in sample table\n", numSamples );
	
	
	processWvx(f, 0, dirname, numSamples, game);
	
	printf("Done.\n");
	fclose(f);
	return 0;
}
