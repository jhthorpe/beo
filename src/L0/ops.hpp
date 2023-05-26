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

#include "comm.hpp"

namespace beo
{

void barrier(Comm& comm);


void barrier(Comm& comm)
{
    #if defined _BEO_MPI_
    MPI_Barrier(comm.comm());
    #endif
}

}//end namespace beo

#endif
