// [rebake project] CR2
// author: SukJoon Oh, acoustikue@yonsei.ac.kr
// '21.02.07.
//

//

#pragma once
#include "CR2.h"

#ifdef CR2

#include "CR2Config.h"
#include <iostream>

#include <assert.h>
#include <math.h>

#include "CR2Type.h"
#include "CR2Cluster.h"

namespace cr2 {

    class CR2Subgraph {
    protected:
        cr2::NODE node_offset[2] = { 0, 0 };
        cr2::EDGE edge_offset[2] = { 0, 0 };        

    public:
        CR2Subgraph() = default;
        virtual ~CR2Subgraph() { };

        // Interface
        // Getter
        virtual cr2::NODE getNodeOffset(unsigned argDir) { return node_offset[argDir]; };
        virtual cr2::EDGE getEdgeOffset(unsigned argDir) { return edge_offset[argDir]; };

        virtual void setNodeOffset(unsigned argDir, const cr2::NODE argSt) { node_offset[argDir] = argSt; }; 
        virtual void setEdgeOFfset(unsigned argDir, const cr2::EDGE argSt) { node_offset[argDir] = argSt; };
        
        void addNodeOffset(unsigned argDir, uint32_t argOffset) { node_offset[argDir] += argOffset; };
        void addEdgeOffset(unsigned argDir, uint32_t argOffset) { edge_offset[argDir] += argOffset; };
    };

    //
    // Children has an additional member, degree.
    template <class T>
    class CR2DegreeSubgraph final : public CR2Subgraph {
    private:
        // log_degree stores the value 0 to 5.
        cr2::DEGREE log_degree = cr2::DEGREE::DEG_1;
        uint32_t num_virtual_nodes[2] = {0, 0}; // INCOMING(0), OUTGOING(1)

        //template <class T>
        friend unsigned cr2::CR2Cluster<T>::doRegisterDegreeSubgraphs(); // need to access num_virtual_nodes

    public:
        CR2DegreeSubgraph() = default;
        virtual ~CR2DegreeSubgraph() { };

        // Interface
        unsigned getDegree() { return static_cast<unsigned>(degree); };
        uint32_t getNumVirtualNodes(unsigned argDir) { return num_virtual_nodes[argDir]; };

        void setDegree(cr2::DEGREE argDeg) { degree = argDeg; };       

        uint32_t& accessNumVirtualNodes(int) = delete; // deprecated
    };
}

#endif


