//
// Created by user on 2019/5/22.
//

#ifndef OPENGL_CL_TEST_CL_RESOURCE_H
#define OPENGL_CL_TEST_CL_RESOURCE_H

#include "CL_PlatformDef.h"

class CL_Resource {

public:
    CL_Resource();
    CL_Resource(void *EGL_Context, void *EGL_Display);
    ~CL_Resource();

    cl_context m_context = 0;
    cl_command_queue m_commandQueue = 0;
    cl_device_id m_device = 0;

private:
    cl_context CreateContext();
    cl_context CreateContext(void *EGL_Context, void *EGL_Display);
    cl_command_queue CreateCommandQueue(cl_context context, cl_device_id *device);
    int Clean();


};


#endif //OPENGL_CL_TEST_CL_RESOURCE_H
