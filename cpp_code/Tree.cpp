#include "Tree.hpp"
#include <vector>
#include <queue>
#include <algorithm>
#include <cstring>
#include <iostream>
using namespace std;

Tree::Tree() :
	mOrder( 0 ),
	mAdj( NULL ),
	mDegrees( NULL )
{
}

Tree::Tree( unsigned int order ) :
	mOrder( order )
{
	// Simple, undirected graph, so we know the
	// adjacency matrix will be symmetric, and have
	// 0 values along the diagonal, so we can
	// use only the upper triangular part of this
	// matrix to save some space.
	int size = order * order;
	mAdj = new unsigned char[size];

	mDegrees = new unsigned int[mOrder];

	memset( mAdj, 0, sizeof( unsigned char ) * size );
	memset( mDegrees, 0, sizeof( unsigned int ) * mOrder );
}

Tree::~Tree()
{
	if( mAdj != NULL )
	{
		delete [] mAdj;
		mAdj = NULL;
	}
	if( mDegrees != NULL )
	{
		delete [] mDegrees;
		mDegrees = NULL;
	}
}

bool Tree::adjacent( unsigned int u, unsigned int v ) const
{
	if( u == v || u > mOrder || v > mOrder )
	{
		return false;
	}
	else
	{
		return mAdj[u * mOrder + v] == 1;
	}
}

void Tree::addEdge( unsigned int u, unsigned int v )
{
	if( u == v || u > mOrder || v > mOrder )
	{
		return;
	}
	else
	{
		mAdj[u * mOrder + v] = 1;
		mAdj[v * mOrder + u] = 1;
		mDegrees[u]++;
		mDegrees[v]++;
	}
}

void Tree::deleteEdge( unsigned int u, unsigned int v )
{
	if( u == v || u > mOrder || v > mOrder )
	{
		return;
	}
	else
	{
		mAdj[u * mOrder + v] = 0;
		mAdj[v * mOrder + u] = 0;
		mDegrees[u]--;
		mDegrees[v]--;
	}
}

std::vector<Edge> Tree::getEdges() const
{
	std::vector<Edge> edges;
	for( int i = 0; i < mOrder; i++ )
	{
		for( int j = i + 1; j < mOrder; j++ )
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

unsigned int* Tree::getDegreeSequence() const
{
	unsigned int *degreeSequence = new unsigned int[mOrder];
	memcpy( degreeSequence, mDegrees, sizeof( unsigned int ) * mOrder );
	std::sort( degreeSequence, degreeSequence + mOrder, std::greater<int>() );
	return degreeSequence;
}

std::vector<unsigned int> Tree::getConnectedComponentSizes() const
{
	bool discovered[mOrder];
	bool processed[mOrder];

	memset( discovered, 0, sizeof( bool ) * mOrder );
	memset( processed, 0, sizeof( bool ) * mOrder );

	queue<unsigned int> vertsToVisit;
	vector<unsigned int> componentSizes;
	unsigned int currentVertex;
	unsigned int successorVertex;
	unsigned int currentComponentSize = 1;

	for( int i = 0; i < mOrder; i++ )
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

				for( int j = 0; j < mOrder; j++ )
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
	return componentSizes;
}

void Tree::print() const
{
	for( int i = 0; i < mOrder; i++ )
	{
		for( int j = 0; j < mOrder; j++ )
		{
			if( adjacent( i, j ) )
			{
				cout << "1 ";
			}
			else
			{
				cout << "0 ";
			}
		}
		cout << endl;
	}
}

Tree & Tree::operator=( const Tree & rhs )
{
	if( this == &rhs )
	{
		return *this;
	}

	delete [] this->mAdj;
	delete [] this->mDegrees;

	this->mOrder = rhs.mOrder;
	this->mAdj = new unsigned char[rhs.mOrder * rhs.mOrder];
	memcpy( this->mAdj, rhs.mAdj, sizeof( unsigned char ) * ( rhs.mOrder * rhs.mOrder ) );
	this->mDegrees = new unsigned int[rhs.mOrder];
	memcpy( this->mDegrees, rhs.mDegrees, sizeof( unsigned int ) * rhs.mOrder );

	return *this;
}

