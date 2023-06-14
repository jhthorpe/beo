/*****************************************
 * chunk.hpp
 *
 * JHT, May 31, 2023, Dallas, TX
 * 	- created
 *
 * Header file for the beo::chunk class,
 *   which is a combination of a chunk_tag and a pointer to the data
 *
 * Thread safety can be enforeced with 
 * the mutex_t m member, and via the lock() and unlock() functions
 *
 * Note that the constructors often request a reference to a const
 *   "other" chunk, which might seem like it doesn't behave well
 *   with mutex. However, these are const_cast behind the scenes
 *   to allow the correct interfaces with container iterators
 *
 * Comparison via == is supported, which compares 
 *   ONLY the offsets. The user is expected to handle everything
 *   accordingly 
 *
 * TODO
 *     - provide functionality to make this a mmap call instead
 *       of malloc?
 *
*****************************************/
#ifndef _BEO_CHUNK_HPP_
#define _BEO_CHUNK_HPP_

#include <vector>
#include <mutex>
#include <utility>
#include <stdlib.h>
#include <assert.h>

#include "def.hpp"
#include "utility.hpp"
#include "chunk_tag.hpp"
#include "chunk_tag_hash.hpp"

namespace beo
{

class Chunk
{
    public:

        using offsets_t = beo::Chunk_Tag::offsets_t; 

        using lengths_t = beo::Chunk_Tag::lengths_t;

        using mutex_t   = std::recursive_mutex;

        using key_t     = offsets_t;

        mutex_t m;

    protected:
    
        Chunk_Tag  chunk_tag_;

        size_t     alignment_{0};

        size_t     bytes_{0};

        void*      data_{nullptr}; 

    public:

        Chunk();

        Chunk(const Chunk& other);

        Chunk(Chunk&& other);

        Chunk(const Chunk_Tag& other);

        Chunk(Chunk_Tag&& other);

        Chunk& operator=(const Chunk& other);

        Chunk& operator=(Chunk&& other);

       ~Chunk();

        //Lock tag and data 
        void lock() {m.lock(); chunk_tag_.lock();}

        //Unlock tag and data
        void unlock() {chunk_tag_.unlock(); m.unlock();}

        //Referances to the tag
        const Chunk_Tag& tag() const {return chunk_tag_;}

        Chunk_Tag& tag() {return chunk_tag_;}

        //Pointer to the data
        const void* data() const {return data_;}

        void* data() {return data_;}
       
        //Location status of chunk
//        const Chunk_Tag::Location_Status& location_status() const {return tag().location_status();}

//        Chunk_Tag::Location_Status& location_status() {return tag().location_status();}

        //Access status of the chunk
//        const Chunk_Tag::Access_Status& access_status() const {return tag().access_status();}

//        Chunk_Tag::Access_Status& access_status() {return tag().access_status();}

        //Number of dimensions
        size_t ndim() const {return tag().ndim();}

        const offsets_t& offsets() const {return tag().offsets();}

        offsets_t& offsets() {return tag().offsets();}

        auto offset(const size_t idx) const {return tag().offset(idx);}

        const lengths_t& lengths() const {return tag().lengths();}

        lengths_t& lengths() {return tag().lengths();}
 
        auto length(const size_t idx) const {return tag().length(idx);}

        size_t size() const 
        {
            size_t sz = 1;
            for (const auto len : lengths()) sz *= len;
            return sz;
        };

        bool operator==(const Chunk& other);

        int allocate(const size_t bytes);

        int aligned_allocate(const size_t alignment, const size_t bytes);

        int free();

        bool is_allocated() {return (nullptr != data_) ? true : false;}

        size_t alignment() const {return alignment_;}

};

/*****************************************
 * allocate
*****************************************/
inline int Chunk::allocate(const size_t bytes)
{
    std::lock_guard<mutex_t> g(m);

    if (is_allocated()) return BEO_FAIL;

    data_ = (void*) malloc(bytes); 

    if (nullptr != data_)
    {
        bytes_     = bytes;
        alignment_ = calc_alignment(data_);
        return BEO_SUCCESS;
    }

    else
    {
        return BEO_FAIL;
    }

}

/*****************************************
 * aligned_allocate
*****************************************/
inline int Chunk::aligned_allocate(const size_t alignment, const size_t bytes)
{
    std::lock_guard<mutex_t> g(m);

    if (is_allocated()) return BEO_FAIL;

    data_ = (void*) aligned_alloc(alignment, bytes); 

    if (nullptr != data_)
    {
        bytes_     = bytes;
        alignment_ = alignment;
        return BEO_SUCCESS;
    }

    else
    {
        return BEO_FAIL;
    }

}

/*****************************************
 * free
*****************************************/
inline int Chunk::free()
{
    std::lock_guard<mutex_t> g(m);

    if (!is_allocated()) std::free(data_); 

    data_      = nullptr;
    bytes_     = 0; 
    alignment_ = 0;

    return BEO_SUCCESS;
}

/*****************************************
 * ==
 *
 * compares two chunks by ONLY their
 * offsets
*****************************************/
inline bool Chunk::operator==(const Chunk& other)
{
    return offsets() == other.offsets() ? true : false; 
}

/*****************************************
 * Destructor
*****************************************/
inline Chunk::~Chunk()
{
    std::lock_guard<mutex_t> guard(m);

    if (is_allocated()) free();
}
/*****************************************
 * Constructors
 * Threadsafe
*****************************************/

//Empty constructor
inline Chunk::Chunk() 
{
    std::lock_guard<mutex_t> guard(m);

    alignment_ = 0;
    bytes_     = 0;
    data_      = nullptr;
}

//Copy constructor from other chunk_tag
//This does not initialize the memory
inline Chunk::Chunk(const Chunk_Tag& cother)
{
    std::lock_guard<mutex_t> g1(m);
    auto& other = const_cast<Chunk_Tag&>(cother);
    std::lock_guard<mutex_t> g2(m);

    chunk_tag_ = other;
}

//Copy constructor from other chunk
//When possible, avoid this, as it must do a
//   deep copy
inline Chunk::Chunk(const Chunk& cother) 
{
    std::lock_guard<mutex_t> guard1(m);
    auto& other = const_cast<Chunk&>(cother);
    std::lock_guard<mutex_t> guard2(other.m);

    chunk_tag_ = other.chunk_tag_;

    if (other.is_allocated())
    {
        if (is_allocated()) free(); 

        aligned_allocate(other.alignment_, other.bytes_); 

        beo::memmove(data_, other.data_, bytes_);
    } 
}

//Move constructor from other Chunk
inline Chunk::Chunk(Chunk&& other) 
{
    std::lock_guard<mutex_t> guard1(m);
    std::lock_guard<mutex_t> guard2(other.m);

    chunk_tag_ = std::move(other.chunk_tag_);
    alignment_ = std::move(other.alignment_);
    bytes_     = std::move(other.bytes_);
    data_      = std::move(other.data_);
}

//Move constructor from other Chunk_Tag
//This does not initialize any memory
inline Chunk::Chunk(Chunk_Tag&& other) 
{
    std::lock_guard<mutex_t> guard1(m);
    std::lock_guard<mutex_t> guard2(other.m);

    chunk_tag_ = std::move(other);
}

//Copy assignement from other chunk
//  When possible, avoid doing this with 
//  allocated data, as this will perform 
//  a deep copy
inline Chunk& Chunk::operator=(const Chunk& cother)
{
    auto& other = const_cast<Chunk&>(cother);
    std::lock_guard<mutex_t> guard1(m);
    std::lock_guard<mutex_t> guard2(other.m);

    if (&other == this) return *this; 

    chunk_tag_ = other.chunk_tag_;
    
    if (other.is_allocated())
    {
        if (is_allocated()) free();
        data_ = (void*) std::aligned_alloc(other.alignment_, other.bytes_);

        if (nullptr == data_) return *this;

        beo::memmove(data_, other.data_, bytes_);
    } 
    
    return *this;
}

//Move assignment from other chunk
inline Chunk& Chunk::operator=(Chunk&& other)
{
    std::lock_guard<mutex_t> guard1(m);
    std::lock_guard<mutex_t> guard2(other.m);

    if (&other == this) return *this; 

    chunk_tag_ = std::move(other.chunk_tag_);
    alignment_ = std::move(other.alignment_);
    bytes_     = std::move(other.bytes_);
    data_      = std::move(other.data_);

    return *this;
}

}//end namespace beo

#endif
