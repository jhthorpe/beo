/*****************************************
 * files.hpp 
 *
 * JHT, May 31, 2023, Dallas, TX
 *	- created
 *
 * Header file for the beo::Files
 *   class, which manages files for 
 *   the user
*****************************************/
#ifndef _BEO_FILES_HPP_
#define _BEO_FILES_HPP_

#include <unordered_map>
#include <string>
#include <mutex>

#include "../L0/shared_file.hpp"

namespace beo
{

class Files 
{
    public:
      
        using mutex_t    = std::recursive_mutex;

        using key_t      = beo::Shared_File::key_t;

        using file_t     = beo::Shared_File;

        using file_map_t = std::unordered_map<key_t, file_t>; 


    protected:

        file_map_t file_map_;

        mutex_t mutex_;
  
    public:

        Files();

//        Files& operator=(const Files& other);

        //Mutex control
        void lock();

        void unlock();

        mutex_t& mutex() { return mutex_; }

        //File map access
        const file_map_t& file_map() const {return file_map_;}

        file_map_t& file_map() {return file_map_;}

        //add
        int add(Shared_File&& shared_file);

        //remove
        int remove(const beo::Shared_File::key_t& key);  

        //retrieve        
        auto& get(const beo::Shared_File::key_t& key); 

        //finalize
        void finalize();

        //Number of files
        size_t num_files() {return file_map().size();}

}; //end class defintion Files

/*****************************************
 * Files()
 *
 * Basic initialization
*****************************************/
inline Files::Files() 
{}

/*****************************************
 * lock
 *
 * mutex locks the shared_file_manager
*****************************************/
inline void Files::lock()
{ 
    mutex().lock();

    for (auto& [key, shared_file] : file_map_) shared_file.lock();
}

/*****************************************
 * unlock
 *
 * unlocks the shared_file_manager
*****************************************/
inline void Files::unlock()
{
    for (auto& [key, shared_file] : file_map_) shared_file.unlock();

    mutex().unlock();
}

/*****************************************
 * add 
 *
 * Add a new Shared_File type to Files. 
 *   Note that this can only be "moved"
 *
 * Returns 0 if the shared_file was added correctly,
 * 1 otherwise
*****************************************/
inline int Files::add(beo::Shared_File&& shared_file)
{
    std::lock_guard<mutex_t> guard(mutex());

    return file_map_.emplace(std::make_pair(shared_file.name(), shared_file)).second ? 0: 1;
}
/*****************************************
 * get
 *
 * returns a reference to the beo::Shared_File 
 *   entiry contained.
 *
 * 
*****************************************/
inline auto& Files::get(const beo::Shared_File::key_t& key) 
{
    std::lock_guard<mutex_t> guard(mutex());

    auto itr = file_map_.find(key);

    if (itr != file_map_.end())
    {
        return itr->second;
    }

    else
    {
        printf("beo::Files::get Could not find shared_file %s on Files\n", 
               key.c_str());
        exit(1);
    } 
}


/*****************************************
 * remove
 *
 * Removes a beo::Shared_File entity from 
 * beo::Files 
 *
 * Returns 0 if no error, 1 if error
*****************************************/
inline int Files::remove(const key_t& key)
{
    std::lock_guard<mutex_t> guard(mutex());

    auto& file = Files::get(key);

    if (file.is_open()) file.close();
  
    return file_map_.erase(key) == 1 ? 0 : 1;
}


/*****************************************
 * finalize
 *
 * this must be called BEFORE the communicators
 * used in the construction of the files are terminated
*****************************************/
inline void Files::finalize()
{
    lock(); 

    for (auto [key, val] : file_map_)
    {
        if (val.is_open()) val.close();
    }

    unlock();
}

} //end namespace beo

#endif
