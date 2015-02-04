import os

# Returns a graph that looks like a bowtie, see Richard Stanley's
# "A Symmetric Function Generalization of the Chromatic Polynomial
# of a Graph" fig. 1, p. 170.
def bowtie():
    return Graph({ 0:[1,2], 1:[0,2], 2:[0,1,3,4], 3:[2,4], 4:[2,3] })

# Returns a graph that looks like a kite, see Richard Stanley's
# "A Symmetric Function Generalization of the Chromatic Polynomial
# of a Graph" fig. 1, p. 170.
def kite():
    return Graph({ 0:[1,2,3], 1:[0,3], 2:[0,3,4], 3:[0,1,2], 4:[2] })

# Glues two graphs, g and h, together by 'stapling' the vertices v
# (from g) and u (from h) together, and returns this glued graph.
def glue(g, v, h, u):
    r = Graph(g.num_verts())
    r.add_edges(g.edges())
	
    n = g.num_verts()
    newVerts = {}
    for vert in xrange(h.num_verts()):
        if vert != u:
            newVerts[vert] = n
            n += 1

    for e in h.edges():
        if u in e:
            if e[0] == u:
                r.add_edge(v, newVerts[e[1]])
            elif e[1] == u:
                r.add_edge(newVerts[e[0]], v)
        else:
            r.add_edge(newVerts[e[0]], newVerts[e[1]] )
    return r

def isomorphic_only(graphs):
    return [graphs[i] for i in range(len(graphs)) if all([not graphs[i].is_isomorphic(graphs[j]) for j in range(i+1, len(graphs))])]

def gluings(g, h):
    glued = []
    for i in range(g.num_verts()):
        for j in range(i, h.num_verts()):
            glued.append(glue(g, i, h, j))
    return isomorphic_only(glued)

def save_plots(graphs, filepath):
    for i in range(len(graphs)):
        graphs[i].plot().save(os.path.join(filepath, '{0}.svg'.format(i)))


