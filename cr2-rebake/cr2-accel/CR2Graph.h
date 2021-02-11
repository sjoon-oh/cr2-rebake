#ifndef CSR_GRAPH_H
#define CSR_GRAPH_H

#include <algorithm>

#include "SubGraph.h"
#include "Config.h"

//#define COMM_SIZE (25318076)
//#define COMM_SIZE (65536)


class CR2Graph {
typedef uint16_t DENSE_TYPE;
public:
  CR2Graph(uint32_t numOfNodes, uint32_t numOfEdges, uint32_t numOfComms,
           DenseGraph* denseGraph, SparseGraph* sparseGraph) {
    this->numOfNodes = numOfNodes;
    this->numOfEdges = numOfEdges;
    this->numOfComms = numOfComms;
    this->denseGraph = denseGraph;
    this->sparseGraph = sparseGraph;
    this->maxID_in = 0;
    this->maxID_out = 0;
  }

  void printGraphInfo() {
    std::cout << "# of nodes: " << numOfNodes << std::endl;
    std::cout << "# of edges: " << numOfEdges << std::endl;
    std::cout << "# of dense edges: " << denseGraph->getNumOfDenseEdges() << std::endl;
    std::cout << "# of sparse edges: " << sparseGraph->getNumOfSparseEdges() << std::endl;
    std::cout << "# of virtual nodes (dense_in): " << denseGraph->getNumOfVirtualNodes_in() << std::endl;
    std::cout << "# of virtual nodes (dense_out): " << denseGraph->getNumOfVirtualNodes_out() << std::endl;
    std::cout << "# of virtual nodes (sparse_in): " << sparseGraph->getNumOfVirtualNodes_in() << std::endl;
    std::cout << "# of virtual nodes (sparse_out): " << sparseGraph->getNumOfVirtualNodes_out() << std::endl;
    std::cout << "# of TBs (dense_in): " << denseGraph->getNumOfTBs_in() << std::endl;
    std::cout << "# of TBs (dense_out): " << denseGraph->getNumOfTBs_out() << std::endl;
    std::cout << "# of TBs (sparse_in): " << sparseGraph->getNumOfTBs_in() << std::endl;
    std::cout << "# of TBs (sparse_out): " << sparseGraph->getNumOfTBs_out() << std::endl;
    std::cout << std::endl;
  }
  void printVirtualNodes_in() {
    denseGraph->printVirtualNodes_in();
    sparseGraph->printVirtualNodes_in();
  }

  void printVirtualNodes_out() {
    denseGraph->printVirtualNodes_out();
    sparseGraph->printVirtualNodes_out();
  }

  void printNumOfVirtualNodes_dense() {
    denseGraph->printNumOfVirtualNodes();
  }
  void printNumOfVirtualNodes_sparse() {
    sparseGraph->printNumOfVirtualNodes();
  }

  uint32_t getMaxID_in() { return maxID_in; }
  uint32_t getMaxID_out() { return maxID_out; }

  uint32_t* buildInDegreeList() {
    uint32_t* inDegrees = new uint32_t[numOfNodes];
    for (uint32_t i = 0; i < numOfNodes; ++i) {
      inDegrees[i] = 
        denseGraph->getInDegrees(i / COMM_SIZE, i % COMM_SIZE) + 
        sparseGraph->getInDegrees(i);
    }
    maxID_in = std::distance(inDegrees, std::max_element(inDegrees, inDegrees + numOfNodes));
    return inDegrees;
  }
  uint32_t* buildOutDegreeList() {
    uint32_t* outDegrees = new uint32_t[numOfNodes];
    for (uint32_t i = 0; i < numOfNodes; ++i) {
      outDegrees[i] = 
        denseGraph->getOutDegrees(i / COMM_SIZE, i % COMM_SIZE) + 
        sparseGraph->getOutDegrees(i);
    }
    maxID_out = std::distance(outDegrees, std::max_element(outDegrees, outDegrees + numOfNodes));
    return outDegrees;
  }
  // get edge list
  DENSE_TYPE* getDenseEdgeListInComm_in(uint32_t commID) {
    return denseGraph->getEdgeListInComm_in(commID);
  }
  DENSE_TYPE* getDenseEdgeListInComm_out(uint32_t commID) {
    return denseGraph->getEdgeListInComm_out(commID);
  }
  uint32_t* getSparseEdgeList_in() {
    return sparseGraph->getEdgeList_in();
  }
  uint32_t* getSparseEdgeList_out() {
    return sparseGraph->getEdgeList_out();
  }

  // get vertex ID list
  DENSE_TYPE* getDenseVertexIDListInComm_in(uint32_t commID) {
    return denseGraph->getVertexIDListInComm_in(commID);
  }
  DENSE_TYPE* getDenseVertexIDListInComm_out(uint32_t commID) {
    return denseGraph->getVertexIDListInComm_out(commID);
  }
  uint32_t* getSparseVertexIDList_in() {
    return sparseGraph->getVertexIDList_in();
  }
  uint32_t* getSparseVertexIDList_out() {
    return sparseGraph->getVertexIDList_out();
  }

  // get thread block info
  ThreadBlock* getDenseTBsInComm_in(uint32_t commID) {
    return denseGraph->getTBsInComm_in(commID);
  }
  ThreadBlock* getDenseTBsInComm_out(uint32_t commID) {
    return denseGraph->getTBsInComm_out(commID);
  }
  ThreadBlock* getSparseTBs_in() {
    return sparseGraph->getTBs_in();
  }
  ThreadBlock* getSparseTBs_out() {
    return sparseGraph->getTBs_out();
  }

  uint32_t getNumOfNodes() { return numOfNodes; }
  uint32_t getNumOfEdges() { return numOfEdges; }
  uint32_t getNumOfComms() { return numOfComms; }
  uint32_t getNumOfDenseEdgesInComm(uint32_t commID) {
    return denseGraph->getNumOfEdgesInComm(commID);
  }
  uint32_t getNumOfDenseVirtualNodes_in() {
    return denseGraph->getNumOfVirtualNodes_in();
  }
  uint32_t getNumOfDenseVirtualNodes_out() {
    return denseGraph->getNumOfVirtualNodes_out();
  }
  uint32_t getNumOfSparseVirtualNodes_in() {
    return sparseGraph->getNumOfVirtualNodes_in();
  }
  uint32_t getNumOfSparseVirtualNodes_out() {
    return sparseGraph->getNumOfVirtualNodes_out();
  }
  uint32_t getNumOfSparseVirtualNodes_in(uint32_t logDegree) {
    return sparseGraph->getNumOfVirtualNodes_in(logDegree);
  }
  uint32_t getNumOfSparseVirtualNodes_out(uint32_t logDegree) {
    return sparseGraph->getNumOfVirtualNodes_out(logDegree);
  }
  uint32_t getNumOfDenseVirtualNodesInComm_in(uint32_t commID) {
    return denseGraph->getNumOfVirtualNodesInComm_in(commID);
  }
  uint32_t getNumOfDenseVirtualNodesInComm_out(uint32_t commID) {
    return denseGraph->getNumOfVirtualNodesInComm_out(commID);
  }
  uint32_t getNumOfDenseVirtualNodesInComm_in(uint32_t commID, uint32_t logDegree) {
    return denseGraph->getNumOfVirtualNodesInComm_in(commID, logDegree);
  }
  uint32_t getNumOfDenseVirtualNodesInComm_out(uint32_t commID, uint32_t logDegree) {
    return denseGraph->getNumOfVirtualNodesInComm_out(commID, logDegree);
  }
  uint32_t getNumOfDenseEdges() {
    return denseGraph->getNumOfDenseEdges();
  }
  uint32_t getNumOfSparseEdges() {
    return sparseGraph->getNumOfSparseEdges();
  }

  uint32_t getNumOfDenseTBs_in() {
    return denseGraph->getNumOfTBs_in();
  }
  uint32_t getNumOfDenseTBs_out() {
    return denseGraph->getNumOfTBs_out();
  }
  uint32_t getNumOfSparseTBs_in() {
    return sparseGraph->getNumOfTBs_in();
  }
  uint32_t getNumOfSparseTBs_out() {
    return sparseGraph->getNumOfTBs_out();
  }

  uint32_t getNumOfDenseTBsInComm_in(uint32_t commID) {
    return denseGraph->getNumOfTBsInComm_in(commID);
  }
  uint32_t getNumOfDenseTBsInComm_out(uint32_t commID) {
    return denseGraph->getNumOfTBsInComm_out(commID);
  }

private:
  uint32_t numOfNodes;
  uint32_t numOfEdges;
  uint32_t numOfComms;

  uint32_t maxID_in;
  uint32_t maxID_out;

  DenseGraph* denseGraph;
  SparseGraph* sparseGraph;

};

#endif
