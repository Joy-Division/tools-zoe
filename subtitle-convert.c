/*
	written by Missingno_force a.k.a. Missingmew
	Copyright (c) 2014-2018
	Expanded accent information by IlDucci (c) 2023
	see LICENSE for details
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <iconv.h>

typedef struct {
	uint32_t startFrame;
	uint32_t endFrame;
	uint32_t textLength;
	uint32_t textId;
}__attribute__((packed)) subLineHeader;

// NOTE FOR FUTURE EDITORS: MAKE SURE TO CONVERT THIS CODE SO IT OUTPUTS UTF-8 STRINGS, OTHERWISE YOU WILL HAVE:
// - ISSUES WHEN DISPLAYING CERTAIN EUC-JP CHARACTERS USED IN THE WESTERN VERSIONS (BUTTON KEYS, 0xA1C8/0xA1C9 QUOTATION MARKS).
// - COLLISIONS BETWEEN THE OPENING EXCLAMATION MARK (¡, 0xA1) AND THE BEFOREMENTIONED CHARACTERS.

char getModdedLetter( int modifier, int letter ) {
	if( modifier == 0x60 ) { // `
		if( letter == 0x41 ) return 0xC0; // A, À
		if( letter == 0x45 ) return 0xC8; // E, È
		if( letter == 0x49 ) return 0xCC; // I, Ì
		if( letter == 0x4F ) return 0xD2; // O, Ò
		if( letter == 0x55 ) return 0xD9; // U, Ù
		if( letter == 0x61 ) return 0xE0; // a, à
		if( letter == 0x65 ) return 0xE8; // e, è
		if( letter == 0x69 ) return 0xEC; // i, ì
		if( letter == 0x6F ) return 0xF2; // o, ò
		if( letter == 0x75 ) return 0xF9; // u, ù
	}
	if( modifier == 0x5E ) { // ^
		if( letter == 0x41 ) return 0xC2; // A, Â
		if( letter == 0x45 ) return 0xCA; // E, Ê
		if( letter == 0x49 ) return 0xCE; // I, Î
		if( letter == 0x4F ) return 0xD4; // O, Ô
		if( letter == 0x55 ) return 0xDB; // U, Û
		if( letter == 0x61 ) return 0xE2; // a, â
		if( letter == 0x65 ) return 0xEA; // e, ê
		if( letter == 0x69 ) return 0xEE; // i, î
		if( letter == 0x6F ) return 0xF2; // o, ô
		if( letter == 0x75 ) return 0xFB; // u, û
	}
	if( modifier == 0x25 ) { // %
		if( letter == 0x4F ) return 0xB0; // O, °
		// THE GAME DOES NOT HAVE A COMBO TO WRITE THE UPPERCASE "Å"
		if( letter == 0x61 ) return 0xE5; // a, å
		if( letter == 0x6F ) return 0xBA; // o, º
	}
	if( modifier == 0x27 ) { // '
		if( letter == 0x41 ) return 0xC1; // A, Á
		if( letter == 0x45 ) return 0xC9; // E, É
		if( letter == 0x49 ) return 0xCD; // I, Í
		if( letter == 0x4F ) return 0xD3; // O, Ó
		if( letter == 0x55 ) return 0xDA; // U, Ú
		if( letter == 0x59 ) return 0xDD; // Y, Ý
		if( letter == 0x61 ) return 0xE1; // a, á
		if( letter == 0x65 ) return 0xE9; // e, é
		if( letter == 0x69 ) return 0xED; // i, í
		if( letter == 0x6F ) return 0xF3; // o, ó
		if( letter == 0x75 ) return 0xFA; // u, ú
		if( letter == 0x79 ) return 0xFF; // y, ý
	}
	if( modifier == 0x7E ) { // ~
		if( letter == 0x21 ) return 0xA1; // !, ¡
		if( letter == 0x3C ) return 0xAB; // <, «
		if( letter == 0x3E ) return 0xBB; // >, »
		if( letter == 0x3F ) return 0xBF; // ?, ¿
		if( letter == 0x41 ) return 0xC4; // A, Ä
		if( letter == 0x45 ) return 0xCB; // E, Ë
		if( letter == 0x49 ) return 0xCF; // I, Ï
		// THE GAME DOES NOT HAVE WORKING COMBOS FOR Ã, Õ, ã, õ.
		if( letter == 0x4E ) return 0xD1; // N, Ñ
		if( letter == 0x4F ) return 0xD6; // O, Ö
		if( letter == 0x51 ) return 0xC7; // Q, Ç
		if( letter == 0x55 ) return 0xDC; // U, Ü
		if( letter == 0x61 ) return 0xE4; // a, ä
		if( letter == 0x65 ) return 0xEB; // e, ë
		if( letter == 0x69 ) return 0xEF; // i, ï
		if( letter == 0x6E ) return 0xF1; // n, ñ
		if( letter == 0x6F ) return 0xF6; // o, ö
		if( letter == 0x71 ) return 0xE7; // q, ç
		if( letter == 0x73 ) return 0xDF; // s, ß
		if( letter == 0x75 ) return 0xFC; // u, ü
		if( letter == 0x79 ) return 0xFF; // y, ÿ
	}
	if( modifier == 0x40 ) { // @
		if( letter == 0x45 ) return 0x8C; // O, Œ
		if( letter == 0x65 ) return 0x9C; // o, œ
	}
	return 0;
}

char *processLine( char *line ) {
	char *retline = malloc( strlen(line)+1 );
	int i, e = 0, modifier, letter;
	for( i = 0; i < strlen(line) + 1; i++ ) {
		if( line[i] != 0x26 ) { // &
			retline[e] = line[i];
			e++;
		}
		else {
			i++;
			modifier = line[i];
			i++;
			letter = line[i];
			retline[e] = getModdedLetter( modifier, letter );
			e++;
		}
	}
	return retline;
}

char *processLineJp( char *line, unsigned int len ) {
	char *tempbuf = malloc(len*2);
	char *workbuf = tempbuf;
	iconv_t cd;
	size_t inbufsize = len;
	size_t outbufsize = len*2;
	cd = iconv_open("UTF-8", "EUC-JP");
	#ifdef _WIN32
	const char *linein = (const char *)line;
	iconv(cd, &linein, &inbufsize, &workbuf, &outbufsize);
	#else
	iconv(cd, &line, &inbufsize, &workbuf, &outbufsize);
	#endif
	iconv_close(cd);
	return tempbuf;
}
	

unsigned char utf8bom[3] = { 0xEF, 0xBB, 0xBF };

int main( int argc, char **argv ) {
	
	FILE *f, *o;
	
	unsigned int fileSize, lineNumber = 1, lineHour, lineMinute, lineSecond, lineMs, jp = 0, bom = 0;
	subLineHeader header;
	char lineStartText[13] = { 0 }, lineEndText[13] = { 0 };
	char *readline = NULL, *outputline = NULL;
	
	if( argc < 2 ) {
		printf("Not enough args!\nUse: %s subtitle file\n", argv[0]);
		return 1;
	}
	
	if( !(f = fopen( argv[1], "rb" ))) {
		printf("Couldnt open file %s\n", argv[1]);
		return 1;
	}
	
	char *outputname = malloc( strlen( argv[1] ) + 4 + 1 );
	sprintf( outputname, "%s.srt", argv[1] );
	
	if( !(o = fopen( outputname, "wb" ))) {
		printf("Couldnt open file %s\n", outputname);
		return 1;
	}
	
	free(outputname);
	
	fseek( f, 0, SEEK_END );
	fileSize = ftell(f);
	fseek( f, 0, SEEK_SET );
	
	while( ftell(f) != fileSize ) {
		fread( &header, sizeof(header), 1, f );
		readline = malloc( header.textLength );
		fread( readline, header.textLength, 1, f );
		lineSecond = header.startFrame/60;
		lineMinute = lineSecond/60;
		lineHour = lineMinute/60;
		lineMs = 1000 * ((float)header.startFrame/60 - lineSecond);
		lineMinute -= 60*lineHour;
		lineSecond -= 60*lineMinute;
		sprintf( lineStartText, "%02d:%02d:%02d,%03d", lineHour, lineMinute, lineSecond, lineMs );
		lineSecond = header.endFrame/60;
		lineMinute = lineSecond/60;
		lineHour = lineMinute/60;
		lineMs = 1000 * ((float)header.endFrame/60 - lineSecond);
		lineMinute -= 60*lineHour;
		lineSecond -= 60*lineMinute;
		sprintf( lineEndText, "%02d:%02d:%02d,%03d", lineHour, lineMinute, lineSecond, lineMs );
		if(readline[0] & 0x80) {
			jp = 1;
			printf("Subtitles contain EUC-JP encoded text.\n");
		}
		if(jp && (!bom)) {
			fwrite( utf8bom, 3, 1, o );
			bom = 1;
		}
		if(jp) outputline = processLineJp( readline, header.textLength );
		else outputline = processLine( readline );
		fprintf( o, "%d\n%s --> %s\n%s\n\n", lineNumber, lineStartText, lineEndText, outputline );
		lineNumber++;
		free(outputline);
		free(readline);
	}
	
	fclose(f);
	fclose(o);
	
	return 0;
}