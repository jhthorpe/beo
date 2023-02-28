#ifndef _BEOCOMP_SHARED_MEMORY_NODE_HPP_
#define _BEOCOMP_SHARED_MEMORY_NODE_HPP_

namespace beocomp {

class shared_memory_node {
    protected:
        int ID_;         

    public:
        int ID() const {return ID_;}

};

}

#endif
