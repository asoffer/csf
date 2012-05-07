def csf( G, basis = 'power' ):
	"""Uses Thm 2.5 from Stanley's "A Symmetric Function Generalization
		of the Chromatic Polynomial of a Graph" to calculate the chromatic
		symmetric function of the graph with the given vertices and edges,
		returning the CSF in the specified basis."""

	n = G.num_verts()    # Number of vertices in the graph
	# A spanning subgraph of G has all its vertices, but only a subset of edges.
	# We want to consider all such edge subsets, so we take their powerset.
	powersetOfEdges = list( powerset( G.edges() ) )

	csf = 0
	for s in powersetOfEdges:
		g = Graph( n )
		g.add_edges( s )
		# Create the partition whose parts are equal fo the vertex sizes
		# of the connected components of the spanning subgraph of G with
		# edge set S
		lambdaOfS = [len( component ) for component in g.connected_components()]
		
		# Add this power-sum term to the CSF
		csf += ( ( -1 )^len( s ) ) * SFAPower( QQ )( lambdaOfS )

	if basis[0] == 'p':
		return csf
	elif basis[0] == 'm':
		return SFAMonomial( QQ )( csf )
	elif basis[0] == 'e':
		return SFAElementary( QQ )( csf )
	elif basis[0] == 'h':
		return SFAHomogeneous( QQ )( csf )
	elif basis[0] == 's':
		return SFASchur( QQ )( csf )
	else:
		return csf
