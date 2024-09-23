#ifndef GSSTATEG4_INCLUDE
#define GSSTATEG4_INCLUDE

#include <iostream>
#include <string>
using namespace std;

class GSStateG4
{
public:
	GSStateG4(int n_br, string name);
	virtual ~GSStateG4(void);
	void SetBranch(int i, double E, double Pb, double Pg, string sname);
	void Ready(void);
	string Decay(double rnd1, double rnd2, int &Ifg, double &Eg);
	inline string GetName(){ return sname; }
protected:
	string sname;
	int n_branch;
	string *decay_sname;
	double *E_gamma;
	double *P_branch;
	double *P_gamma;
	double *SP_branch;
};

#endif
