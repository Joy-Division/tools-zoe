/*
	written by Missingno_force a.k.a. Missingmew
	Copyright (c) 2014-2018
	see LICENSE for details
*/

#include "kojimasound.h"

typedef struct {
	uint32_t offset;
	uint32_t flags;
	unsigned char unknown[8];
}__attribute__((packed)) sampleTableEntry;

typedef struct {
	char magic[4];
	unsigned char version[4];
	unsigned char reserved1[4];
	unsigned char size[4];
	char samplerate[4];
	unsigned char reserved2[12];
	char name[0x10];
}__attribute__((packed)) vagHeader;

unsigned char vagEnd[0x10] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

void vagPrepareHeader( vagHeader *header, uint32_t size, uint32_t samplerate ) {
	header->version[3] = 0x20;
	memset( header->name, 0, 0x10 );
	header->size[0] = (size >> 24) & 0xFF; header->size[1] = (size >> 16) & 0xFF; header->size[2] = (size >> 8) & 0xFF; header->size[3] = size & 0xFF;
	header->samplerate[0] = 0; header->samplerate[1] = 0; header->samplerate[2] = (samplerate >> 8) & 0xFF; header->samplerate[3] = samplerate & 0xFF;
	memset( header->reserved1, 0, 4 );
	memset( header->reserved2, 0, 12 );
	strncpy( header->magic, "VAGp", 4 );
	return;
}

void processWvx( FILE *f, unsigned int baseoffset, char *folder, unsigned int numSamples, unsigned int wvxgame ) {
	FILE *o;
	unsigned int i, offsub, samplerate, vagsize, psx = 0;
	char filename[1024];
	unsigned char vagbuf[0x10];
	sampleTableEntry sampleentry;
	vagHeader vagheader;
	
	switch(wvxgame) {
		case mgs1: {
			psx = 1;
			break;
		}
		case zoe1:
		default: {
			break;
		}
	}
	for( i = 0; i < numSamples; i++ ) {
		fseek(f, baseoffset+0x10+(i*0x10), SEEK_SET);
		
		sprintf(filename, "%s/%08d-%08x.vag", folder, i, i);
		if( !(o = fopen( filename, "wb" ))) {
				printf("Couldnt open file %s\n", filename);
				return;
		}
		
		memset(&sampleentry, 0, 0x10);
		fread(&sampleentry, 0x10, 1, f);
		printf("Sample offset: %08x - Flags %08x\n", sampleentry.offset, sampleentry.flags);
		if( i == 0 ) offsub = sampleentry.offset;
		sampleentry.offset -= offsub;
		if(psx) {
			if(sampleentry.flags > 0x7F000000) samplerate = 22050;
			else samplerate = 11025;
		}
		else {
			if(sampleentry.flags > 0x7F000000) samplerate = 44100;
			else samplerate = 32000;
		}
		fseek(f, baseoffset+0x20+(numSamples*0x10)+sampleentry.offset, SEEK_SET);
		
		memset(&vagheader, 0, 0x30);
		fwrite(&vagheader, 0x30, 1, o);
		vagsize = 0x30;
		memset(vagbuf, 0xFF, 0x10);
		while(1) {
			if(!fread(vagbuf, 0x10, 1, f)) break;
			if(!memcmp(vagbuf, vagEnd, 0x10) && (vagsize!=0x30)) break;
			fwrite(vagbuf, 0x10, 1, o);
			vagsize+=0x10;
		}
		vagPrepareHeader(&vagheader, vagsize, samplerate);
		fseek(o, 0, SEEK_SET);
		fwrite(&vagheader, 0x30, 1, o);
		fclose(o);
	}
	return;
}
