/*****************************************
 * comms.hpp
 * 
 * JHT, March 17, 2023, somewhere between
 * Athens Greece and Atlanta Geoergia
 *	- created
 * 
 * Header file for the beo::Comms
 *   entity, which manages the set of 
 *   beo::Comm entities managed within
 *   the beo enviroment  
 *
 * Note that this must be initialized 
 *   after MPI_Init
 *
*****************************************/
#ifndef _BEO_COMMS_HPP_
#define _BEO_COMMS_HPP_

#if defined _BEO_MPI_
#include <mpi.h>
#endif

#include <utility>
#include <string>

#include "../L0/info.hpp"
#include "../L0/comm.hpp"

namespace beo
{

//Comms without MPI
class Comms
{
    protected:

        beo::Comm world_;

        beo::Comm shared_;

    public:

        Comms();

        Comms(const Info& info);

        const Comm& world() const {return world_;}

        Comm& world() {return world_;}

        const Comm& shared() const {return shared_;}

        Comm& shared() {return shared_;}

        void finalize();
};


/*****************************************
 * Constructors 
*****************************************/
Comms::Comms(const Info& info)
{

    #if defined _BEO_MPI_
    world_ = MPI_COMM_WORLD;

    shared_ = std::move(world_.split_type(MPI_COMM_TYPE_SHARED,
                                          world_.task_id(),
                                          info));
    #else

    shared_ = world_;

    #endif
}

Comms::Comms()
{

    #if defined _BEO_MPI_
    world_ = MPI_COMM_WORLD;

    shared_ = std::move(world_.split_type(MPI_COMM_TYPE_SHARED,
                                          world_.task_id(),
                                          MPI_INFO_NULL));
    #else

    shared_ = world_;

    #endif
}


/*****************************************
 * finalization 
 *
 * Finalizes the communicators. 
*****************************************/
void Comms::finalize()
{
    shared_.finalize();
    world_.finalize();
}

}  //end namespace beo

#endif
