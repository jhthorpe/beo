#include <stdio.h>
#include <future>
#include <unistd.h>

#define _BEO_MPI_

#if defined _BEO_MPI_
#include <mpi.h>
#endif

#include "../include/beo.hpp"

void print_chunk_tag(const beo::Chunk_Tag& chunk_tag)
{
    printf("Chunk_Tag = [");
    for (const auto elm : chunk_tag.offsets()) printf("%zu, ",elm);
    printf("] | [");
    for (const auto elm : chunk_tag.lengths()) printf("%zu, ",elm);
    printf("] | [");
    for (const auto elm : chunk_tag.strides()) printf("%zu, ",elm);
    printf("]\n");
}

void print_chunk_tags(const beo::Data_Tag& data_tag)
{
    for (const auto& [key, chunk_tag] : data_tag.chunk_tags()) print_chunk_tag(chunk_tag);
}

int main()
{
    #if defined _BEO_MPI_
    MPI_Init(NULL,NULL);
    #endif
    
    beo::Enviroment beo;

    auto& world_comm = beo.comms().world();
    auto  world_is_master = world_comm.is_master();
    auto  world_id = world_comm.task_id();

    printf("id within world is: %d,%d\n", world_comm.task_id(),world_id);

    auto& data_tag_manager = beo.data_tag_manager();

    beo::Data_Tag  data_tag("C");
    beo::Chunk_Tag c00{{0,0},{2,2},{1,4}};
    beo::Chunk_Tag c20{{2,0},{2,2},{1,4}};
    beo::Chunk_Tag c02{{0,2},{2,2},{1,4}};
    beo::Chunk_Tag c22{{2,2},{2,2},{1,4}};

    auto t1 = std::async(std::launch::deferred, [&](){
        data_tag.lock(); data_tag.add_chunk_tag(std::move(c00)); printf("added c00\n"); data_tag.unlock(); 
        return 0;
    });

    auto t2 = std::async(std::launch::async, [&](){
        data_tag.lock(); data_tag.add_chunk_tag(std::move(c20)); printf("added c20\n"); data_tag.unlock(); 
        return 0;
    });
   
    auto t3 = std::async(std::launch::async, [&](){
        data_tag.lock(); data_tag.add_chunk_tag(std::move(c02)); printf("added c02\n"); data_tag.unlock(); 
        return 0;
    });

    auto t4 = std::async(std::launch::async, [&](){
        data_tag.lock(); data_tag.add_chunk_tag(std::move(c22)); printf("added c22\n"); data_tag.unlock(); 
        return 0;
    });

    t4.get();
    t2.get();
    t3.get();
    t1.get();

    beo::Data_Tag D_data_tag("D"); 
    D_data_tag.add_chunk_tag(std::move(beo::Chunk_Tag{{0},{2},{1}}));
    D_data_tag.add_chunk_tag(std::move(beo::Chunk_Tag{{2},{1},{1}}));

    data_tag_manager.add(std::move(data_tag));
    data_tag_manager.add(std::move(D_data_tag));
    
    if (world_is_master)
    {
        printf("\nPrinting data_tag within manager:\n");

        data_tag_manager.lock();

        for (const auto& [key, val] : data_tag_manager.data_tag_map())
        {
            printf("Chunk_Tags within %s\n",key.c_str());

            print_chunk_tags(val);
            
        }

        data_tag_manager.unlock();
    }

    beo::barrier(world_comm);
    if (world_is_master) printf("\nwe are here\n");

   //Retrieve a particular set of Data_Tag
   beo::barrier(world_comm);
   if (world_is_master)
   {
       auto& C_data_tag = data_tag_manager.get("C");
       printf("Chunk_Tags within %s\n",C_data_tag.name().c_str());
       print_chunk_tags(C_data_tag);

       printf("\nTrying to find chunk_tag [0,0]\n");
       auto& C_chunk_tag_00 = C_data_tag.get_chunk_tag({0,0});
       print_chunk_tag(C_chunk_tag_00);
   }

   beo::barrier(world_comm);

   if (!world_is_master)
   {
       auto& F_data_tag = data_tag_manager.get("F");
       printf("Chunk_Tags within %s\n",F_data_tag.name().c_str());
       print_chunk_tags(F_data_tag);
   }

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
