/*****************************************
 * data_tag.hpp
 *
 * JHT, May 4, 2023 : created
 *
 * Header file that defines the beo::data_tag
 *   class. This class provides a 
 *   wrapper around beo::chunk_tag, which 
 *   helps manage the data_tag for asynch 
 *   operations. 
 * 
 * Thread safety can be enforced via
 *   the mutex_t m member, and via the 
 *   lock and unlock member functions, 
 *   which additionally lock the chunk_tags 
 *   contained within. 
 * 
 * Note that the copy constructor and assignment
 *   operators take in a reference to a const
 *   "other" beo::Data_Tag entity. These are const_cast
 *   behind the scenes to enable thread safety
 * 
*****************************************/
#ifndef _BEO_DATA_TAG_HPP_
#define _BEO_DATA_TAG_HPP_

#include "../L0/chunk_tag.hpp"
#include "../L0/chunk_tag_hash.hpp"

#include <unordered_map>
#include <string>
#include <vector>
#include <set>
#include <mutex>
#include <utility>
#include <iostream>

namespace beo
{

class Data_Tag
{
    public:

        using lengths_t   = std::vector<size_t>;

        using mutex_t     = std::recursive_mutex;
  
        using chunk_tag_map_t = std::unordered_map<beo::Chunk_Tag::key_t, beo::Chunk_Tag, beo::Chunk_Tag_Hash>;

        using key_t       = std::string;

        mutex_t m;

    protected:

        std::string name_;

        lengths_t   lengths_;

        chunk_tag_map_t chunk_tags_;
    
    public:

        //Constructors
        Data_Tag();

        Data_Tag(const std::string& name);

        Data_Tag(std::string&& name);

        Data_Tag(const std::string& name, 
                 const size_t num);

        Data_Tag(std::string&& name, 
                 const size_t num);

        Data_Tag(const std::string& name, 
                 chunk_tag_map_t& chunk_tags);

        Data_Tag(std::string&& name, 
                 chunk_tag_map_t&& chunk_tags);

        Data_Tag(const Data_Tag& other);

        Data_Tag(Data_Tag&& other);

        //Destructors
       ~Data_Tag();

        //Assignment
        Data_Tag& operator=(const Data_Tag& other);

        Data_Tag& operator=(Data_Tag&& other);

        //Lock and unlock
        void lock();    

        void unlock();

        //getters
        const std::string& name() const {return name_;}

        size_t num_chunk_tags() const {return chunk_tags_.size();}

        size_t ndim() const {return lengths_.size();}

        size_t size() const;

        const chunk_tag_map_t& chunk_tags() const {return chunk_tags_;}

        chunk_tag_map_t& chunk_tags() {return chunk_tags_;} 

        //Iterators
        auto begin() {return chunk_tags_.begin();}

        auto end() {return chunk_tags_.end();}

        auto cbegin() const {return chunk_tags_.cbegin();}

        auto cend() const {return chunk_tags_.cend();}

        //Adders
        void add_chunk_tag(const Chunk_Tag& other);

        void add_chunk_tag(Chunk_Tag&& other);

        void add_chunk_tag(const Chunk_Tag::offsets_t& offsets, 
                           const Chunk_Tag::lengths_t& lengths);

        void add_chunk_tag(Chunk_Tag::offsets_t&& offsets, 
                           Chunk_Tag::lengths_t&& lengths);

        void reserve(const size_t num);

        const key_t& key() const {return name_;}

        //remove chunk_tag
        void remove_chunk_tag(const Chunk_Tag::offsets_t& offsets);

        //retrive a chunk_tag
        auto& get_chunk_tag(const Chunk_Tag::offsets_t& offsets);

};

/*****************************************
 * Remove a chunk_tag
 * 
 * threadsafe
*****************************************/
inline void Data_Tag::remove_chunk_tag(const Chunk_Tag::offsets_t& offsets)
{
    std::lock_guard<mutex_t> guard(m);

    chunk_tags_.erase(offsets);
}


/*****************************************
 * Retrieve a chunk_tag
*****************************************/
inline auto& Data_Tag::get_chunk_tag(const Chunk_Tag::offsets_t& offsets)
{
    std::lock_guard<mutex_t> guard(m);

    auto itr = chunk_tags_.find(offsets);

    if (itr != chunk_tags_.end())
    {
        return itr->second;
    }

    else
    {
        std::string offstr = "[";
        for (const auto elm : offsets)
        {
            offstr += std::to_string(elm);
            offstr += ", ";
        } 
        offstr += "]";
        printf("\nbeo::error - Could not find chunk_tag %s\n", offstr.c_str());
        exit(1);
    }
}


/*****************************************
 * lock and unlock
 * threadsafe
*****************************************/
inline void Data_Tag::lock() 
{
    m.lock();

    for (auto& [key, chunk_tag] : chunk_tags_) chunk_tag.lock();
}

inline void Data_Tag::unlock() 
{
    for (auto& [key, chunk_tag] : chunk_tags_) chunk_tag.unlock();

    m.unlock();
}

/*****************************************
 * size 
 *   returns the number of elements
*****************************************/
inline size_t Data_Tag::size() const 
{
    size_t sz = 1;
    for (const auto& len : lengths_) sz *= len; 
    return sz;
}

/*****************************************
 * constructors and destructors
 * Threadsafe
*****************************************/
inline Data_Tag::Data_Tag() 
{}

inline Data_Tag::Data_Tag(const std::string& name)
{
    std::lock_guard<mutex_t> guard(m);
    name_ = name; 
}

inline Data_Tag::Data_Tag(std::string&& name)
{
    std::lock_guard<mutex_t> guard(m);
    name_ = std::move(name); 
}

inline Data_Tag::Data_Tag(const std::string& name, 
                   const size_t num)
{
    std::lock_guard<mutex_t> guard(m);
    name_ = name; 
    reserve(num);
}

inline Data_Tag::Data_Tag(std::string&& name, 
                   const size_t num)
{
    std::lock_guard<mutex_t> guard(m);
    name_ = std::move(name);
    reserve(num);
}

inline Data_Tag::Data_Tag(const std::string& name, 
                   chunk_tag_map_t& chunk_tags)
{
    lock();

    name_   = name;
    chunk_tags_ = chunk_tags; 

    unlock();
}

inline Data_Tag::Data_Tag(std::string&& name, 
                   chunk_tag_map_t&& chunk_tags)
{
    lock(); 

    name_ = std::move(name); 
    chunk_tags_ = std::move(chunk_tags);

    unlock();
}

inline Data_Tag::Data_Tag(const Data_Tag& cother)
{
    auto& other = const_cast<Data_Tag&>(cother); 

    lock();
    other.lock();

    name_    = other.name_;
    lengths_ = other.lengths_;
    chunk_tags_  = other.chunk_tags_;

    other.unlock();
    unlock();
}

inline Data_Tag::Data_Tag(Data_Tag&& other)
{
    lock();
    other.lock();

    name_    = std::move(other.name_);
    lengths_ = std::move(other.lengths_);
    chunk_tags_  = std::move(other.chunk_tags_);

    other.unlock();
    unlock(); 
}

inline Data_Tag& Data_Tag::operator=(const Data_Tag& cother)
{
    auto& other = const_cast<Data_Tag&>(cother);
    
    std::lock_guard<mutex_t> g1(m);
    std::lock_guard<mutex_t> g2(other.m);

    if (&other == this) return *this;

    lock();
    other.lock();

    name_    = other.name_;
    lengths_ = other.lengths_;
    chunk_tags_  = other.chunk_tags_;

    other.unlock();
    unlock();

    return *this;
}

inline Data_Tag& Data_Tag::operator=(Data_Tag&& other)
{
    std::lock_guard<mutex_t> g1(m);
    std::lock_guard<mutex_t> g2(other.m);

    if (&other == this) return *this;

    lock();
    other.lock();

    name_    = std::move(other.name_);
    lengths_ = std::move(other.lengths_);
    chunk_tags_  = std::move(other.chunk_tags_);

    other.unlock();
    unlock(); 

    return *this;
}

/*****************************************
 * Destructor
*****************************************/
inline Data_Tag::~Data_Tag()
{}

/*****************************************
 * Adding chunk_tags
 * 
 * These are NOT threadsafe, as it is
 * assumed you're generally going to be
 * adding a bunch of chunk_tags at a time,
 * and thus would rather lock and unlock
 * the set of chunk_tags manually
*****************************************/
//Copy add
inline void Data_Tag::add_chunk_tag(const beo::Chunk_Tag& cother)
{
    auto& other = const_cast<beo::Chunk_Tag&>(cother);  

    std::lock_guard<mutex_t> g1(m);  
    std::lock_guard<mutex_t> g2(other.m);  

    chunk_tags_.insert({other.offsets(), other});
}

inline void Data_Tag::add_chunk_tag(beo::Chunk_Tag&& other)
{
    std::lock_guard<mutex_t> g1(m); 

    chunk_tags_.emplace(std::make_pair(other.offsets(), other)); 
}

inline void Data_Tag::add_chunk_tag(const beo::Chunk_Tag::offsets_t& offsets,
                             const beo::Chunk_Tag::lengths_t& lengths)
{
    chunk_tags_.insert({offsets, beo::Chunk_Tag{offsets,lengths}});
}

inline void Data_Tag::add_chunk_tag(beo::Chunk_Tag::offsets_t&& offsets,
                             beo::Chunk_Tag::lengths_t&& lengths)
{
    chunk_tags_.emplace(std::make_pair(offsets, 
                                       beo::Chunk_Tag{std::move(offsets),
                                                      std::move(lengths)}));
}

inline void Data_Tag::reserve(const size_t num) 
{
    chunk_tags_.reserve(num);
}

}//end namespace beo

#endif

