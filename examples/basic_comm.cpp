//    #define _BEO_MPI_
//    #include <mpi.h>

//Note this is included after _BEO_MPI_ is defined
#include "../include/beo.hpp"

#include <stdio.h>

int main()
{
    #if defined _BEO_MPI_
    MPI_Init(NULL,NULL);
    #endif

    //cannot initialize the beo comms until mpi is initialized
    beo::Enviroment beo; 

    auto& world = beo.comms().world();
    auto& shared = beo.comms().shared();

    auto world_id = world.task_id();
    auto shared_id = shared.task_id();

    printf("world task id is %d\n",world_id);

    if (shared.is_master())
    {
        printf("shared size is %d, this task is rank %d \n", beo.comms().shared().num_tasks()
                                                           , beo.comms().shared().task_id());
    }
    
    //Copy a vector to task 0 from task 1 
    std::vector<double> buf{(double) world_id, (double) world_id, (double) world_id};

    printf("Task %d saw %lf %lf %lf at start\n", (int) world_id, buf[0], buf[1], buf[2]);

    beo::barrier(world);

    printf("Task %d completed send_recv w. status %d\n", world_id, 
                                                         beo::send_recv(world, 
                                                                        buf.data(), 
                                                                        buf.data(), 
                                                                        buf.size() * sizeof(double), 
                                                                        0, 
                                                                        1, 
                                                                        0));

    printf("Task %d saw %lf %lf %lf at end\n", (int) world_id, buf[0], buf[1], buf[2]);

    beo.finalize(0,"All done!"); //must be done before final MPI_Finalize

    #if defined _BEO_MPI_
    MPI_Finalize();
    #endif
  
    return 0;
}
