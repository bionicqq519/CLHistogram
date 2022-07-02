//
// Created by user on 2020/6/30.
//

#ifndef ALAILDCV2_CL_BUFFEROBJECT_H
#define ALAILDCV2_CL_BUFFEROBJECT_H

#include "CL_PlatformDef.h"

class CL_BufferObject {

public:

    /// Create buffer object directly
    CL_BufferObject(
            cl_context context,
            cl_mem_flags mem_flags,
            int buffer_size);

    /// Create buffer object directly
    CL_BufferObject(
        cl_context context,
        cl_mem_flags mem_flags,
        int buffer_size,
        void *pBuffer);

    /// Create image object that uses ION buffer
    CL_BufferObject(
            cl_context context,
            cl_mem_flags mem_flags,
            int buffer_size,
            int d_filedesc,
            void *p_host_ptr);

    ~CL_BufferObject();

    void WriteBuffer(cl_command_queue commandQueue, unsigned char *pBuffer, int buffer_size);

    void ReadBuffer(cl_command_queue commandQueue, unsigned char *pBuffer, int buffer_size);

    void* MapBuffer(cl_command_queue commandQueue, int buffer_size);
    int UnMapBuffer(cl_command_queue commandQueue, void *a_pHandle);

    cl_mem m_cl_mem_obj;


};


#endif //ALAILDCV2_CL_BUFFEROBJECT_H
