/*
	written by Missingno_force a.k.a. Missingmew
	Copyright (c) 2014-2018
	see LICENSE for details
*/

#include <stdint.h>
#include <string.h>
#include <stdio.h>

enum wvxType {
	mgs1bgm,
	mgs1sfx,
	zoe1bgm,
	zoe1sfx,
	NUMWVXTYPES
};

enum wvxGame {
	mgs1,
	zoe1,
	NUMWVXGAMES
};

typedef struct {
	uint8_t unknown1;
	uint8_t numSequences;
	uint8_t unknown2;
	uint8_t unknown3;
}__attribute__((packed)) efxFlags;

void processWvx( FILE *f, unsigned int baseoffset, char *folder, unsigned int numSamples, unsigned int wvxgame );