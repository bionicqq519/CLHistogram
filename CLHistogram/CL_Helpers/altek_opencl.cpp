/****************************************************************************
*                               Include File                                *
****************************************************************************/
#include "altek_opencl.h"
#include <string.h>
#include "stdio.h"

#define LOGD(...) do { printf(__VA_ARGS__); }while(0)
#define LLOG(...) do { printf(__VA_ARGS__); }while(0)
#define LOGI(...) do { printf(__VA_ARGS__); }while(0)

/****************************************************************************
*                          Private Global Variable                          *
****************************************************************************/
static cl_context g_context = 0;
static cl_command_queue g_commandQueue = 0;
static cl_device_id g_device = 0;

/****************************************************************************
*                             Private Function                              *
****************************************************************************/
const char *getErrorString(cl_int error)
{
    switch(error){
        // run-time and JIT compiler errors
        case 0: return "CL_SUCCESS";
        case -1: return "CL_DEVICE_NOT_FOUND";
        case -2: return "CL_DEVICE_NOT_AVAILABLE";
        case -3: return "CL_COMPILER_NOT_AVAILABLE";
        case -4: return "CL_MEM_OBJECT_ALLOCATION_FAILURE";
        case -5: return "CL_OUT_OF_RESOURCES";
        case -6: return "CL_OUT_OF_HOST_MEMORY";
        case -7: return "CL_PROFILING_INFO_NOT_AVAILABLE";
        case -8: return "CL_MEM_COPY_OVERLAP";
        case -9: return "CL_IMAGE_FORMAT_MISMATCH";
        case -10: return "CL_IMAGE_FORMAT_NOT_SUPPORTED";
        case -11: return "CL_BUILD_PROGRAM_FAILURE";
        case -12: return "CL_MAP_FAILURE";
        case -13: return "CL_MISALIGNED_SUB_BUFFER_OFFSET";
        case -14: return "CL_EXEC_STATUS_ERROR_FOR_EVENTS_IN_WAIT_LIST";
        case -15: return "CL_COMPILE_PROGRAM_FAILURE";
        case -16: return "CL_LINKER_NOT_AVAILABLE";
        case -17: return "CL_LINK_PROGRAM_FAILURE";
        case -18: return "CL_DEVICE_PARTITION_FAILED";
        case -19: return "CL_KERNEL_ARG_INFO_NOT_AVAILABLE";

        // compile-time errors
        case -30: return "CL_INVALID_VALUE";
        case -31: return "CL_INVALID_DEVICE_TYPE";
        case -32: return "CL_INVALID_PLATFORM";
        case -33: return "CL_INVALID_DEVICE";
        case -34: return "CL_INVALID_CONTEXT";
        case -35: return "CL_INVALID_QUEUE_PROPERTIES";
        case -36: return "CL_INVALID_COMMAND_QUEUE";
        case -37: return "CL_INVALID_HOST_PTR";
        case -38: return "CL_INVALID_MEM_OBJECT";
        case -39: return "CL_INVALID_IMAGE_FORMAT_DESCRIPTOR";
        case -40: return "CL_INVALID_IMAGE_SIZE";
        case -41: return "CL_INVALID_SAMPLER";
        case -42: return "CL_INVALID_BINARY";
        case -43: return "CL_INVALID_BUILD_OPTIONS";
        case -44: return "CL_INVALID_PROGRAM";
        case -45: return "CL_INVALID_PROGRAM_EXECUTABLE";
        case -46: return "CL_INVALID_KERNEL_NAME";
        case -47: return "CL_INVALID_KERNEL_DEFINITION";
        case -48: return "CL_INVALID_KERNEL";
        case -49: return "CL_INVALID_ARG_INDEX";
        case -50: return "CL_INVALID_ARG_VALUE";
        case -51: return "CL_INVALID_ARG_SIZE";
        case -52: return "CL_INVALID_KERNEL_ARGS";
        case -53: return "CL_INVALID_WORK_DIMENSION";
        case -54: return "CL_INVALID_WORK_GROUP_SIZE";
        case -55: return "CL_INVALID_WORK_ITEM_SIZE";
        case -56: return "CL_INVALID_GLOBAL_OFFSET";
        case -57: return "CL_INVALID_EVENT_WAIT_LIST";
        case -58: return "CL_INVALID_EVENT";
        case -59: return "CL_INVALID_OPERATION";
        case -60: return "CL_INVALID_GL_OBJECT";
        case -61: return "CL_INVALID_BUFFER_SIZE";
        case -62: return "CL_INVALID_MIP_LEVEL";
        case -63: return "CL_INVALID_GLOBAL_WORK_SIZE";
        case -64: return "CL_INVALID_PROPERTY";
        case -65: return "CL_INVALID_IMAGE_DESCRIPTOR";
        case -66: return "CL_INVALID_COMPILER_OPTIONS";
        case -67: return "CL_INVALID_LINKER_OPTIONS";
        case -68: return "CL_INVALID_DEVICE_PARTITION_COUNT";

        // extension errors
        case -1000: return "CL_INVALID_GL_SHAREGROUP_REFERENCE_KHR";
        case -1001: return "CL_PLATFORM_NOT_FOUND_KHR";
        case -1002: return "CL_INVALID_D3D10_DEVICE_KHR";
        case -1003: return "CL_INVALID_D3D10_RESOURCE_KHR";
        case -1004: return "CL_D3D10_RESOURCE_ALREADY_ACQUIRED_KHR";
        case -1005: return "CL_D3D10_RESOURCE_NOT_ACQUIRED_KHR";
        default: return "Unknown OpenCL error";
    }
}

static cl_context CreateContext()
{
    cl_int errNum = CL_SUCCESS;
    cl_uint numPlatforms = 0;
    cl_platform_id PlatformId[10];
    cl_context context = NULL;

    // First, select an OpenCL platform to run on.  For this example, we
    // simply choose the first available platform.  Normally, you would
    // query for all available platforms and select the most appropriate one.
    errNum = clGetPlatformIDs(1, PlatformId, &numPlatforms);
	LOGI("[ALTEK_CL] numPlatforms = %d, firstPlatformId = %d\n", numPlatforms, PlatformId[0] );
    if (errNum != CL_SUCCESS || numPlatforms <= 0)
    {
        LOGD("[ALTEK_CL] Failed to find any OpenCL platforms ; %s\n", getErrorString(errNum));
        return NULL;
    }
    LOGI("[ALTEK_CL] numPlatforms = %d\n", numPlatforms);

	for (int i=0; i<numPlatforms; i++)
	{
		char buffer[10240];
        LOGI("[ALTEK_CL] -- numPlatforms %d --\n", i);
		clGetPlatformInfo(PlatformId[i], CL_PLATFORM_PROFILE, 10240, buffer, NULL);
        LOGI("[ALTEK_CL] OpenCL PROFILE = %s\n", buffer);
		clGetPlatformInfo(PlatformId[i], CL_PLATFORM_VERSION, 10240, buffer, NULL);
        LOGI("[ALTEK_CL] OpenCL VERSION = %s\n", buffer);
		clGetPlatformInfo(PlatformId[i], CL_PLATFORM_NAME, 10240, buffer, NULL);
        LOGI("[ALTEK_CL] OpenCL NAME = %s\n", buffer);
		clGetPlatformInfo(PlatformId[i], CL_PLATFORM_VENDOR, 10240, buffer, NULL);
        LOGI("[ALTEK_CL] OpenCL VENDOR = %s\n", buffer);
		clGetPlatformInfo(PlatformId[i], CL_PLATFORM_EXTENSIONS, 10240, buffer, NULL);
        LOGI("[ALTEK_CL] OpenCL EXTENSIONS = %s\n", buffer);
	}

	cl_device_id devices[100];
	cl_uint devices_n = 0;
	clGetDeviceIDs(PlatformId[0], CL_DEVICE_TYPE_GPU, 100, devices, &devices_n);

	for (int i=0; i<devices_n; i++)
	{
		char buffer[10240];
		cl_uint buf_uint;
		cl_ulong buf_ulong;
        LOGI("[ALTEK_CL]   GPU -- %d --\n", i);
		clGetDeviceInfo(devices[i], CL_DEVICE_NAME, sizeof(buffer), buffer, NULL);
        LOGI("[ALTEK_CL] GPU DEVICE_NAME = %s\n", buffer);
		clGetDeviceInfo(devices[i], CL_DEVICE_VENDOR, sizeof(buffer), buffer, NULL);
        LOGI("[ALTEK_CL] GPU DEVICE_VENDOR = %s\n", buffer);
		clGetDeviceInfo(devices[i], CL_DEVICE_VERSION, sizeof(buffer), buffer, NULL);
        LOGI("[ALTEK_CL] GPU DEVICE_VERSION = %s\n", buffer);
		clGetDeviceInfo(devices[i], CL_DRIVER_VERSION, sizeof(buffer), buffer, NULL);
        LOGI("[ALTEK_CL] GPU DRIVER_VERSION = %s\n", buffer);
		clGetDeviceInfo(devices[i], CL_DEVICE_MAX_COMPUTE_UNITS, sizeof(buf_uint), &buf_uint, NULL);
        LOGI("[ALTEK_CL] GPU DEVICE_MAX_COMPUTE_UNITS = %u\n", (unsigned int)buf_uint);
		clGetDeviceInfo(devices[i], CL_DEVICE_MAX_CLOCK_FREQUENCY, sizeof(buf_uint), &buf_uint, NULL);
        LOGI("[ALTEK_CL] GPU DEVICE_MAX_CLOCK_FREQUENCY = %u\n", (unsigned int)buf_uint);
		clGetDeviceInfo(devices[i], CL_DEVICE_MAX_WORK_GROUP_SIZE, sizeof(buf_uint), &buf_uint, NULL);
        LOGI("[ALTEK_CL] GPU DEVICE_MAX_WORK_GROUP_SIZE = %u\n", (unsigned int)buf_uint);
		clGetDeviceInfo(devices[i], CL_DEVICE_MAX_WORK_ITEM_SIZES, sizeof(buf_uint), &buf_uint, NULL);
        LOGI("[ALTEK_CL] GPU DEVICE_MAX_WORK_ITEM_SIZE = %u\n", (unsigned int)buf_uint);
		clGetDeviceInfo(devices[i], CL_DEVICE_GLOBAL_MEM_SIZE, sizeof(buf_ulong), &buf_ulong, NULL);
        LOGI("[ALTEK_CL] GPU DEVICE_GLOBAL_MEM_SIZE = %llu\n", (unsigned long long)buf_ulong);
        clGetDeviceInfo(devices[i], CL_DEVICE_LOCAL_MEM_SIZE, sizeof(buf_ulong), &buf_ulong, NULL);
        LOGI("[ALTEK_CL] GPU DEVICE_LOCAL_MEM_SIZE = %llu\n", (unsigned long long)buf_ulong);
        clGetDeviceInfo(devices[i], CL_DEVICE_MAX_CONSTANT_BUFFER_SIZE, sizeof(buf_ulong), &buf_ulong, NULL);
        LOGI("[ALTEK_CL] GPU DEVICE_MAX_CONSTANT_BUFFER_SIZE = %llu\n", (unsigned long long)buf_ulong);
	}

	clGetDeviceIDs(PlatformId[0], CL_DEVICE_TYPE_CPU, 100, devices, &devices_n);

	for (int i=0; i<devices_n; i++)
	{
		char buffer[10240];
		cl_uint buf_uint;
		cl_ulong buf_ulong;
        LOGI("[ALTEK_CL] CPU -- %d --\n", i);
		clGetDeviceInfo(devices[i], CL_DEVICE_NAME, sizeof(buffer), buffer, NULL);
        LOGI("[ALTEK_CL] CPU DEVICE_NAME = %s\n", buffer);
		clGetDeviceInfo(devices[i], CL_DEVICE_VENDOR, sizeof(buffer), buffer, NULL);
        LOGI("[ALTEK_CL] CPU DEVICE_VENDOR = %s\n", buffer);
		clGetDeviceInfo(devices[i], CL_DEVICE_VERSION, sizeof(buffer), buffer, NULL);
        LOGI("[ALTEK_CL] CPU DEVICE_VERSION = %s\n", buffer);
		clGetDeviceInfo(devices[i], CL_DRIVER_VERSION, sizeof(buffer), buffer, NULL);
        LOGI("[ALTEK_CL] CPU DRIVER_VERSION = %s\n", buffer);
		clGetDeviceInfo(devices[i], CL_DEVICE_MAX_COMPUTE_UNITS, sizeof(buf_uint), &buf_uint, NULL);
        LOGI("[ALTEK_CL] CPU DEVICE_MAX_COMPUTE_UNITS = %u\n", (unsigned int)buf_uint);
		clGetDeviceInfo(devices[i], CL_DEVICE_MAX_CLOCK_FREQUENCY, sizeof(buf_uint), &buf_uint, NULL);
        LOGI("[ALTEK_CL] CPU DEVICE_MAX_CLOCK_FREQUENCY = %u\n", (unsigned int)buf_uint);
		clGetDeviceInfo(devices[i], CL_DEVICE_MAX_WORK_GROUP_SIZE, sizeof(buf_uint), &buf_uint, NULL);
        LOGI("[ALTEK_CL] CPU DEVICE_MAX_WORK_GROUP_SIZE = %u\n", (unsigned int)buf_uint);
		clGetDeviceInfo(devices[i], CL_DEVICE_MAX_WORK_ITEM_SIZES, sizeof(buf_uint), &buf_uint, NULL);
        LOGI("[ALTEK_CL] CPU DEVICE_MAX_WORK_ITEM_SIZE = %u\n", (unsigned int)buf_uint);
		clGetDeviceInfo(devices[i], CL_DEVICE_GLOBAL_MEM_SIZE, sizeof(buf_ulong), &buf_ulong, NULL);
        LOGI("[ALTEK_CL] CPU DEVICE_GLOBAL_MEM_SIZE = %llu\n", (unsigned long long)buf_ulong);
	}

	clGetDeviceIDs(PlatformId[0], CL_DEVICE_TYPE_ALL, 100, devices, &devices_n);

	for (int i=0; i<devices_n; i++)
	{
		char buffer[10240];
		cl_uint buf_uint;
		cl_ulong buf_ulong;
        LOGI("[ALTEK_CL] ALL -- %d --\n", i);
		clGetDeviceInfo(devices[i], CL_DEVICE_NAME, sizeof(buffer), buffer, NULL);
        LOGI("[ALTEK_CL] ALL DEVICE_NAME = %s\n", buffer);
		clGetDeviceInfo(devices[i], CL_DEVICE_VENDOR, sizeof(buffer), buffer, NULL);
        LOGI("[ALTEK_CL] ALL DEVICE_VENDOR = %s\n", buffer);
		clGetDeviceInfo(devices[i], CL_DEVICE_VERSION, sizeof(buffer), buffer, NULL);
        LOGI("[ALTEK_CL] ALL DEVICE_VERSION = %s\n", buffer);
		clGetDeviceInfo(devices[i], CL_DRIVER_VERSION, sizeof(buffer), buffer, NULL);
        LOGI("[ALTEK_CL] ALL DRIVER_VERSION = %s\n", buffer);
		clGetDeviceInfo(devices[i], CL_DEVICE_MAX_COMPUTE_UNITS, sizeof(buf_uint), &buf_uint, NULL);
        LOGI("[ALTEK_CL] ALL DEVICE_MAX_COMPUTE_UNITS = %u\n", (unsigned int)buf_uint);
		clGetDeviceInfo(devices[i], CL_DEVICE_MAX_CLOCK_FREQUENCY, sizeof(buf_uint), &buf_uint, NULL);
        LOGI("[ALTEK_CL] ALL DEVICE_MAX_CLOCK_FREQUENCY = %u\n", (unsigned int)buf_uint);
		clGetDeviceInfo(devices[i], CL_DEVICE_MAX_WORK_GROUP_SIZE, sizeof(buf_uint), &buf_uint, NULL);
        LOGI("[ALTEK_CL] ALL DEVICE_MAX_WORK_GROUP_SIZE = %u\n", (unsigned int)buf_uint);
		clGetDeviceInfo(devices[i], CL_DEVICE_MAX_WORK_ITEM_SIZES, sizeof(buf_uint), &buf_uint, NULL);
        LOGI("[ALTEK_CL] ALL DEVICE_MAX_WORK_ITEM_SIZE = %u\n", (unsigned int)buf_uint);
		clGetDeviceInfo(devices[i], CL_DEVICE_GLOBAL_MEM_SIZE, sizeof(buf_ulong), &buf_ulong, NULL);
        LOGI("[ALTEK_CL] ALL DEVICE_GLOBAL_MEM_SIZE = %llu\n", (unsigned long long)buf_ulong);
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
    context = clCreateContextFromType(contextProperties, CL_DEVICE_TYPE_GPU,
                                      NULL, NULL, &errNum);
    if (errNum != CL_SUCCESS)
    {
        LOGD("[ALTEK_CL] Could not create GPU context, trying CPU...\n");
        context = clCreateContextFromType(contextProperties, CL_DEVICE_TYPE_CPU,
                                          NULL, NULL, &errNum);
        if (errNum != CL_SUCCESS)
        {
            LOGD("[ALTEK_CL] Failed to create an OpenCL GPU or CPU context\n");
            return NULL;
        }
    } else if (errNum == CL_SUCCESS)

    LOGI("[ALTEK_CL] Create GPU context Done.\n");

    return context;
}

//
//  Create a command queue on the first device available on the context
//
static cl_command_queue CreateCommandQueue(cl_context context, cl_device_id *device)
{
    cl_int errNum = CL_SUCCESS;
    cl_device_id *devices;
    cl_command_queue commandQueue = NULL;
    size_t deviceBufferSize = -1;

    // First get the size of the devices buffer
    errNum = clGetContextInfo(context, CL_CONTEXT_DEVICES, 0, NULL, &deviceBufferSize);
    if (errNum != CL_SUCCESS)
    {
        //std::cerr << "Failed call to clGetContextInfo(...,GL_CONTEXT_DEVICES,...)";
        LOGD("[ALTEK_CL] Failed call to clGetContextInfo(...,GL_CONTEXT_DEVICES,...)\n");
        return NULL;
    }

    if (deviceBufferSize <= 0)
    {
        //std::cerr << "No devices available.";
        LOGD("[ALTEK_CL] No devices available\n");
        return NULL;
    }

    // Allocate memory for the devices buffer
    devices = new cl_device_id[deviceBufferSize / sizeof(cl_device_id)];
    errNum = clGetContextInfo(context, CL_CONTEXT_DEVICES, deviceBufferSize, devices, NULL);
    if (errNum != CL_SUCCESS)
    {
        delete [] devices;
        //std::cerr << "Failed to get device IDs";
        LOGD("[ALTEK_CL] Failed to get device IDs\n");
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
        //std::cerr << "Failed to create commandQueue for device 0";
        LOGD("[ALTEK_CL] Failed to create commandQueue for device 0\n");
        return NULL;
    }

    *device = devices[0];
    delete [] devices;

    return commandQueue;
}

//
//  Create an OpenCL program from the kernel source file
//
static cl_program CreateProgram(cl_context a_clContext, cl_device_id a_clDevice, const char* a_sData)
{
    cl_int errNum = CL_SUCCESS;
    cl_program program = NULL;

    LOGI("[ALTEK_CL] CreateProgram");
    program = clCreateProgramWithSource(a_clContext, 1,
                                        (const char**)&a_sData,
                                        NULL, &errNum);
    if (program == NULL)
    {
        LOGI("[ALTEK_CL] Failed to create CL program from source data : %s\n", getErrorString(errNum));
        return NULL;
    }

    errNum = clBuildProgram(program, 0, NULL, NULL, NULL, NULL);
    if (errNum != CL_SUCCESS)
    {
        // Determine the reason for the error
        char buildLog[16384];
        clGetProgramBuildInfo(program, a_clDevice, CL_PROGRAM_BUILD_LOG,
                              sizeof(buildLog), buildLog, NULL);

        //std::cerr << "Error in kernel: " << std::endl;
        //std::cerr << buildLog;
        LOGI("[ALTEK_CL] Error in kernel: %s\n", buildLog);
        clReleaseProgram(program);
        return NULL;
    }

    LOGI("[ALTEK_CL] CreateProgram End\n");

    return program;
}

//
//  Cleanup any created OpenCL resources
//
static void Cleanup(cl_context a_clContext, cl_command_queue a_clCommandQueue)
{
    cl_int errNum = CL_SUCCESS;

    if (a_clCommandQueue != 0)
    {
        errNum = clReleaseCommandQueue(a_clCommandQueue);
        if (errNum != CL_SUCCESS)
            LOGI("[ALTEK_CL] Failed to clReleaseCommandQueue : %s\n", getErrorString(errNum));
    }

    if (a_clContext != 0)
    {
        errNum = clReleaseContext(a_clContext);
        if (errNum != CL_SUCCESS)
            LOGI("[ALTEK_CL] Failed to clReleaseContext : %s\n", getErrorString(errNum));
    }
}

/****************************************************************************
*                              Public Function                              *
****************************************************************************/
int OpenCL_Init()
{
    cl_int errNum = CL_SUCCESS;

    LOGI("[ALTEK_CL] OpenCL_Init start\n");

    // Create an OpenCL context on first available platform
    g_context = CreateContext();
    if (g_context == NULL)
    {
        LOGD("[ALTEK_CL] Failed to create OpenCL context.\n");
        return 1;
    }

    // Create a command-queue on the first device available
    // on the created context
    g_commandQueue = CreateCommandQueue(g_context, &g_device);
    if (g_commandQueue == NULL)
    {
        LOGD("[ALTEK_CL] Failed to commandQueue.\n");
        Cleanup(g_context, g_commandQueue);
        return 1;
    }

    LOGI("[ALTEK_CL] OpenCL_Init end\n");

    return errNum;
}

cl_program OpenCL_CreateProgram(const char *a_sCL_Kernel_code)
{
    if(g_context == NULL)
        return NULL;

    // Create OpenCL program from HelloWorld.cl kernel source
    cl_program program = CreateProgram(g_context, g_device, a_sCL_Kernel_code);
    if (program == NULL)
    {
        LOGI("[ALTEK_CL] Failed to program.\n");
        Cleanup(g_context, g_commandQueue);
        return NULL;
    }

    return program;
}

int OpenCL_DeInit()
{
    cl_int errNum = CL_SUCCESS;

    Cleanup(g_context, g_commandQueue);

    return errNum;
}

cl_context OpenCL_GetCurrentContext()
{
    return g_context;
}

cl_command_queue OpenCL_GetCurrentCommandQueue()
{
    return g_commandQueue;
}

cl_device_id OpenCL_GetCurrentDevice()
{
    return g_device;
}

// Map to CPU
void* OpenCL_Map(cl_mem a_clMemObj, int a_dWidth, int a_dHeight)
{
    cl_int errNum = CL_SUCCESS;

    void *cl_ToCopy = (void *)clEnqueueMapBuffer(g_commandQueue, a_clMemObj, CL_TRUE, CL_MAP_READ | CL_MAP_WRITE, 0,
                                                                   sizeof(unsigned char) * a_dWidth * a_dHeight, 0, NULL, NULL, &errNum);
    if (errNum != CL_SUCCESS)
    {
        LOGI("[ALTEK_CL] Error setting clEnqueueMapBuffer : %s\n", getErrorString(errNum));
        Cleanup(g_context, g_commandQueue);
        return NULL;
    }

    return cl_ToCopy;
}

// Unmap to GPU
int OpenCL_UnMap(cl_mem a_clMemObj, void *a_pHandle)
{
    cl_int errNum = CL_SUCCESS;

    errNum = clEnqueueUnmapMemObject(g_commandQueue, a_clMemObj, a_pHandle, 0, NULL, NULL);

    if (errNum != CL_SUCCESS)
    {
        LOGI("[ALTEK_CL] Error setting clEnqueueUnmapMemObject : %s\n", getErrorString(errNum));
        Cleanup(g_context, g_commandQueue);
        return errNum;
    }

    return errNum;
}

// Map 2D to CPU
void* OpenCL_Instance_Map_2D_Image(unsigned short *a_puwOutRowPitch, cl_mem a_clMemObj, int a_dWidth, int a_dHeight)
{
    cl_int errNum = CL_SUCCESS;

    size_t origin[3] = {0, 0, 0};
    size_t region[3] = {(size_t)a_dWidth, (size_t)a_dHeight, 1};
    size_t row_pitch;
    size_t slice_pitch;

    void *cl_ToCopy = (void *)clEnqueueMapImage(g_commandQueue, a_clMemObj, CL_TRUE, CL_MAP_READ | CL_MAP_WRITE,
                                                origin, region, &row_pitch, &slice_pitch, 0, NULL, NULL, &errNum);
    if (errNum != CL_SUCCESS)
    {
        LOGD("*OpenCL* Error setting clEnqueueMapImage : %s\n", getErrorString(errNum));
        Cleanup(g_context, g_commandQueue);
        return NULL;
    }

    if (a_puwOutRowPitch != NULL)
    {
        *a_puwOutRowPitch = row_pitch;
    }

    LOGD("[ALTEK_CL] OpenCL_Instance_Map_2D_Image row_pitch : %d\n", (int)row_pitch);
    LOGD("[ALTEK_CL] OpenCL_Instance_Map_2D_Image slice_pitch : %d\n", (int)slice_pitch);

    return cl_ToCopy;
}
