/*****************************************
 * enviroment.hpp 
 * 
 * JHT, May 22, 2023, SMU, Dallas, TX
 * 	- created
 *
 * Header file for the beo manager, which
 *   provides the interface between 
 *   the user and the beo internals
 *
 * This class must be initialized after 
 *   MPI_Init is called.
 *
 * Similarly, finalize() should be called 
 *   BEFORE MPI is finalized 
 *
 * Functions contained here:
 *	finalize
 * 
*****************************************/
#ifndef _BEO_ENVIROMENT_HPP_
#define _BEO_ENVIROMENT_HPP_

#include <stdio.h>
#include <string>

#include "../L0/l0.hpp"
#include "comms.hpp"
#include "data_tag_manager.hpp"
#include "files.hpp"

namespace beo
{

class Enviroment
{
    public:

    protected:
 
        Comms            comms_;

        Data_Tag_Manager data_tag_manager_; 

        Files            files_;

    public:

        void finalize();

        void finalize(const int stat, const std::string& message); 

        const Comms& comms() const {return comms_;}

        Comms& comms() {return comms_;}

        const Files& files() const {return files_;}

        Files& files() {return files_;}

        const Data_Tag_Manager& data_tag_manager() const {return data_tag_manager_;}

        Data_Tag_Manager& data_tag_manager() {return data_tag_manager_;}

};

/*****************************************
 * finalize
 *
 * finalizes the beo enviroment. Overloaded
 * to finalize with a status and a message
 * if desired
*****************************************/
void Enviroment::finalize()
{
    files().finalize();

    comms().finalize();
}

void Enviroment::finalize(const int stat, 
                          const std::string& message)
{
    beo::barrier(comms().world());
 
    if (comms().world().is_master())
    {
        printf("beo::%s\nbeo::exiting with status %d\n",message.c_str(),stat);
    }

    finalize();
}



} //end beo namespace

#endif
