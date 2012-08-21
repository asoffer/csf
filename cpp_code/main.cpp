#include <vector>
#include <map>
#include <set>
#include <algorithm>
#include <sstream>
#include <cmath>
#include "Tree.hpp"
#include "TreeGenerator.hpp"
#include "SubsetDeltaGenerator.hpp"
#include "Debug.hpp"
#include <iostream>
using namespace std;

typedef map< vector<unsigned int>, int> CsfHash;
typedef map< vector<unsigned int>, vector<Tree> > TreeHash;
typedef map< vector<unsigned int>, int> kSlice;

unsigned long long getGrayCode( unsigned long long n )
{
	return ( ( n >> 1 ) ^ n );
}

string vectorToString( const vector<unsigned int> & v )
{
	stringstream ostr;
	for( unsigned int i = 0; i < v.size(); i++ )
	{
		ostr << v[i] << ",";
	}

	string result = ostr.str();
	result.resize( result.size() - 1 );
	return result;
}

void csf( const Tree & t, CsfHash & csf );
void testCsf();
void getTreesByDegreeSequence( int n, TreeHash & treesByDegSeq );
void computeKSlices( int n, int k, SubsetDeltaGenerator & subsetGen, const vector<Tree> & treeList, vector<kSlice> & kSliceList ); 
void cullTreesByKSlice( vector<Tree> & treeList, const vector<kSlice> & kSliceList );
void testConjecture( int n );

int main( int argc, char **argv )
{
	if( argc != 2 )
	{
		cout << "Usage: " << argv[0] << " <number of vertices>" << endl;
		return 0;
	}

	int n = atoi( argv[1] );
	testConjecture( n );
	//testCsf();
	return 0;
}

void testCsf()
{
	Tree t( 15 );

	t.addEdge( 0, 13 );
	t.addEdge( 1, 4 );
	t.addEdge( 2, 12 );
	t.addEdge( 3, 5 );
	t.addEdge( 4, 8 );
	t.addEdge( 5, 12 );
	t.addEdge( 6, 10 );
	t.addEdge( 7, 9 );
	t.addEdge( 7, 12 );
	t.addEdge( 8, 10 );
	t.addEdge( 8, 11 );
	t.addEdge( 8, 13 );
	t.addEdge( 11, 12 );
	t.addEdge( 11, 14 );

	CsfHash CSF;
	csf( t, CSF );
}

void csf( const Tree & t, CsfHash & csf )
{
	unsigned long long g1;
	unsigned long long g2;
	unsigned long long diff;

	int n = t.getOrder();
	Tree g( n );

	vector<Edge> edgeSet = t.getEdges();

	vector<unsigned int> componentSizes;
	g.getConnectedComponentSizes( componentSizes );
	csf[componentSizes] = 1;

	int sign = -1;
	unsigned long long powersetCardinality = pow( 2, edgeSet.size() );
	for( unsigned long long i = 1; i < powersetCardinality; i++ )
	{
		g1 = getGrayCode( i - 1 );
		g2 = getGrayCode( i );
		diff = g1 ^ g2;

		Edge e = edgeSet[log2( diff )];
		if( ( diff & g2 ) == diff )
		{
			g.addEdge( e.u, e.v );
		}
		else
		{
			g.deleteEdge( e.u, e.v );
		}

		g.getConnectedComponentSizes( componentSizes );
		csf[componentSizes] += sign;
		sign = -sign;
	}
}

void getTreesByDegreeSequence( int n, TreeHash & treesByDegSeq )
{
	Tree t;
	TreeGenerator treeGen( n );
	vector<unsigned int> degSeq;
	
	while( treeGen.nextTree( t ) )
	{
		t.getDegreeSequence( degSeq ); 
		treesByDegSeq[degSeq].push_back( t );
	}
}

void computeKSlices( int n, int k, SubsetDeltaGenerator & subsetGen, const vector<Tree> & treeList, vector<kSlice> & kSliceList )
{
	int additive = ( k % 2 == 0 ) ? 1 : -1;

	unsigned int numSubsetsOfLenK = subsetGen.getNumSubsetsOfFixedLength( k );
	const SubsetDelta * deltas = subsetGen.getDeltasForSubsetsOfFixedLength( k );

	for( vector<Tree>::const_iterator it2 = treeList.begin();
		 it2 != treeList.end();
		 ++it2 )
	{
		vector<Edge> edgeSet = it2->getEdges();
		kSlice currSlice;
		Tree t( n );

		for( unsigned int i = 0; i < edgeSet.size(); i++ )
		{
			t.addEdge( edgeSet[i].u, edgeSet[i].v );
		}

		vector<unsigned int> lambdaOfS;
		t.getConnectedComponentSizes( lambdaOfS );
		currSlice[lambdaOfS] += additive;

		for( unsigned int i = 1; i < numSubsetsOfLenK; i++ )
		{
			Edge removeEdge = edgeSet[deltas[i].oldValue];
			Edge addEdge = edgeSet[deltas[i].newValue];

			t.deleteEdge( removeEdge.u, removeEdge.v );
			t.addEdge( addEdge.u, addEdge.v );

			t.getConnectedComponentSizes( lambdaOfS );
			currSlice[lambdaOfS] += additive;
		}

		kSliceList.push_back( currSlice );
	}
}

void cullTreesByKSlice( vector<Tree> & treeList, const vector<kSlice> & kSliceList )
{
	set<int> treesToKeep;
	for( unsigned int i = 0; i < kSliceList.size(); i++ )
	{
		for( unsigned int j = i + 1; j < kSliceList.size(); j++ )
		{
			kSlice slice1 = kSliceList[i];
			kSlice slice2 = kSliceList[j];
			bool equal = true;
			kSlice::iterator it1 = slice1.begin();
			kSlice::iterator it2 = slice2.begin();
			while( it1 != slice1.end() && it2 != slice2.end() )
			{
				if( it1->first != it2->first ||
					it1->second != it2->second )
				{
					equal = false;
					break;
				}
				++it1;
				++it2;
			}
			if( equal )
			{
				treesToKeep.insert( i );
				treesToKeep.insert( j );
			}
		}
	}

	vector<Tree> treesKept;
	for( set<int>::iterator it = treesToKeep.begin();
		 it != treesToKeep.end();
		 ++it )
	{
		treesKept.push_back( treeList[*it] );
	}
	treeList = treesKept;

	debugMessage(( "\tRemoved: %d, Remaining: %d\n", (int)( kSliceList.size() - treesKept.size() ), (int)treeList.size() ));
}

void testConjecture( int n )
{
	TreeHash treesByDegSeq;
	getTreesByDegreeSequence( n, treesByDegSeq );
	SubsetDeltaGenerator subsetGen( n - 1 );

	for( TreeHash::iterator it = treesByDegSeq.begin();
		 it != treesByDegSeq.end();
		 ++it )
	{
		vector<unsigned int> degSeq = it->first;
		vector<Tree> treeList = it->second;
		debugMessage(( "Working with degSeq = %s :: count = %d\n", vectorToString( degSeq ).c_str(), (int)treeList.size() ));

		int k = 3;
		while( k < n and treeList.size() > 1 )
		{
			debugMessage(( "k = %d\n", k ));
			vector<kSlice> kSliceList;
			
			computeKSlices( n, k, subsetGen, treeList, kSliceList );
			cullTreesByKSlice( treeList, kSliceList );
			
			treesByDegSeq[degSeq] = treeList;
			k++;
		}

		if( treeList.size() <= 1 )
		{
			treesByDegSeq[degSeq].clear();
		}
		else
		{
			cout << "COUNTEREXAMPLE FOUND!" << endl;
			cout << "Degree sequence: " << vectorToString( degSeq ) << endl;
			for( unsigned int i = 0; i < treeList.size(); i++ )
			{
				treeList[i].print(); 
			}
		}
	}
}
