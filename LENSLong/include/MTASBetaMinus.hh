//using namespace std;
#ifndef MTASBetaMinus_h
#define MTASBetaMinus_h 1

#include <vector>
#include <string>
//#include "globals.hh"


class MTASBetaMinus
{
	public:
	MTASBetaMinus(int id);
	~MTASBetaMinus();

	void SetFinalLevelId(int finalLevelId){m_finalLevelId = finalLevelId;}
	void SetBetaEnergy(double betaEnergy){m_betaEnergy = betaEnergy;}
	void SetBetaIntensity(double betaIntensity, double betaIntensityUnc =0){m_betaIntensity = betaIntensity; 
																																					m_betaIntensityUnc = betaIntensityUnc;}
	void SetBetaLogft(double betaLogft, double betaLogftUnc =0){m_betaLogft = betaLogft;
																															m_betaLogftUnc = betaLogftUnc;}
	void SetBetaForbiddennessClasification(std::string betaForbiddennessClasification)
				{m_betaForbiddennessClasification = betaForbiddennessClasification;}
	void SetBetaEnergyDistribution(double, int, int);
	double GetShape( double energy, double energyMax );
	
	int GetFinalLevelId(){return m_finalLevelId;}
	double GetBetaIntensity(){return m_betaIntensity;}
	double GetBetaEnergy(){return m_betaEnergy;}
	double GerBetaLogft(){return m_betaLogft;}
	std::string GetBetaForbiddennessClasification(){return m_betaForbiddennessClasification;}
	double GetRandomEnergy();
  
	private:
	static const int m_betaEnergyDistributionSize = 1000;
	double * m_betaEnergyDistributionProb;// = new double [m_betaEnegryDistributionSize];
	double *m_betaEnergyDistributionValue;// = new double [m_betaEnegryDistributionSize];
	int m_betaId;
	int m_finalLevelId;
	double m_betaEnergy;
	double m_betaIntensity;
	double m_betaIntensityUnc;
	double m_betaLogft;
	double m_betaLogftUnc;
	std::string m_betaForbiddennessClasification;

};
#endif
