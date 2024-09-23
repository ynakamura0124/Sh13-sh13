using namespace std;
#ifndef MTASBetaPlus_h
#define MTASBetaPlus_h 1

#include <vector>
#include <string>
//#include "globals.hh"


class MTASBetaPlus
{
	public:
	MTASBetaPlus(int id);
	~MTASBetaPlus();

	void SetFinalLevelId(int finalLevelId){m_finalLevelId = finalLevelId;}
	void SetBetaEnergy(double betaEnergy){m_betaEnergy = betaEnergy;}
	void SetBetaPlusIntensity(double betaIntensity, double betaIntensityUnc = 0){m_betaIntensity = betaIntensity;
																																								m_betaIntensityUnc = betaIntensityUnc;}
	void SetElectronCaptureIntensity(double electronCaptureIntensity, double electronCaptureIntensityUnc = 0){m_electronCaptureIntensity = electronCaptureIntensity;
																																		m_electronCaptureIntensityUnc = electronCaptureIntensityUnc;}
	void SetBetaLogft(double betaLogft, double betaLogftUnc = 0){m_betaLogft = betaLogft;
																																m_betaLogftUnc = betaLogftUnc;}
	void SetTotalIntensity(double totalIntensity, double totalIntensityUnc = 0){m_totalIntensity = totalIntensity;
																																							m_totalIntensityUnc = totalIntensityUnc;}
	void SetBetaForbiddennessClasification(std::string betaForbiddennessClasification){m_betaForbiddennessClasification = betaForbiddennessClasification;}
	void SetBetaEnergyDistribution(double, int, int){;}
	
	int GetFinalLevelId(){return m_finalLevelId;}
	double GetBetaEnergy(){return m_betaEnergy;}
	double GetBetaPlusIntensity(){return m_betaIntensity;}
	double GetElectronCaptureIntensity(){return m_electronCaptureIntensity;}
	double GerBetaLogft(){return m_betaLogft;}
	double GetTotalIntensity(){return m_totalIntensity;}
	std::string GetBetaForbiddennessClasification(){return m_betaForbiddennessClasification;}
	double GetRandomEnergy();
  
	private:
	static const int m_betaEnergyDistributionSize = 100;
	double * m_betaEnergyDistributionProb;// = new double [m_betaEnegryDistributionSize];
	double *m_betaEnergyDistributionValue;// = new double [m_betaEnegryDistributionSize];
	int m_betaId;
	int m_finalLevelId;
	double m_betaEnergy;
	double m_betaIntensity;
	double m_betaIntensityUnc;
	double m_electronCaptureIntensity;
	double m_electronCaptureIntensityUnc;
	double m_betaLogft;
	double m_betaLogftUnc;
	double m_totalIntensity;
	double m_totalIntensityUnc;
	std::string m_betaForbiddennessClasification;

};
#endif
