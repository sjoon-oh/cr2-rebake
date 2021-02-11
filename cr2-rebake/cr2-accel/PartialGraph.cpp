#include <iostream>

#include <assert.h>
#include <math.h>

#include "PartialGraph.h"
#include "../util/platform_atomics.h"

#define SM (30)
#define POW(a, b) ((uint32_t)pow(a, b))

void PartialGraph::countNumOfVirtualNodes() {
  #pragma omp parallel for
  for (uint32_t i = 0; i < this->numOfNodes; ++i) {
    uint32_t numOfNodes_i_in[6];
    uint32_t numOfRemains_i_in[6];
    uint32_t numOfNodes_i_out[6];
    uint32_t numOfRemains_i_out[6];
    numOfNodes_i_in[5] = inDegrees[i] / 32;
    numOfRemains_i_in[5] = inDegrees[i] % 32;
    numOfNodes_i_out[5] = outDegrees[i] / 32;
    numOfRemains_i_out[5] = outDegrees[i] % 32;
    for (int i = 4; i >= 0; --i) {
      numOfNodes_i_in[i] = numOfRemains_i_in[i + 1] / POW(2, i);
      numOfRemains_i_in[i] = numOfRemains_i_in[i + 1] % POW(2, i);
      numOfNodes_i_out[i] = numOfRemains_i_out[i + 1] / POW(2, i);
      numOfRemains_i_out[i] = numOfRemains_i_out[i + 1] % POW(2, i);
    }

    for (uint32_t k = 0; k < 6; ++k) {
      if (numOfNodes_i_in[k] != 0)
        fetch_and_add(numOfVirtualNodes_in[k], numOfNodes_i_in[k]);
      if (numOfNodes_i_out[k] != 0)
        fetch_and_add(numOfVirtualNodes_out[k], numOfNodes_i_out[k]);
    }
  }
}

void PartialGraph::countNumOfTotalEdges() {
  for (uint32_t i = 0; i < 6; ++i)
    numOfEdges += (numOfVirtualNodes_in[i] * POW(2, i));

}

void PartialGraph::countNumOfTotalVirtualNodes() {
  for (uint32_t i = 0; i < 6; ++i) {
    numOfTotalVirtualNodes_in += numOfVirtualNodes_in[i];
    numOfTotalVirtualNodes_out += numOfVirtualNodes_out[i];
  }
}

void PartialGraph::calculatePrefixSum() {
  uint32_t nodeCurrent_in = 0;
  uint32_t nodeCurrent_out = 0;
  uint32_t edgeCurrent_in = 0;
  uint32_t edgeCurrent_out = 0;
  for (uint32_t i = 0; i < 6; ++i) {
    nodePrefixSum_in[i] = nodeCurrent_in;
    nodePrefixSum_out[i] = nodeCurrent_out;
    edgePrefixSum_in[i] = edgeCurrent_in;
    edgePrefixSum_out[i] = edgeCurrent_out;

    nodeCurrent_in += numOfVirtualNodes_in[i];
    nodeCurrent_out += numOfVirtualNodes_out[i];

    edgeCurrent_in += (numOfVirtualNodes_in[i] * POW(2, i));
    edgeCurrent_out += (numOfVirtualNodes_out[i] * POW(2, i));
  }
}

// this function also calculate edge position
void PartialGraph::buildVertexIDList(bool isDense) {
  if (isDense) {
    denseVertexIDList_in = new DENSE_TYPE[numOfTotalVirtualNodes_in];
    denseVertexIDList_out = new DENSE_TYPE[numOfTotalVirtualNodes_out];
  } else {
    sparseVertexIDList_in = new uint32_t[numOfTotalVirtualNodes_in];
    sparseVertexIDList_out = new uint32_t[numOfTotalVirtualNodes_out];
  }
  for (uint32_t i = 0; i < this->numOfNodes; ++i) {
    // build in vertex ID list
    if (inDegrees[i] != 0) {
      buildVertexIDList_in(i, isDense);
    }
    if (outDegrees[i] != 0) {
      buildVertexIDList_out(i, isDense);
    }
  }
}

void PartialGraph::buildVertexIDList_in(uint32_t nodeID, bool isDense) {
  uint32_t currentRemain = inDegrees[nodeID];

  for (int i = 5; i >= 0; --i) {
    uint32_t degrees = POW(2, i);
    if (currentRemain >= degrees) {
      uint32_t numOfVirtualNodes = currentRemain / degrees;
      uint32_t start = nodePrefixSum_in[i];
      uint32_t end = start + numOfVirtualNodes;
      for (uint32_t pos = start; pos < end; ++pos) {
        if (isDense) {
          denseVertexIDList_in[pos] = (DENSE_TYPE)nodeID;
        } else {
          sparseVertexIDList_in[pos] = nodeID;
        }
      }
      nodePrefixSum_in[i] += numOfVirtualNodes;
      edgePosition_in[i][nodeID] = edgePrefixSum_in[i];
      remains_in[i][nodeID] = (numOfVirtualNodes * degrees);
      edgePrefixSum_in[i] += (numOfVirtualNodes * degrees);

    }
    currentRemain = currentRemain % degrees;
  }
}

void PartialGraph::buildVertexIDList_out(uint32_t nodeID, bool isDense) {
  uint32_t currentRemain = outDegrees[nodeID];

  for (int i = 5; i >= 0; --i) {
    uint32_t degrees = POW(2, i);
    if (currentRemain >= degrees) {
      uint32_t numOfVirtualNodes = currentRemain / degrees;
      uint32_t start = nodePrefixSum_out[i];
      uint32_t end = start + numOfVirtualNodes;
      for (uint32_t pos = start; pos < end; ++pos) {
        if (isDense) {
          denseVertexIDList_out[pos] = (DENSE_TYPE)nodeID;
        } else {
          sparseVertexIDList_out[pos] = nodeID;
        }
      }
      nodePrefixSum_out[i] += numOfVirtualNodes;
      edgePosition_out[i][nodeID] = edgePrefixSum_out[i];
      remains_out[i][nodeID] = (numOfVirtualNodes * degrees);
      edgePrefixSum_out[i] += (numOfVirtualNodes * degrees);

    }
    currentRemain = currentRemain % degrees;
  }
}


//void PartialGraph::countNumOfTBs_in() {
  //for (uint32_t i = 0; i < 8; ++i) {
    //uint32_t count = (numOfVirtualNodes_in[i] % NUM_OF_THREADS != 0) ?
      //numOfVirtualNodes_in[i] / NUM_OF_THREADS + 1 :
      //numOfVirtualNodes_in[i] / NUM_OF_THREADS;
    //this->numOfTBs_in += count;
  //}
//}

/*
// normal
void PartialGraph::countNumOfTBs_out() {
  for (uint32_t i = 0; i < 8; ++i) {
    uint32_t count = (numOfVirtualNodes_out[i] % NUM_OF_THREADS != 0) ?
      numOfVirtualNodes_out[i] / NUM_OF_THREADS + 1 :
      numOfVirtualNodes_out[i] / NUM_OF_THREADS;
    this->numOfTBs_out += count;
  }
}
*/

/*
void PartialGraph::countNumOfTBs_out() {
  for (uint32_t i = 0; i < 8; ++i) {
    uint32_t currentNumOfEdges = numOfVirtualNodes_out[i] * POW(2, i); // * pow(2, i);
    uint32_t edgesPerThreadBlock = 0;
    if (currentNumOfEdges < SM) 
      edgesPerThreadBlock = 1;
    else
      edgesPerThreadBlock = (currentNumOfEdges / SM) - ((currentNumOfEdges / SM + 1) % POW(2, i));
    uint32_t nodesPerThreadBlock = edgesPerThreadBlock / POW(2, i); // / pow(2, i);

    uint32_t count = (numOfVirtualNodes_out[i] % nodesPerThreadBlock != 0) ?
      numOfVirtualNodes_out[i] / nodesPerThreadBlock + 1 :
      numOfVirtualNodes_out[i] / nodesPerThreadBlock;
    this->numOfTBs_out += count;
  }
  std::cout << "count finish" << std::endl;
}
*/
//edge based 
//void PartialGraph::countNumOfTBs_out() {
  //for (uint32_t i = 0; i < 8; ++i) {
    //uint32_t nodesPerThreadBlock = EDGES_PER_THREADBLOCK / POW(2, i);
    //uint32_t count = (numOfVirtualNodes_out[i] % nodesPerThreadBlock != 0) ?
      //numOfVirtualNodes_out[i] / nodesPerThreadBlock + 1 :
      //numOfVirtualNodes_out[i] / nodesPerThreadBlock;
    //this->numOfTBs_out += count;
  //}
//}

// normal
/*
void PartialGraph::createThreadBlockInfo_in() {
  uint32_t nodeCurrent = 0;
  uint32_t edgeCurrent = 0;
  uint32_t start = 0;
  for (uint32_t i = 0; i < 8; ++i) {
    uint32_t currentNumOfTBs = 
      (numOfVirtualNodes_in[i] % NUM_OF_THREADS != 0) ?
      numOfVirtualNodes_in[i] / NUM_OF_THREADS + 1 :
      numOfVirtualNodes_in[i] / NUM_OF_THREADS;
    for (uint32_t pos = start; pos < start + currentNumOfTBs; ++pos) {
      tbs_in[pos].edgeOffset = edgeCurrent;
      tbs_in[pos].nodeOffset = nodeCurrent;

      if (start + currentNumOfTBs - 1 == pos) {
        uint32_t nodes = (numOfVirtualNodes_in[i] % NUM_OF_THREADS != 0) ?
          numOfVirtualNodes_in[i] % NUM_OF_THREADS : NUM_OF_THREADS;

        edgeCurrent += (nodes * POW(2, i));
        nodeCurrent += nodes;
      } else {
        edgeCurrent += (NUM_OF_THREADS * POW(2, i));
        nodeCurrent += NUM_OF_THREADS;
      }
    }
    start += currentNumOfTBs;
  }
  assert (start == numOfTBs_in);
  tbs_in[numOfTBs_in].edgeOffset = edgeCurrent;
  tbs_in[numOfTBs_in].nodeOffset = nodeCurrent;
}

*/
/*
// edge based with SM count
void PartialGraph::createThreadBlockInfo_out() {
  uint32_t nodeCurrent = 0;
  uint32_t edgeCurrent = 0;
  uint32_t start = 0;
  for (uint32_t i = 0; i < 8; ++i) {
    uint32_t currentNumOfEdges = numOfVirtualNodes_out[i] * POW(2, i); // * pow(2, i);
    uint32_t edgesPerThreadBlock = (currentNumOfEdges / SM) - ((currentNumOfEdges / SM) % POW(2, i));
    uint32_t nodesPerThreadBlock = edgesPerThreadBlock / POW(2, i); // / pow(2, i);
    uint32_t currentNumOfTBs = (currentNumOfEdges % edgesPerThreadBlock == 0) ?
      currentNumOfEdges / edgesPerThreadBlock :
      currentNumOfEdges / edgesPerThreadBlock + 1;

    for (uint32_t pos = start; pos < start + currentNumOfTBs; ++pos) {
      tbs_out[pos].edgeOffset = edgeCurrent;
      tbs_out[pos].nodeOffset = nodeCurrent;

      if (start + currentNumOfTBs - 1 == pos) {
        uint32_t nodes = (numOfVirtualNodes_out[i] % nodesPerThreadBlock == 0) ?
          nodesPerThreadBlock : (numOfVirtualNodes_out[i] % nodesPerThreadBlock);

        edgeCurrent += (nodes * POW(2, i));
        nodeCurrent += nodes;
      } else {
        edgeCurrent += edgesPerThreadBlock;
        nodeCurrent += nodesPerThreadBlock;
      }
    }
    start += currentNumOfTBs;
      fetch_and_add(numOfVirtualNodes_in[i], numOfNodes_i_in[i]);
      fetch_and_add(numOfVirtualNodes_out[i], numOfNodes_i_out[i]);
  }
  assert (start == numOfTBs_out);
  tbs_out[numOfTBs_out].edgeOffset = edgeCurrent;
  tbs_out[numOfTBs_out].nodeOffset = nodeCurrent;
}
*/
// edge based version
/*
void PartialGraph::createThreadBlockInfo_out() {
  uint32_t nodeCurrent = 0;
  uint32_t edgeCurrent = 0;
  uint32_t start = 0;
  for (uint32_t i = 0; i < 8; ++i) {
    uint32_t currentNumOfEdges = numOfVirtualNodes_out[i] * POW(2, i); // * pow(2, i);
    uint32_t nodesPerThreadBlock = EDGES_PER_THREADBLOCK / POW(2, i); // / pow(2, i);
    uint32_t currentNumOfTBs = (currentNumOfEdges % EDGES_PER_THREADBLOCK == 0) ?
      currentNumOfEdges / EDGES_PER_THREADBLOCK :
      currentNumOfEdges / EDGES_PER_THREADBLOCK + 1;

    for (uint32_t pos = start; pos < start + currentNumOfTBs; ++pos) {
      tbs_out[pos].edgeOffset = edgeCurrent;
      tbs_out[pos].nodeOffset = nodeCurrent;

      if (start + currentNumOfTBs - 1 == pos) {
        uint32_t nodes = (numOfVirtualNodes_out[i] % nodesPerThreadBlock == 0) ?
          nodesPerThreadBlock : (numOfVirtualNodes_out[i] % nodesPerThreadBlock);

        edgeCurrent += (nodes * POW(2, i));
        nodeCurrent += nodes;
      } else {
        edgeCurrent += EDGES_PER_THREADBLOCK;
        nodeCurrent += nodesPerThreadBlock;
      }
    }
    start += currentNumOfTBs;
  }
  assert (start == numOfTBs_out);
  tbs_out[numOfTBs_out].edgeOffset = edgeCurrent;
  tbs_out[numOfTBs_out].nodeOffset = nodeCurrent;
}
*/

/*
//normal
void PartialGraph::createThreadBlockInfo_out() {
  uint32_t nodeCurrent = 0;
  uint32_t edgeCurrent = 0;
  uint32_t start = 0;
  for (uint32_t i = 0; i < 8; ++i) {
    uint32_t currentNumOfTBs = 
      (numOfVirtualNodes_out[i] % NUM_OF_THREADS != 0) ?
      numOfVirtualNodes_out[i] / NUM_OF_THREADS + 1 :
      numOfVirtualNodes_out[i] / NUM_OF_THREADS;
    for (uint32_t pos = start; pos < start + currentNumOfTBs; ++pos) {
      tbs_out[pos].edgeOffset = edgeCurrent;
      tbs_out[pos].nodeOffset = nodeCurrent;

      if (start + currentNumOfTBs - 1 == pos) {
        uint32_t nodes = (numOfVirtualNodes_out[i] % NUM_OF_THREADS != 0) ?
          numOfVirtualNodes_out[i] % NUM_OF_THREADS : NUM_OF_THREADS;

        edgeCurrent += (nodes * POW(2, i));
        nodeCurrent += nodes;
      } else {
        edgeCurrent += (NUM_OF_THREADS * POW(2, i));
        nodeCurrent += NUM_OF_THREADS;
      }
    }
    start += currentNumOfTBs;
  }
  assert (start == numOfTBs_out);
  tbs_out[numOfTBs_out].edgeOffset = edgeCurrent;
  tbs_out[numOfTBs_out].nodeOffset = nodeCurrent;
}
*/

// correct
void PartialGraph::insertDegree(uint32_t src, uint32_t dest) {
  fetch_and_add(inDegrees[dest], 1);
  fetch_and_add(outDegrees[src], 1);
}

void PartialGraph::insertDenseNeighbor(DENSE_TYPE src, DENSE_TYPE dest) {
  for (int i = 5; i >= 0; --i) {
    if (remains_in[i][dest] > 0) {
      denseEdgeList_in[edgePosition_in[i][dest]] = src;
      edgePosition_in[i][dest]++;
      remains_in[i][dest]--;
      break;
    }
  }
  for (int i = 5; i >= 0; --i) {
    if (remains_out[i][src] > 0) {
      denseEdgeList_out[edgePosition_out[i][src]] = dest;
      edgePosition_out[i][src]++;
      remains_out[i][src]--;
      break;
    }
  }
}

void PartialGraph::insertSparseNeighbor(uint32_t src, uint32_t dest) {
  for (int i = 5; i >= 0; --i) {
    if (remains_in[i][dest] > 0) {
      sparseEdgeList_in[edgePosition_in[i][dest]] = src;
      edgePosition_in[i][dest]++;
      remains_in[i][dest]--;
      break;
    }
  }
  for (int i = 5; i >= 0; --i) {
    if (remains_out[i][src] > 0) {
      sparseEdgeList_out[edgePosition_out[i][src]] = dest;
      edgePosition_out[i][src]++;
      remains_out[i][src]--;
      break;
    }
  }
}

//void PartialGraph::createThreadBlockInfo() {
  //countNumOfTBs_in();
  //countNumOfTBs_out();
//
  //tbs_in = new ThreadBlock[numOfTBs_in + 1];
  //tbs_out = new ThreadBlock[numOfTBs_out + 1];
  //createThreadBlockInfo_in();
  //createThreadBlockInfo_out();
//}

void PartialGraph::splitNodes() {
  countNumOfVirtualNodes();
  countNumOfTotalVirtualNodes();
  countNumOfTotalEdges();
  if (isDense) {
    denseEdgeList_in = new DENSE_TYPE[numOfEdges];
    denseEdgeList_out = new DENSE_TYPE[numOfEdges];
  } else {
    sparseEdgeList_in = new uint32_t[numOfEdges];
    sparseEdgeList_out = new uint32_t[numOfEdges];
  }

  // calculate prefix sum for building vertex ID list
  calculatePrefixSum();
  // now build vertex ID list;
  buildVertexIDList(isDense);

  //createThreadBlockInfo();
}

