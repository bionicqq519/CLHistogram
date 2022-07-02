#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/stat.h>
#include "utility/debug_tools.h"
#include "CL_Helpers/CL_Resource.h"
#include "inc/mtype.h"

#include "CL_ALG/Histogram_CL.h"

#define USE_OPENCL

static CL_Resource* g_p_cl_resource = nullptr;
static CHistogram_CL* g_p_cl_his = nullptr;

int CL_DeInit()
{
	if (g_p_cl_his != nullptr)
		delete g_p_cl_his;

#ifdef USE_OPENCL
	if (g_p_cl_resource != nullptr)
		delete g_p_cl_resource;
#endif
	return 0;
}

int CL_Init()
{
#ifdef USE_OPENCL
	if (g_p_cl_resource != nullptr)
		delete g_p_cl_resource;
	g_p_cl_resource = new CL_Resource();
#endif 

	if (g_p_cl_his != nullptr)
		delete g_p_cl_his;
	g_p_cl_his = new CHistogram_CL(g_p_cl_resource);

	return 0;
}

//int main(int argc, char *argv[])
int main(void)
{
#if 1
	printf("CLHistogram start\n");
	CL_Init();

	UINT16 uwWidth = 1280;
	UINT16 uwHeight = 720;
	unsigned char* pucInImg = (unsigned char*)calloc(uwWidth * uwHeight * 3 / 2, sizeof(unsigned char));//(unsigned char*)calloc(uwWidth * uwHeight * 3/2, sizeof(unsigned char));
	char fname_Image[256];
	sprintf(fname_Image, "./testcase/lena_1280x720.nv12");	//lena_1280x720
	ReadRAW(pucInImg, fname_Image, uwWidth * uwHeight * 1.5);

	int* pdOutHis = (int*)calloc(256, sizeof(int));

	unsigned int t1 = clock();

	//algorithm start
#if 1
	#if 0
	for (int i = 0; i < 3600*256; i++)
	{
		pdOutHis[pucInImg[i]]++;
	}
	#else
	for (int i = 0; i < uwHeight; i++)
	{
		for (int j = 0; j < uwWidth; j++)
		{
			pdOutHis[pucInImg[i * uwWidth + j]]++;
		}
	}
    #endif
#else
	g_p_cl_his->Histogram_ALLOC(uwWidth, uwHeight);

	g_p_cl_his->Histogram_CL(pdOutHis, pucInImg, uwWidth, uwHeight);

	g_p_cl_his->Histogram_Release();
#endif
	//algorithm end

	printf("algorithm = %d\n", clock() - t1); t1 = clock();

	FILE* fp;
	fp = fopen("./testcase/HisBin.txt", "wb");
	for (int i = 0; i < 256; i++)
	{
		fprintf(fp, "%d bin = %d\n", i, pdOutHis[i]);
		printf("%d bin = %d\n", i, pdOutHis[i]);
	}
	fclose(fp);

	free(pucInImg);
	free(pdOutHis);

	CL_DeInit();
	printf("CLHistogram end\n");
#endif
	system("pause");
	return 0;
}

//class alNN
//{
//public:
//	void Init();
//	void DeInit();
//};
//
//void alNN::Init()
//{
//	printf("Init\n");
//}
//
//void alNN::DeInit()
//{
//	printf("DeInit\n");
//}
//
//typedef struct {
//	alNN** alNN_handle;
//}AI_REMOSAIC_handle;
//
//
//void testDeInit(AI_REMOSAIC_handle* handle)
//{
//	free(handle->alNN_handle);// = (alNN**)malloc(1 * sizeof(alNN**));
//	return;
//}
//
//void testInit(AI_REMOSAIC_handle *handle)
//{
//	handle->alNN_handle = (alNN**)malloc(1 * sizeof(alNN*));
//
//	handle->alNN_handle[0]->Init();
//
//	return;
//}
//int main(void)
//{
//	 AI_REMOSAIC_handle handle;
//	 testInit(&handle);
//	 handle.alNN_handle[0]->DeInit();
//	 testDeInit(&handle);
//	system("pause");
//	return 0;
//}