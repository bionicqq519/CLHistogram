
#define BIN_SIZE 256
#define ELEMENTS_TO_PROCESS 256

__kernel void Histogram_CL(__global int *a_pdOutHis,
                           __global unsigned char *a_pucInImg,
                           int a_dWidth,int a_dHeight)
{
    __local int sharedArray[BIN_SIZE];

    size_t localId = get_local_id(0);
    size_t globalId = get_global_id(0);
    size_t groupId = get_group_id(0);
    size_t groupSize = get_local_size(0);

    int elements_to_process = ELEMENTS_TO_PROCESS;
    int total_pixels = a_dWidth*a_dHeight;

    /* initialize shared array to zero */
    sharedArray[localId] = 0;

    barrier(CLK_LOCAL_MEM_FENCE);

    int groupOffset = groupId * groupSize * ELEMENTS_TO_PROCESS;

    /* For the last work group calculate the number of elements required */
    if(groupId == (get_num_groups(0) - 1))
        elements_to_process = ((total_pixels - groupOffset) + groupSize - 1) /groupSize;

    /* calculate thread-histograms */
    for(int i = 0; i < elements_to_process; ++i)
    {
        int index = groupOffset + i * groupSize + localId;
        if(index > total_pixels)
            break;
        //Coalesced read from global memory
        uchar value = a_pucInImg[index];
        atomic_inc(&sharedArray[value]);
    }
    barrier(CLK_LOCAL_MEM_FENCE);

    //Coalesced write to global memory
    atomic_add(&a_pdOutHis[localId], sharedArray[localId]);

    return;
}