#include "SubsetDeltaGenerator.hpp"
#include <cstddef>
#include <cstring>

SubsetDeltaGenerator::SubsetDeltaGenerator(unsigned int n)
{
    this->n = n;
    k = 0;
    mCurrSubset = NULL;
}

SubsetDeltaGenerator::~SubsetDeltaGenerator()
{
    if(mCurrSubset != NULL)
    {
        delete [] mCurrSubset;
    }
}

void SubsetDeltaGenerator::getDeltasForSubsetsOfFixedLength(unsigned int k, std::vector<SubsetDelta> & deltas)
{
    this->k = k;
    mDeltas.clear();

    if(mCurrSubset != NULL)
    {
        delete [] mCurrSubset;
        mCurrSubset = NULL;
    }

    mCurrSubset = new unsigned int[k];
    for(unsigned int i = 0; i < k; i++)
    {
        mCurrSubset[i] = i;
    }

    process(1, 1);
    forward(1, 0);

    deltas = mDeltas;
}

void SubsetDeltaGenerator::forward(int pointer, int difference)
{
    if((pointer < (int)k) && (difference - pointer < (int)n - (int)k - 1))
    {
        forward(pointer + 2, difference + 2);
        process(pointer + 1, n - k + pointer + 1);
        reverse(pointer + 1, difference + 2);
        process(pointer, difference + 2);
        forward(pointer, difference + 1);
    }
    else if(pointer == (int)k)
    {
        for(int i = difference + 2; i <= (int)n; i++)
        {
            process(k, i);
        }
    }
}

void SubsetDeltaGenerator::reverse(int pointer, int difference)
{
    if((pointer < (int)k) && (difference - pointer < (int)n - (int)k - 1))
    {
        reverse(pointer, difference + 1);
        process(pointer, difference + 1);
        forward(pointer + 1, difference + 2);
        process(pointer + 1, difference + 2);
        reverse(pointer + 2, difference + 2);
    }
    else if(pointer == (int)k)
    {
        for(int i = (int)n - 1; i >= difference + 1; i--)
        {
            process(k, i);
        }
    }
}

void SubsetDeltaGenerator::process(int posChanged, int newValue)
{
    SubsetDelta d;
    d.oldValue = mCurrSubset[posChanged - 1];
    d.newValue = newValue - 1;
    mDeltas.push_back(d);

    mCurrSubset[posChanged - 1] = newValue - 1;
}

