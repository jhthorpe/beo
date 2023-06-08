/*****************************************
 * buffered_data.hpp
 *
 * JHT, May 31, 2023, Dallas, TX
 *	- created
 *
 * Header file for beo::Buffered_Data, 
 *   which manages data that can 
 *   be stored in multiple locations.  
 *
*****************************************/
#ifndef _BEO_BUFFERED_DATA_HPP_
#define _BEO_BUFFERED_DATA_HPP_

#include <mutex>

#if defined _BEO_MPI_
#include <mpi.h>
#endif

#include <unordered_map.hpp>
#include <string>

#include "../L0/l0.hpp"

namespace beo
{

class Buffered_Data
{
    public:

        using name_t  = std::string;

        using map_t   = std::unordered_map<beo::Chunk::key_t, beo::Chunk, beo::Chunk_Hash>; 

        using key_t   = name_t;

        using mutex_t = std::recursive_mutex;

        mutex_t mutex_;

    protected:

        name_t name_;

        map_t  map_;

    public:

        mutex_t& mutex() {return mutex_;} 

        void lock();

        void unlock();


};

} //end namespace beo

#endif
