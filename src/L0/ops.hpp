/*****************************************
 * ops.hpp
 *
 * JHT, May 26, 2023, Dallas, TX
 *	- created
 *
 * Header file for the level-0 beo
 *   operations 
 *
 * Included here:
 *	barrier 
*****************************************/
#ifndef _BEO_L0_OPS_HPP
#define _BEO_L0_OPS_HPP

#if defined _BEO_MPI_
#include "mpi.h"
#endif

#include "stat.hpp"
#include "utility.hpp"
#include "request.hpp"
#include "comm.hpp"

namespace beo
{

//Create a barrier with all tasks on comm
int barrier(Comm& comm);

//two-way blocking send/recieve
int send_recv(Comm& comm,
              void*       dest, 
              const void* src, 
              size_t      bytes,
              int         dest_id,
              int         src_id,
              int         tag);              


/*****************************************
 * creates a barrier on the beo::Comm 
*****************************************/
int barrier(Comm& comm)
{
    #if defined _BEO_MPI_

    return MPI_Barrier(comm.comm()) == MPI_SUCCESS ? BEO_SUCCESS : BEO_FAIL;

    #else 

    return BEO_SUCCESS;
    
    #endif
}

/****************************************
 * send_recv 
 *
 * two-way, blocking, send and recieve
 *   on a communicator
 *
 * In the MPI case, only the buffer used 
 *   by the appropriate party is referenced 
*****************************************/
//two-way blocking send/recieve
int send_recv(Comm& comm,
              void*       dest, 
              const void* src, 
              size_t      bytes,
              int         dest_id,
              int         src_id,
              int         tag)
{

    #if defined _BEO_MPI_

    //MPI-case where both sender and reciever are same task on same comm
    if (src_id == dest_id && comm.task_id() == src_id)
    {
       return beo::memmove(dest, src, bytes);
    }

    //MPI-Sender
    else if (comm.task_id() == src_id)
    {
        int tmp = MPI_Send(src,
                           bytes, 
                           MPI_CHAR,
                           dest_id, 
                           tag,
                           comm.comm());

        return (MPI_SUCCESS == tmp) ? BEO_SUCCESS : BEO_FAIL; 
    }

    //MPI_Reciever
    else if (comm.task_id() == dest_id) 
    {
        int tmp = MPI_Recv(dest,
                           bytes,
                           MPI_CHAR, 
                           src_id, 
                           tag,
                           comm.comm(),
                           MPI_STATUS_IGNORE);

        return (MPI_SUCCESS == tmp) ? BEO_SUCCESS : BEO_FAIL; 
    }

    else
    {
        return BEO_SUCCESS;
    }

    //non-MPI case
    #else

    return beo::memmove(dest, src, bytes);

    #endif

}

}//end namespace beo

#endif
