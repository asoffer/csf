#include "Tree.hpp"
#include "Debug.hpp"
#include <vector>
#include <queue>
#include <algorithm>
#include <cstring>
#include <sstream>
#include <string>
#include <iostream>
using namespace std;

Tree::Tree() :
	mOrder( 0 ),
	mAdj( NULL ),
	mLevels( NULL )
{
}

Tree::Tree( unsigned int order ) :
	mOrder( order ),
	mLevels( NULL )
{
	mAdj = new unsigned char[mOrder * mOrder];
	memset( mAdj, 0, sizeof( unsigned char ) * mOrder * mOrder );
}

Tree::Tree( unsigned int *levels, unsigned int order ) :
	mOrder( order ),
	mAdj( NULL )
{
	mLevels = new unsigned int[mOrder];
	memcpy( mLevels, levels, sizeof( unsigned int ) * mOrder );
}

Tree::Tree( const Tree & that )
{
	mOrder = that.mOrder;
	
	if( that.mLevels != NULL )
	{
		mLevels = new unsigned int[mOrder];
		memcpy( mLevels, that.mLevels, sizeof( unsigned int ) * that.mOrder );
	}
	else
	{
		mLevels = NULL;
	}

	if( that.mAdj != NULL )
	{
		unsigned int size = mOrder * mOrder;
		mAdj = new unsigned char[size];
		memcpy( mAdj, that.mAdj, sizeof( unsigned char ) * size );
	}
	else
	{
		mAdj = NULL;
	}
}

Tree::~Tree()
{
	if( mAdj != NULL )
	{
		delete [] mAdj;
		mAdj = NULL;
	}
	if( mLevels != NULL )
	{
		delete [] mLevels;
		mLevels = NULL;
	}
}

void Tree::buildAdjacency()
{
	fatalAssert( mAdj == NULL, "Trying to build a tree that has already been built!" );

	mAdj = new unsigned char[mOrder * mOrder];
	memset( mAdj, 0, sizeof( unsigned char ) * mOrder * mOrder );
	for( unsigned int i = 2; i <= mOrder; i++ )
	{
		unsigned int u = i - 1;
		unsigned int v = mLevels[i - 1] - 1;
		addEdge( u, v );
	}
}

void Tree::destroyAdjacency()
{
	fatalAssert( mAdj != NULL, "Trying to destroy tree which was never built." );

	delete [] mAdj;
	mAdj = NULL;
}

bool Tree::adjacent( unsigned int u, unsigned int v ) const
{
	fatalAssert( mAdj != NULL, "Called adjacent() without building adjacency matrix first." );
	fatalAssert( u < mOrder && v < mOrder, "adjacent() failed." );

	return mAdj[u * mOrder + v] == 1;
}

void Tree::addEdge( unsigned int u, unsigned int v )
{
	fatalAssert( mAdj != NULL, "Called addEdge() without building adjacency matrix first." );
	fatalAssert( u != v && u < mOrder && v < mOrder, "addEdge() failed." );

	mAdj[u * mOrder + v] = 1;
	mAdj[v * mOrder + u] = 1;
}

void Tree::deleteEdge( unsigned int u, unsigned int v )
{
	fatalAssert( mAdj != NULL, "Called deleteEdge() without building adjacency matrix first." );
	fatalAssert( u != v && u < mOrder && v < mOrder, "deleteEdge() failed." );

	mAdj[u * mOrder + v] = 0;
	mAdj[v * mOrder + u] = 0;
}

std::vector<Edge> Tree::getEdges() const
{
	fatalAssert( mAdj != NULL, "Called getEdges() without building adjacency matrix first." );

	std::vector<Edge> edges;
	for( unsigned int i = 0; i < mOrder; i++ )
	{
		for( unsigned int j = i + 1; j < mOrder; j++ )
		{
			if( adjacent( i, j ) )
			{
				Edge e;
				e.u = i;
				e.v = j;
				edges.push_back( e );
			}
		}
	}

	return edges;
}

void Tree::getDegreeSequence( std::vector<unsigned int> & degreeSequence ) const
{
	unsigned int degSeq[mOrder];
	memset( degSeq, 0, sizeof( unsigned int ) * mOrder );
	
	if( mAdj == NULL )
	{
		for( unsigned int i = 2; i <= mOrder; i++ )
		{
			unsigned int u = i - 1;
			unsigned int v = mLevels[i - 1] - 1;
			degSeq[u]++;
			degSeq[v]++;
		}
	}
	else
	{
		for( unsigned int i = 0; i < mOrder; i++ )
		{
			for( unsigned int j = i + 1; j < mOrder; j++ )
			{
				if( adjacent( i, j ) && i < j )
				{
					degSeq[i]++;
					degSeq[j]++;
				}
			}
		}
	}

	degreeSequence.clear();
	degreeSequence.assign( degSeq, degSeq + mOrder );
	std::sort( degreeSequence.begin(), degreeSequence.end(), std::greater<int>() );
}

void Tree::getConnectedComponentSizes( std::vector<unsigned int> & componentSizes ) const
{
	fatalAssert( mAdj != NULL, "Called getConnectedComponentSizes() without building adjacency matrix first." );

	componentSizes.clear();

	bool discovered[mOrder];
	bool processed[mOrder];

	memset( discovered, 0, sizeof( bool ) * mOrder );
	memset( processed, 0, sizeof( bool ) * mOrder );

	queue<unsigned int> vertsToVisit;
	unsigned int currentVertex;
	unsigned int successorVertex;
	unsigned int currentComponentSize = 1;

	for( unsigned int i = 0; i < mOrder; i++ )
	{
		if( discovered[i] == false )
		{
			vertsToVisit.push( i );
			discovered[i] = true;

			currentComponentSize = 0;
			while( !vertsToVisit.empty() )
			{
				currentVertex = vertsToVisit.front();
				vertsToVisit.pop();

				currentComponentSize++;
				processed[currentVertex] = true;

				for( unsigned int j = 0; j < mOrder; j++ )
				{
					if( adjacent( currentVertex, j ) )
					{
						successorVertex = j;
						if( discovered[successorVertex] == false )
						{
							vertsToVisit.push( successorVertex );
							discovered[successorVertex] = true;
						}
					}
				}
			}

			componentSizes.push_back( currentComponentSize );
		}
	}

	std::sort( componentSizes.begin(), componentSizes.end(), std::greater<int>() );
}

void Tree::getPathNums( std::vector<unsigned int> & pathNums ) const
{
	fatalAssert( mAdj != NULL, "Called getPathNums() without building adjacency matrix first." );

	pathNums.clear();
	for( unsigned int i = 0; i < mOrder; i++ )
	{
		pathNums.push_back( 0 );
	}

	bool * visited = new bool[mOrder];
	for( unsigned int i = 0; i < mOrder; i++ )
	{
		memset( visited, 0, sizeof( bool ) * mOrder ); 
		pathNumsDfs( i, 1, visited, pathNums );
	}
	delete [] visited;

	for( unsigned int i = 0; i < pathNums.size(); i++ )
	{
		pathNums[i] /= 2;
	}
}

void Tree::print() const
{
	fatalAssert( mAdj != NULL, "Called print() without building adjacency matrix first." );

	vector<Edge> edgeSet = getEdges();
	for( unsigned int i = 0; i < edgeSet.size(); i++ )
	{
		cout << "(" << edgeSet[i].u << ", " << edgeSet[i].v << ") ";
	}
	cout << endl;
}

Tree & Tree::operator=( const Tree & that )
{
	if( this != &that )
	{
		mOrder = that.mOrder;

		delete [] mLevels;
		mLevels = new unsigned int[mOrder];
		memcpy( mLevels, that.mLevels, sizeof( unsigned int ) * mOrder );

		if( mAdj != NULL )
		{
			delete [] mAdj;
		}
		if( that.mAdj != NULL )
		{
			int size = mOrder * mOrder;
			mAdj = new unsigned char[size];
			memcpy( mAdj, that.mAdj, sizeof( unsigned char ) * ( size ) );
		}
	}

	return *this;
}

void Tree::pathNumsDfs( int i, int count, bool *& visited, std::vector<unsigned int> & pathNums ) const
{
	visited[i] = true;
	for( unsigned int j = 0; j < mOrder; j++ )
	{
		if( adjacent( i, j ) && !visited[j] )
		{
			pathNums[count]++;
			pathNumsDfs( j, count + 1, visited, pathNums );
		}
	}
}

