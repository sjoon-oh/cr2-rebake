// [rebake project] CR2
// author: SukJoon Oh, acoustikue@yonsei.ac.kr
// '21.02.11.

#include <cstdio>
#include <cassert>

#include <vector>
#include <cstdlib>

#include "./cr2-rebake/util/CR2Parser.h"
#include "./cr2-rebake/CR2.h" // one and only header?

 // Original project
//#include "./cr2-accel/util/Parser.h"
//#include "./cr2-accel/CR2Graph.h"
//#include "./cr2-accel/CommCR2Builder.h"
//#include "./cr2-accel/Community.h" 


// option
#define CR2_REBAKE
// #define CR2_ACCEL

int main() {
	
	printf("Hello, project cr2-rebake!\nwritten by SukJoon Oh\n\n");

#ifdef CR2_REBAKE

	printf("Compilation option: CR2_REBAKE\n");

	cr2::CR2Parser* cr2_parser = new cr2::CR2Parser("uk-2005-part.el");
	cr2_parser->readFiles();

	// Create the manager object
	cr2::CR2Manager cr2_manager;
	cr2_manager.doBuild(cr2_parser->getEdgeList());

#ifdef CONSOLE_OUT_ENABLE
	//cr2_manager.console_out_object_info();
#endif

	// Let's check the results
	cr2::CR2Graph* cr2_graph = const_cast<cr2::CR2Graph*>(cr2_manager.getCr2Graph());

#ifdef CONSOLE_OUT_ENABLE
	printf("Results (Comparison) \n");
	printf("    numOfNodes: %d\n", cr2_graph->getNumNodes());
	printf("    numOfEdges: %d\n", cr2_graph->getNumEdges());
	printf("    numOfComms: %d (# of intra-clusters)\n", cr2_graph->getNumIntraCluster());
	printf("    numOfDenseEdges: %d\n", cr2_graph->getNumIntraClusterEdges());
	printf("    numOfSparseEdges: %d\n", cr2_graph->getNumInterClusterEdges());
	printf("    numOfDenseVirtualNodes_out: %d\n", 
		cr2_graph->getNumIntraClusterVirtualNodes(cr2::OUT));
	printf("    numOfSparseVirtualNodes_out: %d\n", 
		cr2_graph->getNumInterClusterVirtualNodes(cr2::OUT));

#endif

#endif

	// Original project
#ifdef CR2_ACCEL

#endif
	return 0;
}
