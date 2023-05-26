/*****************************************
 * chunk_tag_hash.hpp 
 *
 * JHT, May 23, 2023, Dallas, TX
 *	- created
 *
 * Not threadsafe, since you should be 
 * locking these anyways
 *
 * Hash function taken from cppreference.com
*****************************************/
#ifndef _BEO_DATA_CHUNK_TAG_HASH_HPP
#define _BEO_DATA_CHUNK_TAG_HASH_HPP

#include "chunk_tag.hpp" 

namespace beo{

struct Chunk_Tag_Hash
{
    std::size_t operator()(beo::Chunk_Tag::offsets_t const& offsets) const noexcept
    {
        std::size_t seed = 0;
        for (const auto& elm : offsets)
        {
            seed ^= std::hash<std::size_t>()(elm);
        }
        return seed;
    }
}; //end Chunk_Tag_Hash

} //end namespace beo

#endif
