//
// Created by user on 2019/5/23.
//

#ifndef OPENGL_CL_TEST_CL_PROGRAM_H
#define OPENGL_CL_TEST_CL_PROGRAM_H

#include "CL_PlatformDef.h"
//#include "../Tool/Dec.h"

using namespace std;

class CL_Program//: public DecriptShader
{

public:
    CL_Program(
            cl_context a_clContext, cl_device_id a_clDevice,
            char* a_sData,
            string *kernel_name, int kernel_num);
    ~CL_Program();

    cl_kernel *m_kernel;
    string *m_kernel_name;
    int m_kernel_num;

private:
    cl_program m_program;

    void CL_GenKernel();
    void CL_RleaseKernel();

};


#endif //OPENGL_CL_TEST_CL_PROGRAM_H
