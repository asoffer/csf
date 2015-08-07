#ifndef TREE_HPP
#define TREE_HPP

#include <vector>
#include <string>

#include "typedefs.h"

struct Edge {
    uint u, v;
};

class Tree {
    public:
        Tree(){};
        Tree(unsigned int order, int *levelSequence);

        inline uint size() const { return prufer.size() + 2; };
//        unsigned int getDegree(unsigned int v) const { if(v < mOrder) return mDegrees[v]; return 0; };

//        std::vector<Edge> getEdges() const;
        void degree_sequence(std::vector<uchar>& deg_seq) const;
        void path_sequence(std::vector<ushort>& path_seq) const;

//        void print() const;

    private:
        std::vector<uchar> prufer;
};

#endif

