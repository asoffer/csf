#include "Tree.hpp"
#include "DisjointSet.hpp"
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
	mDegrees( NULL ),
	mComponents( NULL )
{
}

Tree::Tree( unsigned int order ) :
	mOrder( order )
{
	int size = order * order;
	mAdj = new unsigned char[size];
	mDegrees = new unsigned int[mOrder];
	mComponents = new DisjointSet( order );

	memset( mAdj, 0, sizeof( unsigned char ) * size );
	memset( mDegrees, 0, sizeof( unsigned int ) * mOrder );
}

Tree::Tree( const Tree & that )
{
	int size = that.mOrder * that.mOrder;
	mAdj = new unsigned char[size];
	mDegrees = new unsigned int[that.mOrder];
	mComponents = new DisjointSet( *that.mComponents );

	memcpy( mAdj, that.mAdj, sizeof( unsigned char ) * size );
	memcpy( mDegrees, that.mDegrees, sizeof( unsigned int ) * that.mOrder );
	mOrder = that.mOrder;
}

Tree::~Tree()
{
	delete [] mAdj;
	delete [] mDegrees;
	delete mComponents;
}

bool Tree::adjacent( unsigned int u, unsigned int v ) const
{
	fatalAssert( u < mOrder && v < mOrder, "adjacent() failed." );

	return mAdj[u * mOrder + v] == 1;
}

void Tree::addEdge( unsigned int u, unsigned int v )
{
	fatalAssert( u != v && u < mOrder && v < mOrder, "addEdge() failed." );
	fatalAssert( !adjacent( u, v ), "Tried to add edge that was already present." );

	mAdj[u * mOrder + v] = 1;
	mAdj[v * mOrder + u] = 1;
	mDegrees[u]++;
	mDegrees[v]++;

	mComponents->setUnion( u, v );
}

void Tree::deleteEdge( unsigned int u, unsigned int v )
{
	fatalAssert( u != v && u < mOrder && v < mOrder, "deleteEdge() failed." );
	fatalAssert( adjacent( u, v ), "Tried to delete edge that wasn't present." );

	mAdj[u * mOrder + v] = 0;
	mAdj[v * mOrder + u] = 0;
	mDegrees[u]--;
	mDegrees[v]--;

	mComponents->split( u, v );
}

std::vector<Edge> Tree::getEdges() const
{
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
	degreeSequence.clear();
	degreeSequence.insert( degreeSequence.end(), &mDegrees[0], &mDegrees[mOrder] );
	std::sort( degreeSequence.begin(), degreeSequence.end(), std::greater<int>() );
}

void Tree::getConnectedComponentSizes( std::vector<unsigned int> & componentSizes ) const
{
	mComponents->getSetSizes( componentSizes );
}

void Tree::getPathNums( std::vector<unsigned int> & pathNums ) const
{
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
		delete [] mAdj;
		delete [] mDegrees;
		delete mComponents;

		mOrder = that.mOrder;
		int size = mOrder * mOrder;

		mAdj = new unsigned char[size];
		mDegrees = new unsigned int[mOrder];
		mComponents = new DisjointSet( *that.mComponents );

		memcpy( mAdj, that.mAdj, sizeof( unsigned char ) * ( size ) );
		memcpy( mDegrees, that.mDegrees, sizeof( unsigned int ) * mOrder );
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

