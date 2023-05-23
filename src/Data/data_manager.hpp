/*****************************************
 * data_manager.hpp
 *
 * JHT, May 22, 2023, Dallas, TX
 *	- created
 *
 * Header file for the beo::Data_Manager,
 *   which is responsible for managing  
 *   the different beo::Data entities that
 *   beo is responsible for handling
 *
 * Generally, the main operations this thing
 *   will be responsible for is just returning
 *   a data object that already exists. Adding
 *   and removing is rare, so hash table makes
 *   the most sense. Probably could optimize 
 *   based on this, BUT if you're using beo
 *   then checking if the data already exists
 *   within the lookup table is likely far from
 *   your most time-intensive operation
*****************************************/
#ifndef _BEO_DATA_DATA_MANAGER_HPP_
#define _BEO_DATA_DATA_MANAGER_HPP_

#include <unordered_map>
#include <string>
#include <mutex>

#include "data.hpp"

namespace beo
{

class Data_Manager
{
    public:
      
        using mutex_t    = std::recursive_mutex;

        using key_t      = beo::Data::key_t;

        using data_map_t = std::unordered_map<key_t, beo::Data>;

        mutex_t            m;

    protected:

        data_map_t data_map_;
  
    public:

        Data_Manager();

        void lock();

        void unlock();

        const data_map_t& data_map() const {return data_map_;}

        data_map_t& data_map() {return data_map_;}

        //add
        int add(const Data& data);

        int add(Data&& data);

        //remove
        int remove(const beo::Data::key_t& key);  

        //retrieve        
//        auto& get(const beo::Data::key_t& key); 

}; //end class defintion Data_Manager

/*****************************************
 * Data_Manager()
 *
 * Basic initialization
*****************************************/
Data_Manager::Data_Manager() 
{}

/*****************************************
 * lock
 *
 * mutex locks the data_manager
*****************************************/
void Data_Manager::lock()
{ 
    m.lock();

    for (auto& [key, data] : data_map_) data.lock();
}

/*****************************************
 * unlock
 *
 * unlocks the data_manager
*****************************************/
void Data_Manager::unlock()
{
    for (auto& [key, data] : data_map_) data.unlock();

    m.unlock();
}

/*****************************************
 * add 
 *
 * Add a new Data type to Data_Manager
*****************************************/
int Data_Manager::add(const beo::Data& data)
{
    std::lock_guard<mutex_t> guard(m);

    return data_map_.insert({data.name(), data}).second ? 0 : 1;
}

int Data_Manager::add(beo::Data&& data)
{
    std::lock_guard<mutex_t> guard(m);

    return data_map_.emplace(std::make_pair(data.name(), data)).second ? 0: 1;
}


/*****************************************
 * remove
 *
 * Removes a beo::Data entity from 
 * beo::Data_Manager 
*****************************************/
int Data_Manager::remove(const key_t& key)
{
    std::lock_guard<mutex_t> guard(m);
  
    return data_map_.erase(key) == 1 ? 0 : 1;
}

/*****************************************
 * get
 *
 * returns a reference to the beo::Data 
 *   entiry contained.
*****************************************/
/*
auto Data_Manager::get(const beo::Data::key_t& key) 
{
    std::lock_guard<mutex_t> guard(m);
}
*/

} //end namespace beo

#endif
