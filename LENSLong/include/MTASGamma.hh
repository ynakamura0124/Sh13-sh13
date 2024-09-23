#ifndef MTASGamma_h
#define MTASGamma_h 1

#include <vector>
#include <string>
//#include "globals.hh"

using namespace std;

class MTASGamma
{
	public:
	MTASGamma(int id);
	~MTASGamma();
	
	void SetFinalLevelId(int finalLevelId){m_finalLevelId = finalLevelId;}
	void SetPrimaryLevelId(int primaryLevelId){m_primaryLevelId = primaryLevelId;}
	void SetGammaEnergy(double energy, double energyUnc = 0.){m_energy = energy;
																														m_energyUnc = energyUnc;}
	void SetGammaIntensity(double intensity, double intensityUnc){m_gammaIntensity = intensity; m_gammaIntensityUnc = intensityUnc;}
	void SetGammaIntensity(double intensity){m_gammaIntensity = intensity;}
	void SetTotalIntensity(double intensity){m_totalIntensity = intensity;}
	void SetGammaMultiplicity(std::string multiplicity){m_multiplicity = multiplicity;}
	void SetMixingRatio(double mixingRatio, double mixingRatioUnc){m_mixingRatio = mixingRatio; m_mixingRatioUnc = mixingRatioUnc;}
	void SetElectronConversionCoefficient(double electronConversionCoefficient, double electronConversionCoefficientUnc);
	void SetKC(double KC){m_KC=KC;}
	void SetLC(double LC){m_LC=LC;}
	void SetMC(double MC){m_MC=MC;}
	void SetNC(double NC){m_NC=NC;}
	
	int GetGammaId(){return m_gammaId;}
	int GetFinalLevelId(){return m_finalLevelId;}
	int GetPrimaryLevelId(){return m_primaryLevelId;}
	double GetGammaEnergy(){return m_energy;}
	double GetGammaIntensity(){return m_gammaIntensity;}
	double GetTotalIntensity(){return m_totalIntensity;}
	std::string GetGammaMultiplicity(){return m_multiplicity;}
	double GetMixingRatio(){return m_mixingRatio;}
	double GetElectronConversionCoefficient(){return m_electronConversionCoefficient;}
	double GetKC(){return m_KC;}
  double GetLC(){return m_LC;}
  double GetMC(){return m_MC;}
  double GetNC(){return m_NC;}
  
	private:
	int m_gammaId;
	int m_finalLevelId;
	int m_primaryLevelId;
	double m_energy;
	double m_energyUnc;
	double m_gammaIntensity;
	double m_gammaIntensityUnc;
	std::string m_multiplicity;
	double m_totalIntensity;
	double m_mixingRatio;//????
	double m_mixingRatioUnc;
	double m_electronConversionCoefficient;
	double m_electronConversionCoefficientUnc;
	double m_KC;
	double m_LC;
	double m_MC;
	double m_NC;
};
#endif
