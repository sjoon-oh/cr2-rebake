#include <iostream>
//#include <parallel/algorithm>
#include <set>

#include "./SubGraph.h"
#include "./CommCR2Builder.h"
#include "./CR2Graph.h"

CR2Graph* CommCSRBuilder::buildCR2(const std::vector<Edge>& edgeList) {
  numOfNodes = countNumOfNodes(edgeList) + 1;
  numOfEdges = edgeList.size();
  numOfComms = (numOfNodes % COMM_SIZE != 0) ? numOfNodes / COMM_SIZE + 1 : 
                                               numOfNodes / COMM_SIZE;

  printf("numPfNodes: %d\n", numOfNodes);
  printf("numOfEdges: %d\n", numOfEdges);
  printf("numOfComms: %d\n", numOfComms);

  DenseGraph* denseGraph = new DenseGraph(numOfComms, numOfNodes);
  SparseGraph* sparseGraph = new SparseGraph(numOfNodes);

  countDegrees(edgeList, denseGraph, sparseGraph);
  std::cerr << "finish countDegree()" << std::endl;

  denseGraph->splitNodes();
  sparseGraph->splitNodes();


  buildNeighborList(edgeList, denseGraph, sparseGraph);
  std::cerr << "finish build neighbor" << std::endl;


  CR2Graph* cr2Graph = new CR2Graph(numOfNodes, numOfEdges, numOfComms,
                                    denseGraph, sparseGraph);
  std::cerr << "finish instantiate CR2Graph" << std::endl;
  denseGraph->release();
  sparseGraph->release();
  std::cout << "finish release" << std::endl;

  return cr2Graph;
}


void CommCSRBuilder::countDegrees(const std::vector<Edge>& edgeList,
                                  DenseGraph* denseGraph,
                                  SparseGraph* sparseGraph) {
  for (auto it = edgeList.begin(); it < edgeList.end(); ++ it) {
    Edge edge = *it;
    uint32_t src = edge.getSrc();
    uint32_t dest = edge.getDest();

    if (src / COMM_SIZE == dest / COMM_SIZE)
      denseGraph->insertDegree(src / COMM_SIZE, src % COMM_SIZE, dest % COMM_SIZE);
    else
      sparseGraph->insertDegree(src, dest);
  }
}

void CommCSRBuilder::buildNeighborList(const std::vector<Edge>& edgeList,
                                       DenseGraph* denseGraph, 
                                       SparseGraph* sparseGraph) {
  for (auto it = edgeList.begin(); it < edgeList.end(); ++it) {
    Edge edge = *it;
    uint32_t src = edge.getSrc();
    uint32_t dest = edge.getDest();

    if (src / COMM_SIZE == dest / COMM_SIZE)
      denseGraph->insertNeighbor(src / COMM_SIZE, src % COMM_SIZE, dest % COMM_SIZE);
      //denseGraph->insertNeighbor(src / COMM_SIZE, src, dest);
    else 
      sparseGraph->insertNeighbor(src, dest);
  }
  //denseGraph->printVertex();
  //std::cout << std::endl;
  //denseGraph->printDenseEdges();
  //std::cout << std::endl;
  //denseGraph->printTB();
  //std::cout << std::endl;
}

uint32_t CommCSRBuilder::countNumOfNodes(const std::vector<Edge>& edgeList) {
  uint32_t curMax = 0;
  #pragma omp parallel for reduction(max : curMax)
  for (auto it = edgeList.begin(); it < edgeList.end(); ++it) {
    Edge edge = *it;
    curMax = std::max(curMax, edge.getSrc());
    curMax = std::max(curMax, edge.getDest());
  }
  return curMax;
}
