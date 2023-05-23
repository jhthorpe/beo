/*****************************************
 * node_ops.hpp
 * 
 * JHT, May 22, 2023, SMU, Dallas, Texas
 * 	- created
 * 
 * Header file that defines some useful
 *   inter and intra node operations
 *
 * Functions included here:
 *      barrier
*****************************************/
#ifndef _BEO_NODE_NODE_OPPS_
#define _BEO_NODE_NODE_OPPS_

#if defined _BEO_MPI_
    #include <mpi.h>
#endif

#include <stdio.h>
#include <string>

#include "../Global/global.hpp"

namespace beo
{

void barrier(beo::Node_Set& set);

/*****************************************
 * barrier
 *
 * Creates an MPI barrier for a particular 
 * Node_Set 
*****************************************/
void barrier(beo::Node_Set& set)
{
    #if defined _BEO_MPI_
        MPI_Barrier(set.comm());
    #endif
}


} //end beo namespace

#endif
