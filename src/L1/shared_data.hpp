/*****************************************
 * shared_data.hpp
 *
 * JHT, May 31, 2023, Dallas, TX
 *	- created
 *
 * Header file for beo::shared_data, 
 *   which manages and allocates 
 *   shared data on a particular 
 *   communicator. 
 *
*****************************************/
#ifndef _BEO_SHARED_DATA_HPP_
#define _BEO_SHARED_DATA_HPP_

#include <mutex>

#if defined _BEO_MPI_
#include <mpi.h>
#endif

#include "../L0/l0.hpp"
#include "data_tag.hpp"

namespace beo
{

class Shared_Data
{
    public:

    using mutex_t = std::recursive_mutex;

    protected:

    public:

};

} //end namespace beo

#endif
