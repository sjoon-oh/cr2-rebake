// [rebake project] CR2
// author: SukJoon Oh, acoustikue@yonsei.ac.kr
// '21.02.07.
//

#include "./CR2Cluster.h"
#include "./CR2Graph.h"

#include "./util/platform_atomics.h"

// Defined Utility
#define CLUSTER_ID(X) (X / CLUSTER_SIZE)
#define CLUSTER_INNER_IDX(X) (X % CLUSTER_SIZE)

#define IS_SAME_CLUSTER(X, Y) (( (X / CLUSTER_SIZE) == (Y / CLUSTER_SIZE) )) // query

// dtor
cr2::CR2Graph::~CR2Graph() { this->doRelease(); }

//
// This function plays a role of the existing DenseGraph's (and SparseGraph's) constructor.
// Calculates the number of community, counting starting index and the back index of the array as divider.
// Be sure to call this function before calling the doRegisterEdge() function.
unsigned cr2::CR2Graph::doRegisterCluster(uint32_t argNumCommunity, uint32_t argNumNodes) {

    // Record the fixed sized range to CR2Cluster, 
    // this goes for the community that holds intra-clusters.
    for (uint32_t i = 0; i < argNumCommunity; i++) {
        
        uint32_t vertex_st = i * CLUSTER_SIZE; // Next range
        uint32_t vertex_ed = (i == (intra_cluster.size() - 1)) ? 
            argNumNodes : (i + 1) * CLUSTER_SIZE;

        // By having the size of CLUSTER_SIZE, register the new CR2Cluster.
        // Number of nodes are calculated within the ctor of CR2Cluster object.
        intra_cluster.push_back(
            new cr2::CR2Cluster<cr2::DENSE_TYPE>(i, vertex_st, vertex_ed));
    }

    // Now, for the inter-cluster community.
    // Since the inter-cluster is one and only, there is no need for complicated operations.
    // The arguments are: 0 for id, 0 for starting index, and the argNumNodes - 1 for last index.
    inter_cluster = new cr2::CR2Cluster<cr2::SPARSE_TYPE>(0, 0, argNumNodes - 1);

    this->num_nodes = argNumNodes; // Register member
    return 0;
};


//
// This function generates edge list into an empty graph.
unsigned cr2::CR2Graph::doRegisterEdge(const std::vector<cr2::Edge>& argEdgeList) {
    
    // First step!
    // Iterates the edge, and count the degree for each vertex. 
    // How many incoming and outgoing edges are there for a vertex?
    for (auto it: argEdgeList) {

        cr2::EDGE src = it.getSrcVertex();
        cr2::EDGE dst = it.getDstVertex();
        
        if (IS_SAME_CLUSTER(src, dst)) // In case if it belongs to same cluster,
            this->intra_cluster[CLUSTER_ID(src)]->doRecordSingleDegree(
                CLUSTER_INNER_IDX(src), CLUSTER_INNER_IDX(dst)
            ); // At its cluster (with its appropriate community id), should edges be stored.

        else // If it is not the case (not belongs to the same cluster)
            this->inter_cluster->doRecordSingleDegree(
                CLUSTER_INNER_IDX(src), CLUSTER_INNER_IDX(dst)
            );
    } // so far, refactored former CommCSRBuilder::countDegrees
	

    // Next, register degree-subgraphs and do vertex split.
    // Make seats for the vertex!!
    inter_cluster->doBuildDegreeSubgraph();
    inter_cluster->doBuildVertexList(); // Vertex Splitting
	
    for (auto& ic: this->intra_cluster) {
        ic->doBuildDegreeSubgraph();   
        ic->doBuildVertexList(); // Vertex Splitting
    } // so far, refactored former splitNodes()
	
    // Final step: Insert neighbors to the clusters, where seats are ready. 
    for (auto it: argEdgeList) {

        cr2::NODE src = it.getSrcVertex();
        cr2::NODE dst = it.getDstVertex();
		
        if (IS_SAME_CLUSTER(src, dst)) // In case if it belongs to same cluster,
            this->intra_cluster[CLUSTER_ID(src)]->doRecordSingleNeighbor(
                CLUSTER_INNER_IDX(src), CLUSTER_INNER_IDX(dst)
            ); // At its cluster (with its appropriate community id), should edges be stored.

        else // If it is not the case (not belongs to the same cluster)
            this->inter_cluster->doRecordSingleNeighbor(
                CLUSTER_INNER_IDX(src), CLUSTER_INNER_IDX(dst)
            );
    } // so far, refactored former CommCSRBuilder::buildNeighborList
	
    // When arranging, the addtional map_ variables were allocated to the heap.
    // The map_ array is the identical one with the former edge_position and remains_ series.
    //
    // Thus, free them before handling the graph.
    inter_cluster->doReleaseLv2();
    for (auto& ic: this->intra_cluster) ic->doReleaseLv2();
    
    return 0;
};


unsigned cr2::CR2Graph::doBuildDegreeList(unsigned argDir) {
	
	degree_list[argDir] = new uint32_t[num_nodes];

	for (uint32_t vertex_id = 0; vertex_id < num_nodes; ++vertex_id) {
		degree_list[argDir][vertex_id] =
			intra_cluster[CLUSTER_ID(vertex_id)]->getVertexDegrees(argDir, vertex_id)
			+ inter_cluster->getVertexDegrees(argDir, vertex_id);
	}

	return 0;
};


//
// Releases all the memory held in this container.
// Warning: Do not ever explicitly-call this function.
// This will be done in dtor.
unsigned cr2::CR2Graph::doRelease() {
	
	// Remove cluster information
    if (intra_cluster.size() != 0) // If there exists any community
        for (auto it : intra_cluster) 
			if (it != nullptr) delete it; // free

	if (inter_cluster != nullptr) delete inter_cluster;

	// Remove degree information
	if (degree_list[IN] != nullptr) delete[] degree_list[IN];
	if (degree_list[OUT] != nullptr) delete[] degree_list[OUT];

    return 0;
};




#ifdef CONSOLE_OUT_ENABLE // Debugging purpose
void cr2::CR2Graph::console_out_object_info() {

    printf("cr2::CR2Graph\n");
    printf("    num_nodes: %d\n", num_nodes);
    printf("    num_edges: %d\n", num_edges);
	printf("    num_cluster: %d\n", num_cluster);
    printf("    num_intra_cluster: %d\n", num_intra_cluster);
}
#endif