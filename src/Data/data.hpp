/*****************************************
 * data.hpp
 *
 * JHT, May 4, 2023 : created
 *
 * Header file that defines the beo::data
 *   class. This class provides a 
 *   wrapper around beo::chunk, which 
 *   helps manage the data for asynch 
 *   operations. 
 * 
 * Thread safety can be enforced via
 * the mutex_t m member, and via the 
 * lock and unlock member functions, 
 * which additionally lock the chunks 
 * contained within 
 * 
 * Note that the copy constructor and assignment
 *   operators take in a reference to a const
 *   "other" beo::Data entity. These are const_cast
 *   behind the scenes to enable thread safety
 * 
 * TODO:
 *     - change std::vector to std::unordered_map? 
*****************************************/
#ifndef _BEO_DATA_DATA_HPP_
#define _BEO_DATA_DATA_HPP_

#include "chunk.hpp"

#include <string>
#include <vector>
#include <set>
#include <mutex>
#include <utility>
#include <iostream>

namespace beo
{

class Data
{
    public:

        using lengths_t   = std::vector<size_t>;

        using mutex_t     = std::recursive_mutex;

        using chunk_set_t = std::vector<beo::Chunk>;

        using key_t       = std::string;

        mutex_t m;

    protected:

        std::string name_;

        lengths_t   lengths_;

        chunk_set_t chunks_;
    
    public:

        //Constructors
        Data();

        Data(const std::string& name);

        Data(std::string&& name);

        Data(const std::string& name, 
             const size_t num);

        Data(std::string&& name, 
             const size_t num);

        Data(const std::string& name, 
             chunk_set_t& chunks);

        Data(std::string&& name, 
             chunk_set_t&& chunks);

        Data(const Data& other);

        Data(Data&& other);

        //Destructors
       ~Data();

        //Assignment
        Data& operator=(const Data& other);

        Data& operator=(Data&& other);

        //Lock and unlock
        void lock();    

        void unlock();

        //getters
        const std::string& name() const {return name_;}

        size_t num_chunks() const {return chunks_.size();}

        size_t ndim() const {return lengths_.size();}

        size_t size() const;

        const chunk_set_t& chunks() const {return chunks_;}

        chunk_set_t& chunks() {return chunks_;} 

        //Iterators
        auto begin() {return chunks_.begin();}

        auto end() {return chunks_.end();}

        auto cbegin() const {return chunks_.cbegin();}

        auto cend() const {return chunks_.cend();}

        //Adders
        void add_chunk(Chunk& other);

        void add_chunk(Chunk&& other);

        void add_chunk(const Chunk::offsets_t& offsets, 
                       const Chunk::lengths_t& lengths,
                       const Chunk::strides_t& strides);

        void add_chunk(Chunk::offsets_t&& offsets, 
                       Chunk::lengths_t&& lengths,
                       Chunk::strides_t&& strides);

        void reserve(const size_t num);

        const key_t& key() const {return name_;}

//        key_t& key() {return name_;}

};

/*****************************************
 * lock and unlock
 * threadsafe
*****************************************/
void Data::lock() 
{
    m.lock();
    auto current = chunks_.begin();
    const auto end = chunks_.end();
    while (current != end)
    {
        current->lock(); 
        current++;
    } 
}

void Data::unlock() 
{
    for (auto& chunk : chunks_) chunk.m.unlock();
    m.unlock();
}

/*****************************************
 * size 
 *   returns the number of elements
*****************************************/
size_t Data::size() const 
{
    size_t sz = 1;
    for (const auto& len : lengths_) sz *= len; 
    return sz;
}

/*****************************************
 * constructors and destructors
 * Threadsafe
*****************************************/
Data::Data() 
{}

Data::Data(const std::string& name)
{
    std::lock_guard<mutex_t> guard(m);
    name_ = name; 
}

Data::Data(std::string&& name)
{
    std::lock_guard<mutex_t> guard(m);
    name_ = std::move(name); 
}

Data::Data(const std::string& name, 
           const size_t num)
{
    std::lock_guard<mutex_t> guard(m);
    name_ = name; 
    reserve(num);
}

Data::Data(std::string&& name, 
           const size_t num)
{
    std::lock_guard<mutex_t> guard(m);
    name_ = std::move(name);
    reserve(num);
}

Data::Data(const std::string& name, 
           chunk_set_t& chunks)
{
    lock();

    name_   = name;
    chunks_ = chunks; 

    unlock();
}

Data::Data(std::string&& name, 
           chunk_set_t&& chunks)
{
    lock(); 

    name_ = std::move(name); 
    chunks_ = std::move(chunks);

    unlock();
}

Data::Data(const Data& cother)
{
    auto& other = const_cast<Data&>(cother); 

    lock();
    other.lock();

    name_    = other.name_;
    lengths_ = other.lengths_;
    chunks_  = other.chunks_;

    other.unlock();
    unlock();
}

Data::Data(Data&& other)
{
    lock();
    other.lock();

    name_    = std::move(other.name_);
    lengths_ = std::move(other.lengths_);
    chunks_  = std::move(other.chunks_);

    other.unlock();
    unlock(); 
}

Data& Data::operator=(const Data& cother)
{
    auto& other = const_cast<Data&>(cother);
    
    std::lock_guard<mutex_t> g1(m);
    std::lock_guard<mutex_t> g2(other.m);

    if (&other == this) return *this;

    lock();
    other.lock();

    name_    = other.name_;
    lengths_ = other.lengths_;
    chunks_  = other.chunks_;

    other.unlock();
    unlock();

    return *this;
}

Data& Data::operator=(Data&& other)
{
    std::lock_guard<mutex_t> g1(m);
    std::lock_guard<mutex_t> g2(other.m);

    if (&other == this) return *this;

    lock();
    other.lock();

    name_    = std::move(other.name_);
    lengths_ = std::move(other.lengths_);
    chunks_  = std::move(other.chunks_);

    other.unlock();
    unlock(); 

    return *this;
}

/*****************************************
 * Destructor
*****************************************/
Data::~Data()
{}

/*****************************************
 * Adding chunks
 * 
 * These are NOT threadsafe, as it is
 * assumed you're generally going to be
 * adding a bunch of chunks at a time,
 * and thus would rather lock and unlock
 * the set of chunks manually
*****************************************/
//Copy add
void Data::add_chunk(beo::Chunk& other)
{
    chunks_.push_back(beo::Chunk(other));
}

void Data::add_chunk(beo::Chunk&& other)
{
    chunks_.emplace_back(std::move(beo::Chunk(other))); 
}

void Data::add_chunk(const beo::Chunk::offsets_t& offsets,
                     const beo::Chunk::lengths_t& lengths,
                     const beo::Chunk::strides_t& strides)
{
    chunks_.push_back(beo::Chunk{offsets,lengths,strides});
}

void Data::add_chunk(beo::Chunk::offsets_t&& offsets,
                     beo::Chunk::lengths_t&& lengths,
                     beo::Chunk::strides_t&& strides)
{
    chunks_.emplace_back(std::move(offsets),
                         std::move(lengths),
                         std::move(strides));
}

void Data::reserve(const size_t num) 
{
    chunks_.reserve(num);
}

}//end namespace beo

#endif

