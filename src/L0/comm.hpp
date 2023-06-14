/*****************************************
 * comm.hpp
 * 
 * JHT, May 21, 2023, somewhere between
 *     Tampa and Dallas. 
 * 	- created 
 *
 * Header file for the Comm class, 
 *   which manages and provides 
 *   helper functions for managing 
 *   MPI Communicators 
 *
 * Note that the finalize function MUST 
 *   be called before MPI_Finalize
 *   is called 
*****************************************/
#ifndef _BEO_COMM_HPP_
#define _BEO_COMM_HPP_

#if defined _BEO_MPI_
#include <mpi.h>
#endif

#include <utility>

#include "info.hpp" 

namespace beo
{

class Comm
{
    public:

    protected: 
    
        #if defined _BEO_MPI_

        MPI_Comm comm_{MPI_COMM_NULL};

        #endif

        int num_tasks_{1};

        int task_id_{0}; 

        bool is_master_{true};

    public:

        Comm() {};

       ~Comm() {};

        Comm(const Comm& other);

        Comm(Comm&& other);

        Comm& operator=(const Comm& other);

        Comm& operator=(Comm&& other);

        #if defined _BEO_MPI_

        Comm(const MPI_Comm& other);

        MPI_Comm& operator()() {return comm_;}

        MPI_Comm& comm() {return comm_;}

        Comm& operator=(const MPI_Comm& other);

        #endif

        void finalize();
        
        int num_tasks() const {return num_tasks_;} 

        int task_id() const {return task_id_;}

        bool is_master() const {return is_master_;} 

        #if defined _BEO_MPI_

        Comm&& split(int color, int key) const;

        Comm&& split_type(int type, int key) const;

        Comm&& split_type(int type, int key, const Info& info) const;

        #endif

};

/*****************************************
 * Constructors
*****************************************/
inline Comm::Comm(const Comm& other)
{
    #if defined _BEO_MPI_
    MPI_Comm_dup(other.comm_, &comm_);
    #endif

    num_tasks_ = other.num_tasks_;
    task_id_   = other.task_id_;
    is_master_ = other.is_master_;
}

inline Comm::Comm(Comm&& other)
{
    #if defined _BEO_MPI_
    comm_ = std::move(other.comm_);
    #endif

    num_tasks_ = std::move(other.num_tasks_);
    task_id_   = std::move(other.task_id_);
    is_master_ = std::move(other.is_master_);
}

inline Comm& Comm::operator=(const Comm& other)
{
    if (&other == this) return *this;

    #if defined _BEO_MPI_
    MPI_Comm_dup(other.comm_, &comm_);
    #endif

    num_tasks_ = other.num_tasks_;
    task_id_   = other.task_id_;
    is_master_ = other.is_master_;

    return *this;
}

inline Comm& Comm::operator=(Comm&& other)
{
    if (&other == this) return *this;

    #if defined _BEO_MPI_
    comm_ = std::move(other.comm_);
    #endif

    num_tasks_ = std::move(other.num_tasks_);
    task_id_   = std::move(other.task_id_);
    is_master_ = std::move(other.is_master_);

    return *this;
}

#if defined _BEO_MPI_
inline Comm& Comm::operator=(const MPI_Comm& other)
{
    MPI_Comm_dup(other, &comm_);

    MPI_Comm_size(comm_, &num_tasks_);

    MPI_Comm_rank(comm_, &task_id_);

    task_id_ == 0 ? is_master_ = true : is_master_ = false;

    return *this;
}
#endif

#if defined _BEO_MPI_
inline Comm::Comm(const MPI_Comm& mpi_comm)
{
    MPI_Comm_dup(mpi_comm, &comm_);

    MPI_Comm_size(comm_, &num_tasks_);

    MPI_Comm_rank(comm_, &task_id_);

    task_id_ == 0 ? is_master_ = true : is_master_ = false;
    
}
#endif

/*****************************************
 * split functions
*****************************************/
#if defined _BEO_MPI_
inline Comm&& Comm::split(int color, int key) const
{
    Comm new_comm; 

    MPI_Comm_split(comm_, color, key, &new_comm.comm_);

    return std::move(new_comm);
}
#endif

#if defined _BEO_MPI_
inline Comm&& Comm::split_type(int type, int key) const 
{
    Comm new_comm; 

    MPI_Comm_split_type(comm_, type, key, MPI_INFO_NULL, &new_comm.comm_);

    return std::move(new_comm);
}
#endif

#if defined _BEO_MPI_
inline Comm&& Comm::split_type(int type, int key, const Info& info) const 
{
    Comm new_comm; 

    MPI_Comm_split_type(comm_, type, key, info.info(), &new_comm.comm_);

    return std::move(new_comm);
}
#endif

/*****************************************
 * finalize 
 *
 * call this before the MPI_Finalize 
*****************************************/
inline void Comm::finalize()
{
    #if defined _BEO_MPI_
    if (MPI_COMM_NULL != comm_) MPI_Comm_free(&comm_);
    #endif
}

}//end namespace beo


#endif
