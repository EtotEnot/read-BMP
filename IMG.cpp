#include <iostream>
#include "CONTROL_LIST.h"
#include "IMG.h"

int readBMP(form &img, FILE *f) {
	int error = 0;
	if( img.header.bitCount == 16 ) {
		fseek(f, img.header.offsetBits, SEEK_SET);
		img.img16.width = img.header.width;
		img.img16.height = img.header.height;
		if( (error = read16bitmap(img.img16, f)) != READ_OK ) return error;
		return READ_OK;
	}
	if( img.header.bitCount == 24 ) {
		fseek(f, img.header.offsetBits, SEEK_SET);
		img.img24.width = img.header.width;
		img.img24.height = img.header.height;
		if( (error = read24bitmap(img.img24, f)) != READ_OK ) return error;
		return READ_OK;
	}
	if( img.header.bitCount == 8 ) {
		img.img8.width = img.header.width;
		img.img8.height = img.header.height;
		
		fseek(f, sizeof(BITMAPHEADER), SEEK_SET);
		if( (error = read8bitPalette(img.img8, f)) != READ_OK ) return error;
		fseek(f, img.header.offsetBits, SEEK_SET);
		if( (error = read8bitArray(img.img8, f)) != READ_OK ) return error;
		return READ_OK;
	}
	return ERROR_FILE_TYPE;
}
int read24bitmap(image_24 &img, FILE *f) {
	img.pixmap = (pixel_24*)malloc(img.height*img.width*sizeof(pixel_24));
	if(!img.pixmap) return ERROR_MEMORY;

	int byte = fread(img.pixmap, sizeof(pixel_24), img.height*img.width, f);

	if( byte != img.height * img.width) {
		free(img.pixmap);
		img.pixmap = NULL;
		return ERROR_READ_PIXMAP;
	}
	return READ_OK;
}
int read16bitmap(image_16 &img, FILE *f) {
	img.pixmap = (pixel_16*)malloc(img.height*img.width * sizeof(pixel_16));
	if(!img.pixmap) return ERROR_MEMORY;

	int byte = fread(img.pixmap, sizeof(pixel_16), img.height*img.width, f);
	
	if( byte != img.height * img.width) {
		free(img.pixmap);
		img.pixmap = NULL;
		return ERROR_READ_PIXMAP;
	}
	return READ_OK;
}
int read8bitPalette(image_8 &img, FILE *f) {
	int byte = fread(img.palette, sizeof(pixel_8), 256, f);
	if( byte != 256) return ERROR_READ_PIXMAP;
	return READ_OK;
}
int read8bitArray(image_8 &img, FILE *f) {
	int byte = fread(img.colorArray, sizeof(unsigned char), img.height*img.width, f);
	if( byte != img.height*img.width) return ERROR_READ_PIXMAP;
	return READ_OK;
}
int readHEADER(BITMAPHEADER &header, FILE *f) {
	int byte = fread(&header, sizeof(BITMAPHEADER), 1, f);
	if(byte != 1) return ERROR_READ_HEADER;
	if (header.type != 0x4D42)  return ERROR_FILE_TYPE;
	fseek(f, header.offsetBits, SEEK_SET);
	return READ_OK;
}
int readBMP_HEADER(BMP_HEADER &header, FILE *f) {
	int byte = fread(&header, sizeof(BMP_HEADER), 1, f);
	if(byte != 1) return ERROR_READ_HEADER;
	if (header.type != 0x4D42)  return ERROR_FILE_TYPE;
	return READ_OK;
}

void monochrome(form &img, const int k) {
	if( img.header.bitCount == 16 ) {
		monochrome16BMP(img.img16, k);
	}
	if( img.header.bitCount == 24 ) {
		monochrome24BMP(img.img24, k);
	}
	if(img.header.bitCount == 8 ) {
		monochrome8BMP(img.img8, k);
	}
}
void gamma(form &img, double k) {
	if( img.header.bitCount == 16 ) {
		gamma16BMP(img.img16, k);
	}
	if( img.header.bitCount == 24 ) {
		gamma24BMP(img.img24, k);
	}
	if ( img.header.bitCount == 8 ) {
		gamma8BMP(img.img8, k);
	}
}
int medianFilter(form &img, const int n) {
	if( !bufferSize(n) ) {
		return UNKNOWN_EVENT;
	}
	if(img.header.bitCount == 16) {
		medianFilter16BMP(img.img16, n);
		return EVENT_OK;
	}
	if(img.header.bitCount == 24) {
		medianFilter24BMP(img.img24, n);
		return EVENT_OK;
	}
	return UNKNOWN_EVENT;
}
void negative(form &img) {
	if( img.header.bitCount == 16 ) {
		negative16BMP(img.img16);
	}
	if( img.header.bitCount == 24 ) {
		negative24BMP(img.img24);
	}
	if( img.header.bitCount == 8 ) {
		negative8BMP(img.img8);
	}
}
int writeBMP(form &img, FILE *f) {
	int error = 0;
	if( img.header.bitCount == 16 ) {
		if( (error = write16BMP(img.header, img.img16, f)) != WRITE_OK) return error;
	}
	if( img.header.bitCount == 24 ) {
		if( (error = write24BMP(img.header, img.img24, f)) != WRITE_OK) return error;
	}
	if( img.header.bitCount == 8 ) {
		if( (error = write8BMP(img, f)) != WRITE_OK) return error;
	}
	return WRITE_OK;
}

int middle8(pixel_8 px) {
	return (px.r + px.g + px.b)/3;
}
pixel_8 colorPX8(pixel_8 px, unsigned int color) {
	px.r = color;
	px.g = color;
	px.b = color;
	return px;
}
void monochrome8BMP(image_8 &img, const int k) {
	for(int i = 0; i < 256; i++) {
		if( middle8( img.palette[i]) > k ) 
			img.palette[i] = colorPX8(img.palette[i], 0xFF);
		else img.palette[i] = colorPX8(img.palette[i], 0x00);
	}
}

pixel_24 colorAllPixel(pixel_24 px, unsigned int color) {
	px.r = color; 
	px.g = color; 
	px.b = color;
	return px;
}
void monochrome24BMP(image_24 &img, const int k) {
	for(int i = 0; i < img.height*img.width; i++) {
		if( middle(img.pixmap[i]) > k ) 
			img.pixmap[i] = colorAllPixel(img.pixmap[i], 0xFF);
		else img.pixmap[i] = colorAllPixel(img.pixmap[i], 0x00);
	}
}
int middle(pixel_24 px) {
	return (px.r+px.g+px.b)/3;
}

void monochrome16BMP(image_16 &img, const int k) {
	for(int i = 0; i < img.height*img.width; i++) {
		if( middle16(img.pixmap[i]) > k ) 
			 img.pixmap[i] = colorPX(img.pixmap[i], 0xFF);
		else img.pixmap[i] = colorPX(img.pixmap[i], 0x00);
	}
}
int middle16(pixel_16 px) {
	return ( px.r + px.g + px.b ) / 3;
}
pixel_16 colorPX(pixel_16 px, unsigned int color) {
	px.r = color;
	px.g = color;
	px.b = color;
	return px;
}

int write24BMP(BITMAPHEADER header, image_24 img, FILE *f) {
	int byte = fwrite(&header, sizeof(BITMAPHEADER), 1, f);
	if( byte != 1 ) return ERROR_WRITE;

	byte = fwrite(img.pixmap, sizeof(pixel_24), img.height*img.width, f);
	if( byte != img.height*img.width) return ERROR_WRITE;
	
	return READ_OK;
}
int write16BMP(BITMAPHEADER header, image_16 img, FILE *f) {
	int byte = fwrite(&header, sizeof(BITMAPHEADER), 1, f);
	if( byte != 1 ) return ERROR_WRITE;

	byte = fwrite(img.pixmap, sizeof(pixel_16), img.height*img.width, f);
	if( byte != img.height*img.width) return ERROR_WRITE;
	
	return READ_OK;
}
int write8BMP(form img, FILE *f) {
	int byte = fwrite(&img.header, sizeof(BITMAPHEADER), 1, f);
	if( byte != 1 ) return ERROR_WRITE;

	byte = fwrite(&img.img8.palette, sizeof(pixel_8), 256, f);
	if( byte != 256 ) return ERROR_WRITE;

	fseek(f, img.header.offsetBits, SEEK_SET);
	byte = fwrite(&img.img8.colorArray, sizeof(unsigned char), img.header.height*img.header.width,f);
	if( byte != img.header.height*img.header.width ) return ERROR_WRITE;
	return READ_OK;
}

void negative8BMP(image_8 &img) {
	for(int i = 0; i < 256; i++ ) {
		img.palette[i].r = 0xFFFFFFFF - img.palette[i].r;
		img.palette[i].g = 0xFFFFFFFF - img.palette[i].g;
		img.palette[i].b = 0xFFFFFFFF - img.palette[i].b;
	}
}
void negative24BMP(image_24 &img) {
	for(int i = 0; i < img.height*img.width; i++) {
		img.pixmap[i].r = 0xFFFFFFFF - img.pixmap[i].r;
		img.pixmap[i].g = 0xFFFFFFFF - img.pixmap[i].g;
		img.pixmap[i].b = 0xFFFFFFFF - img.pixmap[i].b;
	}
}
void negative16BMP(image_16 &img) {
	for(int i = 0; i < img.height*img.width; i++) {
		img.pixmap[i].r = 0xFFFFFFFF - img.pixmap[i].r;
		img.pixmap[i].g = 0xFFFFFFFF - img.pixmap[i].g;
		img.pixmap[i].b = 0xFFFFFFFF - img.pixmap[i].b;
	}
}

pixel_8 gammaResult(pixel_8 px, const double GAMMA) {
	if( GAMMA < 1 ) {
		if(px.r / GAMMA > 0xFF) px.r = 0xFF;
		else px.r = px.r / GAMMA;
		if(px.g / GAMMA > 0xFF) px.g = 0xFF;
		else px.g = px.g / GAMMA;
		if(px.b / GAMMA > 0xFF) px.b = 0xFF;
		else px.b = px.b / GAMMA;
	}
	else {
		px.r = px.r / GAMMA;
		px.g = px.g / GAMMA;
		px.b = px.b / GAMMA;
	}
	return px;
}
void gamma8BMP(image_8 &img, const double k) {
	for(int i = 0; i < 256; i++) {
		img.palette[i] = gammaResult(img.palette[i], k);
	}
}

pixel_24 gammaResult(pixel_24 px, const double GAMMA) {
	if( GAMMA < 1 ) {
		if(px.r / GAMMA > 0xFF) px.r = 0xFF;
		else px.r = px.r / GAMMA;
		if(px.g / GAMMA > 0xFF) px.g = 0xFF;
		else px.g = px.g / GAMMA;
		if(px.b / GAMMA > 0xFF) px.b = 0xFF;
		else px.b = px.b / GAMMA;
	}
	else {
		px.r = px.r / GAMMA;
		px.g = px.g / GAMMA;
		px.b = px.b / GAMMA;
	}
	
	return px;
}
void gamma24BMP(image_24 &img, const double k) {
	for(int i = 0; i < img.height*img.width; i++) {
		img.pixmap[i] = gammaResult(img.pixmap[i], k);
	}
}

pixel_16 gammaResult(pixel_16 px, const double GAMMA) {
	
	if( GAMMA < 1 ) {
		if(px.r / GAMMA > 0xFF) px.r = 0xFF;
		else px.r = px.r / GAMMA;
		if(px.g / GAMMA > 0xFF) px.g = 0xFF;
		else px.g = px.g / GAMMA;
		if(px.b / GAMMA > 0xFF) px.b = 0xFF;
		else px.b = px.b / GAMMA;
	}
	else {
		px.r = px.r / GAMMA;
		px.g = px.g / GAMMA;
		px.b = px.b / GAMMA;
	}
	
	return px;
}
void gamma16BMP(image_16 &img, const double k) {
	for(int i = 0; i < img.height*img.width; i++) {
		img.pixmap[i] = gammaResult(img.pixmap[i], k);
	}
}

pixel_24 middleColor(pixel_24 *buffer, const int N, const int n) {
	pixel_24 px = {0};
	int red = 0;
	int green = 0;
	int blue = 0;
	
	for(int i = 0; i < n*n; i++) {
		red += buffer[i].r;
		green += buffer[i].g;
		blue += buffer[i].b;
	}
	px.r = red /= N;
	px.g = green /= N;
	px.b = blue /= N;
	return px;
}

int toLine(int i, int j, int width) {
	return j*width + i;
}

int entryBuffer(image_24 img, pixel_24* &buffer, int i, int j, const int N) {
	int n = (N-1)/2;
	int X = i-n, Y = j-n, count = 0;
	int px = 0;
	pixel_24 pix = {0};

	for(int y = Y; y < Y+N; y++) {
		for(int x = X; x < X+N; x++) {
			if( x < 0 || y < 0 || x > img.width || y > img.height ) buffer[count++] = pix;
			else {
				buffer[count++] = img.pixmap[ toLine(x, y, img.width) ];
				px++;
			}
		}
	}
	return px;
}

void medianFilter24BMP(image_24 &img, const int N) {
	pixel_24 *buffer = (pixel_24*)malloc(N*N*sizeof(pixel_24));
	int px = 0;

	for(int j = 0; j < img.height; j++) {
		for(int i = 0; i< img.width; i++) {
			px = entryBuffer(img, buffer, i, j, N);	
			img.pixmap[ toLine(i,j,img.width) ] = middleColor(buffer, px, N);
		}
	}
	free(buffer);
}

void medianFilter16BMP(image_16 &img, const int N) {
	pixel_16 *buffer = (pixel_16*)malloc(N*N*sizeof(pixel_16));
	int px = 0;

	for(int j = 0; j < img.height; j++) {
		for(int i = 0; i< img.width; i++) {
			px = entryBuffer16(img, buffer, i, j, N);	
			img.pixmap[ toLine(i,j,img.width) ] = middleColor16(buffer, px, N);
		}
	}
	free(buffer);
}

int entryBuffer16(image_16 img, pixel_16* &buffer, int i, int j, const int N) {
	int n = (N-1)/2;
	int X = i-n, Y = j-n, count = 0;
	int px = 0;
	pixel_16 pix = {0};

	for(int y = Y; y < Y+N; y++) {
		for(int x = X; x < X+N; x++) {
			if( x < 0 || y < 0 || x > img.width || y > img.height ) buffer[count++] = pix;
			else {
				buffer[count++] = img.pixmap[ toLine(x, y, img.width) ];
				px++;
			}
		}
	}
	return px;
}

pixel_16 middleColor16(pixel_16 *buffer, const int N, const int n) {
	pixel_16 px = {0};
	int red = 0;
	int green = 0;
	int blue = 0;
	
	for(int i = 0; i < n*n; i++) {
		red += buffer[i].r;
		green += buffer[i].g;
		blue += buffer[i].b;
	}
	px.r = red /= N;
	px.g = green /= N;
	px.b = blue /= N;
	return px;
}

bool bufferSize(int size) {
	if( size < 3) return false;
	if( size > 15) return false;
	if( size % 2 == 0 ) return false;
	return true;
}

int convertTo24(image_16 &img16, image_24 &img24) {
	img24.height = img16.height;
	img24.width = img16.width;

	img24.pixmap = (pixel_24*)malloc(img16.height*img16.width*sizeof(pixel_24));
	if(!img24.pixmap) return ERROR_MEMORY;

	for(int i = 0; i < img16.height*img16.width; i++) {
		img24.pixmap[i].r = img16.pixmap[i].r << 3; 
		img24.pixmap[i].g = img16.pixmap[i].g << 3;
		img24.pixmap[i].b = img16.pixmap[i].b << 3; 
	}
	return READ_OK;
}

void freePixmap(form img) {
	if( img.header.bitCount == 16 ) {
		free(img.img16.pixmap);
		img.img16.pixmap = NULL;
	}
	if( img.header.bitCount == 24 ) {
		free(img.img24.pixmap);
		img.img24.pixmap = NULL;
	}
}