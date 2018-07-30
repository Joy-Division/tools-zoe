/*
	written by Missingno_force a.k.a. Missingmew
	Copyright (c) 2014-2018
	see LICENSE for details
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "lodepng/lodepng.h"

typedef struct {
	uint16_t textureNumFrames;
	uint16_t unknown11;
	uint32_t unknown12;
	uint32_t textureNumPalettes;
	uint32_t paletteTableOffset;
	// first 16 bytes block
}__attribute__((packed)) textureHeader;

typedef struct {
	uint32_t unknown20;
	uint32_t unknown21;
	uint16_t textureWidth;
	uint16_t textureHeight;
	uint32_t texturePixelOffset;
	// second 16 bytes block
	// total textureHeader size 32 bytes
}__attribute__((packed)) textureAttributes;

typedef struct {
	uint32_t unknown30;
	uint32_t unknown31;
	uint32_t frameIndexMult; // offset to texture attributes
	uint32_t paletteOffset;
	// first 16 bytes block
	//~ uint32_t unknown40;
	uint16_t framewidthmult; // frame width times 16 minus 1
	uint8_t frameoffsetx; // offset divided by 4
	uint8_t frameheightmult; // height minus 1
	uint32_t frameoffsety; // offset times 4
	uint32_t unknown42;
	uint32_t unknown43;
	// second 16 bytes block
	// total frameHeader size 32 bytes
}__attribute__((packed)) paletteHeader;

typedef struct {
	uint8_t red;
	uint8_t green;
	uint8_t blue;
	uint8_t alpha;
}__attribute__((packed)) paletteEntry;

enum palMode {
	palMode4bpp,
	palMode8bpp
};

int swizzle(int index) {
	int b = (index >> 3) & 3;
	if( b == 1 ) b = 2;
	else if( b == 2 ) b = 1;
	int swiz = b * 8;
	int b32 = (index >> 5 ) * 32;
	int final = b32 + swiz + (index & 7 );
	return final;
}

void generatePalettePairs( unsigned int numPaletteEntries, unsigned char* palette, paletteEntry *paired, unsigned int bpp ) {
	int i, e = 0;
	if( bpp == palMode4bpp ) {
		for( i = 0; i < numPaletteEntries; i++ ) {
			paired[i].red = palette[e];
			e++;
			paired[i].green = palette[e];
			e++;
			paired[i].blue = palette[e];
			e++;
			if( palette[e] >= 0x7F ) paired[i].alpha = 0xFF;
			else if ( palette[e] == 0 ) paired[i].alpha = 0; 
			else paired[i].alpha = (palette[e] + 1)*2;
			e++;
		}
	}
	else if( bpp == palMode8bpp ) {
		for( i = 0; i < numPaletteEntries; i++ ) {
			e = 4 * swizzle(i);
			paired[i].red = palette[e];
			e++;
			paired[i].green = palette[e];
			e++;
			paired[i].blue = palette[e];
			e++;
			if( palette[e] >= 0x7F ) paired[i].alpha = 0xFF;
			else if ( palette[e] == 0 ) paired[i].alpha = 0; 
			else paired[i].alpha = (palette[e] + 1)*2;
			e++;
		}
	}
		return;
}

void texToRgba( textureAttributes header, paletteEntry *paletteTable, unsigned char *texPixelData, unsigned char *rgbaPixelData, unsigned int bpp, unsigned int totalTexPixels, unsigned int widthAligned ) {
	unsigned int curTexPixel = 0, curRgbaPixel = 0, skipPixel = widthAligned - header.textureWidth, currentRow = 0;
	if( bpp == palMode4bpp ) {
		uint8_t pixelA, pixelB;
		for( curTexPixel = 0; curTexPixel < totalTexPixels; curTexPixel++ ) {
			if( curTexPixel == (header.textureWidth/2)*(currentRow + 1) + (skipPixel/2)*currentRow ) {
				curTexPixel += (skipPixel/2);
				currentRow++;
			}
			pixelB = texPixelData[curTexPixel] >> 4;
			pixelA = texPixelData[curTexPixel] & 0x0F;
			rgbaPixelData[curRgbaPixel] = paletteTable[pixelA].red;
			curRgbaPixel++;
			rgbaPixelData[curRgbaPixel] = paletteTable[pixelA].green;
			curRgbaPixel++;
			rgbaPixelData[curRgbaPixel] = paletteTable[pixelA].blue;
			curRgbaPixel++;
			rgbaPixelData[curRgbaPixel] = paletteTable[pixelA].alpha;
			curRgbaPixel++;
			rgbaPixelData[curRgbaPixel] = paletteTable[pixelB].red;
			curRgbaPixel++;
			rgbaPixelData[curRgbaPixel] = paletteTable[pixelB].green;
			curRgbaPixel++;
			rgbaPixelData[curRgbaPixel] = paletteTable[pixelB].blue;
			curRgbaPixel++;
			rgbaPixelData[curRgbaPixel] = paletteTable[pixelB].alpha;
			curRgbaPixel++;
		}
	}
	else if( bpp == palMode8bpp ) {
		for( curTexPixel = 0; curTexPixel < totalTexPixels; curTexPixel++ ) {
			if( curTexPixel == header.textureWidth*(currentRow + 1) + skipPixel*currentRow ) {
				curTexPixel += skipPixel;
				currentRow++;
			}
			rgbaPixelData[curRgbaPixel] = paletteTable[texPixelData[curTexPixel]].red;
			curRgbaPixel++;
			rgbaPixelData[curRgbaPixel] = paletteTable[texPixelData[curTexPixel]].green;
			curRgbaPixel++;
			rgbaPixelData[curRgbaPixel] = paletteTable[texPixelData[curTexPixel]].blue;
			curRgbaPixel++;
			rgbaPixelData[curRgbaPixel] = paletteTable[texPixelData[curTexPixel]].alpha;
			curRgbaPixel++;
		}
	}
	// printf("Done with frame\n");
	return;
}

int main( int argc, char **argv ) {
	textureHeader header;
	unsigned char *paletteData, *pixelData, *rgbaPixelData;
	
	FILE *f, *o;
	unsigned int i, fileSize, paletteSize, textureSize, textureSizeAligned, pixelDataSize, numPaletteEntries, bpp, widthAligned = 0, mode;
	unsigned int curPal = 0;
	
	if( argc < 3 ) {
		printf("Not enough args!\nUse: %s TEX-file mode\nuse mode 0 for rgba, mode 1 for png\n", argv[0]);
		return 1;
	}
	
	mode = strtoul(argv[2], NULL, 10);
	
	if( !(f = fopen( argv[1], "rb" ))) {
		printf("Couldnt open file %s\n", argv[1]);
		return 1;
	}
	
	
	char *outputname = malloc( strlen( argv[1] ) + 9 + 7 + 5 + 1 ); // will contain: infilename + "-frame###" + "-pal###" + ".png" + NULL or ".rgba"
	
	fseek( f, 0, SEEK_END );
	fileSize = ftell(f);
	fseek( f, 0, SEEK_SET );
	
	fread( &header, sizeof(textureHeader), 1, f );
	
	textureAttributes attributes[header.textureNumFrames];
	paletteHeader palettes[header.textureNumPalettes];
	unsigned int paletteStats[header.textureNumFrames];
	
	for( i = 0; i < header.textureNumFrames; i++ ) {
		fread( &attributes[i], sizeof(textureAttributes), 1, f);
		paletteStats[i] = 0;
	}
	
	fseek( f, header.paletteTableOffset, SEEK_SET );
	for( i = 0; i < header.textureNumPalettes; i++ ) {
		fread( &palettes[i], sizeof(paletteHeader), 1, f );
		paletteStats[(palettes[i].frameIndexMult/0x10)-1] += 1;
	}
	
	printf("Number of frames in texture: %d\n", header.textureNumFrames);
	printf("Number of palettes in texture: %d\n", header.textureNumPalettes);
	
	
	for( i = 0; i < header.textureNumFrames; i++ ) {
		printf( "Processing frame %d\n", i );
		
		
		if( attributes[i].textureWidth % 8 ) widthAligned = attributes[i].textureWidth + (8-(attributes[i].textureWidth%8));
		else widthAligned = attributes[i].textureWidth;
		textureSize = attributes[i].textureWidth * attributes[i].textureHeight;
		textureSizeAligned = widthAligned * attributes[i].textureHeight;
		
		if( header.textureNumPalettes == 1 ) numPaletteEntries = (fileSize - palettes[0].paletteOffset) / 4;
		else numPaletteEntries = (palettes[curPal+1].paletteOffset - palettes[curPal].paletteOffset) / 4;
		
		if( numPaletteEntries == 16 ) {
			bpp = palMode4bpp;
			printf("Frame mode is 4bpp.\n");
		}
		else if( numPaletteEntries == 256 ) {
			bpp = palMode8bpp;
			printf("Frame mode is 8bpp.\n");
		}
		
		paletteEntry *framePalette = malloc(2*numPaletteEntries*sizeof(paletteEntry));
		paletteSize = numPaletteEntries*4;
		paletteData = malloc(paletteSize);
		
		if(bpp == palMode4bpp) pixelDataSize = textureSizeAligned/2;
		else if(bpp == palMode8bpp) pixelDataSize = textureSizeAligned;
		else return 1;
		
		printf("Frame width: %d - Aligned width: %d - Frame height: %d\n", attributes[i].textureWidth, widthAligned, attributes[i].textureHeight);
		printf("Number of palettes found for frame: %d\n", paletteStats[i]);
		
		pixelData = malloc(pixelDataSize);
		rgbaPixelData = malloc(textureSize * 4);
		
		fseek( f, attributes[i].texturePixelOffset, SEEK_SET );
		
		fread( pixelData, pixelDataSize, 1, f );
		
		while(paletteStats[i]) {
			fseek(f, palettes[curPal].paletteOffset, SEEK_SET);
			fread(paletteData, paletteSize, 1, f);
			generatePalettePairs( numPaletteEntries, paletteData, framePalette, bpp );
			
			texToRgba( attributes[i], framePalette, pixelData, rgbaPixelData, bpp, pixelDataSize, widthAligned );
			
			if(mode) {
				sprintf( outputname, "%s-frame%03d-pal%03d.png", argv[1], i, curPal );
				
				lodepng_encode32_file( outputname, rgbaPixelData, attributes[i].textureWidth, attributes[i].textureHeight );
			}
			else {
				sprintf( outputname, "%s-frame%03d-pal%03d.rgba", argv[1], i, curPal );
				
				if( !(o = fopen( outputname, "wb" ))) {
					printf("Couldnt open file %s\n", outputname);
					return 1;
				}
				fwrite( rgbaPixelData, textureSize * 4, 1, o );
				fclose(o);
			}
			curPal++;
			paletteStats[i] -= 1;
		}
		free(rgbaPixelData);
		free(paletteData);
		free(framePalette);
		free(pixelData);
	}
	
	fclose(f);
	free( outputname );
	printf("Done.\n");
	return 0;
}
