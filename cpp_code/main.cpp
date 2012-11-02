#include <pthread.h>
#include <stdio.h>
#include <vector>
#include <queue>
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

#define MAX_NUM_THREADS 12

typedef map< vector<unsigned int>, vector<Tree> > TreeByDegSeqHash;
typedef map< pair< vector<unsigned int>, vector<unsigned int> >, vector<Tree> > DegSeqPathNumHash;
typedef map< vector<unsigned int>, int> kSlice;
typedef vector< vector<SubsetDelta> > SubsetDeltaTable;
typedef queue< vector<Tree> > TreeQueue;

DegSeqPathNumHash	treeHash;
TreeQueue			treeQueue;
SubsetDeltaTable	subsetDeltaTable;
unsigned int		n;
unsigned int		numThreads;
pthread_t			myThreads[MAX_NUM_THREADS];
pthread_mutex_t		treeQueueMutex;

void getTrees( unsigned int n );
void *worker( void *arg );
void testConjecture( unsigned int n );

int main( int argc, char **argv )
{
	if( argc != 3 )
	{
		cout << "Usage: " << argv[0] << " <number of vertices> <number of cores to use>" << endl;
		cout << "    Use the command nproc to find how many cores you have available." << endl;
		return 0;
	}

	n = atoi( argv[1] );
	numThreads = atoi( argv[2] );
	if( numThreads > MAX_NUM_THREADS )
	{
		cout << "Only supporting 12 or less threads." << endl;
		return 0;
	}

	testConjecture( n );
	pthread_exit( NULL );
	return 0;
}

void getTrees( unsigned int n )
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

void *worker( void *arg )
{
	bool threadRunning = true;
	vector<Tree> treeList;
	while( threadRunning )
	{
		pthread_mutex_lock( &treeQueueMutex );
			if( !treeQueue.empty() )
			{
				treeList = treeQueue.front();
				treeQueue.pop();
			}
			else
			{
				threadRunning = false;
			}
		pthread_mutex_unlock( &treeQueueMutex );

	
		unsigned int k = 3;
		while( k < n and treeList.size() > 1 )
		{
			vector<kSlice> kSliceList;
	
			int additive = ( k % 2 == 0 ) ? 1 : -1;

			vector<SubsetDelta> deltas;
			deltas = subsetDeltaTable[k];
	
			for( vector<Tree>::const_iterator it = treeList.begin();
				 it != treeList.end();
				 ++it )
			{
				vector<Edge> edgeSet = it->getEdges();
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

			k++;
		}

		if( treeList.size() > 1 )
		{
			cout << "COUNTEREXAMPLE FOUND!" << endl;
			for( unsigned int i = 0; i < treeList.size(); i++ )
			{
				treeList[i].print(); 
			}
		}
	}

	pthread_exit( NULL );
}

void testConjecture( unsigned int n )
{
	getTrees( n );	// Generate the trees

	// 	Create the subset table
	SubsetDeltaGenerator subsetGen( n - 1 );
	vector<SubsetDelta> temp;
	for( unsigned int i = 0; i < 3; i++ )
	{
		subsetDeltaTable.push_back( temp );
	}
	for( unsigned int i = 3; i < n; i++ )
	{
		subsetGen.getDeltasForSubsetsOfFixedLength( i, temp );
		subsetDeltaTable.push_back( temp );
	}

	// Build the tree queue from the tree hash, removing cohorts of trees of
	// size 1 from consideration by not adding them to the tree queue.
	for( DegSeqPathNumHash::const_iterator it = treeHash.begin();
		 it != treeHash.end();
		 ++it )
	{
		if( it->second.size() > 1 )
		{
			treeQueue.push( it->second );
		}
	}
	treeHash.clear();

	// Initialize thread attributes and mutices.
	pthread_attr_t	threadAttributes;
	pthread_mutex_init( &treeQueueMutex, NULL );
	pthread_attr_init( &threadAttributes );
	pthread_attr_setdetachstate( &threadAttributes, PTHREAD_CREATE_JOINABLE );

	// Create the worker threads.
	for( unsigned long i = 0; i < numThreads; i++ )
	{
		pthread_create( &myThreads[i], &threadAttributes, worker, (void *)i );
	}

	pthread_attr_destroy( &threadAttributes );

	void *status;
	// Wait for the threads to finish.
	for( unsigned int i = 0; i < numThreads; i++ )
	{
		pthread_join( myThreads[i], &status );
	}

	pthread_mutex_destroy( &treeQueueMutex );
	pthread_exit( NULL );
}
