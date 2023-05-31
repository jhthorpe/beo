#include <mpi.h>
#include <stdio.h>
#include <vector>
#include <string>

#define _BEO_MPI_
#include "/Users/37348458/beocomp/include/beo.hpp"

//This example demonstrates how the "easy" MPI calls don't work for a shared
//  File. One could make a new file for each task, but ultimately most supercomputers
//  use parallel filesystems anyways, these days.

int main()
{
    MPI_Init(NULL, NULL);

    //Basic beo enviroment stuff
    beo::Enviroment beo;
    auto& world = beo.comms().world();
    auto task_id = world.task_id(); 
    auto num_tasks = world.num_tasks();

    //Buffers we might want to read/write
    std::vector<size_t> out {(size_t) task_id, (size_t) task_id, (size_t) task_id};
    std::vector<size_t> in(out.size());

    //Generating the file
    beo::Shared_File file("foo"); 
    auto stat = file.open(world, "w+");
    if (stat != BEO_SUCCESS) {printf("bad file open\n"); exit(1);}

    beo::barrier(world); 

    //-----------------------------------------------------------------------
    //Generate offsets for the writing (unique for each task)
    size_t bytes = out.size() * sizeof(size_t);
    size_t off1 = bytes * task_id;
    size_t off2 = bytes * (task_id + num_tasks);

    //-----------------------------------------------------------------------
    // Example of asyncronous, collective write
    auto write_req = file.async_write_at_all(off2, out.data(), bytes); 

    //-----------------------------------------------------------------------
    // Example of (independent) writing 
    file.write_at(off1, out.data(), out.size() * sizeof(size_t));

    printf("Task %d saw position %lld \n", task_id, file.get_pos());

    beo::barrier(world);

    file.read_at(off1, in.data(), in.size()*sizeof(size_t));
    printf("Task %d saw %zu %zu %zu for in\n",task_id, in[0], in[1], in[2]);

    //-----------------------------------------------------------------------
    // Async collective read
    write_req.wait();

    auto read_req = file.async_read_at_all(off2, out.data(), bytes);

    read_req.wait();
    printf("Task %d saw %zu %zu %zu for async in\n", task_id, in[0], in[1], in[2]);

    file.close();

    beo.finalize();

    MPI_Finalize();

}
