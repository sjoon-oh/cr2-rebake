// [rebake project] CR2
// author: SukJoon Oh, acoustikue@yonsei.ac.kr
// '21.02.07.
//

//

#pragma once
#include "CR2Config.h"

#ifdef CR2

#include <cstdio>
#include <cstdlib>
#include <cmath>

#include "CR2Type.h"
#include "CR2DegreeSubgraph.h"

#include "./util/platform_atomics.h"

#define POW2(X) (uint32_t(pow(2, X)))

namespace cr2 {

    // Community class
    template <class T>
    class CR2Cluster final {
    private:
        // Meta data (level 1)
        uint32_t id = 0;
        uint32_t num_nodes = 0;
        uint32_t num_edges = 0;
        uint32_t num_virtual_nodes[2] = {0, 0}; // Number of total virtual nodes
        
        // Meta data (level 2)
        uint32_t* vertex_degrees[2] = { nullptr, nullptr }; // IN(0), OUT(1)
        
        uint32_t* index_map_pos[2][6]; // former edgePosition 2*2 matrix
        uint32_t* index_map_rem[2][6]; // former remains 2*2 matrix
            // These are 3 Dimensional arrays.
            // First index should be the direction of an edge: IN(0), OUT(1)
            // Second index should be the log_degree: cr2::DEG_1 to cr2::DEG_32
            // Final index is the vertex id. Thus, the overall size will be, 2 * 6 * num_nodes.

        // Data
        // Degree-ordered Subgraph
        cr2::CR2DegreeSubgraph<T> degree_subgraph[6];
            // DEG_1(0), DEG_2(1), DEG_4(2), DEG_8(3), DEG_16(4), DEG_32(5)

        // handle
        T* vertex_list[2] = { nullptr, nullptr };
        T* edge_list[2] = { nullptr, nullptr };

    public:
        // ctor & dtor
        CR2Cluster() = default;
        CR2Cluster(uint32_t, uint32_t, uint32_t);
        ~CR2Cluster();

        // Interface
        // Call in a sequence. 
        // 1. doRegisterSingleDegree (Call with iterations)
        // 2. doBuildDegreeSubgraph
        // 3. doBuildVertexList
        unsigned doRecordSingleDegree(uint32_t, uint32_t);
        unsigned doRecordSingleNeighbor(uint32_t, uint32_t);

        unsigned doBuildDegreeSubgraph();
        unsigned doBuildVertexList();
        unsigned doBuildEdgeList() = delete;

        // Clearing meta data
        unsigned doReleaseLv1() = delete;
        unsigned doReleaseLv2();

        // Getter
        // First arg: argDir (Direction; IN(0) or OUT(1))
        uint32_t getNumNodes() { return this->num_nodes; };
        uint32_t getNumEdges() { return this->num_edges; };

        uint32_t getVertexDegrees(unsigned argDir, uint32_t argVid) { return vertex_degrees[argDir][argVid]; };
        uint32_t getNumVirtualNodes(unsigned argDir) { return num_virtual_nodes[argDir]; }
        
        T* getVertexList(unsigned argDir) { return &vertex_list[argDir]; }
        T* getEdgeList(unsigned argDir) { return &edge_list[argDir]; }

        uint32_t getDegreeSubgraphSize(unsigned argDir,  unsigned argDeg) { 
            return degree_subgraph[argDeg].getNumVirtualNodes(argDir); 
        }
        

#ifdef CONSOLE_OUT_ENABLE // Debugging purpose
        void console_out_object_info();
#endif
    };
};


// Methods definition starts from here.
// ctor
template <class T>
cr2::CR2Cluster<T>::CR2Cluster(uint32_t argId, uint32_t argHead, uint32_t argTail) 
    : id(argId), // This is the cluster id (Community id)
    num_edges(0) { 

    // To all zeros!
    num_virtual_nodes[IN] = num_virtual_nodes[OUT] = 0;
    vertex_list[IN] = vertex_list[OUT] = nullptr;

    for (int log_deg = cr2::DEG_1; log_deg < cr2::DEG_32; log_deg++)
        degree_subgraph[log_deg].setDegree(log_deg);

    // allocate using the size
    num_nodes = argTail - argHead;

    // Each index value represents a vertex id
    // Initialize arrays with all zeros. (Meta data level 2)
    this->vertex_degrees[IN] = new uint32_t[num_nodes]();
    this->vertex_degrees[OUT] = new uint32_t[num_nodes]();

    for (int log_deg = cr2::DEG_1; log_deg < cr2::DEG_32; log_deg++) {

        index_map_pos[IN][log_deg] = new uint32_t[num_nodes]();
        index_map_pos[OUT][log_deg] = new uint32_t[num_nodes]();

        index_map_rem[IN][log_deg] = new uint32_t[num_nodes]();
        index_map_rem[OUT][log_deg] = new uint32_t[num_nodes]();
    }

};

template <class T>
cr2::CR2Cluster<T>::~CR2Cluster() {

    delete[] this->vertex_degrees[IN];
    delete[] this->vertex_degrees[OUT];

    if (vertex_list[IN] != nullptr) delete[] vertex_list[IN];
    if (vertex_list[OUT] != nullptr) delete[] vertex_list[OUT];

    for (unsigned log_deg = cr2::DEG_1; log_deg <= cr2::DEG_32; log_deg++ {
        for (unsigned direction = IN; direction <= OUT; direction++) {
            if ( index_map_pos[direction][log_deg] != nullptr ) delete[] index_map_pos[direction][log_deg]
            if ( index_map_rem[direction][log_deg] != nullptr ) delete[] index_map_rem[direction][log_deg]
        }
    }
};

template <class T>
unsigned cr2::CR2Cluster<T>::doReleaseLv2() {

    for (unsigned log_deg = cr2::DEG_1; log_deg <= cr2::DEG_32; log_deg++ {
        for (unsigned direction = IN; direction <= OUT; direction++) {

            delete[] index_map_pos[direction][log_deg];
            delete[] index_map_rem[IdirectionN][log_deg];

            index_map_pos[direction][log_deg] = nullptr;
            index_map_rem[direction][log_deg] = nullptr;
        }
    }

    return 0;
}


// Interface
// To fully function, the api should be called in a sequence.
// 
// 1. Call doRecordSingleDegree in iteration to count vertices' incoming/outgoing degrees.
// 2. Then call doBuildDegreeSubgraph to generate metadata for the community
// 3. The next call should be doBuildVertexList to generate vertex_list!! 
// (former denseVertexIDList_in _out, sparseVetexIdList_in _out)
// 4. Call doRecordSingleNeighbor in iteration to generate edge_list!! 
// (former denseEdgeList_in _out, sparseEdgeList_in _out)

//
// Registers single degree. It receives single edge, (src, dst) for its argument.
template <class T>
unsigned cr2::CR2Cluster<T>::doRecordSingleDegree(uint32_t argSrc, uint32_t argDst) {

    fetch_and_add(vertex_degrees[IN][argDst], 1);
    fetch_and_add(vertex_degrees[OUT][argSrc], 1);

    return 0;
};

template <class T>
unsigned cr2::CR2Cluster<T>::doRecordSingleNeighbor(uint32_t argSrc, uint32_t argDst) {

    // For a given degree,
    for (unsigned log_deg = cr2::DEG_32; log_deg >= cr2::DEG_1; log_deg--) {
        // and with an incoming/outgoing direction,
        if (index_map_rem[IN][log_deg][argDst] > 0) { // only if the remain is positive,

            edge_list[IN][ index_map_pos[IN][log_deg][] ] = argSrc;
            index_map_pos[IN][log_deg][argDst]++;
            index_map_rem[IN][log_deg][argDst]--;

            break;
        }

        if (index_map_rem[OUT][log_deg][argSrc] > 0) { // only if the remain is positive,

            edge_list[OUT][ index_map_pos[OUT][log_deg][] ] = argDst;
            index_map_pos[OUT][log_deg][argSrc]++;
            index_map_rem[OUT][log_deg][argSrc]--;

            break;
        }
    }

    return 0;
}


//
// Generates DegreeSubgraph information. Example: Size, offset etc.
template <class T>
unsigned cr2::CR2Cluster<T>::doBuildDegreeSubgraph() {

    //
    // 1st Step. Prepare the degreed subgraph.
    // There are six Degreed subgraphs for a single cluster(community). 
    #pragma omp parallel for
    for (int vertex_id = 0; vertex_id < this->num_nodes; vertex_id++) {
        // num_nodes refers to the cluster size; the number of nodes within a cluster.

        // Temporary values!!
        // First option: IN(0), OUT(1)
        // Second option: DEG_1(0), DEG_2(1), DEG_4(2), DEG_8(3), DEG_16(4), DEG_32(5)
        uint32_t num_nodes_of[2][6];
        uint32_t num_remains_of[2][6];

        num_nodes_of[IN][cr2::DEG_32] = vertex_degrees[IN][vertex_id] / 32; // Number of 32-degreed vertex
        num_remains_of[IN][cr2::DEG_32] = vertex_degrees[IN][vertex_id] % 32; 

        num_nodes_of[OUT][cr2::DEG_32] = vertex_degrees[OUT][vertex_id] / 32;
        num_remains_of[OUT][cr2::DEG_32] = vertex_degrees[OUT][vertex_id] % 32;

        for (unsigned log_deg = cr2::DEG_16; cr2::DEG_1 <= log_deg; log_deg--) {
            // Keep dividing and store remains to propagate to the next degree-array.
            
            num_nodes_of[IN][log_deg] = num_remains_of[IN][log_deg + 1] / POW2(log_deg);
            num_nodes_of[IN][log_deg] = num_remains_of[IN][log_deg + 1] % POW2(log_deg);
            
            num_nodes_of[OUT][log_deg] = num_remains_of[OUT][log_deg + 1] / POW2(log_deg);
            num_nodes_of[OUT][log_deg] = num_remains_of[OUT][log_deg + 1] % POW2(log_deg);
        } // So far, we have recorded the number of size of DegreeSubgraph.
        
        // This is the key point of all these shits.
        // Record the number of virtual nodes to each DegreeSubgraph objects.
        for (unsigned log_deg = cr2::DEG_1; log_deg <= cr2::DEG_32; log_deg++) {
            if (num_nodes_of[IN][log_deg] != 0)
                fetch_and_add(
                    degree_subgraph[log_deg].num_virtual_nodes[IN], num_nodes[IN][log_deg] );

            if (num_nodes_of[OUT][log_deg] != 0)
                fetch_and_add(
                    degree_subgraph[log_deg].num_virtual_nodes[OUT], num_nodes[OUT][log_deg] );
        }
    }

    //
    // 2nd Step.
    { // Originally, PartialGraph::calculatePrefixSum()
        uint32_t node_current[2] = { 0, };
        uint32_t edge_current[2] = { 0, };

        // Record the starting point of each degree-subgraphs.
        // Recall that the structures share the same single target array: 
        //     vertex_list - The ultimate purpose of all these shits.
        for (unsigned log_deg = cr2::DEG_1; log_deg <= cr2::DEG_32; log_deg++) {

            degree_subgraph[log_deg].setNodeOffset(IN, node_current[IN]); // former prefix sum
            degree_subgraph[log_deg].setEdgeOffset(IN, edge_current[IN]);

            degree_subgraph[log_deg].setNodeOffset(OUT, node_current[OUT]);
            degree_subgraph[log_deg].setEdgeOffset(OUT, edge_current[OUT]);

            node_current[IN] += degree_subgraph[log_deg].getNumVirtualNodes(IN);
            edge_current[IN] += 
                (degree_subgraph[log_deg].getNumVirtualNodes(IN) * POW2(log_deg));
            
            node_current[OUT] += degree_subgraph[log_deg].getNumVirtualNodes(OUT);
            edge_current[OUT] += 
                (degree_subgraph[log_deg].getNumVirtualNodes(OUT) * POW2(log_deg));
        }
    } // PartialGraph::calculatePrefixSum() done.
    
    // Bonus Step. Record additional information such as number of virtual nodes etc.
    // Refactored - Originally countNumOfTotalEdges and countNumOfTotalVirtualNodes from PartialGraph.
    for (unsigned log_deg = cr2::DEG_1; log_deg <= cr2::DEG_32; log_deg++) {

        this->num_edges 
            += (degree_subgraph->num_virtual_nodes[IN] * POW2(log_deg));

        this->num_virtual_nodes[IN] += degree_subgraph->accessNumVirtualNodes(IN);
        this->num_virtual_nodes[OUT] += degree_subgraph->accessNumVirtualNodes(OUT);

        // Note that num_virtual_nodes from CR2DegreeSubgraph refers to 
        // the number that belongs to each CR2DegreeSubgraph object,
        // and the num_virtual_nodes from CR2Cluster refers to 
        // the number of total virtual nodes in the CR2Cluster.
    }

    return 0;
}


//
// Refactored version of PartialGraph::buildVertexIDList() (and bulildVertexIDList_in or _out). 
template <class T>
unsigned cr2::CR2Cluster<T>::doBuildVertexList() { // Third step!!

    // Prepare to fight!
    this->vertex_list[IN] = new T[num_edges];
    this->vertex_list[OUT] = new T[num_edges];

    // Originally, PartialGraph::buildVertexIDList_in() or _out()
    // 1. For a specific vertex,
    for (unsigned vertex_id = 0; vertex_id < num_nodes; vertex_id++) {
        uint32_t current_remain = vertex_degrees[direction][vertex_id];

        // 2. with a given direction,
        for (int direction = cr2::IN; direction <= cr2::OUT; direction++) {
            
            // 3. and only if the vertex has non-zero degree
            if (vertex_degrees[direction][vertex_id] != 0) {
                
                // 4. iterate through the degree.
                for (unsigned log_deg = cr2::DEG_32; log_deg >= cr2::DEG_1; log_deg--) {
                    if (current_remain >= POW2(log_deg)) {

                        // For a vertex, number of virtual nodes can be obtained via division of 2^deg
                        // The starting point of the corresponding(degree) index were saved beforehand,
                        uint32_t num_virtual_nodes_s = current_remain / POW2(log_deg);
                        uint32_t p_start = degree_subgraph[log_deg].getNodeOffset(direction);
                        uint32_t p_end = pstart + num_virtual_nodes_s;
                        
                        // Thus, for the region between p_start ~ p_end should the vertex_id be saved.
                        for (uint32_t pos = p_start; pos < p_end; pos++)
                            vertex_list[direction][pos] = T(vertex_id);
                        
                        // map_XX series are index maps for buildNeighborList. 
                        this->index_map_pos[direction][log_deg][vertex_id]
                            = degree_subgraph[log_deg].getEdgeOffset(direction);
                        this->index_map_rem[direction][log_deg][vertex_id]
                            = (num_virtual_nodes_s * POW2(log_deg));

                        // Alter the node_offset and edge_offset for each degree-ordered subgraphs.
                        degree_subgraph[log_deg].addNodeOffset(
                            direction, num_virtual_nodes_s
                        );
                        degree_subgraph[log_deg].addEdgeOffset(
                            direction, (num_virtual_nodes_s * POW2(log_deg))
                        );
                    }

                    current_remain = current_remain % degree;
                }
            }            
        }
    }

    return 0;
}


#ifdef CONSOLE_OUT_ENABLE // Debugging purpose
template <class T>
void cr2::CR2Cluster<T>::console_out_object_info() {

    printf("[cr2::CR2Cluster]\n");
    printf("    id: %d\n", id);
    printf("    num_nodes: %d\n", this->num_nodes);
    printf("    num_edges: %d\n", this->num_edges);
    printf("    num_virtual_nodes: %d\n", this->num_virtual_nodes);

    printf("    vertex_degrees[IN]:%d\n", this->vertex_degrees);
    for (unsigned i = 0; i < num_nodes; i++) { printf("        %5d", this->vertex_degrees[IN][i]); }

    printf("    vertex_degrees[OUT]:%d\n", this->vertex_degrees);
    for (unsigned i = 0; i < num_nodes; i++) { printf("        %5d", this->vertex_degrees[OUT][i]); }
}
#endif
#endif

