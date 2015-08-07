#include "Tree.h"
#include <algorithm>

Tree::Tree(unsigned int order, int *almost_prufer) : prufer(order - 2) {
  for (uint i = 0; i < order - 2; ++i) {
    prufer[i] = almost_prufer[i + 2] - 1;
  }
}

void Tree::degree_sequence(std::vector<uchar>& deg_seq) const {
  deg_seq.clear();
  deg_seq.resize(size());

  for (ushort v: prufer) ++deg_seq[v];

  std::sort(deg_seq.begin(), deg_seq.end());
}

uint binom2(uint k) {
  return (k * k - k) >> 1;
}

void Tree::path_sequence(std::vector<ushort>& path_seq) const {
  uint s = binom2(size());
  auto pair_distances = new ushort[s];

  pair_distances[0] = 1;

  uint index = 1;
  for (uint i = 0; i < prufer.size(); ++i) {
    ushort p = prufer[i];
    uint bp = binom2(p);
    for (uint j = 0; j < p; ++j) {
      pair_distances[ index ] = pair_distances[ bp + j ] + 1;
      ++index;
    }

    pair_distances[ index ] = 1;
    ++index;

    uint bj_plus_p = binom2(p + 1) + p;
    for (uint j = p + 1; j < i + 2; ++j) {
      pair_distances[ index ] = pair_distances[ bj_plus_p ] + 1;
      ++index;
      bj_plus_p += j; // increment to next binomial (plus p)
    }

  }


  path_seq.clear();
  path_seq.resize(size());

  for (uint i = 0; i < s; ++i) {
    path_seq[ pair_distances[i] ]++;
  }

  delete[] pair_distances;
}
