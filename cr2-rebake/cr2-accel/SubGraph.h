#ifndef SUB_GRAPH_H
#define SUB_GRAPH_H

#include <vector>

#include "Community.h"

typedef uint16_t DENSE_TYPE;

class DenseGraph {
public:
  DenseGraph(uint32_t numOfComms, uint32_t numOfNodes) {
    this->numOfComms = numOfComms;
    comms.resize(numOfComms);
    for (uint32_t i = 0; i < numOfComms; ++i) {
      uint32_t vertexStart = i * COMM_SIZE;
      uint32_t vertexEnd = (i == comms.size() - 1) ?
        numOfNodes : (i + 1) * COMM_SIZE;
      Community* comm = new Community(i, vertexStart, vertexEnd);
      comms[i] = comm;
    }
  }
  void printDenseEdges() {
    comms[0]->printDenseEdges();
  }
  void printVertex() {
    comms[0]->printVertex();
  }
  void printNumOfVirtualNodes() {
    std::cout << "----- Dense Graph -----" << std::endl;
    for (uint32_t i = 0; i < numOfComms; ++i) {
      std::cout << "Community ID: " << i << std::endl;
      comms[i]->printNumOfVirtualNodes();
    }
  }
  void printVirtualNodes_in() {
    std::cout << "----- dense graph in-----" << std::endl;
    comms[0]->printVirtualNodes_in();
  }
  void printVirtualNodes_out() {
    std::cout << "----- dense graph out 0-----" << std::endl;
    comms[0]->printVirtualNodes_out();
    std::cout << "----- dense graph out 1-----" << std::endl;
    comms[1]->printVirtualNodes_out();
  }
  void release() {
    for (uint32_t i = 0; i < numOfComms; ++i) {
      comms[i]->release();
    }
  }

  // get edge list
  DENSE_TYPE* getEdgeListInComm_in(uint32_t commID) {
    return comms[commID]->getEdgeList_in();
  }
  DENSE_TYPE* getEdgeListInComm_out(uint32_t commID) {
    return comms[commID]->getEdgeList_out();
  }
  // get vertex ID list
  DENSE_TYPE* getVertexIDListInComm_in(uint32_t commID) {
    return comms[commID]->getVertexIDList_in();
  }
  DENSE_TYPE* getVertexIDListInComm_out(uint32_t commID) {
    return comms[commID]->getVertexIDList_out();
  }
  // get TBs
  ThreadBlock* getTBsInComm_in(uint32_t commID) {
    return comms[commID]->getTBs_in();
  }
  ThreadBlock* getTBsInComm_out(uint32_t commID) {
    return comms[commID]->getTBs_out();
  }

  void insertDegree(uint32_t commId, uint32_t src, uint32_t dest) {
    comms[commId]->insertDegree(src, dest);
  }
  void insertNeighbor(uint32_t commId, DENSE_TYPE src, DENSE_TYPE dest) {
    comms[commId]->insertNeighbor(src, dest);
  }
  void splitNodes() {
    for (Community* comm : comms)
      comm->splitNodes();
  }
  uint32_t getInDegrees(uint32_t commID, uint32_t id) {
    return comms[commID]->getInDegrees(id);
  }
  uint32_t getOutDegrees(uint32_t commID, uint32_t id) {
    return comms[commID]->getOutDegrees(id);
  }
  uint32_t getNumOfDenseEdges() {
    uint32_t sum = 0;
    for (Community* comm : comms)
      sum += comm->getNumOfEdges();

    return sum;
  }
  uint32_t getNumOfEdgesInComm(uint32_t commID) {
    return comms[commID]->getNumOfEdges();
  }
  uint32_t getNumOfVirtualNodes_in() {
    uint32_t sum = 0;
    for (Community* comm : comms) 
      sum += comm->getNumOfVirtualNodes_in();

    return sum;
  }
  uint32_t getNumOfVirtualNodes_out() {
    uint32_t sum = 0;
    for (Community* comm : comms) 
      sum += comm->getNumOfVirtualNodes_out();

    return sum;
  }

  uint32_t getNumOfVirtualNodesInComm_in(uint32_t commID) {
    return comms[commID]->getNumOfVirtualNodes_in();
  }
  uint32_t getNumOfVirtualNodesInComm_out(uint32_t commID) {
    return comms[commID]->getNumOfVirtualNodes_out();
  }
  uint32_t getNumOfVirtualNodesInComm_in(uint32_t commID, uint32_t logDegree) {
    return comms[commID]->getNumOfVirtualNodes_in(logDegree);
  }
  uint32_t getNumOfVirtualNodesInComm_out(uint32_t commID, uint32_t logDegree) {
    return comms[commID]->getNumOfVirtualNodes_out(logDegree);
  }

  uint32_t getNumOfTBs_in() {
    uint32_t sum = 0;
    for (Community* comm : comms) 
      sum += comm->getNumOfTBs_in();
    return sum;
  }
  uint32_t getNumOfTBs_out() {
    uint32_t sum = 0;
    for (Community* comm : comms) 
      sum += comm->getNumOfTBs_out();
    return sum;
  }

  uint32_t getNumOfTBsInComm_in(uint32_t commID) {
    return comms[commID]->getNumOfTBs_in();
  }
  uint32_t getNumOfTBsInComm_out(uint32_t commID) {
    return comms[commID]->getNumOfTBs_out();
  }

private:
  uint32_t numOfEdges;
  uint32_t numOfVirtualNodes;

  uint32_t numOfComms;
  std::vector<Community*> comms;
};

class SparseGraph {
public:
  SparseGraph(uint32_t numOfNodes) {
    partialGraph = new PartialGraph(numOfNodes, 0, false);
  }
  void printVirtualNodes_in() {
    std::cout << "----- sparse graph out-----" << std::endl;
    partialGraph->printVirtualNodes_in();
  }
  void printVirtualNodes_out() {
    std::cout << "----- sparse graph out-----" << std::endl;
    partialGraph->printVirtualNodes_out();
  }
  // get edge list
  uint32_t* getEdgeList_in() {
    return partialGraph->getSparseEdgeList_in();
  }
  uint32_t* getEdgeList_out() {
    return partialGraph->getSparseEdgeList_out();
  }

  // get vertex ID list
  uint32_t* getVertexIDList_in() {
    return partialGraph->getSparseVertexIDList_in();
  }
  uint32_t* getVertexIDList_out() {
    return partialGraph->getSparseVertexIDList_out();
  }

  // get TBs
  ThreadBlock* getTBs_in() {
    return partialGraph->getTBs_in();
  }
  ThreadBlock* getTBs_out() {
    return partialGraph->getTBs_out();
  }

  void insertDegree(uint32_t src, uint32_t dest) {
    partialGraph->insertDegree(src, dest);
  }
  void insertNeighbor(uint32_t src, uint32_t dest) {
    partialGraph->insertSparseNeighbor(src, dest);
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

  uint32_t getNumOfSparseEdges() {
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

  void printNumOfVirtualNodes() {
    std::cout << "----- Sparse Graph -----" << std::endl;
    partialGraph->printNumOfVirtualNodes();
  }
  void release() {
    partialGraph->release();
  }

private:
  PartialGraph* partialGraph;
};

#endif
