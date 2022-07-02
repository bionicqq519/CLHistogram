//
// Created by user on 2019/5/23.
//

#include "CL_Program.h"

#define LOG_TAG _LOG_TAG_NAME "CL_Program.cpp"


CL_Program::CL_Program(
        cl_context a_clContext, cl_device_id a_clDevice,
        char* a_sData,
        string *kernel_name, int kernel_num)
{
    cl_int errNum = CL_SUCCESS;

    if (kernel_num == 0)
        kernel_num = 1;
    m_kernel_num = kernel_num;
    m_kernel_name = new string[m_kernel_num];
    //memcpy(m_kernel_name, kernel_name, sizeof(string)*m_kernel_num);
    for (int i = 0; i < m_kernel_num; i++)
    {
        m_kernel_name[i] = kernel_name[i];
    }

    LOG_INFO("[CL_Program] CreateProgram ++\n");
    const char* ProgrmaCode = a_sData;//Decript_entry(a_sData);
    m_program = clCreateProgramWithSource(
            a_clContext,
            1,
            (const char**)&ProgrmaCode,
            nullptr,
            &errNum);
    CL_CHECK(errNum, "clCreateProgramWithSource");

    errNum = clBuildProgram(m_program, 0, nullptr, nullptr, nullptr, nullptr);
    CL_CHECK(errNum, "clBuildProgram");

    if (errNum != CL_SUCCESS)
    {
        // Determine the reason for the error
        char buildLog[16384];
        clGetProgramBuildInfo(
                m_program,
                a_clDevice,
                CL_PROGRAM_BUILD_LOG,
                sizeof(buildLog),
                buildLog,
                nullptr);

        LOG_DEBUG("[CL_Program] Error in kernel: %s", buildLog);
        errNum = clReleaseProgram(m_program);
        CL_CHECK(errNum, "clReleaseProgram");
    }
    LOG_INFO("[CL_Program] CreateProgram --\n");


    LOG_INFO("[CL_Program] CreateKernel ++\n");
    CL_GenKernel();
    LOG_INFO("[CL_Program] CreateKernel --\n");
}


CL_Program::~CL_Program()
{
    cl_int errNum = clReleaseProgram(m_program);
    CL_CHECK(errNum, "clReleaseProgram");

    CL_RleaseKernel();

    if (m_kernel_name!= nullptr)
    {
        delete[] m_kernel_name;
        m_kernel_name = nullptr;
    }

    m_kernel_num = 0;
}


void CL_Program::CL_GenKernel()
{
    cl_int errNum = CL_SUCCESS;
    string tmp_str;

    m_kernel = new cl_kernel[m_kernel_num];

    for (int i = 0; i <m_kernel_num ; i++)
    {
        m_kernel[i] = clCreateKernel(m_program, m_kernel_name[i].c_str(), &errNum);
        tmp_str = "clCreateKernel"+ m_kernel_name[i];
        CL_CHECK(errNum, tmp_str.c_str());
    }
}


void CL_Program::CL_RleaseKernel()
{
    string tmp_str;
    if(m_kernel!= nullptr)
    {
        for (int i = 0; i < m_kernel_num; i++)
        {
            if (m_kernel[i]!= nullptr)
            {
                tmp_str = "clReleaseKernel" + m_kernel_name[i];
                CL_CHECK(clReleaseKernel(m_kernel[i]), tmp_str.c_str());
            }
        }
    }
}