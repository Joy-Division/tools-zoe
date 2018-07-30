/*
	written by Missingno_force a.k.a. Missingmew
	Copyright (c) 2014-2018
	see LICENSE for details
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>

#define BLOCKSIZE 0x800

#ifdef _WIN32
#include <direct.h>
#define createDirectory(dirname) mkdir(dirname)
#else
#include <sys/stat.h>
#include <sys/types.h>
#define createDirectory(dirname) mkdir(dirname, 0777)
#endif

typedef struct {
	uint32_t offsetBgmWvx;
	uint32_t unknown1;
	uint32_t offsetSfxWvx;
	uint32_t unknown2;
	uint32_t offsetSequenceEfx;
	uint32_t unknown3;
	uint32_t offsetSequenceMdx;
	uint32_t unknown4;
}__attribute__((packed)) sdxHeader;

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
	
	sdxHeader header;
	unsigned int fileSize = 0, bgmSize = 0, sfxSize = 0, seqSize1 = 0, seqSize2 = 0;
	
	if( argc < 2 ) {
		printf("Not enough args!\nUse: %s SDX-file\n", argv[0]);
		return 1;
	}
	
	if( strlen( argv[1] ) > 511 ) {
		printf("Input filename too long, please rename it to be less then 511 chars!\n");
		return 1;
	}
	
	FILE *f, *o;
	
	if( !(f = fopen( argv[1], "rb" ))) {
		printf("Couldnt open file %s\n", argv[1]);
		return 1;
	}
	
	fseek( f, 0, SEEK_END );
	fileSize = ftell(f);
	fseek( f, 0, SEEK_SET );
	
	char filename[512] = {0};
	
	fread( &header, sizeof(header), 1, f );
	
	if( header.offsetSequenceMdx ) seqSize2 = fileSize - header.offsetSequenceMdx*BLOCKSIZE;
	if( header.offsetSequenceEfx ) seqSize1 = fileSize - seqSize2 - header.offsetSequenceEfx*BLOCKSIZE;
	if( header.offsetSfxWvx ) sfxSize  = fileSize - seqSize2 - seqSize1 - header.offsetSfxWvx*BLOCKSIZE;
	if( header.offsetBgmWvx ) bgmSize  = fileSize - seqSize2 - seqSize1 - sfxSize - header.offsetBgmWvx*BLOCKSIZE;
	printf("Music samples offset: %08x - Size %08x\nEffect samples offset: %08x - Size %08x\nSequence data 1 offset: %08x - Size %08x\nSequence data 2 offset: %08x - Size %08x\n", header.offsetBgmWvx*BLOCKSIZE, bgmSize, header.offsetSfxWvx*BLOCKSIZE, sfxSize, header.offsetSequenceEfx*BLOCKSIZE, seqSize1, header.offsetSequenceMdx*BLOCKSIZE, seqSize2 );
	
	if( bgmSize ) {
		fseek( f, (header.offsetBgmWvx * BLOCKSIZE), SEEK_SET );
		sprintf( filename, "%s-BGMsampleBlock.wvx", argv[1] );
		if( !(o = fopen( filename, "wb" ))) {
			printf("Couldnt open file %s\n", filename);
			return 1;
		}
		writeFile( f, bgmSize, o );
		fclose(o);
	}
	
	if( sfxSize ) {
		fseek( f, header.offsetSfxWvx*BLOCKSIZE, SEEK_SET );
		sprintf( filename, "%s-SFXsampleBlock.wvx", argv[1] );
		if( !(o = fopen( filename, "wb" ))) {
			printf("Couldnt open file %s\n", filename);
			return 1;
		}
		writeFile( f, sfxSize, o );
		fclose(o);
	}
	
	if( seqSize1 ) {
		fseek( f, header.offsetSequenceEfx*BLOCKSIZE, SEEK_SET );
		sprintf( filename, "%s-SequenceDataBlock1.efx", argv[1] );
		if( !(o = fopen( filename, "wb" ))) {
			printf("Couldnt open file %s\n", filename);
			return 1;
		}
		writeFile( f, seqSize1, o );
		fclose(o);
	}
	
	if( seqSize2 ) {
		fseek( f, header.offsetSequenceMdx*BLOCKSIZE, SEEK_SET );
		sprintf( filename, "%s-SequenceDataBlock2.mdx", argv[1] );
		if( !(o = fopen( filename, "wb" ))) {
			printf("Couldnt open file %s\n", filename);
			return 1;
		}
		writeFile( f, seqSize2, o );
		fclose(o);
	}
	
	fclose(f);
	printf("Done.\n");
	return 0;
	
};