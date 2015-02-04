import cython
cimport cython
import numpy as np
from sage.graphs.graph import Graph
from sage.graphs.base.sparse_graph cimport SparseGraph
from collections import defaultdict
from sage.combinat.combination import Combinations
from sage.graphs.trees import TreeIterator

# Get a dictionary of trees with degree sequences as keys, and a list of trees
# with that degree sequence as the values
cdef dict getTrees(int n):
    cdef dict treesByDegreeSequence = <dict>defaultdict(list)
    for t in TreeIterator(n):
        treesByDegreeSequence[tuple(t.degree_sequence())].append(t)
    return treesByDegreeSequence

# Generates subsets of edge indices with strong minimal change property. That
# is, it returns a list of subsets of indices to an edge array such that the
# difference between a one subset and the next is the replacement of one
# element with another. This is a Hamiltonian circuit of the subsets.
# REFERENCE: Eades, McKay. An Algorithm for Generating Subsets of Fixed Size
#            with a Strong Minimal Change Property. Information Processing
#            Letters 19 (1984), pp131-133.
class SubsetGenerator:
    def __init__(self, n):
        self.n = n
        self.k = 0
        self.subset = []
        self.listOfSubsets = []
        self.deltas = []

    def forward(self, pointer, difference):
        if pointer < self.k and (difference - pointer < self.n - self.k - 1):
            self.forward(pointer + 2, difference + 2)
            self.process(pointer + 1, self.n - self.k + pointer + 1)
            self.reverse(pointer + 1, difference + 2)
            self.process(pointer, difference + 2)
            self.forward(pointer, difference + 1)
        elif pointer == self.k:
            for lastInArray in xrange(difference + 2, self.n + 1):
                self.process(self.k, lastInArray)

    def reverse(self, pointer, difference):
        if pointer < self.k and (difference - pointer < self.n - self.k - 1):
            self.reverse(pointer, difference + 1)
            self.process(pointer, difference + 1)
            self.forward(pointer + 1, difference + 2)
            self.process(pointer + 1, difference + 2)
            self.reverse(pointer + 2, difference + 2)
        elif pointer == self.k:
            for lastInArray in reversed(xrange(difference + 1, self.n)):
                self.process(self.k, lastInArray)

    def process(self, posChanged, newValue):
        self.deltas.append((self.subset[posChanged - 1], newValue - 1))
        self.subset[posChanged - 1] = newValue - 1
        self.L.append(self.subset[:])

    def generateSubsets(self, k):
        self.L = []
        self.deltas = []
        self.k = k
        self.subset = range(k)

        self.process(1, 1)
        self.forward(1, 0)

        return (self.L, self.deltas)

# Certain trees (like paths, stars, spiders) are already settled by Martin,
# Morin, and Wagner, so they don't need to be checked. This probably doesn't
# result in massive speed gains, since the vast majority of trees will not
# have these degree sequences. Can't make it TOO easy!
def settledCase(degSeq):
    n = len(degSeq)
    # Star
    if degSeq[1] == 1:
        return True
    # Path
    elif degSeq[n-3] == 2:
        return True
    # Spider
    elif degSeq[1] == 2:
        return True
    # 'Crab' according to Tan
    elif degSeq[2] == 1:
        return True
    # (a,b,2,1,...,1)
    elif degSeq[2] == 2 and degSeq[3] == 1:
        return True
    # (a,b,2,2,1,...,1)
    elif degSeq[2] == 2 and degSeq[3] == 2 and degSeq[4] == 1:
        return True
    # (a,b,3,1,...,1)
    elif degSeq[2] == 3 and degSeq[4] == 1:
        return True
    # (k,k,k,1,...,1)
    elif degSeq[0] == degSeq[1] and degSeq[1] == degSeq[2] and degSeq[3] == 1:
        return True
    else:
        return False

# Test Stanley's conjecture that no two trees have the same chromatic symmetric
# function by taking the k-slice of each tree's chromatic symmetric function
# and comparing this against the k-slice of each other tree with the same
# degree sequence.
cdef testConjecture(int n):
    cdef dict treesByDegSeq
    cdef list treeList
    cdef object t
    cdef object temp_t
    cdef SparseGraph sparse_temp_t
    cdef list edgeSet
    cdef list edgeDeltas
    cdef tuple edgeChange
    cdef object subsetGen = SubsetGenerator(n - 1)    # Generate subsets of indices to edges
    cdef list lambdaOfS
    cdef list partialCsfList
    cdef dict currPartialCsf
    cdef list treesToRemove
    cdef list tempList
    cdef int additive
    cdef int i = 0
    cdef int j = 0
    cdef int size
    cdef k = 2

    # Organize the trees by degree sequence
    treesByDegSeq = getTrees(n)
    for degSeq in treesByDegSeq:
        treeList = treesByDegSeq[degSeq]
        print "Working with degSeq =",degSeq,", count =",len(treeList)
        k = 3
        # Continue incrementing k and taking subsequently larger k-slices
        # in an attempt to eliminate trees with the same CSF
        while(k < n and len(treeList) > 1):
            print "\tk =",k,

            additive = (-1) ** k
            # This is the k-slice, a nomenclature in development
            partialCsfList = []
            # Compute the k-slice for each tree with the current degree sequence
            for t in treeList:
                edgeSet = t.edges()
                edgeDeltas = subsetGen.generateSubsets(k)[1]
                currPartialCsf = <dict>defaultdict(int)
                temp_t = Graph(n, implementation = 'c_graph', sparse = True)
                sparse_temp_t = <SparseGraph> temp_t._backend._cg
                
                for e in range(k):
                    temp_t.add_edge(edgeSet[e])
                lambdaOfS = [len(c) for c in temp_t.connected_components()]
                currPartialCsf[tuple(lambdaOfS)] += additive
                edgeDeltas.pop(0)

                for edgeChange in edgeDeltas:
                    removeEdge = edgeSet[edgeChange[0]]
                    addEdge = edgeSet[edgeChange[1]]
                    
                    sparse_temp_t.del_arc_unsafe(removeEdge[0], removeEdge[1])
                    sparse_temp_t.del_arc_unsafe(removeEdge[1], removeEdge[0])
                    sparse_temp_t.add_arc_unsafe(addEdge[0], addEdge[1])
                    sparse_temp_t.add_arc_unsafe(addEdge[1], addEdge[0])

                    lambdaOfS = [len(c) for c in temp_t.connected_components()]
                    currPartialCsf[tuple(lambdaOfS)] += additive

                partialCsfList.append(currPartialCsf)

            # Find trees that have different k-slices; these will have
            # different CSFs
            i = 0
            j = 0
            size = len(partialCsfList)
            treesToKeep = []
            while(i < size):
                j = i + 1
                while(j < size):
                    csf1 = partialCsfList[i]
                    csf2 = partialCsfList[j]
                    if csf1 == csf2:
                        treesToKeep.append(i)
                        treesToKeep.append(j)
                    j += 1
                i += 1

            treesToKeep = list(set(treesToKeep))
            treesKept = []
            for idx in treesToKeep:
                treesKept.append(treeList[idx])
            treeList = treesKept[:]

            print "\tRemoved", size - len(treesKept),"trees,",len(treeList),"left to consider"
            treesByDegSeq[degSeq] = treeList
            k += 1

        if(len(treeList) <= 1):
            treesByDegSeq[degSeq] = []
        else:
            print "COUNTER-EXAMPLE FOUND!"
            print degSeq
            for counterexample in treeList:
                print counterexample.edges(labels = False)

    # Once we're all done, print the length of the lists of trees with the same
    # degree sequence. If each is 0, then the conjecture has been verified
    for k in treesByDegSeq:
        print k, len(treesByDegSeq[k])

# Run the conjecture tester.
# I can get up to about order 17 on my VM (quite limited resources), but at 18
# my VM starts to hang/freeze up (probably due to digging into swap space, and
# taking an increasing amount of time to actually generate the trees.
testConjecture(15)

