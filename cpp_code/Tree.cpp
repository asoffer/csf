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
	mDegrees( NULL )
{
}

Tree::Tree( unsigned int order ) :
	mOrder( order )
{
	int size = order * order;
	mAdj = new unsigned char[size];
	mDegrees = new unsigned int[mOrder];

	memset( mAdj, 0, sizeof( unsigned char ) * size );
	memset( mDegrees, 0, sizeof( unsigned int ) * mOrder );
}

Tree::Tree( const Tree & that )
{
	int size = that.mOrder * that.mOrder;
	mAdj = new unsigned char[size];
	mDegrees = new unsigned int[that.mOrder];

	memcpy( mAdj, that.mAdj, sizeof( unsigned char ) * size );
	memcpy( mDegrees, that.mDegrees, sizeof( unsigned int ) * that.mOrder );
	mOrder = that.mOrder;
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
	fatalAssert( u < mOrder && v < mOrder, "adjacent() failed." );

	return mAdj[u * mOrder + v] == 1;
}

void Tree::addEdge( unsigned int u, unsigned int v )
{
	fatalAssert( u != v && u < mOrder && v < mOrder, "addEdge() failed." );

	mAdj[u * mOrder + v] = 1;
	mAdj[v * mOrder + u] = 1;
	mDegrees[u]++;
	mDegrees[v]++;
}

void Tree::deleteEdge( unsigned int u, unsigned int v )
{
	fatalAssert( u != v && u < mOrder && v < mOrder, "deleteEdge() failed." );

	mAdj[u * mOrder + v] = 0;
	mAdj[v * mOrder + u] = 0;
	mDegrees[u]--;
	mDegrees[v]--;
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

void Tree::getConnectedComponentSizes( std::string & componentSizes ) const
{
	bool discovered[mOrder];
	bool processed[mOrder];

	memset( discovered, 0, sizeof( bool ) * mOrder );
	memset( processed, 0, sizeof( bool ) * mOrder );

	queue<unsigned int> vertsToVisit;
	vector<int> componentSizesVector;
	ostringstream componentSizesStream;
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

			componentSizesVector.push_back( currentComponentSize );
		}
	}

	std::sort( componentSizesVector.begin(), componentSizesVector.end(), std::greater<int>() );
	for( unsigned int i = 0; i < componentSizesVector.size(); i++ )
	{
		componentSizesStream << componentSizesVector[i];
		if( i != componentSizesVector.size() - 1 )
		{
			componentSizesStream << ",";
		}
	}

	componentSizes = componentSizesStream.str();
}

void Tree::getDegreeSequence( std::string & degreeSequence ) const
{
	unsigned int *degSeq = getDegreeSequence();
	stringstream ostr;
	for( int i = 0; i < mOrder; i++ )
	{
		ostr << degSeq[i];
		if( i != mOrder - 1 )
		{
			ostr << ",";
		}
	}
	degreeSequence = ostr.str();
	delete [] degSeq;
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

		mOrder = that.mOrder;
		int size = mOrder * mOrder;

		mAdj = new unsigned char[size];
		mDegrees = new unsigned int[mOrder];

		memcpy( mAdj, that.mAdj, sizeof( unsigned char ) * ( size ) );
		memcpy( mDegrees, that.mDegrees, sizeof( unsigned int ) * mOrder );
	}

	return *this;
}

