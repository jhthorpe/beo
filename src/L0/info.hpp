/*****************************************
 * info.hpp 
 * 
 * JHT, May 21, 2023, somewhere between
 *     Tampa and Dallas. 
 * 	- created 
 *
 * Header file for the Info class, 
 *   which manages and provides 
 *   helper functions for managing 
 *   MPI_Info 
 *
 * Note that the finalize function MUST 
 *   be called before MPI_Finalize
 *   is called 
*****************************************/
#ifndef _BEO_INFO_HPP_
#define _BEO_INFO_HPP_

#if defined _BEO_MPI_
#include <mpi.h>
#endif

#include <utility>

namespace beo
{

class Info
{
    public:

    protected: 
    
        #if defined _BEO_MPI_

        MPI_Info info_{MPI_INFO_NULL};

        #endif

    public:

        Info() {};

        Info(const Info& other);

        Info(Info&& other);

        Info& operator=(const Info& other);

        Info& operator=(Info&& other);

        #if defined _BEO_MPI_

        Info(const MPI_Info& other);

        Info(MPI_Info&& other); 

        MPI_Info& operator()() {return info_;}

        const MPI_Info& info() const {return info_;}

        MPI_Info& info() {return info_;}

        #endif

        void finalize();
        
};

/*****************************************
 * Constructors
*****************************************/
inline Info::Info(const Info& other)
{
    #if defined _BEO_MPI_
    MPI_Info_dup(other.info_, &info_);
    #endif
}

inline Info::Info(Info&& other)
{
    #if defined _BEO_MPI_
    info_ = std::move(other.info_);
    #endif
}

inline Info& Info::operator=(const Info& other)
{
    if (&other == this) return *this;

    #if defined _BEO_MPI_
    MPI_Info_dup(other.info_, &info_);
    #endif

    return *this;
}

inline Info& Info::operator=(Info&& other)
{
    if (&other == this) return *this;

    #if defined _BEO_MPI_
    info_ = std::move(other.info_);
    #endif

    return *this;
}

#if defined _BEO_MPI_
inline Info::Info(MPI_Info&& other)
{
    info_ = std::move(other);
}
#endif

/*****************************************
 * finalize 
 *
 * call this before the MPI_Finalize 
*****************************************/
inline void Info::finalize()
{
    #if defined _BEO_MPI_
    if (MPI_INFO_NULL != info_) MPI_Info_free(&info_);
    #endif
}

}//end namespace beo


#endif
