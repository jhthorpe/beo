/*****************************************
 * data_tag_manager.hpp
 *
 * JHT, May 22, 2023, Dallas, TX
 *	- created
 *
 * Header file for the beo::Data_Tag_Manager,
 *   which is responsible for managing  
 *   the different beo::Data_Tag entities that
 *   beo is responsible for handling
 *
 * Generally, the main operations this thing
 *   will be responsible for is just returning
 *   a data_tag object that already exists. Adding
 *   and removing is rare, so hash table makes
 *   the most sense. Probably could optimize 
 *   based on this, BUT if you're using beo
 *   then checking if the data_tag already exists
 *   within the lookup table is likely far from
 *   your most time-intensive operation
*****************************************/
#ifndef _BEO_DATA_TAG_MANAGER_HPP_
#define _BEO_DATA_TAG_MANAGER_HPP_

#include <unordered_map>
#include <string>
#include <mutex>

#include "../L1/data_tag.hpp"

namespace beo
{

class Data_Tag_Manager
{
    public:
      
        using mutex_t        = std::recursive_mutex;

        using key_t          = beo::Data_Tag::key_t;

        using data_tag_map_t = std::unordered_map<key_t, beo::Data_Tag>;

        mutex_t m;

    protected:

        data_tag_map_t data_tag_map_;
  
    public:

        Data_Tag_Manager();

        void lock();

        void unlock();

        const data_tag_map_t& data_tag_map() const {return data_tag_map_;}

        data_tag_map_t& data_tag_map() {return data_tag_map_;}

        //add
        int add(const Data_Tag& data_tag);

        int add(Data_Tag&& data_tag);

        //remove
        int remove(const beo::Data_Tag::key_t& key);  

        //retrieve        
        auto& get(const beo::Data_Tag::key_t& key); 

}; //end class defintion Data_Tag_Manager

/*****************************************
 * Data_Tag_Manager()
 *
 * Basic initialization
*****************************************/
Data_Tag_Manager::Data_Tag_Manager() 
{}

/*****************************************
 * lock
 *
 * mutex locks the data_tag_manager
*****************************************/
void Data_Tag_Manager::lock()
{ 
    m.lock();

    for (auto& [key, data_tag] : data_tag_map_) data_tag.lock();
}

/*****************************************
 * unlock
 *
 * unlocks the data_tag_manager
*****************************************/
void Data_Tag_Manager::unlock()
{
    for (auto& [key, data_tag] : data_tag_map_) data_tag.unlock();

    m.unlock();
}

/*****************************************
 * add 
 *
 * Add a new Data_Tag type to Data_Tag_Manager
 *
 * Returns 0 if the data_tag was added correctly,
 * 1 otherwise
*****************************************/
int Data_Tag_Manager::add(const beo::Data_Tag& data_tag)
{
    std::lock_guard<mutex_t> guard(m);

    return data_tag_map_.insert({data_tag.name(), data_tag}).second ? 0 : 1;
}

int Data_Tag_Manager::add(beo::Data_Tag&& data_tag)
{
    std::lock_guard<mutex_t> guard(m);

    return data_tag_map_.emplace(std::make_pair(data_tag.name(), data_tag)).second ? 0: 1;
}

/*****************************************
 * remove
 *
 * Removes a beo::Data_Tag entity from 
 * beo::Data_Tag_Manager 
 *
 * Returns 0 if no error, 1 if error
*****************************************/
int Data_Tag_Manager::remove(const key_t& key)
{
    std::lock_guard<mutex_t> guard(m);
  
    return data_tag_map_.erase(key) == 1 ? 0 : 1;
}

/*****************************************
 * get
 *
 * returns a reference to the beo::Data_Tag 
 *   entiry contained.
 *
 * 
*****************************************/
auto& Data_Tag_Manager::get(const beo::Data_Tag::key_t& key) 
{
    std::lock_guard<mutex_t> guard(m);

    auto itr = data_tag_map_.find(key);

    if (itr != data_tag_map_.end())
    {
        return itr->second;
    }

    else
    {
        printf("\nbeo::error\n");                
        printf("Could not find data_tag %s on Data_Tag_Manager\n",key.c_str());
        exit(1);
    } 

}

} //end namespace beo

#endif
