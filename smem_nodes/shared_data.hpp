/* 
 *  shared_data.hpp
 * 	JHT, Feb 27, 2023 : created
 * 
 *  header file for the shared_data struct, which just contains information about 
 *    a piece of data registered as shared
 *
 *  In effect, this is simply a collection of some metadata
 */	

#ifndef _BEOCOMP_SHARED_DATA_HPP_
#define _BEOCOMP_SHARED_DATA_HPP_

#include <string>

namespace beocomp {

class shared_data {
    protected:
 //       std::type_info type_; 		//type of data
        std::size_t len_;  		//number of elements
        std::size_t chunk_len_; 	//number of elements per chunk
        std::size_t num_chunks_; 	//number of total chunks
        std::size_t base_size_;		//number of bytes for a unit of data
        std::size_t size_; 		//size of data in bytes
        std::size_t chunk_size_; 	//maximum chunk size in bytes
//        std::vector<beocomp::shared_mem_ids> node_list_; //list of nodes the chunks reside on 

    public:
//        shared_data(const std::type_info type, const std::size_t base_size, const std::size_t len, const std::size_t chunk_len) :
//            type_{type}, 
        shared_data(const std::size_t base_size, const std::size_t len, const std::size_t chunk_len) :
            len_{len}, 
            base_size_{base_size},
            size_{base_size*len}, 
            chunk_size_{chunk_len*base_size},
            num_chunks_{std::size_t(len/chunk_len) + std::size_t(len%chunk_len)} {}

//        std::type_info type()    const {return type_;}
        std::size_t len()        const {return len_;}
        std::size_t chunk_len()  const {return chunk_len_;}
        std::size_t num_chunks() const {return num_chunks_;}
        std::size_t size()       const {return size_;}
        std::size_t chunk_size() const {return chunk_size_;}
        std::size_t base_size()  const {return base_size_;}

};

}
#endif
