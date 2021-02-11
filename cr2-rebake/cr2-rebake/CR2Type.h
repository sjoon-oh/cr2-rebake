// [rebake project] CR2
// author: SukJoon Oh, acoustikue@yonsei.ac.kr
// '21.02.07.
//

//

#include <stdint.h>

namespace cr2 {

    typedef uint16_t DENSE_TYPE;
    typedef uint32_t SPARSE_TYPE;

    typedef uint32_t NODE; // Node type
    typedef uint32_t EDGE; // Edge type

    // Specifies degree
    enum DEGREE {
        DEG_1 = 0, DEG_2, DEG_4, DEG_8, DEG_16, DEG_32
    };

    enum DIR { IN = 0, OUT };
    
    class Edge final {
    private:
        NODE src_vertex;
        NODE dst_vertex;

    public:
        Edge(unsigned argsrc, unsigned argdst) : src_vertex(argsrc), dst_vertex(argdst) { };

        // Getter
        const unsigned getSrcVertex() { return src_vertex; };
        const unsigned getDstVertex() { return dst_vertex; };
    };

    // Cluster ID
    inline DENSE_TYPE cluster_id(SPARSE_TYPE argVid) { return argVid & 0xFFFF0000; }
    inline SPARSE_TYPE global_id(DENSE_TYPE argVid, SPARSE_TYPE argSVid) { 
    
        return (argVid << 16) | (argSVid & 0XFFFF0000);
    };

    enum LOC { START = 0, END};
    // Index values.
}