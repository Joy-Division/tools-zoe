/*
	written by Missingno_force a.k.a. Missingmew
	Copyright (c) 2014-2018
	see LICENSE for details
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>

#define FILENAMELENGTH 32
#define DIRNAMELENGTH 8

typedef struct {
	char name[FILENAMELENGTH];
	uint32_t unknown1;
	uint32_t unknown2;
	uint32_t unknown3;
	uint32_t size;
}__attribute__((packed)) fileEntry;

int writeFile( FILE *input, int length, FILE *output ) {
	unsigned char dataBuffer[1024];
	unsigned int bytesLeft = length;
	
	while(bytesLeft) {
		unsigned int wantedRead;
		if(bytesLeft >= sizeof(dataBuffer))
			wantedRead = sizeof(dataBuffer);
		else
			wantedRead = bytesLeft;
		unsigned int haveRead = fread(dataBuffer, 1, wantedRead, input);
		if(haveRead != wantedRead) {
			printf("haveRead != wantedRead: %d != %d\n", haveRead, wantedRead);
			perror("This broke");
			return 0;
		}

		unsigned int haveWrite = fwrite(dataBuffer, 1, haveRead, output);
		if(haveWrite != haveRead) {
			printf("haveWrite != haveRead: %d != %d\n", haveWrite, haveRead);
			return 0;
		}
		
		bytesLeft -= haveRead;
	}
	return 1;
}

int main( int argc, char **argv ) {
	
	fileEntry workfile;
	
	unsigned char temp[4];
	
	memset( workfile.name, 0, FILENAMELENGTH );
	
	if( argc < 2 ) {
		printf("Not enough args!\nUse: %s PAK-file\n", argv[0]);
		return 1;
	}
	
	unsigned int numFiles, workfiles;
	FILE *f, *o;
	
	if( !(f = fopen( argv[1], "rb" ))) {
		printf("Couldnt open file %s\n", argv[1]);
		return 1;
	}
	
	fread( temp, 4, 1, f );
	numFiles  = temp[3] << 24;
	numFiles |= temp[2] << 16;
	numFiles |= temp[1] << 8;
	numFiles |= temp[0];
	
	fseek( f, 16, SEEK_SET );
	
	printf("Number of files: %u\n", numFiles );
	
	for( workfiles = 0; workfiles < numFiles; workfiles++ ) {
		fread( &workfile, sizeof(workfile), 1, f );
		printf("Current file: %s\n", workfile.name);
		
		if( !(o = fopen( workfile.name, "wb" ))) {
			printf("Couldnt open output %s\n", workfile.name);
			return 1;
		}
		
		writeFile( f, workfile.size, o );
		fclose(o);
		memset( workfile.name, 0, FILENAMELENGTH );
	}
	printf("Done.\n");
	fclose(f);
	return 0;
}
