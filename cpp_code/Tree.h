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

//        unsigned int getOrder() const { return mOrder; };
//        unsigned int getDegree(unsigned int v) const { if(v < mOrder) return mDegrees[v]; return 0; };

//        std::vector<Edge> getEdges() const;
//        void getDegreeSequence(std::vector<unsigned int> & degreeSeqnence) const;
//        void getPathNums(std::vector<unsigned int> & pathNums) const;

//        void print() const;

    private:
        std::vector<uchar> prufer;
};

#endif

