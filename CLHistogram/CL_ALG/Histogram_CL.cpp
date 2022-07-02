
#include "Histogram_CL.h"
#include "../CL_ALG/Histogram_CLKernel.h"

CHistogram_CL::CHistogram_CL(CL_Resource* a_p_cl_resource)
{
	m_p_cl_resource = a_p_cl_resource;

	m_p_prog_his = nullptr;

	string kernel_name[_kernel_num] = { "Histogram_CL" };
	m_p_prog_his = new CL_Program(
		a_p_cl_resource->m_context,
		a_p_cl_resource->m_device,
		(char*)sHis_CL_KernelCode,
		kernel_name,
		_kernel_num);

	m_pucInImg = nullptr;
	m_pdOutHis = nullptr;
}

CHistogram_CL::~CHistogram_CL()
{
	m_pucInImg = nullptr;
	m_pdOutHis = nullptr;
}

int CHistogram_CL::SetLocalWorkSize(int a_dInStridW, int a_dInStridH)
{
	cl_int errcode_ret = CL_SUCCESS;

	local_worksize = 16;
	while (!(a_dInStridW % local_worksize == 0 && a_dInStridH % local_worksize == 0))
	{
		local_worksize /= 2;
	}
	// LOG_INFO("[SetLocalWorkSize] local_worksize = %d", local_worksize);
	LOG_INFO("[CASharpness_CL] local_worksize = %d\n", local_worksize);

	return errcode_ret;
}

int CHistogram_CL::Histogram_ALLOC(int a_dWidth, int a_dHeight)
{
	cl_int errcode_ret = CL_SUCCESS;

	if (m_pucInImg != nullptr)
		delete m_pucInImg;
	m_pucInImg = new CL_BufferObject(
		m_p_cl_resource->m_context,
		CL_MEM_READ_WRITE,
		a_dWidth * a_dHeight * sizeof(unsigned char));
	
	if (m_pdOutHis != nullptr)
		delete m_pdOutHis;
	m_pdOutHis = new CL_BufferObject(
		m_p_cl_resource->m_context,
		CL_MEM_READ_WRITE,
		a_dWidth * a_dHeight * sizeof(int));

	return errcode_ret;
}

int CHistogram_CL::Histogram_Release()
{
	cl_int errcode_ret = CL_SUCCESS;

	if (m_pucInImg != nullptr)
	{
		delete m_pucInImg;
		m_pucInImg = nullptr;
	}

	if (m_pdOutHis != nullptr)
	{
		delete m_pdOutHis;
		m_pdOutHis = nullptr;
	}

	return errcode_ret;
}

#define ELEMENTS_TO_PROCESS 256

int CHistogram_CL::Histogram_CL(int* pdOutImg, unsigned char* pucInImg, int a_dWidth, int a_dHeight)
{
	cl_int errNum = CL_SUCCESS;

	m_pucInImg->WriteBuffer(m_p_cl_resource->m_commandQueue,
		(unsigned char*)pucInImg,
		a_dWidth * a_dHeight * sizeof(unsigned char));

	errNum = clSetKernelArg(m_p_prog_his->m_kernel[_kernel_Histogram_CL], 0,
		sizeof(cl_mem), &m_pdOutHis->m_cl_mem_obj);
	CL_CHECK(errNum, "clSetKernelArg: m_pdOutHis");
	errNum = clSetKernelArg(m_p_prog_his->m_kernel[_kernel_Histogram_CL], 1,
		sizeof(cl_mem), &m_pucInImg->m_cl_mem_obj);
	CL_CHECK(errNum, "clSetKernelArg: m_pucInImg");
	errNum = clSetKernelArg(m_p_prog_his->m_kernel[_kernel_Histogram_CL], 2,
		sizeof(int), &a_dWidth);
	CL_CHECK(errNum, "clSetKernelArg: a_dWidth");
	errNum = clSetKernelArg(m_p_prog_his->m_kernel[_kernel_Histogram_CL], 3,
		sizeof(int), &a_dHeight);
	CL_CHECK(errNum, "clSetKernelArg: a_dHeight");

	int ndRangeLen = a_dWidth * a_dHeight / ELEMENTS_TO_PROCESS;

	ndRangeLen = ((ndRangeLen % 256) == 0) ? ndRangeLen : (ndRangeLen/256 + 1) * 256;

	size_t globalWorkSize[2] = { (size_t)(ndRangeLen)};
	size_t localWorkSize[2] = { 256 };

	errNum = clEnqueueNDRangeKernel(
		m_p_cl_resource->m_commandQueue,
		m_p_prog_his->m_kernel[_kernel_Histogram_CL],
		1,
		nullptr,
		globalWorkSize,
		localWorkSize,
		0,
		nullptr,
		nullptr);
	CL_CHECK(errNum, "clEnqueueNDRangeKernel");

	m_pdOutHis->ReadBuffer(m_p_cl_resource->m_commandQueue, (unsigned char *)pdOutImg, 256 * sizeof(int));

	clFinish(m_p_cl_resource->m_commandQueue);

	return errNum;
}