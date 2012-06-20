#ifndef TREE_HPP
#define TREE_HPP

#include <vector>

struct Edge
{
	unsigned int u;
	unsigned int v;
};

class Tree
{
	public:
		Tree( unsigned int order );
		~Tree();

		unsigned int getOrder() const { return mOrder; };
		unsigned int getDegree( unsigned int v ) const { if( v < mOrder ) return mDegrees[v]; return 0; };

		bool adjacent( unsigned int u, unsigned int v ) const;
		void addEdge( unsigned int u, unsigned int v );
		void deleteEdge( unsigned int u, unsigned int v );

		std::vector<Edge> getEdges() const;
		unsigned int* getDegreeSequence() const;
		std::vector<unsigned int> getConnectedComponentSizes() const;

		void print() const;

		Tree & operator=( const Tree & rhs );
	
	private:
		unsigned int	mOrder;
		unsigned char	*mAdj;		// Adjacency matrix
		unsigned int	*mDegrees;
};

#endif

