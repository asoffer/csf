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
		Tree( unsigned int *levels, unsigned int order );
		Tree( const Tree & that );
		~Tree();

		void buildAdjacency();
		void destroyAdjacency();

		unsigned int getOrder() const { return mOrder; };

		bool adjacent( unsigned int u, unsigned int v ) const;
		void addEdge( unsigned int u, unsigned int v );
		void deleteEdge( unsigned int u, unsigned int v );

		std::vector<Edge> getEdges() const;
		void getDegreeSequence( std::vector<unsigned int> & degreeSeqnence ) const;
		void getConnectedComponentSizes( std::vector<unsigned int> & componentSizes ) const;
		void getPathNums( std::vector<unsigned int> & pathNums ) const;

		void print() const;

		Tree & operator=( const Tree & rhs );
	
	private:
		void pathNumsDfs( int i, int count, bool *& visited, std::vector<unsigned int> & pathNums ) const;

		unsigned int	mOrder;
		unsigned char	*mAdj;		// Adjacency matrix
		unsigned int	*mLevels;
};

#endif

