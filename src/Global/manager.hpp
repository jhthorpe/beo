/*****************************************
 * manager.hpp
 * 
 * JHT, May 22, 2023, SMU, Dallas, TX
 * 	- created
 *
 * Header file for the beo manager, which
 *   provides the interface between 
 *   the user and the beo internals
 *
 * For practical reasons, the init() 
 *   function is provided to actually 
 *   initialize the beo manager --
 *   this should be called AFTER MPI
 *   is initialized if you indend to use
 *   mpi, with _BEO_MPI_ defined. 
 *
 * Similarly, finalize() should be called 
 *   BEFORE MPI is finalized 
 *
 * Functions contained here:
 * 	init
 *	finalize
 * 
*****************************************/
#ifndef _BEO_GLOBAL_MANAGER_HPP_
#define _BEO_GLOBAL_MANAGER_HPP_

#include <stdio.h>
#include <string>

#include "global.hpp"
#include "../Node/node_inc.hpp"
#include "../Data/data_inc.hpp"

namespace beo
{

class Manager
{
    public:

    protected:
 
        Comms comms_;

    public:

        void init();

        void finalize();

        void finalize(const int stat, const std::string& message); 

        const Comms& comms() const {return comms_;}

        Comms& comms() {return comms_;}

};

/*****************************************
 * init
 *
 * initializes the beo enviroment
*****************************************/
void Manager::init()
{
    comms().init();
}

/*****************************************
 * finalize
 *
 * finalizes the beo enviroment. Overloaded
 * to finalize with a status and a message
 * if desired
*****************************************/
void Manager::finalize()
{
    comms().finalize();
}

void Manager::finalize(const int stat, 
                       const std::string& message)
{
    beo::barrier(comms().world());
 
    if (comms().world().is_master())
    {
        printf("beo::%s\nbeo::exiting with status %d\n",message.c_str(),stat);
    }
    comms().finalize();
}



} //end beo namespace

#endif
