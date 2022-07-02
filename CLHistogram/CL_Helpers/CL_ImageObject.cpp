//
// Created by user on 2019/7/23.
//

#include "CL_ImageObject.h"
//#include <cl_ext_qcom.h>

#define LOG_TAG _LOG_TAG_NAME "CL_ImageObject.cpp "


CL_ImageObject::CL_ImageObject(
        int width, int height,
        cl_context context,
        cl_uint channel_order,
        cl_uint channel_data_type,
        cl_uint image_type,
        cl_mem_flags mem_flags)
{
    m_clImageFormat.image_channel_order = channel_order;
    m_clImageFormat.image_channel_data_type = channel_data_type;

    m_clImageDesc.image_type = image_type;
    m_clImageDesc.image_width = width;
    m_clImageDesc.image_height = height;
    m_clImageDesc.image_row_pitch = 0;
    m_clImageDesc.image_slice_pitch = 0;
    m_clImageDesc.num_mip_levels = 0;
    m_clImageDesc.num_samples = 0;
    m_clImageDesc.buffer = nullptr;

    cl_int errNum = CL_SUCCESS;

    m_cl_mem_obj = clCreateImage(
            context,
            mem_flags,
            &m_clImageFormat,
            &m_clImageDesc,
            nullptr,
            &errNum);
    CL_CHECK(errNum, "clCreateImage");
}

CL_ImageObject::CL_ImageObject(
        int width, int height,
        cl_context context,
        cl_uint channel_order,
        cl_uint channel_data_type,
        cl_uint image_type,
        cl_mem_flags mem_flags,
        cl_mem parent_planar_image)
{
    m_clImageFormat.image_channel_order = channel_order;
    m_clImageFormat.image_channel_data_type = channel_data_type;

    m_clImageDesc.image_type = image_type;
    m_clImageDesc.image_width = width;
    m_clImageDesc.image_height = height;
    m_clImageDesc.image_row_pitch = 0;
    m_clImageDesc.image_slice_pitch = 0;
    m_clImageDesc.num_mip_levels = 0;
    m_clImageDesc.num_samples = 0;
    m_clImageDesc.buffer = nullptr;
    m_clImageDesc.mem_object = parent_planar_image;

    cl_int errNum = CL_SUCCESS;

    m_cl_mem_obj = clCreateImage(
            context,
            mem_flags,
            &m_clImageFormat,
            &m_clImageDesc,
            nullptr,
            &errNum);
    CL_CHECK(errNum, "clCreateImage");
}

//CL_ImageObject::CL_ImageObject(
//        int width, int height,
//        int image_row_pitch,
//        cl_context context,
//        cl_uint channel_order,
//        cl_uint channel_data_type,
//        cl_uint image_type,
//        cl_mem_flags mem_flags,
//        int d_filedesc,
//        void *p_host_ptr)
//{
//    m_clImageFormat.image_channel_order = channel_order;
//    m_clImageFormat.image_channel_data_type = channel_data_type;
//
//    m_clImageDesc = {0};
//    m_clImageDesc.image_type = image_type;
//    m_clImageDesc.image_width = width;
//    m_clImageDesc.image_height = height;
//    m_clImageDesc.image_row_pitch = image_row_pitch;
//    m_clImageDesc.image_slice_pitch = 0;
//    //m_clImageDesc.num_mip_levels = 0;
//    //m_clImageDesc.num_samples = 0;
//    //m_clImageDesc.buffer = nullptr;
//
//    cl_int errNum = CL_SUCCESS;
//
//    cl_mem_ion_host_ptr ion_host_ptr = { 0 };
//    ion_host_ptr.ext_host_ptr.allocation_type = CL_MEM_ION_HOST_PTR_QCOM;
//    ion_host_ptr.ext_host_ptr.host_cache_policy = CL_MEM_HOST_UNCACHED_QCOM;
//    //ion_host_ptr.ext_host_ptr.host_cache_policy = CL_MEM_HOST_WRITEBACK_QCOM;
//    //ion_host_ptr.ext_host_ptr.host_cache_policy = CL_MEM_HOST_IOCOHERENT_QCOM;
//    ion_host_ptr.ion_filedesc = d_filedesc;           //fd from rpc memory
//    ion_host_ptr.ion_hostptr = p_host_ptr;            //pointer of rpc memory
//
//    m_cl_mem_obj = clCreateImage(
//            context,
//            mem_flags | CL_MEM_USE_HOST_PTR | CL_MEM_EXT_HOST_PTR_QCOM,
//            //CL_MEM_USE_HOST_PTR | CL_MEM_EXT_HOST_PTR_QCOM,
//            &m_clImageFormat,
//            &m_clImageDesc,
//            &ion_host_ptr,
//            &errNum);
//    CL_CHECK(errNum, "clCreateImage");
//}


CL_ImageObject::CL_ImageObject(
        cl_context context,
        cl_mem_flags mem_flags,
        cl_GLenum target,
        cl_GLuint tex_id)
{
    /** Create input image memory object **/
    cl_int errNum = CL_SUCCESS;
    m_cl_mem_obj = clCreateFromGLTexture(
            context,
            mem_flags,
            target,
            0,
            tex_id,
            &errNum);
    CL_CHECK(errNum, "clCreateFromGLTexture");

}


CL_ImageObject::~CL_ImageObject()
{
    cl_int errNum = CL_SUCCESS;

    errNum = clReleaseMemObject(m_cl_mem_obj);
    CL_CHECK(errNum, "clReleaseMemObject");

}

void CL_ImageObject::WriteImage(cl_command_queue commandQueue, unsigned char *pBuffer, int width, int height)
{
    cl_int errNum = CL_SUCCESS;

    size_t origin[3] = { 0, 0, 0 };
    size_t region[3] = { (size_t)width, (size_t)height, 1 };
    errNum = clEnqueueWriteImage(
            commandQueue,                           /* command_queue */
            m_cl_mem_obj,                      /* image */
            CL_TRUE,                           /* blocking_write */
            origin,                            /* origin[3] */
            region,                            /* region[3] */
            0,                                 /* input_row_pitch */
            0,                                 /* input_slice_pitch */
            pBuffer,                           /* ptr */
            0,                                 /* num_events_in_wait_list */
            nullptr,                            /* event_wait_list */
            nullptr);                           /* event */
    CL_CHECK(errNum, "clEnqueueWriteImage");
}


void CL_ImageObject::ReadImage(cl_command_queue commandQueue, unsigned char *pBuffer, int width, int height)
{
    cl_int errNum = CL_SUCCESS;

    size_t origin[3] = { 0, 0, 0 };
    size_t region[3] = { (size_t)width, (size_t)height, 1 };
    size_t row_pitch = width;
    errNum = clEnqueueReadImage(
            commandQueue,              /* command_queue */
            m_cl_mem_obj,         /* image */
            CL_TRUE,              /* blocking_read */
            origin,               /* origin[3] */
            region,               /* region[3] */
            0,            /* row_pitch */
            0,                    /* slice_pitch */
            pBuffer,              /* ptr */
            0,                    /* num_events_in_wait_list */
            nullptr,               /* event_wait_list */
            nullptr);              /* event */

    CL_CHECK(errNum, "clEnqueueReadImage");
}


