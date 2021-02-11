#ifndef EDGE_H
#define EDGE_H

class Edge {
public:
  Edge(unsigned src, unsigned dest) {
    this->src = src;
    this->dest = dest;
  }

  unsigned getSrc() { return this->src; }
  unsigned getDest() { return this->dest; }

private:
  unsigned src;
  unsigned dest;
};

#endif
