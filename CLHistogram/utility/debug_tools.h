#pragma once
bool WriteRAW(void* a_pBuf, const char* a_pcPFN, int a_dSize);
bool ReadRAW(void* a_pBuf, const char* a_pcPFN, int a_dSize);

void TOOL_SimilarityShow(void *a_pBufSrc, const char *a_pcFileName, int a_dBufW, int a_dBufH, int a_dUnitSize, int a_dWriteBMP);

struct BITMAPINFOHEADER { // bmih
	unsigned long biSize;
	long   biWidth;
	long   biHeight;
	unsigned short biPlanes;
	unsigned short   biBitCount;
	unsigned long  biCompression;
	unsigned long  biSizeImage;
	long   biXPelsPerMeter;
	long   biYPelsPerMeter;
	unsigned long  biClrUsed;
	unsigned long  biClrImportant;
};

struct BITMAPFILEHEADER { // bmfh
	unsigned short    bfType;
	unsigned int      bfSize;
	unsigned short    bfReserved1;
	unsigned short    bfReserved2;
	unsigned int      bfOffBits;
};