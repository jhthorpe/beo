/*****************************************
 * chunk_tag.hpp
 *
 * JHT, May 7, 2023, somewhere between 
 *     Athens and Spetses :
 *     created
 *
 * Header file for the beo::chunk_tag class,
 *   which contains information about some chunk of data 
 *   (the smallest unit that beo will manage). 
 *
 * Thread safety can be enforeced with 
 * the mutex_t m member, and via the lock() and unlock() functions
 *
 * Note that the constructors often request a reference to a const
 *   "other" chunk_tag, which might seem like it doesn't behave well
 *   with mutex. However, these are const_cast behind the scenes
 *   to allow the correct interfaces with container iterators
 *
 * Comparison via == is supported, which compares 
 *   ONLY the offsets. The user is expected to handle everything
 *   accordingly 
 *
*****************************************/
#ifndef _BEO_CHUNK_TAG_HPP_
#define _BEO_CHUNK_TAG_HPP_

#include <vector>
#include <mutex>
#include <utility>
#include <stdlib.h>
#include <assert.h>

namespace beo
{

class Chunk_Tag
{
    public:

        using offsets_t = std::vector<size_t>;

        using lengths_t = std::vector<size_t>;

        using mutex_t   = std::recursive_mutex;

        using key_t     = offsets_t;

//    friend bool comp(const beo::Chunk_Tag& a, const beo::Chunk_Tag& b);

        friend bool comp();

//        enum class Location_Status {unassigned, in_memory, on_disk};

//        enum class Access_Status {free, read, write, readwrite};

        mutex_t m;

    protected:

//        Location_Status loc_status_;

//        Access_Status   acc_status_;

        offsets_t       offsets_;

        lengths_t       lengths_;

    public:

        //Constructors
        Chunk_Tag();

        Chunk_Tag(const offsets_t& offsets, 
                  const lengths_t& lengths);

        Chunk_Tag(offsets_t&& offsets, 
                  lengths_t&& lengths);

        Chunk_Tag(const Chunk_Tag& other);

        Chunk_Tag(Chunk_Tag&& other);

        Chunk_Tag& operator=(const Chunk_Tag& other);

        Chunk_Tag& operator=(Chunk_Tag&& other);

        //Lock and unlock
        void lock() {m.lock();}

        void unlock() {m.unlock();}
   
/*
        //Status getters and setters
        const Location_Status& location_status() const {return loc_status_;}

        Location_Status& location_status() {return loc_status_;}

        const Access_Status& access_status() const {return acc_status_;}

        Access_Status& access_status() {return acc_status_;}
*/

        size_t ndim() const {return lengths_.size();}

        const offsets_t& offsets() const {return offsets_;}

        offsets_t& offsets() {return offsets_;}

        auto offset(const size_t idx) const {return offsets_[idx];}

        const lengths_t& lengths() const {return lengths_;}

        lengths_t& lengths() {return lengths_;}
 
        auto length(const size_t idx) const {return lengths_[idx];}

        size_t size() const 
        {
            size_t sz = 1;
            for (const auto& len : lengths_) sz *= len;
            return sz;
        };

        bool operator==(const Chunk_Tag& other);

};

/*****************************************
 * ==
 *
 * compares two chunk_tags by ONLY their
 * offsets
*****************************************/
inline bool Chunk_Tag::operator==(const Chunk_Tag& other)
{
    return offsets() == other.offsets() ? true : false; 
}

/*****************************************
 * Constructors
 * Threadsafe
*****************************************/

//Empty constructor
inline Chunk_Tag::Chunk_Tag() 
{
//    std::lock_guard<mutex_t> guard(m);
//    loc_status_ = Location_Status::unassigned;
//    acc_status_ = Access_Status::free; 
}

//Constructor from offsets and lengths
inline Chunk_Tag::Chunk_Tag(const offsets_t& offsets, 
                     const lengths_t& lengths)
{
    std::lock_guard<mutex_t> guard(m);
//    loc_status_ = Location_Status::unassigned;
//    acc_status_ = Access_Status::free; 
    offsets_    = offsets;
    lengths_    = lengths;
}

//Constructor from offsets and lengths
inline Chunk_Tag::Chunk_Tag(offsets_t&& offsets, 
                     lengths_t&& lengths)
{
    std::lock_guard<mutex_t> guard(m);
//    loc_status_ = Location_Status::unassigned;
//    acc_status_ = Access_Status::free; 
    offsets_    = std::move(offsets);
    lengths_    = std::move(lengths);
}

//Copy constructor from other chunk_tag
inline Chunk_Tag::Chunk_Tag(const Chunk_Tag& cother) 
{
    auto& other = const_cast<Chunk_Tag&>(cother);
    std::lock_guard<mutex_t> guard1(m);
    std::lock_guard<mutex_t> guard2(other.m);
//    loc_status_ = other.loc_status_; 
//    acc_status_ = other.acc_status_; 
    offsets_    = other.offsets_;
    lengths_    = other.lengths_;
}

//Move constructor from other chunk_tag
inline Chunk_Tag::Chunk_Tag(Chunk_Tag&& other) 
{
    std::lock_guard<mutex_t> guard1(m);
    std::lock_guard<mutex_t> guard2(other.m);
//    loc_status_ = std::move(other.loc_status_); 
//    acc_status_ = std::move(other.acc_status_); 
    offsets_    = std::move(other.offsets_);
    lengths_    = std::move(other.lengths_);
}

//Copy assignement from other chunk_tag
inline Chunk_Tag& Chunk_Tag::operator=(const Chunk_Tag& cother)
{
    auto& other = const_cast<Chunk_Tag&>(cother);
    std::lock_guard<mutex_t> guard1(m);
    std::lock_guard<mutex_t> guard2(other.m);
    if (&other == this) return *this; 
//    loc_status_ = other.loc_status_; 
//    acc_status_ = other.acc_status_; 
    offsets_    = other.offsets_;
    lengths_    = other.lengths_;
    return *this;
}

//Move assignment from other chunk_tag
inline Chunk_Tag& Chunk_Tag::operator=(Chunk_Tag&& other)
{
    std::lock_guard<mutex_t> guard1(m);
    std::lock_guard<mutex_t> guard2(other.m);
    if (&other == this) return *this; 
//    loc_status_ = std::move(other.loc_status_); 
//    acc_status_ = std::move(other.acc_status_); 
    offsets_    = std::move(other.offsets_);
    lengths_    = std::move(other.lengths_);
    return *this;
}

}//end beo

#endif
