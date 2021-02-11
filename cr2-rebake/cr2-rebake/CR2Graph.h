// [rebake project] CR2
// author: SukJoon Oh, acoustikue@yonsei.ac.kr
// '21.02.07.
//

//

#pragma once
#include "CR2Config.h"

#ifdef CR2

#include <vector>
#include <cstdio>

#include "CR2Type.h"
#include "CR2Cluster.h"

namespace cr2 {

    // CR2Graph class is just a container.
    // 
    class CR2Graph final {
        private:
            // Meta data
            uint32_t num_nodes = 0; // number of nodes
            uint32_t num_edges = 0; // numver of edges

            uint32_t num_cluster = 0; // number of clusters
            uint32_t num_intra_cluster = 0; // number of intra cluster

            // Graph information
            std::vector<CR2Cluster<cr2::DENSE_TYPE>*> intra_cluster;
            cr2::CR2Cluster<cr2::SPARSE_TYPE>* inter_cluster = nullptr; // Fixed to have only one.

			uint32_t* degree_list[2] = {nullptr, nullptr};

            // Alt
        public:
            // ctor & dtor
            CR2Graph() = default;
            ~CR2Graph();

            // Interface
			// Main!!
            unsigned doRegisterCluster(uint32_t, uint32_t);
            unsigned doRegisterEdge(const std::vector<cr2::Edge>&);

			unsigned doBuildDegreeList(unsigned);

            unsigned doRelease();
            

            //
            // Setter
            unsigned setNumNodes(uint32_t argNodes) { num_nodes = argNodes; };
            unsigned setNumEdges(uint32_t argEdges) { num_edges = argEdges; };
            
            // unsigned setNumCluster(uint32_t argNumCluster) { num_cluster = argNumCluster; };
            unsigned setNumIntraCluster(uint32_t argNumIC) { num_intra_cluster = argNumIC; };

            // Getter
            uint32_t getNumNodes() { return num_nodes; };
            uint32_t getNumEdges() { return num_edges; };

            uint32_t getNumCluster() { return num_cluster; };
            uint32_t getNumIntraCluster() { return num_intra_cluster; };
	
            uint32_t getNumIntraClusterEdges() {
				uint32_t sum = 0;
				for (auto& it : intra_cluster) sum += it->getNumEdges();
				return sum;
            };
			uint32_t getNumInterClusterEdges() {
				return this->inter_cluster->getNumEdges();
			}

			uint32_t getNumIntraClusterVirtualNodes(unsigned argDir) {
				uint32_t sum = 0;
				for (auto& it : intra_cluster) sum += it->getNumVirtualNodes(argDir);
				return sum;
			}
			uint32_t getNumInterClusterVirtualNodes(unsigned argDir) {
				this->inter_cluster->getNumVirtualNodes(argDir);
			}

			uint32_t* getDegreeList(unsigned argDir) {
				return this->degree_list[argDir];
			}

#ifdef CONSOLE_OUT_ENABLE // Debugging purpose
            void console_out_object_info();
#endif
    };
};

#endif