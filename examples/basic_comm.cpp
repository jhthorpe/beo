#define _BEO_MPI_
#include <mpi.h>

//Note this is included after _BEO_MPI_ is defined
#include "../include/beo.hpp"

#include <stdio.h>
#include <unistd.h>


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

    //Copy a vector to task 0 from task 1 
    std::vector<double> buf(3);

    //-----------------------------------------------------------------------------------------------------
    //Basic blocking communication example
    fflush(stdout);
    beo::barrier(world);
    if (world.is_master()) {printf("\nBlocking-example\n"); sleep(1);}
    buf = {(double) world_id, (double) world_id, (double) world_id}; 
    beo::barrier(world);


    printf("Task %d saw %lf %lf %lf at start\n", (int) world_id, buf[0], buf[1], buf[2]);

    printf("Task %d completed send_recv w. status %d\n", world_id, 
                                                         beo::send_recv(world, 
                                                                        buf.data(), 
                                                                        buf.data(), 
                                                                        buf.size() * sizeof(double), 
                                                                        0, 
                                                                        1, 
                                                                        0));

    printf("Task %d saw %lf %lf %lf at end\n", (int) world_id, buf[0], buf[1], buf[2]);

    //-----------------------------------------------------------------------------------------------------
    //An asynchronous example
    fflush(stdout);
    beo::barrier(world);
    if (world.is_master()) {printf("\nNon-Blocking-example\n"); sleep(1);}
    buf = {(double) world_id, (double) world_id, (double) world_id}; 
    beo::barrier(world);
    
    #if defined _BEO_MPI_
    auto req_0_1 = beo::async_send_recv(world, buf.data(), buf.data(), buf.size()*sizeof(double), 0, 1, 0);
    auto req_1_2 = beo::async_send_recv(world, buf.data(), buf.data(), buf.size()*sizeof(double), 1, 2, 0);
    #else
    auto req_0_1 = beo::async_send_recv(world, buf.data(), buf.data(), buf.size()*sizeof(double), 0, 0, 0);
    auto req_1_2 = beo::async_send_recv(world, buf.data(), buf.data(), buf.size()*sizeof(double), 0, 0, 0);
    #endif

    printf("Task %d saw %lf %lf %lf at middle\n", (int) world_id, buf[0], buf[1], buf[2]); sleep(1);
    beo::barrier(world);

    printf("Task %d saw req_0_1 as %s", (int) world_id, req_0_1.is_valid() ? "valid\n" : "invalid\n");
    printf("Task %d saw req_1_2 as %s", (int) world_id, req_1_2.is_valid() ? "valid\n" : "invalid\n");

    req_0_1.wait();
    req_1_2.wait();


    printf("Task %d saw %lf %lf %lf at end\n", (int) world_id, buf[0], buf[1], buf[2]);
    beo::barrier(world);

//    req_0_1.finalize();

    beo.finalize(0,"All done!"); //must be done before final MPI_Finalize

    #if defined _BEO_MPI_
    MPI_Finalize();
    #endif
  
    return 0;
}
