#include <stdio.h>
#include <future>
#include <unistd.h>

#define _MPI

#define _BEO_MPI_

#if defined _BEO_MPI_
    #include <mpi.h>
#endif

#include "../include/beo.hpp"

void print_chunk(const beo::Chunk& chunk)
{
    printf("Chunk = [");
    for (const auto elm : chunk.offsets()) printf("%zu, ",elm);
    printf("] | [");
    for (const auto elm : chunk.lengths()) printf("%zu, ",elm);
    printf("] | [");
    for (const auto elm : chunk.strides()) printf("%zu, ",elm);
    printf("]\n");
}

void print_chunks(const beo::Data& data)
{
    for (const auto& [key, chunk] : data.chunks()) print_chunk(chunk);
}

int main()
{
    #if defined _BEO_MPI_
    MPI_Init(NULL,NULL);
    #endif
    
    beo::Manager beo;
    beo.init();

    auto& world_comm = beo.comms().world();
    auto  world_is_master = world_comm.is_master();
    auto  world_id = world_comm.task_id();

    printf("id within world is: %d,%d\n", world_comm.task_id(),world_id);

    auto& data_manager = beo.data_manager();

    beo::Data  data("C");
    beo::Chunk c00{{0,0},{2,2},{1,4}};
    beo::Chunk c20{{2,0},{2,2},{1,4}};
    beo::Chunk c02{{0,2},{2,2},{1,4}};
    beo::Chunk c22{{2,2},{2,2},{1,4}};

    auto t1 = std::async(std::launch::deferred, [&](){
        data.lock(); data.add_chunk(std::move(c00)); printf("added c00\n"); data.unlock(); 
        return 0;
    });

    auto t2 = std::async(std::launch::async, [&](){
        data.lock(); data.add_chunk(std::move(c20)); printf("added c20\n"); data.unlock(); 
        return 0;
    });
   
    auto t3 = std::async(std::launch::async, [&](){
        data.lock(); data.add_chunk(std::move(c02)); printf("added c02\n"); data.unlock(); 
        return 0;
    });

    auto t4 = std::async(std::launch::async, [&](){
        data.lock(); data.add_chunk(std::move(c22)); printf("added c22\n"); data.unlock(); 
        return 0;
    });

    t4.get();
    t2.get();
    t3.get();
    t1.get();

    beo::Data D_data("D"); 
    D_data.add_chunk(std::move(beo::Chunk{{0},{2},{1}}));
    D_data.add_chunk(std::move(beo::Chunk{{2},{1},{1}}));

    data_manager.add(std::move(data));
    data_manager.add(std::move(D_data));
    
    if (world_is_master)
    {
        printf("\nPrinting data within manager:\n");

        data_manager.lock();

        for (const auto& [key, val] : data_manager.data_map())
        {
            printf("Chunks within %s\n",key.c_str());

            print_chunks(val);
            
        }

        data_manager.unlock();
    }

    beo::barrier(world_comm);
    if (world_is_master) printf("\nwe are here\n");

   //Retrieve a particular set of Data
   beo::barrier(world_comm);
   if (world_is_master)
   {
       auto& C_data = data_manager.get("C");
       printf("Chunks within %s\n",C_data.name().c_str());
       print_chunks(C_data);

       printf("\nTrying to find chunk [0,0]\n");
       auto& C_chunk_00 = C_data.get_chunk({0,0});
       print_chunk(C_chunk_00);
   }

   beo::barrier(world_comm);

   if (!world_is_master)
   {
       auto& F_data = data_manager.get("F");
       printf("Chunks within %s\n",F_data.name().c_str());
       print_chunks(F_data);
   }

   beo::barrier(world_comm);

/*
    printf("%s\n", comp(c00, c20) ? "bad\n" : "good\n");
    printf("%s\n", comp(c00, c02) ? "bad\n" : "good\n");
    printf("%s\n", comp(c00, c22) ? "bad\n" : "good\n");
    printf("%s\n", comp(c00, c00) ? "good\n" : "bad\n");
    printf("%s\n", comp(c00, d00) ? "good\n" : "bad\n");
*/

    beo.finalize();

    #if defined _BEO_MPI_
    MPI_Finalize();
    #endif

    return 0; 
}
