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

    beo::Manager beo; 
    beo.init();

    if (beo.comms().shared().is_master())
    {
    printf("world size is %d, this task is rank %d\n", beo.comms().world().num_tasks()
                                                     , beo.comms().world().task_id());

    printf("shared size is %d, this task is rank %d \n", beo.comms().shared().num_tasks()
                                                       , beo.comms().shared().task_id());
    }

    #if defined _MPI
    beo.finalize(0,"All done!"); //must be done before final MPI_Finalize
    MPI_Finalize();
    #endif
  
    return 0;
}
