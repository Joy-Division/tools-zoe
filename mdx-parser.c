/*
	written by Missingno_force a.k.a. Missingmew
	Copyright (c) 2014-2018
	see LICENSE for details
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
//~ #include "itreference/itstuff.h"
#include "kojimasound/kojimasound.h"
//~ #include <smf.h>

char *midinotes[128] = {
	"C0", "C#0", "D0", "D#0", "E0", "F0", "F#0", "G0", "G#0", "A0", "A#0", "B0",
	"C1", "C#1", "D1", "D#1", "E1", "F1", "F#1", "G1", "G#1", "A1", "A#1", "B1",
	"C2", "C#2", "D2", "D#2", "E2", "F2", "F#2", "G2", "G#2", "A2", "A#2", "B2",
	"C3", "C#3", "D3", "D#3", "E3", "F3", "F#3", "G3", "G#3", "A3", "A#3", "B3",
	"C4", "C#4", "D4", "D#4", "E4", "F4", "F#4", "G4", "G#4", "A4", "A#4", "B4",
	"C5", "C#5", "D5", "D#5", "E5", "F5", "F#5", "G5", "G#5", "A5", "A#5", "B5",
	"C6", "C#6", "D6", "D#6", "E6", "F6", "F#6", "G6", "G#6", "A6", "A#6", "B6",
	"C7", "C#7", "D7", "D#7", "E7", "F7", "F#7", "G7", "G#7", "A7", "A#7", "B7",
	"C8", "C#8", "D8", "D#8", "E8", "F8", "F#8", "G8", "G#8", "A8", "A#8", "B8",
	"C9", "C#9", "D9", "D#9", "E9", "F9", "F#9", "G9", "G#9", "A9", "A#9", "B9",
	"C10", "C#10", "D10", "D#10", "E10", "F10", "F#10", "G10"
};

unsigned int snap0(unsigned int data) {
	if(data < 26) return 5*data+0x80;
	else if(data < 52) return 5*data+0x8100;
	else if(data < 76) return 5*data+0x8180;
	else if(data < 102) return 5*data+0x8200;
	else if(data < 127) return 5*data+0x8280;
	else if(data < 153) return 5*data+0x8300;
	else if(data < 179) return 5*data+0x8380;
	else if(data < 205) return 5*data+0x8400;
	else if(data < 283) return 5*data+0x8480;
	else {
		printf("snap0: data is %d\n", data);
		return 0;
	}
}	

int main( int argc, char **argv ) {
	
	if ( argc < 3 ) {
		printf("Not enough args!\nUse: %s MDX-file game\n", argv[0]);
		printf("supported games are:\n%d (MGS1)\n%d (ZoE1 and MGS2HD)\n", mgs1, zoe1);
		return 1;
	}
	
	
	FILE *f, *o;
	char outputname[512] = {0};
	unsigned int game, tracklistsize, numtracks;
	unsigned int cursong, curtrack;
	unsigned int event, param0, param1, param2, note, velocity, length, snap;
	uint32_t numsongs, songoffset, *tracklist;
	uint32_t curtoken;
	game = strtoul(argv[2], NULL, 10);
	if(!(game<NUMWVXGAMES)) {
		printf("Unsupported game %d!\nRun without args to see supported types.\n", game);
		return 1;
	}
	
	
	switch(game) {
		case mgs1: {
			tracklistsize = 0x60;
			tracklist = malloc(0x60);
			break;
		}
		case zoe1: {
			tracklistsize = 0x80;
			tracklist = malloc(0x80);
			break;
		}
	}
	numtracks = tracklistsize / 4;
	
	if( !(f = fopen( argv[1], "rb" ))) {
		printf("Couldnt open file %s\n", argv[1]);
		return 1;
	}
	
	fseek(f, 0, SEEK_SET);
	fread(&numsongs, 4, 1, f);
	printf("Number of songs in MDX: %d\n", numsongs);
	
	for(cursong = 0;cursong < numsongs;cursong++) {
		fseek(f, 4+(4*cursong), SEEK_SET);
		fread(&songoffset, 4, 1, f);
		printf("Song %d - Offset: %08x\n", cursong, songoffset);
		fseek(f, songoffset, SEEK_SET);
		fread(tracklist, tracklistsize, 1, f);
		for(curtrack = 0;curtrack < numtracks;curtrack++) {
			sprintf(outputname, "song%05d-track%05d.bin", cursong, curtrack);
			curtoken = 0;
			if( !(o = fopen( outputname, "wb" ))) {
				printf("Couldnt open file %s\n", outputname);
				return 1;
			}
			printf("Song %d - Track %02d - Offset: %08x\n", cursong, curtrack, tracklist[curtrack]);
			fseek(f, tracklist[curtrack], SEEK_SET);
			
			/* smf stuffs */
			//~ track = smf_track_new();
			
			while(curtoken != 0xFFFE0000) {
				printf("\t");
				fread(&curtoken, 4, 1, f);
				
				event = curtoken >> 24;
				param0 = (curtoken >> 16) & 0xFF;
				param1 = (curtoken >> 8) & 0xFF;
				param2 = curtoken & 0xFF;
				
				if(event < 0x80 ) {
					/* possible midi note */
					note = event;
					snap = param0;
					length = param1;
					velocity = param2;
					printf("Encountered note %x(%s)\n", note, midinotes[note]);
					printf("\tsnap %x - length %x - velocity %x\n", snap, length, velocity);
				}
				else {
					switch(event) {
						case 0xD0: {
							printf("Encountered Tempo event at %08x, parameters %02x %02x %02x\n", (unsigned int)ftell(f)-4, param0, param1, param2);
							break;
						}
						
						case 0xD1: {
							printf("Encountered Decrease event at %08x, parameters %02x %02x %02x\n", (unsigned int)ftell(f)-4, param0, param1, param2);
							break;
						}
						
						case 0xD2: {
							printf("Encountered Instrument A event at %08x, parameters %02x %02x %02x\n", (unsigned int)ftell(f)-4, param0, param1, param2);
							break;
						}
						
						case 0xD3: {
							printf("Encountered Instrument B event at %08x, parameters %02x %02x %02x\n", (unsigned int)ftell(f)-4, param0, param1, param2);
							break;
						}
						
						case 0xD4: {
							printf("Encountered Instrument C event at %08x, parameters %02x %02x %02x\n", (unsigned int)ftell(f)-4, param0, param1, param2);
							break;
						}
						
						case 0xD5: {
							printf("Encountered Change Master Volume event at %08x, parameters %02x %02x %02x\n", (unsigned int)ftell(f)-4, param0, param1, param2);
							break;
						}
						
						case 0xD6: {
							printf("Encountered Fade out event at %08x, parameters %02x %02x %02x\n", (unsigned int)ftell(f)-4, param0, param1, param2);
							break;
						}
						
						case 0xD7: {
							printf("Encountered Change Channel Volume event at %08x, parameters %02x %02x %02x\n", (unsigned int)ftell(f)-4, param0, param1, param2);
							break;
						}
						
						case 0xD8: {
							printf("Encountered Fade out note event at %08x, parameters %02x %02x %02x\n", (unsigned int)ftell(f)-4, param0, param1, param2);
							break;
						}
						
						case 0xDD: {
							printf("Encountered Pan event at %08x, complete event %08x\n", (unsigned int)ftell(f)-4, curtoken);
							break;
						}
						
						/*
						case 0xDE: {
							printf("Encountered Pan1 event at %08x, complete event %08x\n", (unsigned int)ftell(f)-4, curtoken);
							break;
						}
						*/
						
						case 0xDF: {
							printf("Encountered Pitch event at %08x, parameters %02x %02x %02x\n", (unsigned int)ftell(f)-4, param0, param1, param2);
							break;
						}
						
						case 0xE0: {
							printf("Encountered Pitch note when changed event at %08x, parameters %02x %02x %02x\n", (unsigned int)ftell(f)-4, param0, param1, param2);
							break;
						}
						
						case 0xE3: {
							printf("Encountered Modulation event at %08x, parameters %02x %02x %02x\n", (unsigned int)ftell(f)-4, param0, param1, param2);
							break;
						}
						
						case 0xE5: {
							printf("Encountered Swirl up note event at %08x, parameters %02x %02x %02x\n", (unsigned int)ftell(f)-4, param0, param1, param2);
							break;
						}
						
						case 0xE6: {
							printf("Encountered Pitch bend on change event at %08x, parameters %02x %02x %02x\n", (unsigned int)ftell(f)-4, param0, param1, param2);
							break;
						}
						
						case 0xE7: {
							printf("Encountered Set Loop Start (MEMORY A) event at %08x, complete event %08x\n", (unsigned int)ftell(f)-4, curtoken);
							break;
						}
						
						case 0xE8: {
							printf("Encountered Set Loop End (MEMORY A) (repeat PARAM0 times) event at %08x, parameters %02x %02x %02x\n", (unsigned int)ftell(f)-4, param0, param1, param2);
							break;
						}
						
						case 0xE9: {
							printf("Encountered Set Loop Start (MEMORY B) event at %08x, complete event %08x\n", (unsigned int)ftell(f)-4, curtoken);
							break;
						}
						
						case 0xEA: {
							printf("Encountered Set Loop End (MEMORY B) (repeat PARAM0 times) event at %08x, parameters %02x %02x %02x\n", (unsigned int)ftell(f)-4, param0, param1, param2);
							break;
						}
						
						case 0xEB: {
							printf("Encountered Set Loop Start event at %08x, complete event %08x\n", (unsigned int)ftell(f)-4, curtoken);
							break;
						}
						
						case 0xEC: {
							printf("Encountered Set Loop End event at %08x, complete event %08x\n", (unsigned int)ftell(f)-4, curtoken);
							break;
						}
						
						case 0xED: {
							printf("Encountered Set Loop Start (0xED) event at %08x, complete event %08x\n", (unsigned int)ftell(f)-4, curtoken);
							break;
						}
						
						case 0xEE: {
							printf("Encountered Set Loop End (0xEE) (two per 0xED) event at %08x, complete event %08x\n", (unsigned int)ftell(f)-4, curtoken);
							break;
						}
						
						case 0xF2: {
							printf("Encountered Silence A event at %08x, parameters %02x %02x %02x\n", (unsigned int)ftell(f)-4, param0, param1, param2);
							break;
						}
						
						case 0xF3: {
							printf("Encountered Silence B event at %08x, parameters %02x %02x %02x\n", (unsigned int)ftell(f)-4, param0, param1, param2);
							break;
						}
						
						case 0xFF: {
							printf("Encountered End Of Track event at %08x, complete event %08x\n", (unsigned int)ftell(f)-4, curtoken);
							break;
						}
						
						default: {
							printf("Unknown event %x at %08x\n", curtoken, (unsigned int)ftell(f)-4);
							break;
						}
					}
				}
				fwrite(&curtoken, 4, 1, o);
			}
			fclose(o);
		}
	}
	
	printf("Done.\n");
	
	fclose(f);
	return 0;
}
