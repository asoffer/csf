#include "Tree.h"
#include "Debug.hpp"

Tree::Tree(unsigned int order, int *almost_prufer) : prufer(order - 2) {
  for (uint i = 0; i < order - 2; ++i) {
    prufer[i] = almost_prufer[i + 2] - 1;
  }
}
