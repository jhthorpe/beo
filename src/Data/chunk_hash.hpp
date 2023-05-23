/*****************************************
 * chunk_hash.hpp 
 *
 * JHT, May 23, 2023, Dallas, TX
 *	- created
 *
 * Not threadsafe, since you should be 
 * locking these anyways
 *
 * Hash function taken from cppreference.com
*****************************************/
#ifndef _BEO_DATA_CHUNK_HASH_HPP
#define _BEO_DATA_CHUNK_HASH_HPP

#include "chunk.hpp" 

namespace beo{

struct Chunk_Hash
{
    std::size_t operator()(beo::Chunk::offsets_t const& offsets) const noexcept
    {
        std::size_t seed = 0;
        for (const auto& elm : offsets)
        {
            seed ^= std::hash<std::size_t>()(elm);
        }
        return seed;
    }
}; //end Chunk_Hash

} //end namespace beo

#endif
