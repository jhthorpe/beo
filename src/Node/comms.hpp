/*****************************************
 * comms.hpp
 * 
 * JHT, March 17, 2023, somewhere between
 * Athens Greece and Atlanta Geoergia
 *	- created
 * 
 * Header file for the beo::Comms
 * entity, which manages the (possibly)
 * mpi communicators
*****************************************/
#ifndef _BEO_NODE_COMMS_HPP_
#define _BEO_NODE_COMMS_HPP_

#if defined _BEO_MPI_
    #include <mpi.h>
#endif

#include <utility>
#include <string>

#include "node_set.hpp"

namespace beo
{

//Comms without MPI
class Comms
{
    protected:

        beo::Node_Set world_;

        beo::Node_Set shared_;

    public:

        const Node_Set& world() const {return world_;}

        Node_Set& world() {return world_;}

        const Node_Set& shared() const {return shared_;}

        Node_Set& shared() {return shared_;}

        void init(); 

        void finalize();

};


/*****************************************
 * init
 *
 * Initializes the MPI communicators if
 * _BEO_MPI_ is defined. 
*****************************************/
#if defined _BEO_MPI_
void Comms::init()
{
    MPI_Comm tmp_world;

    MPI_Comm_dup(MPI_COMM_WORLD, &tmp_world);

    world_.init(std::move(tmp_world));

    MPI_Comm tmp_shared;

    MPI_Comm_split_type(world_.comm(),
                        MPI_COMM_TYPE_SHARED,
                        world_.task_id(),
                        world_.info(),
                        &tmp_shared);

    shared_.init(std::move(tmp_shared));
}


#else
void Comms::init() {};

#endif

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
