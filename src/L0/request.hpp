/*****************************************
 * request
 *
 * JHT, May 26, 2023, Dallas, TX
 *	- created
 *
 * Header file for the Request class,
 *   which is just a wrapper around
 *   MPI_Request
*****************************************/
#ifndef _BEO_REQUEST_HPP_
#define _BEO_REQUEST_HPP_

#if defined _BEO_MPI_
#include <mpi.h>
#endif

#include "comm.hpp"

namespace beo
{

class Request
{
    public:

    protected:

        #if defined _BEO_MPI_
  
        MPI_Request request_;

        #endif

    public:

        //Constructors
        #if defined _BEO_MPI_

        Request& operator=(const MPI_Request& other);

        Request& operator=(MPI_Request&& other);

        #endif

        Request& operator=(const Request& other);

        Request& operator=(Request&& other);

        #if defined _BEO_MPI_

        //Access operators
        const MPI_Request& request() const {return request_;}
        
        MPI_Request& request() {return request_;}

        MPI_Request& operator() () {return request_;}

        #else

        void request() {};

        void operator() () {};

        #endif

        bool is_complete();

        bool is_valid() const;
};

/*****************************************
 * is_complete()
 *
 * returns true if the request is complete
*****************************************/
bool Request::is_complete() 
{
    #if defined _BEO_MPI_
    int tmp;

    MPI_Test(&request_, &tmp, MPI_STATUS_IGNORE);

    return (tmp == 0) ? false : true;

    #else

    return true;

    #endif
}

/*****************************************
 * is_valid()
 *
 * returns true if the request is valid
*****************************************/
bool Request::is_valid() const
{
    #if defined _BEO_MPI_

    return MPI_REQUEST_NULL != request_ ? true : false;

    #else

    return true;

    #endif
}

/*****************************************
 * Assignment operators
*****************************************/
#if defined _BEO_MPI_
Request& Request::operator=(const MPI_Request& other)
{
    request_ = other;
    return *this;
}
#endif

#if defined _BEO_MPI_
Request& Request::operator=(MPI_Request&& other)
{
    request_ = std::move(other);
    return *this;
}
#endif

#if defined _BEO_MPI_
Request& Request::operator=(const Request& other)
{
    if (&other == this) return *this;
    request_ = other.request_;
    return *this;
}
#endif

#if defined _BEO_MPI_
Request& Request::operator=(Request&& other)
{
    if (&other == this) return *this;
    request_ = std::move(other.request_);
    return *this;
}
#endif


}//end namespace beo

#endif
