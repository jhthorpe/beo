/*
 *  shared_memory_server.hpp
 * 	JHT, Febuary 27, 2023 : created
 *
 *  header file that contains the shared_memory_server class, which
 *    collects a set of shared_memory_nodes (which contain the data) 
 *    and a shared_memory_manager (which tracks which data is stored on which node)
 */

#ifndef _BEOCOMP_SHARED_MEMORY_SERVER_HPP_
#define _BEOCOMP_SHARED_MEMORY_SERVER_HPP_

#include "shared_memory_node.hpp"
#include "shared_data.hpp"

#include <vector>
#include <string>
#include <unordered_map>

namespace beocomp {

class shared_memory_server {
    protected:
        //List of nodes in the shared memory server
        std::vector<beocomp::shared_memory_node> nodes_;

        //hash map of the data spread on the nodes
        std::unordered_map<std::string,beocomp::shared_data> MAP_;
        
        
    public:
        bool add_shared_data(const std::string& str, const shared_data& data) {
            return MAP_.insert({str,data}).second;
        }
        
        void print() const;

};

}

#endif
