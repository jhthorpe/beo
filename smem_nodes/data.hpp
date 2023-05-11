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
 * the std::mutex m member 
 * 
 * Threadsafe Operations:
 *     Constructors
 *     Destructors
 *
 * Threadsafe Operations:
 *     add_chunk
 *
 * Non-Threadsafe Operations:
 *     name
 *     num_chunks
 *     add_chunk
 *
 * TODO:
 *     - change std::vector to unorderd_map?
*****************************************/
#ifndef _BEO_DATA_HPP_
#define _BEO_DATA_HPP_

#include "chunk.hpp"

#include <string>
#include <vector>
#include <mutex>
#include <utility>
#include <iostream>

namespace beo
{

template<class T>
class Data
{
    public:
    std::mutex m;

    protected:
    std::string name_;
    std::vector<beo::Chunk<T>> chunks_;

    public:
    //Constructors
    Data(const std::string& name);

    //getters
    const std::string& name() const {return name_;}
    size_t num_chunks() const {return chunks_.size();}

    //Adders
    void add_chunk(T& other);
    void add_chunk(T&& other);

    void print() {
        printf("Name: %s\n",name().c_str());
        printf("Num_Chunks: %zu \n", num_chunks());
        for (const auto& chunk : chunks_) 
        {
            printf("Data: ");
            const T& b = chunk.cbase();
            for (const auto& elm : b)
            {
                std::cout << elm << ", ";
            } 
            std::cout << "\n";
        }
    }

};

/*****************************************
 * constructors and destructors
*****************************************/
template <class T>
Data<T>::Data(const std::string& name)
{
    name_ = name; 
}

/*****************************************
 * Adding chunks
*****************************************/
//Copy add
template <class T>
void Data<T>::add_chunk(T& other)
{
    std::lock_guard<std::mutex> guard(m);
    chunks_.push_back(beo::Chunk<T>(other));
}

template <class T>
void Data<T>::add_chunk(T&& other)
{
    std::lock_guard<std::mutex> guard(m);
    chunks_.emplace_back(beo::Chunk<T>(other)); 
}

}//end namespace beo

#endif

