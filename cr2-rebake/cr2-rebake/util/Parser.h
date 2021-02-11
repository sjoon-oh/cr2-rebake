#ifndef PARSER_H
#define PARSER_H

#include <string>
#include <vector>
#include <fstream>
#include <cstdlib>
#include <cstring>

#include "Edge.h"

class Parser {
  typedef std::vector<Edge> EdgeList;

public:
  Parser(std::string edgeListFileName) {
    this->edgeListFileName = edgeListFileName;
  }

  void readFiles() {
    // read edge file
    uint32_t src, dest;

    std::ifstream edgeFile(edgeListFileName);

    if (!edgeFile.is_open()) {
      std::cout << "file open fail: " << edgeListFileName << std::endl;
      std::exit(-1);
    }

    while (edgeFile >> src >> dest) {
      if (src != dest)
        edgeList.push_back(Edge(src, dest));
    }
    std::cout << "finish reading" <<std::endl;
  }

  const EdgeList getEdgeList() { return edgeList; }

private:
  std::string edgeListFileName;

  EdgeList edgeList;
};

#endif
