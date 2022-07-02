//
// Created by user on 2020/6/30.
//

#include "CL_BufferObject.h"

#define LOG_TAG _LOG_TAG_NAME "CL_BufferObject.cpp "

CL_BufferObject::CL_BufferObject(
        cl_context context,
        cl_mem_flags mem_flags,
        int buffer_size)
{
    cl_int errNum = CL_SUCCESS;
    m_cl_mem_obj = clCreateBuffer(
            context,
            mem_flags,
            buffer_size, nullptr, &errNum);
    CL_CHECK(errNum, "clCreateBuffer");
}

CL_BufferObject::CL_BufferObject(
    cl_context context,
    cl_mem_flags mem_flags,
    int buffer_size,
    void* pBuffer)
{
    cl_int errNum = CL_SUCCESS;
    m_cl_mem_obj = clCreateBuffer(
        context,
        mem_flags,
        buffer_size, pBuffer, &errNum);
    CL_CHECK(errNum, "clCreateBuffer");
}

CL_BufferObject::CL_BufferObject(
        cl_context context,
        cl_mem_flags mem_flags,
        int buffer_size,
        int d_filedesc,
        void *p_host_ptr)
{
    cl_mem_ion_host_ptr ion_host_ptr = { 0 };
    ion_host_ptr.ext_host_ptr.allocation_type = CL_MEM_ION_HOST_PTR_QCOM;
    ion_host_ptr.ext_host_ptr.host_cache_policy = CL_MEM_HOST_UNCACHED_QCOM;
    //ion_host_ptr.ext_host_ptr.host_cache_policy = CL_MEM_HOST_WRITEBACK_QCOM;
    //ion_host_ptr.ext_host_ptr.host_cache_policy = CL_MEM_HOST_IOCOHERENT_QCOM;
    ion_host_ptr.ion_filedesc = d_filedesc;   //fd from rpc memory
    ion_host_ptr.ion_hostptr = p_host_ptr;  //pointer of rpc memory

    cl_int errNum = CL_SUCCESS;
    m_cl_mem_obj = clCreateBuffer(
            context,
            mem_flags | CL_MEM_USE_HOST_PTR | CL_MEM_EXT_HOST_PTR_QCOM,
            buffer_size, &ion_host_ptr, &errNum);
    CL_CHECK(errNum, "clCreateBuffer");

}


CL_BufferObject::~CL_BufferObject()
{
    if (m_cl_mem_obj != nullptr)
    {
        int errNum = clReleaseMemObject(m_cl_mem_obj);
        CL_CHECK(errNum, "clReleaseMemObject: m_cl_mem_obj");
        m_cl_mem_obj = nullptr;
    }
}


void CL_BufferObject::WriteBuffer(cl_command_queue commandQueue, unsigned char *pBuffer, int buffer_size)
{
    int errNum = clEnqueueWriteBuffer(
            commandQueue,
            m_cl_mem_obj,
            CL_TRUE,
            0,
            buffer_size,
            pBuffer,
            0,
            nullptr,
            nullptr);
    CL_CHECK(errNum, "clEnqueueWriteBuffer: pfBufMeshSrc_float2");
}

void CL_BufferObject::ReadBuffer(cl_command_queue commandQueue, unsigned char *pBuffer, int buffer_size)
{
    int errNum = clEnqueueReadBuffer(
            commandQueue,
            m_cl_mem_obj,
            CL_TRUE,
            0,
            buffer_size,
            pBuffer,
            0,
            nullptr,
            nullptr);
    CL_CHECK(errNum, "clEnqueueWriteBuffer: pfBufMeshSrc_float2");

}

void* CL_BufferObject::MapBuffer(cl_command_queue commandQueue, int buffer_size)
{
    cl_int errNum = CL_SUCCESS;

    void* cl_ToCopy = (void*)clEnqueueMapBuffer(commandQueue, m_cl_mem_obj, CL_TRUE, CL_MAP_READ | CL_MAP_WRITE, 0,
        sizeof(unsigned char) * buffer_size, 0, NULL, NULL, &errNum);
    if (errNum != CL_SUCCESS)
    {
        printf("[ALTEK_CL] Error setting clEnqueueMapBuffer : %s", getErrorString(errNum));
        //Cleanup(g_context, commandQueue);
        return NULL;
    }
    return cl_ToCopy;
}

int CL_BufferObject::UnMapBuffer(cl_command_queue commandQueue, void* a_pHandle)
{
    cl_int errNum = CL_SUCCESS;

    errNum = clEnqueueUnmapMemObject(commandQueue, m_cl_mem_obj, a_pHandle, 0, NULL, NULL);

    if (errNum != CL_SUCCESS)
    {
        printf("[ALTEK_CL] Error setting clEnqueueUnmapMemObject : %s", getErrorString(errNum));
        //Cleanup(g_context, g_commandQueue);
        return errNum;
    }
    return errNum;
}


