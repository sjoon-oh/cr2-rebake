#ifndef COMM_CSR_BUILDER_H
#define COMM_CSR_BUILDER_H

#include <vector>

#include "../util/Edge.h"
#include "CR2Graph.h"
#include "Community.h"

// class for building std::vector<Communtiy>-centric CSR representation
//
class CommCSRBuilder {

public:
  CommCSRBuilder() {}

  CR2Graph* buildCR2(const std::vector<Edge>& edgeList);
private:
  uint32_t numOfNodes;
  uint32_t numOfEdges; 
  uint32_t numOfComms;
  uint32_t numOfSparseVertices;
  
  void countDegrees(const std::vector<Edge>& edgeList,
                    DenseGraph* denseGraph, SparseGraph* sparseGraph);
  void buildNeighborList(const std::vector<Edge>& edgeList,
                         DenseGraph* denseGraph, SparseGraph* sparseGraph);
  uint32_t countNumOfNodes(const std::vector<Edge>& edgeList);

};

#endif
