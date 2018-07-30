/*
	written by Missingno_force a.k.a. Missingmew
	Copyright (c) 2014-2018
	see LICENSE for details
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>

#define FILENAMELENGTH 16
#define DIRNAMELENGTH 8
#define BLOCKSIZE 0x800

#ifdef _WIN32
#include <direct.h>
#define createDirectory(dirname) mkdir(dirname)
#else
#include <sys/stat.h>
#include <sys/types.h>
#define createDirectory(dirname) mkdir(dirname, 0777)
#endif

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

typedef struct {
	char name[DIRNAMELENGTH];
	uint32_t subDirCount;
	uint32_t subDirOffset;
	uint32_t subFileCount;
	uint32_t subFileOffset;
	uint32_t unknown2;
	uint32_t unknown3;
}__attribute__((packed)) tocDirectoryEntry;

typedef struct {
	char name[FILENAMELENGTH];
	uint32_t fileOffset;
	uint32_t fileSize;
	uint32_t unknown1;
	uint32_t unknown2;
}__attribute__((packed)) tocFileEntry;


char directoryState[4][16];
char completeName[48] = { 0 };
int dirlevel = 0;
char rootchar[4] = "ZOE";

void processDirectoryListing( FILE *f, tocDirectoryEntry entry ) {
	memset( completeName, 0, 48 );
	tocFileEntry file;
	int i;
	FILE *o;
	memcpy( directoryState[dirlevel], entry.name, DIRNAMELENGTH );
	sprintf( completeName, "%s/%s/%s/%s", directoryState[0], directoryState[1], directoryState[2], directoryState[3] );
	printf("Current directory: %s\n", completeName );
	createDirectory( completeName );
	for( i = 0; i < entry.subFileCount; i++ ) {
		fseek( f, (entry.subFileOffset*BLOCKSIZE)+(32*i), SEEK_SET );
		fread( &file, sizeof(file), 1, f );
		file.fileOffset -= 0x18;
		memcpy( directoryState[3], file.name, FILENAMELENGTH );
		sprintf( completeName, "%s/%s/%s/%s", directoryState[0], directoryState[1], directoryState[2], directoryState[3] );
		printf("Current file: %s\n", completeName );
		if( !(o = fopen( completeName, "wb" ))) {
			printf("Couldnt open file %s\n", completeName);
			return;
		}
		fseek( f, file.fileOffset*BLOCKSIZE, SEEK_SET );
		writeFile( f, file.fileSize, o );
		fclose(o);
	}
	memset( directoryState[3], 0, 16 );
	return;
}
	
void processDirectory( FILE *f, tocDirectoryEntry entry ) {
	memset( completeName, 0, 48 );
	tocDirectoryEntry workentry;
	int i;
	
	if( dirlevel == 0 ) memcpy( directoryState[dirlevel], rootchar, 4 );
	else memcpy( directoryState[dirlevel], entry.name, DIRNAMELENGTH );
	sprintf( completeName, "%s/%s/%s/%s", directoryState[0], directoryState[1], directoryState[2], directoryState[3] );
	printf("Current directory: %s\n", completeName );
	createDirectory( completeName );
	
	for( i = 0; i < entry.subDirCount; i++ ) {
		fseek( f, entry.subDirOffset+(i*32), SEEK_SET );
		fread( &workentry, sizeof(workentry), 1, f );
		if(workentry.subFileOffset) workentry.subFileOffset -= 0x18;
		dirlevel++;
		if( workentry.subDirCount ) processDirectory( f, workentry );
		else processDirectoryListing( f, workentry );
		memset( directoryState[dirlevel], 0, 16 );
		dirlevel--;
	}
	return;
}

int main( int argc, char **argv ) {
	
	tocDirectoryEntry workdir;
	
	memset( workdir.name, 0, DIRNAMELENGTH );
	
	if ( argc < 2 ) {
		printf("Not enough args!\nUse: %s DAT-file\n", argv[0]);
		return 1;
	}
	
	FILE *f;
	
	if( !(f = fopen( argv[1], "rb" ))) {
		printf("Couldnt open file %s\n", argv[1]);
		return 1;
	}
	fseek( f, 0, SEEK_SET );
	
	printf("NOTE:\nAll offsets in the TGS2000 ZOE.DAT are increased by 0x18!\nTo get real offset, subtract 0x18 before multiplying by 0x800.");
	
	fread( &workdir, sizeof(workdir), 1, f );
	
	processDirectory( f, workdir );
	
	printf("Done.\n");
	
	fclose(f);
	return 0;
}
