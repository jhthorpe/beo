/*****************************************
 * utility.hpp
 *
 * JHT, May 28, 2023, Dallas, TX
 *	- created
 *
 * contains utility functions 
*****************************************/
#ifndef _BEO_UTILITY_HPP_
#define _BEO_UTILITY_HPP_

#include <string.h>

#include "def.hpp"

namespace beo
{

bool can_alias(const void*  a, 
               const size_t al, 
               const void*  b, 
               const size_t bl);

int memmove(void*        dest, 
            const void*  src, 
            const size_t len);

size_t calc_alignment(const void* src);

/*****************************************
 * can_alias
 *
 * returns true if buffers a and b (with lengths
 * al and bl) can alias one another
*****************************************/
inline bool can_alias(const void*  a, 
                      const size_t al, 
                      const void*  b, 
                      const size_t bl)
{
    const size_t aa = (size_t) a; 
    const size_t ae = (size_t) aa + al; 
    const size_t bb = (size_t) b; 
    const size_t be = (size_t) bb + bl; 
    return  (  (bb <= aa && aa <= be) || (bb <= ae && ae <= be)
            || (aa <= bb && bb <= ae) || (aa <= be && be <= ae)) ? true : false;
}

/*****************************************
 * memmove
 *
 * Performs the correct memcpy, memmove 
 *   operation for two pointers
*****************************************/
inline int memmove(void*        dest,
                   const void*  src,
                   const size_t len)
{

    if (dest == src) 
    {
    }

    else if (!beo::can_alias(src, len, dest, len))
    {
        memcpy(dest, src, len);
    }

    else 
    {
        memmove(dest, src, len);
    }

    return BEO_SUCCESS;

}
 
/*****************************************
 * calculates alignment of the pointer up
 *   to some power of 2
*****************************************/
inline size_t calc_alignment(const void* src)
{
  size_t alignment = 1;

  //2,4,8,16,32,64,128,256,512
  for (size_t m = 2; m <= 2048; m *= 2)
  {
    if ((size_t) src % m != 0) {return alignment;}
    alignment *= 2;
  }
  return alignment;
}


} //end namespace beo

#endif
