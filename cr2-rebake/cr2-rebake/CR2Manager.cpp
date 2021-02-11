// [rebake project] CR2
// author: SukJoon Oh, acoustikue@yonsei.ac.kr
// '21.02.07.
//

#include "./CR2Manager.h"
#include "./util/platform_atomics.h"

#include <algorithm>



//
// Finds the maximum vertex id, which becomes the (total number of edges - 1).
// arg: const std::vector<cr2::Edge>&
unsigned cr2::CR2Manager::altCountNodes(const std::vector<cr2::Edge>& argEdgeList) {
    unsigned num_nodes = 0;

    #pragma omp parallel for reduction(max : num_nodes)
    for (auto it: argEdgeList) {
        num_nodes = std::max(num_nodes, it.getSrcVertex());
        num_nodes = std::max(num_nodes, it.getDstVertex());
    }

    return 0;
};


//
// Generates GR2Graph and returns the graph.
// arg: const std::vector<cr2::Edge>&
// return: cr2::CR2Graph*
cr2::CR2Graph* cr2::CR2Manager::doBuild(const std::vector<cr2::Edge>& argEdgeList) {

    if (this->cr2_graph != nullptr) doReset(); // unlink the existing graph
    this->cr2_graph  = new cr2::CR2Graph(); // New dummy
    
    // Finding the maximum vertex id of the input graph
    this->num_original_nodes = this->altCountNodes(argEdgeList) + 1;
    this->num_edges = argEdgeList.size(); // Record the number of edges (original input graph)

    // Calculate the number of community
    // Partition the nodes using the community size
    this->num_cluster = 
        (num_original_nodes % CLUSTER_SIZE != 0) ? 
            num_original_nodes / CLUSTER_SIZE + 1 :
            num_original_nodes / CLUSTER_SIZE;

    // cr2_graph->setNumNodes(num_original_nodes);
    // automatically records num_nodes of cr2_graph.
    cr2_graph->setNumEdges(num_edges);
    cr2_graph->setNumIntraCluster(num_cluster);

    // Generate the range of communities, not yet graph inserted.
    // Former DenseGraph/SparseGraph ctor.
    cr2_graph->doRegisterCluster(num_cluster, num_original_nodes);

    // Now, start inserting the graph information.
    // Former countDegrees
    cr2_graph->doRegisterEdge(argEdgeList);

    return this->cr2_graph;
};

//
// Frees all memory set.
unsigned cr2::CR2Manager::doReset() {

    if (cr2_graph != nullptr) delete cr2_graph;
    cr2_graph = nullptr;

    return 0;
}

#ifdef CONSOLE_OUT_ENABLE 
// Debugging purpose
void cr2::CR2Manager::console_out_object_info() {

    printf("[cr2::CR2Manager]\n");
    printf("    cr2_graph: %p\n", cr2_graph);
    printf("    num_original_nodes: %d\n", num_original_nodes);
    printf("    num_edges: %d\n", num_edges);
    printf("    num_cluster: %d\n", num_cluster);
}
#endif