
#include <stdio.h>
#include <future>
#include <unistd.h>

#include "../include/beo.hpp"

int main()
{
    beo::Chunk c00{{0,0},{1,1},{1,4}};
    beo::Chunk c20{{2,0},{1,1},{1,4}};
    beo::Chunk c02{{0,2},{1,1},{1,4}};
    beo::Chunk c22{{2,2},{1,1},{1,4}};
    beo::Chunk d00{{0,0},{1,1},{1,4}};

    beo::Data dd("C",4);

    auto t1 = std::async(std::launch::deferred, [&](){
        dd.lock(); dd.add_chunk(c00); printf("added c00\n"); dd.unlock(); 
        return 0;
    });

    auto t2 = std::async(std::launch::async, [&](){
        dd.lock(); dd.add_chunk(c20); printf("added c20\n"); dd.unlock(); 
        return 0;
    });
   
    auto t3 = std::async(std::launch::async, [&](){
        dd.lock(); dd.add_chunk(c02); printf("added c02\n"); dd.unlock(); 
        return 0;
    });

    auto t4 = std::async(std::launch::async, [&](){
        dd.lock(); dd.add_chunk(c22); printf("added c22\n"); dd.unlock(); 
        return 0;
    });

    t4.get();
    t2.get();
    t3.get();
    t1.get();
  
    dd.lock();
    for (const auto& chunk : dd.chunks())
    {
        printf("Chunk = [");
        for (const auto elm : chunk.offsets()) printf("%zu, ",elm);
        printf("] | [");
        for (const auto elm : chunk.lengths()) printf("%zu, ",elm);
        printf("] | [");
        for (const auto elm : chunk.strides()) printf("%zu, ",elm);
        printf("]\n");
    }
    dd.unlock();
    

/*
    printf("%s\n", comp(c00, c20) ? "bad\n" : "good\n");
    printf("%s\n", comp(c00, c02) ? "bad\n" : "good\n");
    printf("%s\n", comp(c00, c22) ? "bad\n" : "good\n");
    printf("%s\n", comp(c00, c00) ? "good\n" : "bad\n");
    printf("%s\n", comp(c00, d00) ? "good\n" : "bad\n");
*/

    return 0; 
}
