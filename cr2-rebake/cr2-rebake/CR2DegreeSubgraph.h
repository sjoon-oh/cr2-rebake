// [rebake project] CR2
// author: SukJoon Oh, acoustikue@yonsei.ac.kr
// '21.02.07.
//

//

#pragma once
#include "./CR2Config.h"

#ifdef CR2

// #include "CR2Config.h"
#include "./CR2Type.h"

namespace cr2 {

    class CR2Subgraph {
    protected:
        NODE node_offset[2] = { 0, 0 };
        EDGE edge_offset[2] = { 0, 0 };        
		// IN(0), OUT(1)

    public:
        CR2Subgraph() = default;
        virtual ~CR2Subgraph() { };

        // Interface
        // Getter
        virtual NODE getNodeOffset(unsigned argDir) { return node_offset[argDir]; };
        virtual EDGE getEdgeOffset(unsigned argDir) { return edge_offset[argDir]; };

        virtual void setNodeOffset(unsigned argDir, const NODE argSt) { node_offset[argDir] = argSt; }; 
        virtual void setEdgeOffset(unsigned argDir, const EDGE argSt) { edge_offset[argDir] = argSt; };
        
        void addNodeOffset(unsigned argDir, uint32_t argOffset) { node_offset[argDir] += argOffset; };
        void addEdgeOffset(unsigned argDir, uint32_t argOffset) { edge_offset[argDir] += argOffset; };
    };

    //
    // Children has an additional member, degree.
    class CR2DegreeSubgraph final : public CR2Subgraph {
    private:
        // log_degree stores the value 0 to 5.
		uint32_t log_degree = DEG_1;
        uint32_t num_virtual_nodes[2] = {0, 0}; // INCOMING(0), OUTGOING(1)

    public:
        CR2DegreeSubgraph() = default;
        virtual ~CR2DegreeSubgraph() { };

		template <class T> friend class CR2Cluster;

        // Interface
		uint32_t getDegree() { return log_degree; };
        uint32_t getNumVirtualNodes(unsigned argDir) { return num_virtual_nodes[argDir]; };

        void setDegree(uint32_t argDeg) { log_degree = argDeg; };

        uint32_t& accessNumVirtualNodes(int) = delete; // deprecated

		
    };
}

#endif


