#include <stdio.h>
#include <future>
#include <unistd.h>

#define _BEO_MPI_

#if defined _BEO_MPI_
#include <mpi.h>
#endif

#include "../include/beo.hpp"

int main()
{
    #if defined _BEO_MPI_
    MPI_Init(NULL, NULL);
    #endif

    beo::Enviroment beo;
    auto& world_comm = beo.comms().world();
    auto  world_is_master = world_comm.is_master();
    auto  world_id = world_comm.task_id();

    //Create chunks for a 4x4 matrix 
    beo::Chunk c00{beo::Chunk_Tag{{0,0},{2,2}}};     
    beo::Chunk c20{beo::Chunk_Tag{{2,0},{2,2}}};     
    beo::Chunk c02{beo::Chunk_Tag{{0,2},{2,2}}};     
    beo::Chunk c22{beo::Chunk_Tag{{2,2},{2,2}}};     

    if (BEO_FAIL == c00.allocate(c00.size() * sizeof(double))) {printf("bad alloc on c00\n");}
    c20.allocate(c20.size() * sizeof(double));
    c02.allocate(c02.size() * sizeof(double));
    c22.allocate(c22.size() * sizeof(double));

    double* d00_ptr = (double*) c00.data();
    d00_ptr[0] = 1.0;
    d00_ptr[3] = 3.0;

    double* d22_ptr = (double*) c22.data();
    d22_ptr[3] = 16.0;


    c00.free();
    c20.free();
    c02.free();
    c22.free();

    beo.finalize();

    #if defined _BEO_MPI_
    MPI_Finalize();
    #endif

    return 0;
}

