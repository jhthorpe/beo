/*****************************************
 * chunk.hpp
 *
 * JHT, May 7, 2023, somewhere between 
 *     Athens and Spetses :
 *     created
 *
 *
 * Header file for the beo::chunk class,
 *   which contains some particular 
 *   chunk of data, which is the smallest
 *   unit that beo will manage.
 *
 * Thread safety can be enforeced with 
 * the std::mutex m member
 *
 * Non-threadsafe member functions:
 *     constructors and destructors
 *     status
 *     set_status
 *
 *
 * TODO:
*****************************************/
#ifndef _BEO_CHUNK_HPP_
#define _BEO_CHUNK_HPP_

#include <vector>
#include <mutex>
#include <utility>
#include <stdlib.h>

namespace beo
{

template<class T>
class Chunk
{
    public:
    enum class Status {u, f, r, rw, w};
    std::mutex m;

    protected:
    Status status_;
    T base_; 

    public:
    Chunk();
    Chunk(const T& other);
    Chunk(Chunk<T>& other);
    Chunk(T&& other);
    Chunk(Chunk<T>&& other);
    Chunk<T>& operator=(const T& other);
    Chunk<T>& operator=(Chunk<T>& other);
    Chunk<T>& operator=(T&& other);
    Chunk<T>& operator=(Chunk<T>&& other);

    //get the current status of the base 
    Status status() const {return status_;}
    void set_status(const Status stat) {status_ = stat;};
    
    T& base() {return base_;} 
    const T& cbase() const {return base_;}

};

/*****************************************
 * Constructors
*****************************************/

//Empty constructor
template<class T>
Chunk<T>::Chunk() 
{
    std::lock_guard<std::mutex> guard(m);
    status_ = Status::u; 
}

//Copy constructor from other base type
template<class T>
Chunk<T>::Chunk(const T& other) 
{
    std::lock_guard<std::mutex> guard(m);
    base_   = other; 
    status_ = Status::f;
}

//Copy constructor from other chunk
template<class T>
Chunk<T>::Chunk(Chunk<T>& other) 
{
    std::lock_guard<std::mutex> guard1(m);
    std::lock_guard<std::mutex> guard2(other.m);
    base_   = other.base_; 
    status_ = Status::f;
}

//Move constructor from other base type
template<class T>
Chunk<T>::Chunk(T&& other) 
{
    std::lock_guard<std::mutex> guard(m);
    base_   = std::move(other.base_); 
    status_ = Status::f;
}

//Move constructor from other chunk
template<class T>
Chunk<T>::Chunk(Chunk<T>&& other) 
{
    std::lock_guard<std::mutex> guard1(m);
    std::lock_guard<std::mutex> guard2(other.m);
    base_   = std::move(other.base_); 
    status_ = std::move(other.status_); 
}

//Copy assignment from other base type
template<class T>
Chunk<T>& Chunk<T>::operator=(const T& other)
{
    std::lock_guard<std::mutex> guard1(m);
    base_ = other;
}

//Copy assignement from other chunk
template<class T>
Chunk<T>& Chunk<T>::operator=(Chunk<T>& other)
{
    std::lock_guard<std::mutex> guard1(m);
    std::lock_guard<std::mutex> guard2(other.m);
    base_   = other.base_;
    status_ = other.status_;
}

//Move assignment from other base type
template<class T>
Chunk<T>& Chunk<T>::operator=(T&& other)
{
    std::lock_guard<std::mutex> guard(m);
    base_ = other;
}

//Move assignment from other chunk
template<class T>
Chunk<T>& Chunk<T>::operator=(Chunk<T>&& other)
{
    std::lock_guard<std::mutex> guard1(m);
    std::lock_guard<std::mutex> guard2(other.m);
    base_   = std::move(other.base_);
    status_ = std::move(other.status_);
}


}//end beo

#endif
