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

#include <future>

#include "stat.hpp"
#include "comm.hpp"

#include <unistd.h>
namespace beo
{

class Request
{
    public:

        #if defined _BEO_MPI_
        
        using request_t = MPI_Request;

        #else
    
        using request_t = std::future<int>;

        #endif

    protected:

        request_t request_;

    public:

        //Constructors
        Request() {}

        Request(request_t&& other);

        Request(Request&& other);

        //Assignment operators. Note they are ONLY move-able
        Request& operator=(request_t&& other);

        Request& operator=(Request&& other);

        //Access operators
        request_t& request() {return request_;}
        
        bool is_complete();

        bool is_valid();

        int wait();

        void finalize();
};

/*****************************************
 * finalize
 *
 * Finalizes the request object
*****************************************/
void Request::finalize()
{
    wait();

    #if defined _BEO_MPI_

    MPI_Request_free(&request_);

    #endif
}

/*****************************************
 * wait
 *
 * Waits for the request to be complete. 
*****************************************/
int Request::wait()
{
    if (is_valid())
    {
        #if defined _BEO_MPI_


        int res = MPI_Wait(&request_, MPI_STATUS_IGNORE);
        return res == 0 ? BEO_SUCCESS : BEO_FAIL;

        #else

        request_.get();

        return BEO_SUCCESS;

        #endif
    }

    return BEO_SUCCESS;
}

/*****************************************
 * is_complete()
 *
 * returns true if the request is complete
*****************************************/
bool Request::is_complete() 
{
    #if defined _BEO_MPI_
    if (is_valid())
    {
        int tmp;
        MPI_Test(&request_, &tmp, MPI_STATUS_IGNORE);

        return (tmp == 0) ? false : true;
    }
    
    else
    {
        return true;
    } 
   
    #else

    return !request_.valid();

    #endif
}

/*****************************************
 * is_valid()
 *
 * returns true if the request is valid
*****************************************/
bool Request::is_valid()
{
    #if defined _BEO_MPI_

    return MPI_REQUEST_NULL != request_ ? true : false;

    #else

    return request_.valid();

    #endif
}
/*****************************************
 * Constructors
*****************************************/
Request::Request(request_t&& other)
{
    request_ = std::move(other);
}

Request::Request(Request&& other)
{
    if (&other != this) request_ = std::move(other.request_);
}

/*****************************************
 * Assignment operators
*****************************************/
Request& Request::operator=(request_t&& other)
{
    request_ = std::move(other);
    return *this;
}

Request& Request::operator=(Request&& other)
{
    if (&other == this) return *this;
    request_ = std::move(other.request_);
    return *this;
}

}//end namespace beo

#endif
