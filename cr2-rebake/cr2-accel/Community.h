#ifndef COMMUNITY_H
#define COMMUNITY_H

#include <vector>
#include <assert.h>

#include "PartialGraph.h"

#define COMM_SIZE (65536)

// num of edges per thread block = num of edges / num of virtual nodes
class Community {
public:
  Community(uint32_t commId, uint32_t vertexStart, uint32_t vertexEnd) {
    this->commId = commId;
    this->vertexStart = vertexStart;
    this->vertexEnd = vertexEnd;
    this->numOfNodes = vertexEnd - vertexStart;
    this->partialGraph= new PartialGraph(numOfNodes, commId, true);
  }
  void release() {
    partialGraph->release();
  }
  DENSE_TYPE* getEdgeList_in() {
    return partialGraph->getDenseEdgeList_in();
  }
  DENSE_TYPE* getEdgeList_out() {
    return partialGraph->getDenseEdgeList_out();
  }
  DENSE_TYPE* getVertexIDList_in() {
    return partialGraph->getDenseVertexIDList_in();
  }
  DENSE_TYPE* getVertexIDList_out() {
    return partialGraph->getDenseVertexIDList_out();
  }
  ThreadBlock* getTBs_in() {
    return partialGraph->getTBs_in();
  }
  ThreadBlock* getTBs_out() {
    return partialGraph->getTBs_out();
  }

  void printVertex() {
    partialGraph->printDenseVertexIDList_out();
  }

  void printDenseEdges() {
    partialGraph->printDenseEdgeList_out();
  }
  void printTB() {
    partialGraph->printThreadBlock_out();
  }
  void printVirtualNodes_in() {
    partialGraph->printVirtualNodes_in();
  }
  void printVirtualNodes_out() {
    partialGraph->printVirtualNodes_out();
  }

  void printNumOfVirtualNodes() {
    partialGraph->printNumOfVirtualNodes();
  }

  void insertDegree(uint32_t src, uint32_t dest) {
    partialGraph->insertDegree(src, dest);
  }
  void insertNeighbor(DENSE_TYPE src, DENSE_TYPE dest) {
    partialGraph->insertDenseNeighbor(src, dest);
  }
  void splitNodes() {
    partialGraph->splitNodes();
  }
  uint32_t getInDegrees(uint32_t id) {
    return partialGraph->getInDegrees(id);
  }
  uint32_t getOutDegrees(uint32_t id) {
    return partialGraph->getOutDegrees(id);
  }
  uint32_t getNumOfEdges() {
    return partialGraph->getNumOfEdges();
  }
  uint32_t getNumOfVirtualNodes_in() {
    return partialGraph->getNumOfVirtualNodes_in();
  }
  uint32_t getNumOfVirtualNodes_out() {
    return partialGraph->getNumOfVirtualNodes_out();
  }
  uint32_t getNumOfVirtualNodes_in(uint32_t logDegree) {
    return partialGraph->getNumOfVirtualNodes_in(logDegree);
  }
  uint32_t getNumOfVirtualNodes_out(uint32_t logDegree) {
    return partialGraph->getNumOfVirtualNodes_out(logDegree);
  }

  uint32_t getNumOfTBs_in() {
    return partialGraph->getNumOfTBs_in();
  }
  uint32_t getNumOfTBs_out() {
    return partialGraph->getNumOfTBs_out();
  }

private:
  uint32_t commId;
  uint32_t vertexStart;
  uint32_t vertexEnd;
  uint32_t numOfNodes;

  PartialGraph* partialGraph;
};

#endif
