#ifndef TREE_HPP
#define TREE_HPP

#include <vector>
#include <string>

struct Edge
{
	unsigned int u;
	unsigned int v;
};

class Tree
{
	public:
		Tree();
		Tree( unsigned int order );
		Tree( const Tree & that );
		~Tree();

		unsigned int getOrder() const { return mOrder; };
		unsigned int getDegree( unsigned int v ) const { if( v < mOrder ) return mDegrees[v]; return 0; };

		bool adjacent( unsigned int u, unsigned int v ) const;
		void addEdge( unsigned int u, unsigned int v );
		void deleteEdge( unsigned int u, unsigned int v );

		std::vector<Edge> getEdges() const;
		unsigned int* getDegreeSequence() const;
		void getDegreeSequence( std::string & degreeSeqnence ) const;
		void getConnectedComponentSizes( std::string & componentSizes ) const;

		void print() const;

		Tree & operator=( const Tree & rhs );
	
	private:
		unsigned int	mOrder;
		unsigned char	*mAdj;		// Adjacency matrix
		unsigned int	*mDegrees;
};

#endif

