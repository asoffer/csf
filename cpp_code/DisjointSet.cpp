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

	// If they're both representatives, make the bigger one the parent
	if( isRepresentative( x ) && isRepresentative( y ) )
	{
		if( mSizes[x] >= mSizes[y] )
		{
			mParents[y] = x;
			mSizes[x] += mSizes[y];
		}
		else
		{
			mParents[x] = y;
			mSizes[y] += mSizes[x];
		}
	}
	// If at least one of them is a representative, make the
	// representative point to the other, because it is easier.
	else if( isRepresentative( x ) || isRepresentative( y ) )
	{
		unsigned int rep = ( isRepresentative( x ) ? x : y );
		unsigned int nonRep = ( isRepresentative( x ) ? y : x );

		mParents[rep] = nonRep;

		unsigned int curr = nonRep;
		while( !isRepresentative( curr ) )
		{
			mSizes[curr] += mSizes[rep];
			curr = mParents[curr];
		}

		mSizes[curr] += mSizes[rep];
	}
	// Otherwise, both are non-representatives, so they both need
	// to be rebased. This makes them both representatives, so
	// point the smaller toward the larger.
	else
	{
		rebase( x );
		rebase( y );
		
		if( mSizes[x] >= mSizes[y] )
		{
			mParents[y] = x;
			mSizes[x] += mSizes[y];
		}
		else
		{
			mParents[x] = y;
			mSizes[y] += mSizes[x];
		}
	}
}

unsigned int DisjointSet::find( unsigned int x ) const
{
	fatalAssert( x < mNumItems, "You tried to make find() cause a segfault." );

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

void DisjointSet::rebase( unsigned int x )
{
	// If x is already a representative, we should terminate because this is a
	// waste of a function call.
	fatalAssert( !isRepresentative( x ), "Pointless to rebase a representative." );

	// Essentially, this will boil down to a linked-list reversal, with some
	// additional size-adjusting logic at the end. Start with the reversal.
	unsigned int prev = x;
	unsigned int curr = x;
	unsigned int next = mParents[x];
	while( !isRepresentative( curr ) )
	{
		mParents[curr] = prev;
		prev = curr;
		curr = next;
		next = mParents[next];
	}
	mParents[curr] = prev;

	// Now, adjust the sizes of the elements
	unsigned int formerRepSize = mSizes[curr];
	while( !isRepresentative( curr ) )
	{
		next = mParents[curr];
		mSizes[curr] = formerRepSize - mSizes[next];
		curr = next;
	}
	mSizes[curr] = formerRepSize;
}
