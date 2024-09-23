#include "GSStateG4.hpp"


GSStateG4::GSStateG4(int n_br, string name)
{
	sname = name;
	n_branch = n_br;
	decay_sname = new string[n_br];
	E_gamma = new double[n_br];
	P_branch = new double[n_br];
	P_gamma = new double[n_br];
	SP_branch = new double[n_br];
	for(int i=0; i<n_br; i++){
		SP_branch[i] = 0;
	}
}

GSStateG4::~GSStateG4(void)
{
}

void GSStateG4::SetBranch(int i, double E, double Pb, double Pg, string sname)
{
	decay_sname[i] = sname;
	E_gamma[i] = E;
	P_branch[i] = Pb;
	P_gamma[i] = Pg;
	return;
}

void GSStateG4::Ready()
{
	double p = 0;
	for(int i=0; i<n_branch; i++){
		p += P_branch[i];
		SP_branch[i] = p;
	}
	return;
}

string GSStateG4::Decay(double rnd1, double rnd2, int &Ifg, double &Eg)
{
	Ifg = 0;
	Eg = 0;
	for(int i=0; i<n_branch; i++){
		if(rnd1 < SP_branch[i]){
			if( rnd2 < P_gamma[i] ){
				Ifg = 1;
				Eg = E_gamma[i];
			}
			return decay_sname[i];
		}
	}
	return "gnd";
}
