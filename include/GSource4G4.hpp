#ifndef GSOURCE4G4_INCLUDE
#define GSOURCE4G4_INCLUDE

#include <iostream>
#include <string>
#include <fstream>  
#include <time.h>
#include <TRandom3.h>
#include "GSStateG4.hpp"
using namespace std;

class GSource4G4
{
public:
	GSource4G4(string prm_file);
	virtual ~GSource4G4(void);
	void ReadPrm(string fname);
	int EmitGamma();
	void SetNevent(unsigned long long n);
	int FindStateID(string sname);
	inline double GetEGamma(int n){ return E_gamma[n]; }
	inline bool IfNext(void){ return (cevnt<nevnt); }
	inline double Progress(void){ return 100.0*(double)cevnt/(double)nevnt; }
protected:
	int n_state;
	GSStateG4 **State;
	unsigned long long nevnt;
	unsigned long long cevnt;
	int n_gamma;
	double *E_gamma;
	TRandom *rnd;
};

#endif
