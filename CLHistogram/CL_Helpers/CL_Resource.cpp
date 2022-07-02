//
// Created by user on 2019/5/22.
//

#include "CL_Resource.h"
#include "stdio.h"

#define LOG_TAG _LOG_TAG_NAME "CL_Resource.cpp"

CL_Resource::CL_Resource()
{
    LOG_DEBUG("[CL_Resource] Create OpenCL resource ++\n");

    // Create an OpenCL context on first available platform
    m_context = CreateContext();
    if (m_context == nullptr)
    {
        LOG_ERROR("[CL_Resource] Failed to create OpenCL context.\n");
        return;
    }

    // Create a command-queue on the first device available
    // on the created context
    m_commandQueue = CreateCommandQueue(m_context, &m_device);
    if (m_commandQueue == NULL)
    {
        LOG_ERROR("[CL_Resource] Failed to commandQueue.\n");
        Clean();
        return;
    }

    LOG_DEBUG("[CL_Resource] Create OpenCL resource --\n");
}


CL_Resource::CL_Resource(void *EGL_Context, void *EGL_Display)
{
    LOG_DEBUG("[CL_Resource] Create OpenCL resource ++\n");

    // Create an OpenCL context on first available platform
    m_context = CreateContext(EGL_Context, EGL_Display);
    if (m_context == nullptr)
    {
        LOG_ERROR("[CL_Resource] Failed to create OpenCL context.\n");
        return;
    }

    // Create a command-queue on the first device available
    // on the created context
    m_commandQueue = CreateCommandQueue(m_context, &m_device);
    if (m_commandQueue == NULL)
    {
        LOG_ERROR("[CL_Resource] Failed to commandQueue.\n");
        Clean();
        return;
    }

    LOG_DEBUG("[CL_Resource] Create OpenCL resource --\n");

}


CL_Resource::~CL_Resource()
{
    Clean();
}


cl_context CL_Resource::CreateContext()
{
    cl_int errNum = CL_SUCCESS;
    cl_uint numPlatforms = 0;
    cl_platform_id PlatformId[10];
    cl_context context = NULL;

    // First, select an OpenCL platform to run on.  For this example, we
    // simply choose the first available platform.  Normally, you would
    // query for all available platforms and select the most appropriate one.
    errNum = clGetPlatformIDs(1, PlatformId, &numPlatforms);
    LOG_INFO("[CreateContext] numPlatforms = %d, firstPlatformId = %d\n", numPlatforms, PlatformId[0] );
    if (errNum != CL_SUCCESS || numPlatforms <= 0)
    {
        LOG_ERROR("[CreateContext] Failed to find any OpenCL platforms ; %s\n", getErrorString(errNum));
        return nullptr;
    }
    LOG_INFO("[CreateContext] numPlatforms = %d\n", numPlatforms);

    // list platform 0 only
    numPlatforms = (numPlatforms > 1) ? 1 : 0;
    for (int i=0; i<numPlatforms; i++)
    {
        char buffer[10240] = {0};
        LOG_INFO("[CreateContext] -- %d --\n", i);
        clGetPlatformInfo(PlatformId[i], CL_PLATFORM_PROFILE, 10240, buffer, NULL);
        LOG_INFO("[CreateContext] OpenCL PROFILE = %s\n", buffer);
        clGetPlatformInfo(PlatformId[i], CL_PLATFORM_VERSION, 10240, buffer, NULL);
        LOG_INFO("[CreateContext] OpenCL VERSION = %s\n", buffer);
        clGetPlatformInfo(PlatformId[i], CL_PLATFORM_NAME, 10240, buffer, NULL);
        LOG_INFO("[CreateContext] OpenCL NAME = %s\n", buffer);
        clGetPlatformInfo(PlatformId[i], CL_PLATFORM_VENDOR, 10240, buffer, NULL);
        LOG_INFO("[CreateContext] OpenCL VENDOR = %s\n", buffer);
        clGetPlatformInfo(PlatformId[i], CL_PLATFORM_EXTENSIONS, 10240, buffer, NULL);
        LOG_INFO("[CreateContext] OpenCL EXTENSIONS = %s\n", buffer);
    }

    cl_device_id devices[100];
    cl_uint devices_n = 0;
    clGetDeviceIDs(PlatformId[0], CL_DEVICE_TYPE_GPU, 100, devices, &devices_n);

    for (int i=0; i<devices_n; i++)
    {
        char buffer[10240] = { 0 };
        cl_uint buf_uint;
        cl_ulong buf_ulong;
        LOG_INFO("[CreateContext]   GPU -- %d --\n", i);
        clGetDeviceInfo(devices[i], CL_DEVICE_NAME, sizeof(buffer), buffer, NULL);
        LOG_INFO("[CreateContext] GPU DEVICE_NAME = %s\n", buffer);
        clGetDeviceInfo(devices[i], CL_DEVICE_VENDOR, sizeof(buffer), buffer, NULL);
        LOG_INFO("[CreateContext] GPU DEVICE_VENDOR = %s\n", buffer);
        clGetDeviceInfo(devices[i], CL_DEVICE_VERSION, sizeof(buffer), buffer, NULL);
        LOG_INFO("[CreateContext] GPU DEVICE_VERSION = %s\n", buffer);
        clGetDeviceInfo(devices[i], CL_DRIVER_VERSION, sizeof(buffer), buffer, NULL);
        LOG_INFO("[CreateContext] GPU DRIVER_VERSION = %s\n", buffer);
        clGetDeviceInfo(devices[i], CL_DEVICE_MAX_COMPUTE_UNITS, sizeof(buf_uint), &buf_uint, NULL);
        LOG_INFO("[CreateContext] GPU DEVICE_MAX_COMPUTE_UNITS = %u\n", (unsigned int)buf_uint);
        clGetDeviceInfo(devices[i], CL_DEVICE_MAX_CLOCK_FREQUENCY, sizeof(buf_uint), &buf_uint, NULL);
        LOG_INFO("[CreateContext] GPU DEVICE_MAX_CLOCK_FREQUENCY = %u\n", (unsigned int)buf_uint);
        clGetDeviceInfo(devices[i], CL_DEVICE_MAX_WORK_GROUP_SIZE, sizeof(buf_uint), &buf_uint, NULL);
        LOG_INFO("[CreateContext] GPU DEVICE_MAX_WORK_GROUP_SIZE = %u\n", (unsigned int)buf_uint);
        clGetDeviceInfo(devices[i], CL_DEVICE_MAX_WORK_ITEM_SIZES, sizeof(buf_uint), &buf_uint, NULL);
        LOG_INFO("[CreateContext] GPU DEVICE_MAX_WORK_ITEM_SIZE = %u\n", (unsigned int)buf_uint);
        clGetDeviceInfo(devices[i], CL_DEVICE_GLOBAL_MEM_SIZE, sizeof(buf_ulong), &buf_ulong, NULL);
        LOG_INFO("[CreateContext] GPU DEVICE_GLOBAL_MEM_SIZE = %llu\n", (unsigned long long)buf_ulong);
        clGetDeviceInfo(devices[i], CL_DEVICE_LOCAL_MEM_SIZE, sizeof(buf_ulong), &buf_ulong, NULL);
        LOG_INFO("[CreateContext] GPU DEVICE_LOCAL_MEM_SIZE = %llu\n", (unsigned long long)buf_ulong);
    }

    clGetDeviceIDs(PlatformId[0], CL_DEVICE_TYPE_CPU, 100, devices, &devices_n);

    for (int i=0; i<devices_n; i++)
    {
        char buffer[10240] = { 0 };
        cl_uint buf_uint;
        cl_ulong buf_ulong;
        LOG_INFO("[CreateContext] CPU -- %d --\n", i);
        clGetDeviceInfo(devices[i], CL_DEVICE_NAME, sizeof(buffer), buffer, NULL);
        LOG_INFO("[CreateContext] CPU DEVICE_NAME = %s\n", buffer);
        clGetDeviceInfo(devices[i], CL_DEVICE_VENDOR, sizeof(buffer), buffer, NULL);
        LOG_INFO("[CreateContext] CPU DEVICE_VENDOR = %s\n", buffer);
        clGetDeviceInfo(devices[i], CL_DEVICE_VERSION, sizeof(buffer), buffer, NULL);
        LOG_INFO("[CreateContext] CPU DEVICE_VERSION = %s\n", buffer);
        clGetDeviceInfo(devices[i], CL_DRIVER_VERSION, sizeof(buffer), buffer, NULL);
        LOG_INFO("[CreateContext] CPU DRIVER_VERSION = %s\n", buffer);
        clGetDeviceInfo(devices[i], CL_DEVICE_MAX_COMPUTE_UNITS, sizeof(buf_uint), &buf_uint, NULL);
        LOG_INFO("[CreateContext] CPU DEVICE_MAX_COMPUTE_UNITS = %u\n", (unsigned int)buf_uint);
        clGetDeviceInfo(devices[i], CL_DEVICE_MAX_CLOCK_FREQUENCY, sizeof(buf_uint), &buf_uint, NULL);
        LOG_INFO("[CreateContext] CPU DEVICE_MAX_CLOCK_FREQUENCY = %u\n", (unsigned int)buf_uint);
        clGetDeviceInfo(devices[i], CL_DEVICE_MAX_WORK_GROUP_SIZE, sizeof(buf_uint), &buf_uint, NULL);
        LOG_INFO("[CreateContext] CPU DEVICE_MAX_WORK_GROUP_SIZE = %u\n", (unsigned int)buf_uint);
        clGetDeviceInfo(devices[i], CL_DEVICE_MAX_WORK_ITEM_SIZES, sizeof(buf_uint), &buf_uint, NULL);
        LOG_INFO("[CreateContext] CPU DEVICE_MAX_WORK_ITEM_SIZE = %u\n", (unsigned int)buf_uint);
        clGetDeviceInfo(devices[i], CL_DEVICE_GLOBAL_MEM_SIZE, sizeof(buf_ulong), &buf_ulong, NULL);
        LOG_INFO("[CreateContext] CPU DEVICE_GLOBAL_MEM_SIZE = %llu\n", (unsigned long long)buf_ulong);
        clGetDeviceInfo(devices[i], CL_DEVICE_LOCAL_MEM_SIZE, sizeof(buf_ulong), &buf_ulong, NULL);
        LOG_INFO("[CreateContext] GPU DEVICE_LOCAL_MEM_SIZE = %llu\n", (unsigned long long)buf_ulong);
    }

    clGetDeviceIDs(PlatformId[0], CL_DEVICE_TYPE_ALL, 100, devices, &devices_n);

    for (int i=0; i<devices_n; i++)
    {
        char buffer[10240] = { 0 };
        cl_uint buf_uint;
        cl_ulong buf_ulong;
        LOG_INFO("[CreateContext] ALL -- %d --\n", i);
        clGetDeviceInfo(devices[i], CL_DEVICE_NAME, sizeof(buffer), buffer, NULL);
        LOG_INFO("[CreateContext] ALL DEVICE_NAME = %s\n", buffer);
        clGetDeviceInfo(devices[i], CL_DEVICE_VENDOR, sizeof(buffer), buffer, NULL);
        LOG_INFO("[CreateContext] ALL DEVICE_VENDOR = %s\n", buffer);
        clGetDeviceInfo(devices[i], CL_DEVICE_VERSION, sizeof(buffer), buffer, NULL);
        LOG_INFO("[CreateContext] ALL DEVICE_VERSION = %s\n", buffer);
        clGetDeviceInfo(devices[i], CL_DRIVER_VERSION, sizeof(buffer), buffer, NULL);
        LOG_INFO("[CreateContext] ALL DRIVER_VERSION = %s\n", buffer);
        clGetDeviceInfo(devices[i], CL_DEVICE_MAX_COMPUTE_UNITS, sizeof(buf_uint), &buf_uint, NULL);
        LOG_INFO("[CreateContext] ALL DEVICE_MAX_COMPUTE_UNITS = %u\n", (unsigned int)buf_uint);
        clGetDeviceInfo(devices[i], CL_DEVICE_MAX_CLOCK_FREQUENCY, sizeof(buf_uint), &buf_uint, NULL);
        LOG_INFO("[CreateContext] ALL DEVICE_MAX_CLOCK_FREQUENCY = %u\n", (unsigned int)buf_uint);
        clGetDeviceInfo(devices[i], CL_DEVICE_MAX_WORK_GROUP_SIZE, sizeof(buf_uint), &buf_uint, NULL);
        LOG_INFO("[CreateContext] ALL DEVICE_MAX_WORK_GROUP_SIZE = %u\n", (unsigned int)buf_uint);
        clGetDeviceInfo(devices[i], CL_DEVICE_MAX_WORK_ITEM_SIZES, sizeof(buf_uint), &buf_uint, NULL);
        LOG_INFO("[CreateContext] ALL DEVICE_MAX_WORK_ITEM_SIZE = %u\n", (unsigned int)buf_uint);
        clGetDeviceInfo(devices[i], CL_DEVICE_GLOBAL_MEM_SIZE, sizeof(buf_ulong), &buf_ulong, NULL);
        LOG_INFO("[CreateContext] ALL DEVICE_GLOBAL_MEM_SIZE = %llu\n", (unsigned long long)buf_ulong);
        clGetDeviceInfo(devices[i], CL_DEVICE_LOCAL_MEM_SIZE, sizeof(buf_ulong), &buf_ulong, NULL);
        LOG_INFO("[CreateContext] GPU DEVICE_LOCAL_MEM_SIZE = %llu\n", (unsigned long long)buf_ulong);
    }

    // Next, create an OpenCL context on the platform.  Attempt to
    // create a GPU-based context, and if that fails, try to create
    // a CPU-based context.
    cl_context_properties contextProperties[] =
            {
                    CL_CONTEXT_PLATFORM,
                    (cl_context_properties)PlatformId[0],
                    0
            };
#if CL_DEVICE_TYPE == 1
    context = clCreateContextFromType(contextProperties, CL_DEVICE_TYPE_GPU, NULL, NULL, &errNum);
#else
    context = clCreateContextFromType(contextProperties, CL_DEVICE_TYPE_CPU, NULL, NULL, &errNum);
#endif
    if (errNum != CL_SUCCESS)
    {
        LOG_ERROR("[CreateContext] Could not create GPU context, trying CPU...\n");
#if CL_DEVICE_TYPE == 1
        context = clCreateContextFromType(contextProperties, CL_DEVICE_TYPE_CPU, NULL, NULL, &errNum);
#else
        context = clCreateContextFromType(contextProperties, CL_DEVICE_TYPE_GPU, NULL, NULL, &errNum);
#endif
        if (errNum != CL_SUCCESS)
        {
            LOG_ERROR("[CreateContext] Failed to create an OpenCL GPU or CPU context\n");
            return NULL;
        }
    } 
    else if (errNum == CL_SUCCESS)
        LOG_DEBUG("[CreateContext] Create GPU context Done.\n");

    return context;
}


cl_context CL_Resource::CreateContext(void *EGL_Context, void *EGL_Display)
{
    cl_int errNum = CL_SUCCESS;
    cl_uint numPlatforms = 0;
    cl_platform_id PlatformId[10];
    cl_context context = NULL;

    // First, select an OpenCL platform to run on.  For this example, we
    // simply choose the first available platform.  Normally, you would
    // query for all available platforms and select the most appropriate one.
    errNum = clGetPlatformIDs(1, PlatformId, &numPlatforms);
    LOG_INFO("[CreateContext] numPlatforms = %d, firstPlatformId = %d", numPlatforms, PlatformId[0] );
    if (errNum != CL_SUCCESS || numPlatforms <= 0)
    {
        LOG_ERROR("[CreateContext] Failed to find any OpenCL platforms ; %s", getErrorString(errNum));
        return nullptr;
    }
    LOG_INFO("[CreateContext] numPlatforms = %d", numPlatforms);

    for (int i=0; i<numPlatforms; i++)
    {
        char buffer[10240];
        LOG_INFO("[CreateContext] -- %d --\n", i);
        clGetPlatformInfo(PlatformId[i], CL_PLATFORM_PROFILE, 10240, buffer, NULL);
        LOG_INFO("[CreateContext] OpenCL PROFILE = %s\n", buffer);
        clGetPlatformInfo(PlatformId[i], CL_PLATFORM_VERSION, 10240, buffer, NULL);
        LOG_INFO("[CreateContext] OpenCL VERSION = %s\n", buffer);
        clGetPlatformInfo(PlatformId[i], CL_PLATFORM_NAME, 10240, buffer, NULL);
        LOG_INFO("[CreateContext] OpenCL NAME = %s\n", buffer);
        clGetPlatformInfo(PlatformId[i], CL_PLATFORM_VENDOR, 10240, buffer, NULL);
        LOG_INFO("[CreateContext] OpenCL VENDOR = %s\n", buffer);
        clGetPlatformInfo(PlatformId[i], CL_PLATFORM_EXTENSIONS, 10240, buffer, NULL);
        LOG_INFO("[CreateContext] OpenCL EXTENSIONS = %s\n", buffer);
    }

    cl_device_id devices[100];
    cl_uint devices_n = 0;
    clGetDeviceIDs(PlatformId[0], CL_DEVICE_TYPE_GPU, 100, devices, &devices_n);

    m_device = devices[0];

    for (int i=0; i<devices_n; i++)
    {
        char buffer[10240];
        cl_uint buf_uint;
        cl_ulong buf_ulong;
        LOG_INFO("[CreateContext]   GPU -- %d --\n", i);
        clGetDeviceInfo(devices[i], CL_DEVICE_NAME, sizeof(buffer), buffer, NULL);
        LOG_INFO("[CreateContext] GPU DEVICE_NAME = %s\n", buffer);
        clGetDeviceInfo(devices[i], CL_DEVICE_VENDOR, sizeof(buffer), buffer, NULL);
        LOG_INFO("[CreateContext] GPU DEVICE_VENDOR = %s\n", buffer);
        clGetDeviceInfo(devices[i], CL_DEVICE_VERSION, sizeof(buffer), buffer, NULL);
        LOG_INFO("[CreateContext] GPU DEVICE_VERSION = %s\n", buffer);
        clGetDeviceInfo(devices[i], CL_DRIVER_VERSION, sizeof(buffer), buffer, NULL);
        LOG_INFO("[CreateContext] GPU DRIVER_VERSION = %s\n", buffer);
        clGetDeviceInfo(devices[i], CL_DEVICE_MAX_COMPUTE_UNITS, sizeof(buf_uint), &buf_uint, NULL);
        LOG_INFO("[CreateContext] GPU DEVICE_MAX_COMPUTE_UNITS = %u\n", (unsigned int)buf_uint);
        clGetDeviceInfo(devices[i], CL_DEVICE_MAX_CLOCK_FREQUENCY, sizeof(buf_uint), &buf_uint, NULL);
        LOG_INFO("[CreateContext] GPU DEVICE_MAX_CLOCK_FREQUENCY = %u\n", (unsigned int)buf_uint);
        clGetDeviceInfo(devices[i], CL_DEVICE_MAX_WORK_GROUP_SIZE, sizeof(buf_uint), &buf_uint, NULL);
        LOG_INFO("[CreateContext] GPU DEVICE_MAX_WORK_GROUP_SIZE = %u\n", (unsigned int)buf_uint);
        clGetDeviceInfo(devices[i], CL_DEVICE_MAX_WORK_ITEM_SIZES, sizeof(buf_uint), &buf_uint, NULL);
        LOG_INFO("[CreateContext] GPU DEVICE_MAX_WORK_ITEM_SIZE = %u\n", (unsigned int)buf_uint);
        clGetDeviceInfo(devices[i], CL_DEVICE_GLOBAL_MEM_SIZE, sizeof(buf_ulong), &buf_ulong, NULL);
        LOG_INFO("[CreateContext] GPU DEVICE_GLOBAL_MEM_SIZE = %llu\n", (unsigned long long)buf_ulong);
    }


    // Next, create an OpenCL context on the platform.  Attempt to
    // create a GPU-based context, and if that fails, try to create
    // a CPU-based context.
    cl_context_properties contextProperties[] =
            {
            CL_CONTEXT_PLATFORM, (cl_context_properties)PlatformId[0],
            CL_GL_CONTEXT_KHR, (cl_context_properties)EGL_Context,
            CL_EGL_DISPLAY_KHR, (cl_context_properties)EGL_Display,
            0
            };

    context = clCreateContext(contextProperties, 1, &m_device, NULL, NULL, &errNum);
    CL_CHECK(errNum, "[CreateContext] Could not create GPU context");

    if (errNum == CL_SUCCESS)
    {
        LOG_ERROR("[[CreateContext]] Create CL context interoperable with GL context");
    }

    return context;
}


//
//  Create a command queue on the first device available on the context
//
cl_command_queue CL_Resource::CreateCommandQueue(cl_context context, cl_device_id *device)
{
    cl_int errNum = CL_SUCCESS;
    cl_device_id *devices;
    cl_command_queue commandQueue = NULL;
    size_t deviceBufferSize = -1;

    // First get the size of the devices buffer
    errNum = clGetContextInfo(context, CL_CONTEXT_DEVICES, 0, NULL, &deviceBufferSize);
    if (errNum != CL_SUCCESS)
    {
        LOG_ERROR("[CreateCommandQueue] Failed call to clGetContextInfo(...,GL_CONTEXT_DEVICES,...)");
        return NULL;
    }

    if (deviceBufferSize <= 0)
    {
        LOG_ERROR("[CreateCommandQueue] No devices available");
        return NULL;
    }

    // Allocate memory for the devices buffer
    devices = new cl_device_id[deviceBufferSize / sizeof(cl_device_id)];
    errNum = clGetContextInfo(context, CL_CONTEXT_DEVICES, deviceBufferSize, devices, NULL);
    if (errNum != CL_SUCCESS)
    {
        delete [] devices;
        LOG_ERROR("[CreateCommandQueue] Failed to get device IDs");
        return NULL;
    }

    // In this example, we just choose the first available device.  In a
    // real program, you would likely use all available devices or choose
    // the highest performance device based on OpenCL device queries
#ifdef CL_TIME_DUMP
    commandQueue = clCreateCommandQueue(context, devices[0], CL_QUEUE_PROFILING_ENABLE, NULL);
#else
    commandQueue = clCreateCommandQueue(context, devices[0], 0, NULL);
#endif
    if (commandQueue == NULL)
    {
        delete [] devices;
        LOG_ERROR("[CreateCommandQueue] Failed to create commandQueue for device 0");
        return NULL;
    }

    *device = devices[0];
    delete [] devices;

    return commandQueue;
}


int CL_Resource::Clean()
{
    cl_int errNum = CL_SUCCESS;

    LOG_DEBUG("[Clean] clReleaseCommandQueue\n");
    if (m_commandQueue != 0)
    {
        errNum = clReleaseCommandQueue(m_commandQueue);
        CL_CHECK(errNum, "clReleaseCommandQueue");
    }

    LOG_DEBUG("[Clean] clReleaseContext\n");
    if (m_context != 0)
    {
        errNum = clReleaseContext(m_context);
        CL_CHECK(errNum, "clReleaseContext\n");
    }
    return errNum;
}

