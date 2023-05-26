#define _MPI

#if defined _MPI
    #define _BEO_MPI_
    #include <mpi.h>
#endif

//Note this is included after _BEO_MPI_ is defined
#include "../include/beo.hpp"

#include <stdio.h>

int main()
{
    #if defined _MPI
    MPI_Init(NULL,NULL);
    #endif

    //cannot initialize the beo comms until mpi is initialized
    beo::Enviroment beo; 

    auto& world_comm = beo.comms().world();
    auto& shared_comm = beo.comms().shared();

    auto world_id = world_comm.task_id();
    auto shared_id = shared_comm.task_id();

    printf("world task id is %d\n",world_id);

    if (shared_comm.is_master())
    {
        printf("shared size is %d, this task is rank %d \n", beo.comms().shared().num_tasks()
                                                           , beo.comms().shared().task_id());
    }
    
    

    //Copy a vector from 0 to 1 
    std::vector<double> buf(3);
    if (world_id == 0)
    {
        buf[0] = 1; buf[1] = 2; buf[2] = 3; 
        printf("task 0 made: %lf %lf %lf\n",buf[0],buf[1],buf[2]);
    } 
    else 
    {
        buf[0] = 4; buf[1] = 5; buf[2] = 6; 
        printf("task 1 made: %lf %lf %lf\n",buf[0],buf[1],buf[2]);
    }

    if (world_id == 0)
    {
        MPI_Send(buf.data(),
                 buf.size()*sizeof(double),
                 MPI_CHAR,
                 1,
                 0,
                 world_comm.comm());
    }

    else 
    {
        MPI_Status status;

        MPI_Recv(buf.data(),
                 buf.size()*sizeof(double),
                 MPI_CHAR,
                 0,
                 0,
                 world_comm.comm(),
                 &status);
    }

    if (world_id == 0)
    {
        printf("task 0 saw: %lf %lf %lf\n",buf[0],buf[1],buf[2]);
    } 
    else 
    { 
        printf("task 1 saw: %lf %lf %lf\n",buf[0],buf[1],buf[2]);
    }


//    beo::sendrecv(beo.comms().world, 0, 1, 
                  

    beo.finalize(0,"All done!"); //must be done before final MPI_Finalize

    #if defined _MPI
    MPI_Finalize();
    #endif
  
    return 0;
}
