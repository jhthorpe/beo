/*****************************************
 * node_set.hpp
 * 
 * JHT, May 21, 2023, somewhere between
 *     Tampa and Dallas. 
 * 	- created 
 *
 * Header file for the Node_Set class, 
 *   which contains the MPI info 
 *   for a particular set of MPI
 *   tasks
 *
 * TODO:
 *   - figure out the constructor so that
 *     we can just have move constrution
 *     within Comms
*****************************************/
#ifndef _BEO_NODES_NODE_SET_HPP_
#define _BEO_NODES_NODE_SET_HPP_

#if defined _BEO_MPI_
    #include <mpi.h>
#endif

#include <utility>

namespace beo
{

//If we have MPI
#if defined _BEO_MPI_
class Node_Set
{
    protected: 
    
        MPI_Comm comm_;

        MPI_Info info_;

        int      num_tasks_;

        int      task_id_;

        bool     is_master_;    

    public:

//        Node_Set() {};

//       ~Node_Set();

        // In the MPI case, this must be called 
        // after construction. 
        void init(MPI_Comm&& comm); 

        void finalize();
        
        MPI_Comm& comm() {return comm_;}

        MPI_Info& info() {return info_;}

        int num_tasks() const {return num_tasks_;} 

        int task_id() const {return task_id_;}

        bool is_master() const {return is_master_;} 

};

//If we do not have MPI
#else
class Node_Set
{
    protected:

        int num_tasks_{1};

        int task_id_{1}; 

        bool is_master_{true};

    public:

        Node_Set() {};

       ~Node_Set() {};

        void init() {};

        void finalize() {};
        
        int num_tasks() const {return num_tasks_;} 

        int task_id() const {return task_id_;}

        bool is_master() const {return is_master_;} 

    
};

#endif

//Member functions if with have MPI
#if defined _BEO_MPI_
/*****************************************
 * Constructors with MPI
*****************************************/
//Constructor
void Node_Set::init(MPI_Comm&& comm)
{
    comm_ = std::move(comm); 

    MPI_Info_create(&info_);

    MPI_Comm_size(comm_, &num_tasks_);

    MPI_Comm_rank(comm_, &task_id_);

    task_id_ == 0 ? is_master_ = true : is_master_ = false;
}

/*****************************************
 * Destructors with MPI 
*****************************************/
void Node_Set::finalize()
{
    MPI_Comm_free(&comm_);
}

#endif

}//end namespace beo


#endif
