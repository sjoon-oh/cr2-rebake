// [rebake project] CR2
// author: SukJoon Oh, acoustikue@yonsei.ac.kr
// '21.02.07.
//

//

#pragma once
#include "./CR2Config.h"

#ifdef CR2
#include "./CR2Type.h"
#include "./CR2Graph.h"

namespace cr2 {

    // CR2Manager class. Manages the CR2Graph.
    class CR2Manager final {
        private:
            cr2::CR2Graph* cr2_graph = nullptr;
            
            unsigned num_original_nodes = 0; // Number of original graph's nodes
            unsigned num_edges = 0;
            unsigned num_virtual_nodes = 0; // Total number of virtual nodes
            unsigned num_cluster = 0;

            // Core
			uint32_t altCountNodes(const std::vector<cr2::Edge>&);


        public:
            // ctor & dtor
            CR2Manager() = default;
            ~CR2Manager() { this->doReset(); };

            // Interface
            cr2::CR2Graph* doBuild(const std::vector<cr2::Edge>&);
            // unsigned doRegister();
            unsigned doReset();

            // Getter
            const cr2::CR2Graph* getCr2Graph() { return cr2_graph; };
            const unsigned getNumVirtualNodes() { return num_virtual_nodes; };

#ifdef CONSOLE_OUT_ENABLE // Debugging purpose
            void console_out_object_info();
#endif
    };
};

#endif
