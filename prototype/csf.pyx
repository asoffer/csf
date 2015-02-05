import cython
cimport cython
from sage.graphs.graph import Graph
from sage.graphs.base.sparse_graph cimport SparseGraph
from collections import defaultdict

cdef extern from "math.h":
    double log2(double)

# Compute the Gray code of a given number. Gray_code(n) = n xor floor(n).
cdef inline unsigned long long getGrayCode(unsigned long long n): return (n >> 1) ^ n

cdef dict fastCsf(object G):
    cdef unsigned long long i = 1
    cdef unsigned long long g1
    cdef unsigned long long g2
    cdef unsigned long long diff

    cdef long n = G.num_verts()     # Degree of G
    cdef object H = Graph(n, implementation = 'c_graph', sparse = True)
    cdef SparseGraph sparseH = <SparseGraph> H._backend._cg
    cdef list edgeSet = G.edges()

    cdef dict csf = <dict>defaultdict(int)
    cdef list lambdaOfS = [1] * n
    csf[tuple(lambdaOfS)] += 1
    
    cdef unsigned long long powersetCardinality = 2 ** len(edgeSet)
    cdef int sign = -1
    while(i < powersetCardinality):
        g1 = getGrayCode(i - 1)
        g2 = getGrayCode(i)
        diff = g1 ^ g2

        # If a 1 was added from g1 to g2, then we insert the element corresponding to that 1
        if(diff & g2 == diff):
            edge = edgeSet[<long>log2(diff)]
            sparseH.add_arc_unsafe(edge[0], edge[1])
            sparseH.add_arc_unsafe(edge[1], edge[0])
        # If a 1 was removed from g1 to g2, then we remove the element corresponding to that 1
        else:
            edge = edgeSet[<long>log2(diff)]
            sparseH.del_arc_unsafe(edge[0], edge[1])
            sparseH.del_arc_unsafe(edge[1], edge[0])
            
        # Create the partition whose parts are equal fo the vertex sizes
        # of the connected components of the spanning subgraph of G with
        # edge set S
        lambdaOfS = [len(component) for component in H.connected_components()]
        # Add this power-sum term to the CSF coefficient dictionary
        csf[tuple(lambdaOfS)] += sign
        sign = -sign
        
        i += 1

    return csf

