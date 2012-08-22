#ifndef DISJOINT_SET_HPP
#define DISJOINT_SET_HPP

#include <vector>

class DisjointSet
{
	public:
		DisjointSet( unsigned int numItems );
		DisjointSet( const DisjointSet & that );
		~DisjointSet();

		void setUnion( unsigned int x, unsigned int y );
		unsigned int find( unsigned int x ) const;
		void split( unsigned int x, unsigned int y );

		void getSetSizes( std::vector<unsigned int> & sizes ) const;
		void print() const;

		DisjointSet & operator=( const DisjointSet & that );

	private:
		unsigned int mNumItems;
		unsigned int *mParents;
		unsigned int *mSizes;

		bool isRepresentative( unsigned int x ) const
		{
			return mParents[x] == x;
		};
};

#endif

