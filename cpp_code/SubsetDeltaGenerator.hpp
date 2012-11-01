#ifndef SUBSET_DELTA_GENERATOR_HPP
#define SUBSET_DELTA_GENERATOR_HPP

#include <vector>

struct SubsetDelta
{
	unsigned int oldValue;
	unsigned int newValue;
};

class SubsetDeltaGenerator
{
	public:
		SubsetDeltaGenerator( unsigned int n );
		~SubsetDeltaGenerator();

		void getDeltasForSubsetsOfFixedLength( unsigned int k, std::vector<SubsetDelta> & deltas );

	private:
		unsigned int		n;						// Num items in the set { 0, 1, ..., n - 1 }
		unsigned int		k;						// Num items in a subset in the list of subsets
		unsigned int		*mCurrSubset;			// Used to track state
		// List of minimum changes needed to go from one subset to the next in the list.
		// First item indicates what needs to be replaced, second item is what to replace it with,
		// starting with the subset { 0, 1, ... , k - 1 }
		std::vector<SubsetDelta> mDeltas;

		void forward( int pointer, int difference );
		void reverse( int pointer, int difference );
		void process( int posChanged, int newValue );
};

#endif
