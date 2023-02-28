/* 
 * shared_memory_server.hpp
 * 	JHt, Febuary 28, 2023 : created
 *
 * .cxx file for the shared_memory_server class, which constructs a beowulf style
 * shared memory server
 */

#include "shared_memory_server.hpp"

namespace beocomp {

std::string sizestr(const std::size_t size) {
    if (size < 1e3)  {return std::to_string(size) + "B  ";}
    if (size < 1e6)  {return std::to_string(int(size/1e3)) + "KB ";}
    if (size < 1e9)  {return std::to_string(int(size/1e6)) + "MB ";}
    if (size < 1e12) {return std::to_string(int(size/1e9)) + "GB ";}
    return std::to_string(size/1e12) + " TB";
};

void shared_memory_server::print() const {
    printf("Data recorderd on Shared Memory Server\n");
    printf("Name    size    chunk_size    chunks\n");
    printf("----------------------------------------------------\n");
    for (const std::pair<std::string,shared_data>& KEYVAL : MAP_) {
        printf("%s     %s    %s    %s \n",
            KEYVAL.first.c_str(),
            sizestr(KEYVAL.second.size()).c_str(),
            sizestr(KEYVAL.second.chunk_size()).c_str(),
            std::to_string(KEYVAL.second.num_chunks()).c_str()
        );
    }
}

}//end namespace beocomp
