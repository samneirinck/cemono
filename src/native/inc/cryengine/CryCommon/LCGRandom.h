#ifndef _LCG_RAND__H__
#define _LCG_RAND__H__

//////////////////////////////////////////////////////////////////////////
//
// A Simple LCG Random Number Generator, that is cheap to seed
//
// NOTE: This should _NOT_ be used for any encryption methods

class CLCGRndGen
{ 
public:
	// default constructor
	CLCGRndGen() { seed(5489UL); }

	// constructor with 32 bit int as seed
	CLCGRndGen(uint32 s) { seed(s); }

	void seed(uint32 s) // seed with 32 bit integer
	{
		m_seed = (uint64)s;
	}

	uint32 Generate()
	{
    	const uint64 newSeed = ((uint64)214013)*m_seed + ((uint64)2531011);
		m_seed = newSeed;
		return (uint32)(newSeed>>16);
	}

	// generates random floating number in the closed interval [0,1].
	float GenerateFloat() { return (float)Generate()*(1.0f/4294967295.0f); }

private:
	// make copy constructor and assignment operator unavailable, they don't make sense
	CLCGRndGen(const CLCGRndGen&); // copy constructor not defined
	void operator=(const CLCGRndGen&); // assignment operator not defined
private:



	static uint64 m_seed;


};

#endif
