/*****************************************
 * chunk.hpp
 *
 * JHT, May 7, 2023, somewhere between 
 *     Athens and Spetses :
 *     created
 *
 * Header file for the beo::chunk class,
 *   which contains information about some chunk of data 
 *   (the smallest unit that beo will manage). 
 *   It is assumed that the data
 *   within this chunk is stored contigously.  
 *
 * Thread safety can be enforeced with 
 * the mutex_t m member, and via the lock() and unlock() functions
 *
 * Note that the constructors often request a reference to a const
 *   "other" chunk, which might seem like it doesn't behave well
 *   with mutex. However, these are const_cast behind the scenes
 *   to allow the correct interfaces with container iterators
 *
 * TODO:
 *    - optimize < operator, since this
 *        will be called a lot
*****************************************/
#ifndef _BEO_DATA_CHUNK_HPP_
#define _BEO_DATA_CHUNK_HPP_

#include <vector>
#include <mutex>
#include <utility>
#include <stdlib.h>
#include <assert.h>


namespace beo
{

class Chunk
{
    public:
        using offsets_t = std::vector<size_t>;

        using lengths_t = std::vector<size_t>;

        using strides_t = std::vector<size_t>;

        using mutex_t   = std::recursive_mutex;

//    friend bool comp(const beo::Chunk& a, const beo::Chunk& b);
        friend bool comp();

        enum class Status {u, f, r, rw, w};

        mutex_t m;

    protected:
        Status status_;

        offsets_t offsets_;

        lengths_t lengths_;

        strides_t strides_;
    
    public:
        //Constructors
        Chunk();

        Chunk(const offsets_t& offsets, 
              const lengths_t& lengths,
              const strides_t& strides);

        Chunk(offsets_t&& offsets, 
              lengths_t&& lengths,
              strides_t&& strides);

        Chunk(const Chunk& other);

        Chunk(Chunk&& other);

        Chunk& operator=(const Chunk& other);

        Chunk& operator=(Chunk&& other);

        //Lock and unlock
        void lock() {m.lock();}

        void unlock() {m.unlock();}
   
        //Status getters and setters
        Status status() const {return status_;}

        void set_status(const Status stat) {status_ = stat;};

        //Dimension info
        const size_t ndim() const {return lengths_.size();}

        const offsets_t& offsets() const {return offsets_;}

        const auto offset(const size_t idx) const {return offsets_[idx];}

        const lengths_t& lengths() const {return lengths_;}
 
        const auto length(const size_t idx) const {return lengths_[idx];}

        const strides_t& strides() const {return strides_;}

        const auto stride(const size_t idx) const {return strides_[idx];}

        const size_t size() const 
        {
            size_t sz = 1;
            for (const auto& len : lengths_) sz *= len;
            return sz;
        };


};

/*****************************************
 * Constructors
 * Threadsafe
*****************************************/

//Empty constructor
Chunk::Chunk() 
{
    std::lock_guard<mutex_t> guard(m);
    status_ = Status::u; 
}

//Constructor from offsets and lengths
Chunk::Chunk(const offsets_t& offsets, 
             const lengths_t& lengths, 
             const strides_t& strides)
{
    std::lock_guard<mutex_t> guard(m);
    status_  = Status::u;
    offsets_ = offsets;
    lengths_ = lengths;
    strides_ = strides;
}

//Constructor from offsets and lengths
Chunk::Chunk(offsets_t&& offsets, 
             lengths_t&& lengths,
             strides_t&& strides)
{
    std::lock_guard<mutex_t> guard(m);
    status_  = Status::u;
    offsets_ = std::move(offsets);
    lengths_ = std::move(lengths);
    strides_ = std::move(strides);
}

//Copy constructor from other chunk
Chunk::Chunk(const Chunk& cother) 
{
    auto& other = const_cast<Chunk&>(cother);
    std::lock_guard<mutex_t> guard1(m);
    std::lock_guard<mutex_t> guard2(other.m);
    status_  = Status::f;
    offsets_ = other.offsets_;
    lengths_ = other.lengths_;
    strides_ = other.strides_;
}

//Move constructor from other chunk
Chunk::Chunk(Chunk&& other) 
{
    std::lock_guard<mutex_t> guard1(m);
    std::lock_guard<mutex_t> guard2(other.m);
    status_  = std::move(other.status_); 
    offsets_ = std::move(other.offsets_);
    lengths_ = std::move(other.lengths_);
    strides_ = std::move(other.strides_);
}

//Copy assignement from other chunk
Chunk& Chunk::operator=(const Chunk& cother)
{
    auto& other = const_cast<Chunk&>(cother);
    std::lock_guard<mutex_t> guard1(m);
    std::lock_guard<mutex_t> guard2(other.m);
    if (&other == this) return *this; 
    status_  = other.status_;
    offsets_ = other.offsets_;
    lengths_ = other.lengths_;
    strides_ = other.strides_;
    return *this;
}

//Move assignment from other chunk
Chunk& Chunk::operator=(Chunk&& other)
{
    std::lock_guard<mutex_t> guard1(m);
    std::lock_guard<mutex_t> guard2(other.m);
    if (&other == this) return *this; 
    status_  = std::move(other.status_);
    offsets_ = std::move(other.offsets_);
    lengths_ = std::move(other.lengths_);
    strides_ = std::move(other.strides_);
    return *this;
}

}//end beo

/*****************************************
 * Comparion operator 
*****************************************/

/*
bool comp(const beo::Chunk& a, const beo::Chunk& b)
{
    assert(a.ndim() == b.ndim()); 

    auto aoff{a.offsets_.cbegin()}; auto boff{b.offsets_.cbegin()};
    auto eoff{a.offsets_.cend()};
    while (aoff < eoff) 
    {
        if (*aoff != *boff) return false;  
        aoff++;
    } 

    return true;
}
*/

#endif
