#include "GSource4G4.hpp"


GSource4G4::GSource4G4(string prm_file)
{
	ReadPrm(prm_file);
	nevnt = 10000;
	cevnt = 0;
	rnd = new TRandom3(0);
	E_gamma = new double[n_state];
	for(int i=0; i<n_state+1; i++)
		State[i]->Ready();
}


GSource4G4::~GSource4G4(void)
{
}

void GSource4G4::ReadPrm(string fname)
{
	ifstream fin(fname.c_str(), ios::in);
	if(!fin){ cout << "[GSource4G4-E]: cannot open file: " << fname << endl; }
	fin >> n_state;
	State = new GSStateG4*[n_state+1];
	State[0] = new GSStateG4(n_state,"parent");
	double brsum = 0;
	for(int i=0; i<n_state; i++){
		string sname, buf;
		double bratio;
		int brnum;
		double branchsum = 0;
		fin >> sname;
		fin >> bratio;
		fin >> brnum;
		brsum += bratio;
		State[0]->SetBranch(i,0,bratio,0,sname);
		getline(fin, buf);		getline(fin, buf);
		State[i+1] = new GSStateG4(brnum,sname);
		for(int j=0; j<brnum; j++){
			double Energy, Pbr, Pg;
			fin >> Energy;
			fin >> sname;
			fin >> Pbr;
			fin >> Pg;
			branchsum += Pbr;
			State[i+1]->SetBranch(j,Energy,Pbr,Pg,sname);
		}
	}
	fin.close();
	return;
}

void GSource4G4::SetNevent(unsigned long long n)
{
	nevnt = n;
}

int GSource4G4::EmitGamma()
{
	cevnt++;
	int Ifg;
	double Eg;
	n_gamma = 0;
	for(int i=0; i<n_state; i++){
		E_gamma[i] = 0;
	}
	string sname = State[FindStateID("parent")]->Decay(rnd->Uniform(),rnd->Uniform(),Ifg,Eg);
	while(sname!="gnd"){
		int id = FindStateID(sname);
		if(id < 0)
			break;
		string st = State[id]->Decay(rnd->Uniform(),rnd->Uniform(),Ifg,Eg);
		if(Ifg){
			E_gamma[n_gamma] = Eg;
			n_gamma++;
		}
		sname = st;
	}
	return n_gamma;
}

int GSource4G4::FindStateID(string sname)
{
	for(int i=0; i<n_state+1; i++){
		if(sname == State[i]->GetName())
			return i;
	}
	return -1;
}