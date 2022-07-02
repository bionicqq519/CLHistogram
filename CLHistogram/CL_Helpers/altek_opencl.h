/****************************************************************************
*                                                                           *
*       This software is confidential and proprietary and may be used       *
*        only as expressly authorized by a licensing agreement from         *
*                                                                           *
*                            Altek Semiconductor                            *
*                                                                           *
*                   (C) COPYRIGHT 2014 ALTEK SEMICONDUCTOR                  *
*                            ALL RIGHTS RESERVED                            *
*                                                                           *
*                 The entire notice above must be reproduced                *
*                  on all copies and should not be removed.                 *
*                                                                           *
*****************************************************************************

**************************************************
* \file     ALTEK_opencl.h                        *
* \brief    header file of altek_opencl           *
**************************************************/
#ifndef __ALTEK_OPENCL_H__
#define __ALTEK_OPENCL_H__

/****************************************************************************
*                               Include File                                *
****************************************************************************/
//#include "mtype.h"
#include "CL/cl.h"
#include <CL/cl_ext.h>

/****************************************************************************
*                         Public Function Prototype                         *
****************************************************************************/

const char *getErrorString(cl_int error);

// Create Context and CommandQueue
int OpenCL_Init();

// Create Program from kernel code
cl_program OpenCL_CreateProgram(const char *a_sCL_Kernel_code);

int OpenCL_DeInit();

cl_context OpenCL_GetCurrentContext();
cl_command_queue OpenCL_GetCurrentCommandQueue();
cl_device_id OpenCL_GetCurrentDevice();

void* OpenCL_Map(cl_mem a_clMemObj, int a_dWidth, int a_dHeight);

int OpenCL_UnMap(cl_mem a_clMemObj, void *a_pHandle);

void* OpenCL_Instance_Map_2D_Image(unsigned short *a_puwOutRowPitch, cl_mem a_clMemObj, int a_dWidth, int a_dHeight);

#endif
