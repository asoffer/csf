#include <vector>
#include <map>
#include <set>
#include <algorithm>
#include <utility>
#include <sstream>
#include <cmath>
#include "Tree.hpp"
#include "TreeGenerator.hpp"
#include "DisjointSet.hpp"
#include "SubsetDeltaGenerator.hpp"
#include "Debug.hpp"
#include <iostream>
using namespace std;

typedef map< vector<unsigned int>, vector<Tree> > TreeByDegSeqHash;
typedef map< pair< vector<unsigned int>, vector<unsigned int> >, vector<Tree> > DegSeqPathNumHash;
typedef map< vector<unsigned int>, int> kSlice;

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

void getTrees( unsigned int n, DegSeqPathNumHash & treeHash );
void computeKSlices( unsigned int n, unsigned int k, SubsetDeltaGenerator & subsetGen, const vector<Tree> & treeList, vector<kSlice> & kSliceList ); 
void cullTreesByKSlice( vector<Tree> & treeList, const vector<kSlice> & kSliceList );
void testConjecture( unsigned int n );

int main( int argc, char **argv )
{
	if( argc != 2 )
	{
		cout << "Usage: " << argv[0] << " <number of vertices>" << endl;
		return 0;
	}

	unsigned int n = atoi( argv[1] );
	testConjecture( n );
	return 0;
}

void getTrees( unsigned int n, DegSeqPathNumHash & treeHash )
{
	Tree t;
	TreeGenerator treeGen( n );
	vector<unsigned int> degSeq;
	vector<unsigned int> pathNums;
	
	while( treeGen.nextTree( t ) )
	{
		t.getDegreeSequence( degSeq );
		t.getPathNums( pathNums );
		treeHash[ make_pair( degSeq, pathNums ) ].push_back( t );
	}
}

void computeKSlices( unsigned int n, unsigned int k, SubsetDeltaGenerator & subsetGen, const vector<Tree> & treeList, vector<kSlice> & kSliceList )
{
	int additive = ( k % 2 == 0 ) ? 1 : -1;

	vector<SubsetDelta> deltas;
	subsetGen.getDeltasForSubsetsOfFixedLength( k, deltas );

	for( vector<Tree>::const_iterator it2 = treeList.begin();
		 it2 != treeList.end();
		 ++it2 )
	{
		vector<Edge> edgeSet = it2->getEdges();
		kSlice currSlice;
		
		DisjointSet components( n );		// Track connected components with a disjoint set

		for( unsigned int i = 0; i < k; i++ )
		{
			components.setUnion( edgeSet[i].u, edgeSet[i].v );
		}

		vector<unsigned int> lambdaOfS;
		components.getSetSizes( lambdaOfS );
		currSlice[lambdaOfS] += additive;

		for( unsigned int i = 1; i < deltas.size(); i++ )
		{
			Edge removeEdge = edgeSet[deltas[i].oldValue];
			Edge addEdge = edgeSet[deltas[i].newValue];

			components.split( removeEdge.u, removeEdge.v );
			components.setUnion( addEdge.u, addEdge.v );

			components.getSetSizes( lambdaOfS );
			currSlice[lambdaOfS] += additive;
		}

		kSliceList.push_back( currSlice );
	}
}

void cullTreesByKSlice( vector<Tree> & treeList, const vector<kSlice> & kSliceList )
{
	set<unsigned int> treesToKeep;
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
	for( set<unsigned int>::iterator it = treesToKeep.begin();
		 it != treesToKeep.end();
		 ++it )
	{
		treesKept.push_back( treeList[*it] );
	}
	treeList = treesKept;

	debugMessage(( "\tRemoved: %d, Remaining: %d\n", (int)( kSliceList.size() - treesKept.size() ), (int)treeList.size() ));
}

void testConjecture( unsigned int n )
{
	DegSeqPathNumHash treeHash;
	getTrees( n, treeHash );
	SubsetDeltaGenerator subsetGen( n - 1 );

	for( DegSeqPathNumHash::iterator it = treeHash.begin();
		 it != treeHash.end();
		 ++it )
	{
		vector<unsigned int> degSeq = it->first.first;
		vector<unsigned int> pathNums = it->first.second;
		vector<Tree> treeList = it->second;
		debugMessage(( "Working with degSeq = %s, pathNums = %s :: count = %d\n", vectorToString( degSeq ).c_str(), vectorToString( pathNums ).c_str(), (int)treeList.size() ));

		unsigned int k = 3;
		while( k < n and treeList.size() > 1 )
		{
			debugMessage(( "k = %d\n", k ));
			vector<kSlice> kSliceList;
			
			computeKSlices( n, k, subsetGen, treeList, kSliceList );
			cullTreesByKSlice( treeList, kSliceList );
			
			treeHash[ make_pair( degSeq, pathNums ) ] = treeList;
			k++;
		}

		if( treeList.size() > 1 )
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
