#pragma once
#include "../CL_Helpers/CL_Resource.h"
#include "../CL_Helpers/CL_Program.h"
#include "../CL_Helpers/CL_BufferObject.h"

typedef enum {
	_kernel_Histogram_CL = 0,
	_kernel_num = 1
} _enum_program_ASharpness;

class CHistogram_CL
{
public:
	CHistogram_CL(CL_Resource* a_p_cl_resource);
	~CHistogram_CL();

	int Histogram_ALLOC(int a_dWidth, int a_dHeight);
	int Histogram_Release();

	int Histogram_CL(int* pdOutImg, unsigned char* pucInImg, int a_dWidth, int a_dHeight);

	int SetLocalWorkSize(int a_dInStridW, int a_dInStridH);
private:
	CL_Resource* m_p_cl_resource;
	CL_Program* m_p_prog_his;

	CL_BufferObject* m_pucInImg;
	CL_BufferObject* m_pdOutHis;

	int local_worksize;
};