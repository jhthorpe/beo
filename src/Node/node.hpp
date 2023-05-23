/*****************************************
 * node.hpp
 *
 * JHT, May 22, 2023, Dallas, TX
 *	- created
 *
 * Header file for the beo::Node class,
 *   which manages a set of data. 
 *   This generally is the master task
 *   of a set within the shared group.
 *
 * The purpose of this class is to 
 *   manage the "physical" data 
 *   (in the form of chunks) that 
 *   and actual node in the cluster
 *   has stored in memory or disk 
*****************************************/
#ifndef _BEO_NODE_NODE_HPP_
#define _BEO_NODE_NODE_HPP_

#if defined _BEO_MPI_
    #include <mpi.h>
#endif

#include "../Global/global.hpp"

namespace beo
{

class Node
{
    public:

    protected:

    public:

};

} //end namespace beo

#endif
