//
// Created by user on 2019/7/23.
//

#ifndef ALAILDC_CL_IMAGEOBJECT_H
#define ALAILDC_CL_IMAGEOBJECT_H

#include "CL_PlatformDef.h"

class CL_ImageObject {

public:

    /// Create image object directly
    CL_ImageObject(
            int width, int height,
            cl_context context,
            cl_uint channel_order,
            cl_uint channel_data_type,
            cl_uint image_type,
            cl_mem_flags mem_flags);

    /// Create image object that uses parent planar image (ex. NV12_Y, NV12_UV)
    CL_ImageObject(
            int width, int height,
            cl_context context,
            cl_uint channel_order,
            cl_uint channel_data_type,
            cl_uint image_type,
            cl_mem_flags mem_flags,
            cl_mem parent_planar_image);

    /// Create image object that uses ION buffer
    // CL_ImageObject(
    //         int width, int height,
    //         int image_row_pitch,
    //         cl_context context,
    //         cl_uint channel_order,
    //         cl_uint channel_data_type,
    //         cl_uint image_type,
    //         cl_mem_flags mem_flags,
    //         int d_filedesc,
    //         void *p_host_ptr);

    /// Create image object from OpenGL texture
    CL_ImageObject(
            cl_context context,
            cl_mem_flags mem_flags,
            cl_GLenum target,
            cl_GLuint tex_id);

    ~CL_ImageObject();

    void WriteImage(cl_command_queue commandQueue, unsigned char *pBuffer, int width, int height);
    void ReadImage(cl_command_queue commandQueue, unsigned char *pBuffer, int width, int height);

    cl_mem m_cl_mem_obj;

private:

    cl_image_format m_clImageFormat;
    cl_image_desc m_clImageDesc;


};


#endif //ALAILDC_CL_IMAGEOBJECT_H
