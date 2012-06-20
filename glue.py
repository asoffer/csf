# Returns a graph that looks like a bowtie, see Richard Stanley's
# "A Symmetric Function Generalization of the Chromatic Polynomial
# of a Graph" fig. 1, p. 170.
def bowtie():
	return Graph( { 0:[1,2], 1:[0,2], 2:[0,1,3,4], 3:[2,4], 4:[2,3] } )

# Returns a graph that looks like a kite, see Richard Stanley's
# "A Symmetric Function Generalization of the Chromatic Polynomial
# of a Graph" fig. 1, p. 170.
def kite():
	return Graph( { 0:[1,2,3], 1:[0,3], 2:[0,3,4], 3:[0,1,2], 4:[2] } )

# Glues two graphs, g and h, together by 'stapling' the vertices v
# (from g) and u (from h) together, and returns this glued graph.
def glue( g, v, h, u ):
	r = Graph( g.num_verts() )
	r.add_edges( g.edges() )
	
	n = g.num_verts()
	newVerts = {}
	for vert in xrange( h.num_verts() ):
		if vert != u:
			newVerts[vert] = n
			n += 1

	for e in h.edges():
		if u in e:
			if e[0] == u:
				r.add_edge( v, newVerts[e[1]] )
			elif e[1] == u:
				r.add_edge( newVerts[e[0]], v )
		else:
			r.add_edge( newVerts[e[0]], newVerts[e[1]]  )
	return r

