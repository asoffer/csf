#include "DisjointSet.hpp"
#include "Debug.hpp"
#include <iostream>
#include <cstring>
#include <algorithm>
using namespace std;

DisjointSet::DisjointSet( unsigned int numItems ) :
	mNumItems( numItems )
{
	mParents = new unsigned int[mNumItems];
	mSizes = new unsigned int[mNumItems];

	// Each item is its own parent to begin with
	for( unsigned int i = 0; i < mNumItems; i++ )
	{
		mParents[i] = i;
		mSizes[i] = 1;
	}
}

DisjointSet::DisjointSet( const DisjointSet & that )
{
	mParents = new unsigned int[that.mNumItems];
	mSizes = new unsigned int[that.mNumItems];

	mNumItems = that.mNumItems;
	memcpy( mParents, that.mParents, sizeof( unsigned int ) * mNumItems );
	memcpy( mSizes, that.mSizes, sizeof( unsigned int ) * mNumItems );
}

DisjointSet::~DisjointSet()
{
	delete [] mParents;
	delete [] mSizes;
}

void DisjointSet::setUnion( unsigned int x, unsigned int y )
{
	fatalAssert( x != y && x < mNumItems && y < mNumItems, "Failure in setUnion()." );

	if( isRepresentative( x ) )
	{
		mParents[x] = y;

		unsigned int curr = y;
		while( !isRepresentative( curr ) )
		{
			mSizes[curr] += mSizes[x];
			curr = mParents[curr];
		}

		mSizes[curr] += mSizes[x];
	}
	else if( isRepresentative( y ) )
	{
		mParents[y] = x;

		unsigned int curr = x;
		while( !isRepresentative( curr ) )
		{
			mSizes[curr] += mSizes[y];
			curr = mParents[curr];
		}

		mSizes[curr] += mSizes[y];
	}
	else
	{
		unsigned int parentOfX = mParents[x];

		mParents[parentOfX] = x;
		mParents[x] = y;

		mSizes[parentOfX]--;
		mSizes[x] += mSizes[parentOfX];
		mSizes[y] += mSizes[x];

		unsigned int curr = mParents[y];
		while( !isRepresentative( curr ) )
		{
			mSizes[curr] += mSizes[y];
			curr = mParents[curr];
		}

		mSizes[curr] += mSizes[y];
	}
}

unsigned int DisjointSet::find( unsigned int x ) const
{
	fatalAssert( x < mNumElements, "You tried to make find() cause a segfault." );

	unsigned int curr = x;
	while( !isRepresentative( curr ) )
	{
		curr = mParents[curr];
	}

	return curr;
}

void DisjointSet::split( unsigned int x, unsigned int y )
{
	fatalAssert( x != y && x < mNumItems && y < mNumItems, "Failure in split()." );
	
	if( mParents[x] == y )
	{
		unsigned int curr = y;
		while( !isRepresentative( curr ) )
		{
			mSizes[curr] -= mSizes[x];
			curr = mParents[curr];
		}

		mSizes[curr] -= mSizes[x];

		mParents[x] = x;
	}
	else if( mParents[y] == x )
	{
		unsigned int curr = x;
		while( !isRepresentative( curr ) )
		{
			mSizes[curr] -= mSizes[y];
			curr = mParents[curr];
		}

		mSizes[curr] -= mSizes[y];

		mParents[y] = y;
	}
}

void DisjointSet::getSetSizes( std::vector<unsigned int> & sizes ) const
{
	sizes.clear();
	for( unsigned int i = 0; i < mNumItems; i++ )
	{
		if( isRepresentative( i ) )
		{
			sizes.push_back( mSizes[i] );
		}
	}

	std::sort( sizes.begin(), sizes.end(), std::greater<int>() );
}

void DisjointSet::print() const
{
	cout << ">>> Set State <<< " << endl;
	for( unsigned int i = 0; i < mNumItems; i++ )
	{
		cout << "Item: " << i
			 << " (Parent: " << mParents[i]
			 << ", Size: " << mSizes[i]
			 << ", Representative: " << find( i ) << ")" << endl;
	}
}

DisjointSet & DisjointSet::operator=( const DisjointSet & that )
{
	if( this != &that )
	{
		delete [] mParents;
		delete [] mSizes;

		mNumItems = that.mNumItems;
		mParents = new unsigned int[mNumItems];
		mSizes = new unsigned int[mNumItems];

		memcpy( mParents, that.mParents, sizeof( unsigned int ) * mNumItems );
		memcpy( mSizes, that.mSizes, sizeof( unsigned int ) * mNumItems );
	}

	return *this;
}
