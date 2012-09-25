#include "SubsetDeltaGenerator.hpp"
#include <cstddef>
#include <cstring>
#include <iostream>
#include <cstdlib>

SubsetDeltaGenerator::SubsetDeltaGenerator( unsigned int n )
{
	this->n = n;
	k = 0;
	mNumSubsetsOfLengthK = 1;
	mCurrSubset = NULL;
	mDeltas = NULL;
}

SubsetDeltaGenerator::~SubsetDeltaGenerator()
{
	if( mCurrSubset != NULL )
	{
		delete [] mCurrSubset;
	}
	if( mDeltas != NULL )
	{
		delete [] mDeltas;
	}
}

unsigned long long factorial( unsigned long long x )
{
	if( x <= 1 )
	{
		return 1;
	}

	unsigned long long r = 1;
	for( unsigned long long i = 2; i <= x; i++ )
	{
		r *= i;
	}

	return r;
}

unsigned long long SubsetDeltaGenerator::getNumSubsetsOfFixedLength( unsigned int k ) const
{
	return factorial( n ) / ( factorial( k ) * factorial( n - k ) );
}

const SubsetDelta* const SubsetDeltaGenerator::getDeltasForSubsetsOfFixedLength( unsigned int k )
{
	if( this->k == k )
	{
		return mDeltas;
	}
	else
	{
		this->k = k;
		mNumSubsetsOfLengthK = 0;

		if( mDeltas != NULL )
		{
			delete [] mDeltas;
		}
		if( mCurrSubset != NULL )
		{
			delete [] mCurrSubset;
		}

		unsigned int numSubsets = getNumSubsetsOfFixedLength( k );
		mDeltas = new(std::nothrow) SubsetDelta[numSubsets];
		mCurrSubset = new(std::nothrow) unsigned int[k];
		if( mDeltas == NULL || mCurrSubset == NULL )
		{
			std::cerr << "Ran out of memory!" << std::endl;
			exit( 1 );
		}
		
		for( unsigned int i = 0; i < k; i++ )
		{
			mCurrSubset[i] = i;
		}

		process( 1, 1 );
		forward( 1, 0 );

		return mDeltas;
	}
}

void SubsetDeltaGenerator::forward( int pointer, int difference )
{
	if( ( pointer < (int)k ) && ( difference - pointer < (int)n - (int)k - 1 ) )
	{
		forward( pointer + 2, difference + 2 );
		process( pointer + 1, n - k + pointer + 1 );
		reverse( pointer + 1, difference + 2 );
		process( pointer, difference + 2 );
		forward( pointer, difference + 1 );
	}
	else if( pointer == (int)k )
	{
		for( int i = difference + 2; i <= (int)n; i++ )
		{
			process( k, i );
		}
	}
}

void SubsetDeltaGenerator::reverse( int pointer, int difference )
{
	if( ( pointer < (int)k ) && ( difference - pointer < (int)n - (int)k - 1 ) )
	{
		reverse( pointer, difference + 1 );
		process( pointer, difference + 1 );
		forward( pointer + 1, difference + 2 );
		process( pointer + 1, difference + 2 );
		reverse( pointer + 2, difference + 2 );
	}
	else if( pointer == (int)k )
	{
		for( int i = (int)n - 1; i >= difference + 1; i-- )
		{
			process( k, i );
		}
	}
}

void SubsetDeltaGenerator::process( int posChanged, int newValue )
{
	SubsetDelta d;
	d.oldValue = mCurrSubset[posChanged - 1];
	d.newValue = newValue - 1;
	mDeltas[mNumSubsetsOfLengthK++] = d;

	mCurrSubset[posChanged - 1] = newValue - 1;
}

