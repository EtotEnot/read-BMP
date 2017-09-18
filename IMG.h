#pragma once
#pragma comment(linker, "/STACK:10000000000")
#pragma pack (1)
struct BMP_HEADER {
	unsigned short type;
	unsigned int file_size;
	unsigned int reserver;
	unsigned int offsetBits;
};
struct BMP_INFOHEADER {
	unsigned int size;
	unsigned int width;
	unsigned int height;
	unsigned short planes;
	unsigned short bitCount;
	unsigned int compression;
	unsigned int imageSize;
	unsigned int XPixelsPerM;
	unsigned int YPixelsPerM;
	unsigned int ColorsUsed;
	unsigned int ColorsImportant;
};
struct BITMAPHEADER {
	unsigned short type;
	unsigned int file_size;
	unsigned int reserver;
	unsigned int offsetBits;

	unsigned int size;
	unsigned int width;
	unsigned int height;
	unsigned short planes;
	unsigned short bitCount;
	unsigned int compression;
	unsigned int imageSize;
	unsigned int XPixelsPerM;
	unsigned int YPixelsPerM;
	unsigned int ColorsUsed;
	unsigned int ColorsImportant;
};
struct pixel_16 {
	unsigned short r : 5;
	unsigned short g : 5;
	unsigned short b : 5;
	unsigned short alfa: 1;
};
#pragma pack (0)
struct pixel_24 {
	unsigned char r;
	unsigned char g;
	unsigned char b;
};
struct pixel_8{
	unsigned char r;
	unsigned char g;
	unsigned char b;
	unsigned char reserved;
};
struct image_24 {
	unsigned int width;
	unsigned int height;
	pixel_24 *pixmap;
};
struct image_16 {
	unsigned int width;
	unsigned int height;
	pixel_16 *pixmap;
};
struct image_8 {
	unsigned int width;
	unsigned int height;
	pixel_8 palette[256];
	unsigned char colorArray[1024*1024];
};
struct form {
	BITMAPHEADER header;
	image_24 img24;
	image_16 img16;
	image_8 img8;
};

int readHEADER(BITMAPHEADER &header, FILE *f);
int readBMP_HEADER(BMP_HEADER &header, FILE *f);
int readBMP(form &img, FILE *f);
int read24bitmap(image_24 &img, FILE *f);
int read16bitmap(image_16 &img, FILE *f);
int read8bitArray(image_8 &img, FILE *f);
int read8bitPalette(image_8 &img, FILE *f);
int read8bitmap(image_8 &img, FILE *f);

int write24BMP(BITMAPHEADER header, image_24 img, FILE *f);
int write16BMP(BITMAPHEADER header, image_16 img, FILE *f);
int write8BMP(form img, FILE *f);

pixel_24 colorAllPixel(pixel_24 px, unsigned int color);
void monochrome24BMP(image_24 &img, const int k);
int middle(pixel_24 pix);

int middle8(pixel_8 px);
pixel_8 colorPX8(pixel_8 px, unsigned int color);
void monochrome8BMP(image_8 &img, const int k);

void monochrome16BMP(image_16 &img, const int k);
int middle16(pixel_16 px);
pixel_16 colorPX(pixel_16 px, unsigned int color);

void negative8BMP(image_8 &img);
void negative24BMP(image_24 &img);
void negative24BMP2(image_24 &img);
void negative16BMP(image_16 &img);

pixel_8 gammaResult(pixel_8 px, const double GAMMA);
void gamma8BMP(image_8 &img, const double k);

pixel_24 gammaResult(pixel_24 px, const double GAMMA);
void gamma24BMP(image_24 &img, const double k);

pixel_16 gammaResult(pixel_16 px, const double GAMMA);
void gamma16BMP(image_16 &img, const double k);

pixel_24 middleColor(pixel_24 *buffer, const int N);
int toLine(int i, int j, int width);

pixel_24 middleColor(pixel_24 *buffer, const int N);
int toLine(int i, int j, int width);
int entryBuffer(image_24 img, pixel_24* &buffer, int i, int j, const int N);
void medianFilter24BMP(image_24 &img, const int N, const int n);
void medianFilter24BMP(image_24 &img, const int N);
void medianFilter16BMP(image_16 &img, const int N);
int entryBuffer16(image_16 img, pixel_16* &buffer, int i, int j, const int N);
pixel_16 middleColor16(pixel_16 *buffer, const int N, const int n);

void monochrome(form &img, const int k);
void gamma(form &img, double k);
int medianFilter(form &img, const int n);
void negative(form &img);
int writeBMP(form &img, FILE *f);
bool bufferSize(int size);

int convertTo24(image_16 &img16, image_24 &img24);
void freePixmap(form img);
