#include <stdio.h>
#include <iostream>
#include <sys/stat.h>
#include "debug_tools.h"

bool ReadRAW(void* a_pBuf, const char* a_pcPFN, int a_dSize)
{
	FILE* fp;
	if ((fp = fopen(a_pcPFN, "rb")) != NULL)
	{
		fread(a_pBuf, sizeof(unsigned char), a_dSize, fp);
		fclose(fp);
		struct stat t;
		stat(a_pcPFN, &t);

		printf("[Read-size: %d] [Real-size: %d] [%s]\n", a_dSize, (int)t.st_size, a_pcPFN);
		return true;
	}
	else
	{
		printf("-Read-raw Fail.-%s-\n", a_pcPFN);
		return false;
	}
}

bool WriteRAW(void* a_pBuf, const char* a_pcPFN, int a_dSize)
{
	FILE* fp;
	if ((fp = fopen(a_pcPFN, "wb")) != NULL)
	{
		fwrite(a_pBuf, sizeof(unsigned char), a_dSize, fp);
		fclose(fp);

		printf("[Write-size: %d] [%s]\n", a_dSize, a_pcPFN);
		return true;
	}
	else
	{
		printf("-Write-raw Fail.-%s-\n", a_pcPFN);
		return false;
	}
}

static bool CCSaveBMPFile(char *a_strFileName, unsigned char *a_pucImageBuf, unsigned short a_uwImageWidth, unsigned short a_uwImageHeight)
{
	unsigned long udWriteSize = a_uwImageWidth * 3;
	unsigned long mod = udWriteSize % 4;
	if (mod != 0)
		mod = 4 - mod;

	BITMAPFILEHEADER bmfh;
	bmfh.bfType = 0x4d42;//'MB';
	bmfh.bfSize = 54 + (udWriteSize + mod)*a_uwImageHeight;
	bmfh.bfReserved1 = 0;
	bmfh.bfReserved2 = 0;
	bmfh.bfOffBits = 54;

	BITMAPINFOHEADER bmih;
	bmih.biSize = 40;
	bmih.biWidth = a_uwImageWidth;
	bmih.biHeight = a_uwImageHeight;
	bmih.biPlanes = 1;
	bmih.biBitCount = 24;
	bmih.biCompression = 0;
	bmih.biSizeImage = (udWriteSize + mod)*a_uwImageHeight;
	bmih.biXPelsPerMeter = 2834;
	bmih.biYPelsPerMeter = 2834;
	bmih.biClrUsed = 0;
	bmih.biClrImportant = 0;

	FILE* fp = fopen(a_strFileName, "wb");
	if (fp == NULL)
		return false;

	fwrite(&bmfh.bfType, sizeof(bmfh.bfType), 1, fp);
	fwrite(&bmfh.bfSize, sizeof(bmfh.bfSize), 1, fp);
	fwrite(&bmfh.bfReserved1, sizeof(bmfh.bfReserved1), 1, fp);
	fwrite(&bmfh.bfReserved2, sizeof(bmfh.bfReserved2), 1, fp);
	fwrite(&bmfh.bfOffBits, sizeof(bmfh.bfOffBits), 1, fp);
	fwrite(&bmih, sizeof(BITMAPINFOHEADER), 1, fp);

	unsigned short i, j;
	//convert rgb to bgr
	unsigned char temp;
	unsigned char *pucSrcImageBuf = a_pucImageBuf;
	for (i = 0; i<a_uwImageHeight; i++)
	{
		for (j = 0; j<a_uwImageWidth; j++)
		{
			temp = (*pucSrcImageBuf);
			*(pucSrcImageBuf) = *(pucSrcImageBuf + 2);
			*(pucSrcImageBuf + 2) = temp;
			pucSrcImageBuf += 3;
		}
	}

	pucSrcImageBuf = a_pucImageBuf + (a_uwImageHeight - 1)*udWriteSize;
	for (i = 0; i<a_uwImageHeight; i++)
	{
		fwrite(pucSrcImageBuf, sizeof(unsigned char), udWriteSize + mod, fp);
		pucSrcImageBuf -= udWriteSize;
	}

	fclose(fp);

	return true;
}


static double TOOL_SimilarityEstimate(void *a_pBufSrc1, void *a_pBufSrc2, int a_dBufW, int a_dBufH, int a_dUnitSize, int a_dTolerance)
{
	int dCount = 0;
	int dCountAll = 0;
	int i, j;
	if (a_dUnitSize == 1)
	{
		char *pSrcTmp = (char*)a_pBufSrc1;
		char *pDstTmp = (char*)a_pBufSrc2;
		for (i = 0; i<a_dBufH; i++)
		{
			for (j = 0; j<a_dBufW; j++)
			{
				int dPos = i*a_dBufW + j;
				if (abs(pSrcTmp[dPos] - pDstTmp[dPos]) <= a_dTolerance)
					dCount++;
				dCountAll++;
			}
		}
	}
	if (a_dUnitSize == 2)
	{
		short *pSrcTmp = (short*)a_pBufSrc1;
		short *pDstTmp = (short*)a_pBufSrc2;
		for (i = 0; i<a_dBufH; i++)
		{
			for (j = 0; j<a_dBufW; j++)
			{
				int dPos = i*a_dBufW + j;
				if (abs(pSrcTmp[dPos] - pDstTmp[dPos]) <= a_dTolerance)
					dCount++;
				//else
				//	LOGD("[Tank](%d  %d)%d  %d", i, j, pSrcTmp[dPos], pDstTmp[dPos]);
				dCountAll++;
			}
		}
	}
	if (a_dUnitSize == 4)
	{
		int *pSrcTmp = (int*)a_pBufSrc1;
		int *pDstTmp = (int*)a_pBufSrc2;
		for (i = 0; i<a_dBufH; i++)
		{
			for (j = 0; j<a_dBufW; j++)
			{
				int dPos = i*a_dBufW + j;
				if (abs(pSrcTmp[dPos] - pDstTmp[dPos]) <= a_dTolerance)
					dCount++;
				dCountAll++;
			}
		}
	}
	return (double)dCount*100.0f / dCountAll;
}


void TOOL_SimilarityShow(void *a_pBufSrc, const char *a_pcFileName, int a_dBufW, int a_dBufH, int a_dUnitSize, int a_dWriteBMP)
{
	char *pucVerifyBuf = new char[a_dBufW*a_dBufH*a_dUnitSize];
	if (!ReadRAW(pucVerifyBuf, a_pcFileName, a_dBufW*a_dBufH*a_dUnitSize))
	{
		printf("*AJ* Similarity-%s: 0.00000(%%)(File not found.)", a_pcFileName);
		return;
	}

	if (a_dWriteBMP != 0)
	{
		char acPathFileNameBMP[1024];
		sprintf(acPathFileNameBMP, "%s.PC.bmp", a_pcFileName);
		CCSaveBMPFile(acPathFileNameBMP, (unsigned char*)(void*)pucVerifyBuf, a_dBufW*a_dUnitSize, a_dBufH);
		sprintf(acPathFileNameBMP, "%s.Android.bmp", a_pcFileName);
		CCSaveBMPFile(acPathFileNameBMP, (unsigned char*)a_pBufSrc, a_dBufW*a_dUnitSize, a_dBufH);
	}

	double eSimilarity0 = TOOL_SimilarityEstimate(a_pBufSrc, (void*)pucVerifyBuf, a_dBufW, a_dBufH, a_dUnitSize, 0);
	double eSimilarity1 = TOOL_SimilarityEstimate(a_pBufSrc, (void*)pucVerifyBuf, a_dBufW, a_dBufH, a_dUnitSize, 1);
	double eSimilarity2 = TOOL_SimilarityEstimate(a_pBufSrc, (void*)pucVerifyBuf, a_dBufW, a_dBufH, a_dUnitSize, 2);

	delete[] pucVerifyBuf;

	if (eSimilarity2<99.0f) {
		printf("*AJ* Similarity-%s: %.5f(%%) %.5f(%%) %.5f(%%) (Bad)\n", a_pcFileName, (float)eSimilarity0, (float)eSimilarity1, (float)eSimilarity2);
	}
	else if (eSimilarity2 >= 99.0f && eSimilarity2<99.9) {
		printf("*AJ* Similarity-%s: %.5f(%%) %.5f(%%) %.5f(%%) (Fine)\n", a_pcFileName, (float)eSimilarity0, (float)eSimilarity1, (float)eSimilarity2);
	}
	else {
		printf("*AJ* Similarity-%s: %.5f(%%) %.5f(%%) %.5f(%%) (Good)\n", a_pcFileName, (float)eSimilarity0, (float)eSimilarity1, (float)eSimilarity2);
	}
}