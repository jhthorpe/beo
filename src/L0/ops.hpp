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
 *      send_recieve
 *      async_send_recieve
*****************************************/
#ifndef _BEO_L0_OPS_HPP
#define _BEO_L0_OPS_HPP

#if defined _BEO_MPI_
#include "mpi.h"
#endif

#include <future>

#include "def.hpp"
#include "utility.hpp"
#include "request.hpp"
#include "comm.hpp"

namespace beo
{

void check_task_id(Comm& comm, int task_id); 

//Create a barrier on comm. 
// NOTE: this does NOT guarentee that your 
//   non-blocking calls are all completed at the time
//   this barrier is called. 
int barrier(Comm& comm);

//two-way blocking send/recieve
int send_recv(Comm& comm,
              void*       dest, 
              const void* src, 
              size_t      bytes,
              int         dest_id,
              int         src_id,
              int         tag);              

//two-way nonblocking send/recieve
Request async_send_recv(Comm& comm,
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

    //Check for valid dest and src ids
    if (dest_id >= comm.num_tasks())
    {
        printf("beo::send_recv Task %d input dest_id(%d) is invalid\n", comm.task_id(), dest_id);
        exit(1);
    }

    if (src_id >= comm.num_tasks())
    {
        printf("beo::send_recv Task %d input src_id(%d) is invalid\n", comm.task_id(), src_id);
        exit(1);
    }
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

/****************************************
 * async_send_recv
 *
 * Asynchronous send recieve
****************************************/
Request async_send_recv(Comm& comm,
                        void*       dest, 
                        const void* src, 
                        size_t      bytes,
                        int         dest_id,
                        int         src_id,
                        int         tag)
{
    //Check for valid dest and src ids
    if (dest_id >= comm.num_tasks())
    {
        printf("beo::send_recv Task %d input dest_id(%d) is invalid\n", comm.task_id(), dest_id);
        exit(1);
    }

    if (src_id >= comm.num_tasks())
    {
        printf("beo::send_recv Task %d input src_id(%d) is invalid\n", comm.task_id(), src_id);
        exit(1);
    }
    
    #if defined _BEO_MPI_

    //MPI case for when src and dest are same data on same task
    if (src_id == dest_id && comm.task_id() == src_id)
    {
        MPI_Request fake = MPI_REQUEST_NULL;
        Request request = std::move(fake);
        return request; 
    }

    //MPI-Sender
    else if (comm.task_id() == src_id)
    {
        MPI_Request fake; 
        
        int tmp = MPI_Isend(src,
                            bytes, 
                            MPI_CHAR,
                            dest_id, 
                            tag,
                            comm.comm(),
                            &fake); 

        if (tmp != MPI_SUCCESS) exit(1);

        Request request = std::move(fake);

        return request; 
    }

    //MPI_Reciever
    else if (comm.task_id() == dest_id) 
    {
        MPI_Request fake; 

        int tmp = MPI_Irecv(dest,
                            bytes,
                            MPI_CHAR, 
                            src_id, 
                            tag,
                            comm.comm(),
                            &fake); 

        if (tmp != MPI_SUCCESS) exit(1);

        Request request = std::move(fake);

        return request; 
    }

    else
    {
        MPI_Request fake = MPI_REQUEST_NULL;
        Request request = std::move(fake);
        return request; 
    }

    //non-MPI case
    #else

    Request request = std::async(std::launch::async, [=]()
    {
        return beo::memmove(dest, src, bytes);
    });
    return request; 

    #endif
}



}//end namespace beo

#endif
