#ifndef PARTIAL_GRAPH_H
#define PARTIAL_GRAPH_H

#include <stdint.h>

#define EDGES_PER_THREAD (128)
#define EDGES_PER_THREADBLOCK (65536)
#define NUM_OF_THREADS (64)
#define COMM_SIZE (65536)

typedef uint16_t DENSE_TYPE;

typedef struct ThreadBlock {
  uint32_t nodeOffset;
  uint32_t edgeOffset;
} ThreadBlock;

class PartialGraph {
public:
	PartialGraph(uint32_t numOfNodes, uint32_t commID, bool isDense) {
    this->isDense = isDense;
    this->numOfNodes = numOfNodes;
    numOfTotalVirtualNodes_in = 0;
    numOfTotalVirtualNodes_out = 0;
    numOfEdges = 0;
    numOfTBs_in = 0;
    numOfTBs_out = 0;

    inDegrees = new uint32_t[numOfNodes];
    outDegrees = new uint32_t[numOfNodes];

    edgePosition_in = new uint32_t*[6];
    edgePosition_out = new uint32_t*[6];

    remains_in = new uint32_t*[numOfNodes];
    remains_out = new uint32_t*[numOfNodes];

    for (uint32_t i = 0; i < 6; ++i) {
      edgePosition_in[i] = new uint32_t[numOfNodes];
      edgePosition_out[i] = new uint32_t[numOfNodes];
      remains_in[i] = new uint32_t[numOfNodes];
      remains_out[i] = new uint32_t[numOfNodes];
    }

    #pragma omp parallel for
    for (uint32_t i = 0; i < numOfNodes; ++i) {
      inDegrees[i] = 0;
      outDegrees[i] = 0;
      for (uint32_t j = 0; j < 6; ++j) {
        edgePosition_in[j][i] = 0;
        edgePosition_out[j][i] = 0;
        remains_in[j][i] = 0;
        remains_out[j][i] = 0;
      }
    }

    for (uint32_t i = 0; i < 6; ++i) {
      numOfVirtualNodes_in[i] = 0;
      numOfVirtualNodes_out[i] = 0;
      nodePrefixSum_in[i] = 0;
      nodePrefixSum_out[i] = 0;
      edgePrefixSum_in[i] = 0;
      edgePrefixSum_out[i] = 0;
    }
  }

  void splitNodes();
  void insertDegree(uint32_t src, uint32_t dest);
  void insertDenseNeighbor(DENSE_TYPE src, DENSE_TYPE dest);
  void insertSparseNeighbor(uint32_t src, uint32_t dest);

  uint32_t getInDegrees(uint32_t id) { return inDegrees[id]; }
  uint32_t getOutDegrees(uint32_t id) { return outDegrees[id]; }
  uint32_t getNumOfEdges() { return numOfEdges; }

  uint32_t getNumOfVirtualNodes_in() { return numOfTotalVirtualNodes_in; }
  uint32_t getNumOfVirtualNodes_out() { return numOfTotalVirtualNodes_out; }
  uint32_t getNumOfVirtualNodes_in(uint32_t logDegree) { return numOfVirtualNodes_in[logDegree]; }
  uint32_t getNumOfVirtualNodes_out(uint32_t logDegree) { return numOfVirtualNodes_out[logDegree]; }

  uint32_t getNumOfTBs_in() { return numOfTBs_in; }
  uint32_t getNumOfTBs_out() { return numOfTBs_out; }

  DENSE_TYPE* getDenseEdgeList_in() { return denseEdgeList_in; }
  DENSE_TYPE* getDenseEdgeList_out() { return denseEdgeList_out; }

  DENSE_TYPE* getDenseVertexIDList_in() { return denseVertexIDList_in; }
  DENSE_TYPE* getDenseVertexIDList_out() { return denseVertexIDList_out; }

  uint32_t* getSparseEdgeList_in() { return sparseEdgeList_in; }
  uint32_t* getSparseEdgeList_out() { return sparseEdgeList_out; }

  uint32_t* getSparseVertexIDList_in() { return sparseVertexIDList_in; }
  uint32_t* getSparseVertexIDList_out() { return sparseVertexIDList_out; }

  ThreadBlock* getTBs_in() { return tbs_in; }
  ThreadBlock* getTBs_out() { return tbs_out; }

  void printDenseVertexIDList_out() {
    for (uint32_t i = 0; i < numOfTotalVirtualNodes_out; ++i) {
      std::cout << denseVertexIDList_out[i] << " (" << numOfTotalVirtualNodes_out << ") " << " ";
    }
  }

  void printDenseEdgeList_out() {
    for (uint32_t i = 0; i < numOfEdges; ++i) {
      std::cout << denseEdgeList_out[i] << " ";
    }
  }
  void printThreadBlock_out() {
    for (uint32_t i = 0; i < numOfTBs_out + 1; ++i) {
      std::cout << "edge offset: " << tbs_out[i].edgeOffset;
      std::cout << ", node offset: " << tbs_out[i].nodeOffset << std::endl;
    }
  }
  void release() {
    for (uint32_t i = 0; i < 6; ++i) {
      delete[] remains_in[i];
      delete[] remains_out[i];
      delete[] edgePosition_in[i];
      delete[] edgePosition_out[i];
    }
    delete[] remains_in;
    delete[] remains_out;
    delete[] edgePosition_in;
    delete[] edgePosition_out;
  }

  void printNumOfVirtualNodes() {
    for (uint32_t i = 0; i < EDGES_PER_THREAD; ++i) {
      std::cout << "Degree " << i + 1 << ": " << numOfVirtualNodes_out[i] << std::endl;
    }
  }

  void printVirtualNodes_in() {
    for (uint32_t i = 0; i < EDGES_PER_THREAD; ++i)
      std::cout << i + 1 << ": " << numOfVirtualNodes_in[i] << " (" << numOfVirtualNodes_in[i] % NUM_OF_THREADS <<")" << std::endl;
  }
  void printVirtualNodes_out() {
    for (uint32_t i = 0; i < EDGES_PER_THREAD; ++i)
      std::cout << i + 1 << ": " << numOfVirtualNodes_out[i] << " (" << numOfVirtualNodes_out[i] % NUM_OF_THREADS <<")" << std::endl;
  }


private:
  bool isDense;
  uint32_t numOfNodes;
  uint32_t numOfTotalVirtualNodes_in;
  uint32_t numOfTotalVirtualNodes_out;
  uint32_t numOfEdges;

  uint32_t* inDegrees;
  uint32_t* outDegrees;

  uint32_t** edgePosition_in;
  uint32_t** edgePosition_out;

  uint32_t** remains_in;
  uint32_t** remains_out;

  uint32_t numOfVirtualNodes_in[8];
  uint32_t numOfVirtualNodes_out[8];

  uint32_t nodePrefixSum_in[8];
  uint32_t nodePrefixSum_out[8];

  uint32_t edgePrefixSum_in[8];
  uint32_t edgePrefixSum_out[8];

  DENSE_TYPE* denseVertexIDList_in;
  DENSE_TYPE* denseVertexIDList_out;
  DENSE_TYPE* denseEdgeList_in;
  DENSE_TYPE* denseEdgeList_out;

  uint32_t* sparseVertexIDList_in;
  uint32_t* sparseVertexIDList_out;
  uint32_t* sparseEdgeList_in;
  uint32_t* sparseEdgeList_out;

  uint32_t numOfTBs_in;
  uint32_t numOfTBs_out;
  ThreadBlock* tbs_in;
  ThreadBlock* tbs_out;

  uint32_t maxID;

  uint32_t commID;

  void countNumOfVirtualNodes();
  void countNumOfTotalVirtualNodes();
  void countNumOfTotalEdges();

  void calculatePrefixSum();

  void buildVertexIDList(bool isDense);
  void buildVertexIDList_in(uint32_t nodeID, bool isDense);
  void buildVertexIDList_out(uint32_t nodeID, bool isDense);

  void createThreadBlockInfo();
  void createThreadBlockInfo_in();
  void createThreadBlockInfo_out();

  void countNumOfTBs_in();
  void countNumOfTBs_out();

};

#endif


