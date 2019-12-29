#include <array>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <string>
#include <thread>
#include <vector>

#include "absl/container/flat_hash_map.h"

template <size_t N>
struct LevelSequence {
  static LevelSequence<N> RootedPath() {
    LevelSequence<N> seq;
    int i = 0;
    for (; i <= static_cast<int>(N / 2); ++i) { seq.data_[i] = i; }
    int j = 0;
    for (; i < static_cast<int>(N); ++i) { seq.data_[i] = ++j; }
    return seq;
  }

  int operator[](int n) const { return data_[n]; }

  bool IsCentrallyRooted() const {
    static bool first = false;
    // m is going to be incremented until it becomes the index of the second
    // `1`, or no `1`s are found.
    int m = 2;

    int max = 1;
    while (data_[m] != 1) {
      max = std::max(max, data_[m]);
      ++m;
      if (m == N) { return false; }
    }
    int max_less_one = max - 1;

    int second_half_max = 0;
    for (int i = m; i < static_cast<int>(N); ++i) {
      second_half_max = std::max(second_half_max, data_[i]);
    }

    if (second_half_max > max_less_one) { return true; }
    if (second_half_max < max_less_one) { return false; }
    int first_tree_size  = m - 1;
    int second_tree_size = N - first_tree_size;
    if (first_tree_size < second_tree_size) { return true; }
    if (first_tree_size > second_tree_size) { return false; }
    for (int i = 2; i < m; ++i) {
      if (data_[i] - 1 > data_[i + m - 2]) { return false; }
      if (data_[i] - 1 < data_[i + m - 2]) { return true; }
    }

    return true;
  }

  bool Next() {
    int last_non_one_index = N - 1;
    while (data_[last_non_one_index] == 1) { --last_non_one_index; }
    int val    = data_[last_non_one_index];
    if (last_non_one_index == 0) { return false; }

    int parent = ParentOf(last_non_one_index, val);

    while (last_non_one_index < static_cast<int>(N)) {
      data_[last_non_one_index++] = data_[parent++];
    }
    return true;
  }

  std::string Serialize() {
    std::string s;
    for (int n : data_) { s.push_back(static_cast<char>(n + 'a')); }
    s.push_back('\n');
    return s;
  }

   private:
    int ParentOf(int index, int val) const {
      int parent = index - 1;
      switch (parent % 4) {
        case 3:
          if (data_[parent--] == val - 1) { return ++parent; }
          [[fallthrough]];
        case 2:
          if (data_[parent--] == val - 1) { return ++parent; }
          [[fallthrough]];
        case 1:
          if (data_[parent--] == val - 1) { return ++parent; }
          [[fallthrough]];
        case 0:;
      }
      while (data_[parent] != val - 1) { --parent; }
      return parent;
    }

    std::array<int, N> data_;
};

template <size_t N>
struct DegreeSequence {
  DegreeSequence(LevelSequence<N> const& l) {
    std::array<int, N> degs = {};
    for(int i = 0; i < static_cast<int>(N); ++i) {
      int j = i - 1;
      while (j >= 0) {
        if (l[j] == l[i] - 1){
          ++degs[j];
          ++degs[i];
          break;
        }

        --j;
      }
    }

    for (int d : degs) { ++data_[d]; }
  }
  std::array<int8_t, N> data_ = {};

  template <typename H>
  friend H AbslHashValue(H h, DegreeSequence<N> const& d) {
    return H::combine(std::move(h), d.data_);
  }

  friend bool operator==(DegreeSequence<N> const& lhs,
                         DegreeSequence<N> const& rhs) {
    return lhs.data_ == rhs.data_;
  }

  friend bool operator<(DegreeSequence<N> const& lhs,
                         DegreeSequence<N> const& rhs) {
    return lhs.data_ < rhs.data_;
  }
  friend bool operator!=(DegreeSequence<N> const& lhs,
                         DegreeSequence<N> const& rhs) {
    return !(lhs == rhs);
  }

  std::string ToString() const {
    std::string s = "[ ";
    for (int n : data_) {
      s.append(std::to_string(static_cast<int>(n)));
      s.push_back(' ');
    }
    s.push_back(']');
    return s;
  }
  std::string Serialize() {
    std::string s;
    for (int n : data_) { s.push_back(static_cast<char>(n + 'a')); }
    s.push_back('\n');
    return s;
  }
};

template <size_t N>
void ToOrderedVector(
    absl::flat_hash_map<DegreeSequence<N>, std::vector<LevelSequence<N>>>&&
        seqs) {}

template <size_t N, typename Fn>
int WriteLevelSequences(std::string const& prefix, Fn fn) {
  std::vector<std::thread> threads;
  constexpr size_t NumTreesPerShard = 1 << 19;

  auto l = LevelSequence<N>::RootedPath();

  int shard_num        = 0;
  std::string filename = prefix + std::to_string(shard_num);
  std::FILE* f         = std::fopen(filename.c_str(), "w");
  int count            = 0;
 
  absl::flat_hash_map<DegreeSequence<N>, std::vector<LevelSequence<N>>> seqs;
  do {
    bool b = l.IsCentrallyRooted();
    if (b) {
      seqs[DegreeSequence(l)].push_back(l);

      ++count;
      if (count % NumTreesPerShard == 0) {
        threads.emplace_back(
            [seqs(std::move(seqs))] () mutable { ToOrderedVector(std::move(seqs)); });
        seqs.clear();
        ++shard_num;
        // v.emplace_back(fn, filename);
        // std::fclose(f);
        filename = prefix + std::to_string(shard_num);
        f        = std::fopen(filename.c_str(), "w");
      }
    }
  } while (l.Next());
  std::fclose(f);
  // v.emplace_back(fn, filename);
  // return v;
  for (auto& t : threads) { t.join(); }
  return count;
}

void ReadTrees(std::string filename) {
  std::ifstream ifs(filename);
  std::string line;
  while (ifs >> line) { std::cerr << line << "\n"; }
}

std::atomic<int> count = 0;

template <size_t N>
struct LevelSequenceStream {
  template <typename Next>
  void Chunk(size_t shard_size, Next n) const {
    std::vector<LevelSequence<N>> result;
    result.reserve(shard_size);
    auto l = LevelSequence<N>::RootedPath();
    do {
      if (not l.IsCentrallyRooted()) { continue; }
      result.push_back(l);
      if (result.size() == shard_size) {
        ++count;
        std::thread t([n, r(std::move(result))]() mutable {
          n(std::move(r));
          --count;
        });
        t.detach();
        
        result.clear();
        result.reserve(shard_size);
      }
    } while (l.Next());

    ++count;
    std::thread t([n, r(std::move(result))]() mutable {
      n(std::move(r));
      --count;
    });
    t.detach();
  }
};

template <typename Fn, typename Next>
struct Map {
  template <typename T>
  void operator()(T input) {
    Next{}(Fn{}(std::move(input)));
  }
};

template <size_t N>
struct DegreeSequenceHash {
  absl::flat_hash_map<DegreeSequence<N>, std::vector<LevelSequence<N>>>
  operator()(std::vector<LevelSequence<N>> const& v) const {
    absl::flat_hash_map<DegreeSequence<N>, std::vector<LevelSequence<N>>> seqs;
    for (auto const& l : v) { seqs[DegreeSequence(l)].push_back(l); }
    return seqs;
  }
};

std::atomic <int> full_total = 0;
struct ToKeyOrderedVector {
  template <typename K, typename V>
  std::vector<std::pair<K, V>> operator()(absl::flat_hash_map<K, V> map) const {
    std::vector<std::pair<K, V>> result;
    for (auto& [k, v] : map) { result.emplace_back(k, std::move(v)); }
    std::sort(
        result.begin(), result.end(),
        [](auto const& lhs, auto const& rhs) { return lhs.first < rhs.first; });

    size_t total = 0;
    for (auto const& r : result) { total += r.second.size(); }
    full_total += total;
    return result;
  }
};

// LevelSequenceStream()
//     .Chunk(1 << 19)
//     .Map(ToKeyOrderedVector)
//     .Merge()
//     .FilterValues([](auto const& v) { return v.size() > 1; });
//     .Dump("/tmp/trees-");
struct Dummy {
  template <typename T>
  void operator()(T const&) {}
};
int main(int argc, char* argv[]) {
  constexpr size_t N = 25;
  /*auto threads =*/ 
  // std::cerr << WriteLevelSequences<N>("/tmp/trees-", ReadTrees) << " trees";
  //for (auto& thread : threads) { thread.join(); }
  // Pipeline<MapBy<DegreeSequenceHash<N>>, MapBy<ToKeyOrderedVector>, Dummy>;
  LevelSequenceStream<N>().Chunk(
      1 << 21, Map<DegreeSequenceHash<N>, Map<ToKeyOrderedVector, Dummy>>{});

  while (count != 0)
    ;
  std::cerr << "*** " << full_total << "\n";
  return 0;
}
