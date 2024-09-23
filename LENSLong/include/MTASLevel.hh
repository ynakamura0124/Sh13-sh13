using namespace std;
#ifndef MTASLevel_h
#define MTASLevel_h 1

#include <vector>
#include <string>
//#include "globals.hh"

class MTASParentLevel
{
	public:
	
	MTASParentLevel();
	~MTASParentLevel(){};
	
	void SetLevelEnergy(double levelEnergy, double energyUncertainty = 0.);
	void SetSpinAndParity(std::string spinAndParity) {m_spinAndParity = spinAndParity;}
	void SetHalfLifeTime(std::string halfLife, double halfLifeTimeUncertainty = 0.);
	void SetGroundStateQValue(double groundStateQValue, double groundStateQUncertainty = 0.);
	void SetIonizationState(double ionisationState) {m_ionisationState = ionisationState;}
	
	double GetLevelEnergy() {return m_levelEnergy;}
	double GetHalfLifeTimeInSecond() {return m_halfLifeTimeInSeconds;}
	double GetGroundStateQValue() {return m_groundStateQValue;}
	private:
	double m_levelEnergy;
	double m_energyUncertainty;
	std::string m_spinAndParity;
	double m_halfLifeTimeUncertainty;
	double m_halfLifeTimeInSeconds;
	double m_groundStateQValue;
	double m_groundStateQUncertainty;
	double m_ionisationState;
};


class MTASLevel
{
	public:
	MTASLevel(int id);
	~MTASLevel();
	
	void SetGammasId(int gammaId){m_gammasId.push_back(gammaId);}
	void SetBetaId(int betaId){m_betaId=betaId;}
	void SetLevelEnergy(double levelEnergy, double energyUncertainty = 0);
	void SetLevelSpin(std::string spin){m_spin = spin;}
	void SetHalfLifeTime(std::string halfLife, double halfLifeTimeUncertainty);
	void SetDeltaAngularMomentum(std::string deltaAngularMomentum){m_deltaAngularMomentum = deltaAngularMomentum;}
	void SetIsomerFlag(std::string isomerFlag){m_isomerFlag = isomerFlag;}
	void SetBetaIntensity(double betaIntensity){m_betaIntensity = betaIntensity;}
	void SetDecayType(char decayType){m_decayType = decayType;}

	
	vector<int> GetGammasId(){return m_gammasId;}
	int GetNumberOfGammas(){return m_gammasId.size();}
	int GetBetaId(){return m_betaId;}
	int GetLevelId(){return m_levelId;}
	double GetLevelEnergy(){return m_levelEnergy;}//what about unc?
	std::string GetLevelSpin(){return m_spin;}
	double GetHalfLifeTimeInSecond() {return m_halfLifeTimeInSeconds;}
	std::string GetDeltaAngularMomentum(){return m_deltaAngularMomentum;}
	std::string GetIsomerFlag(){return m_isomerFlag;}
	double GetBetaIntensity(){return m_betaIntensity;}
	//double GetBetaEnergy(){return m_betaEnergy;}
	char GetDecayType(){return m_decayType;}
  
	private:
	int m_levelId;
	int m_betaId;
	vector<int> m_gammasId;
	char m_decayType;
	double m_levelEnergy;
	double m_energyUncertainty;
	std::string m_spin;
	double m_halfLifeTimeUncertainty;
	double m_halfLifeTimeInSeconds;
	std::string m_deltaAngularMomentum;
	std::string m_isomerFlag;
	double m_betaIntensity;
	double m_betaEnergy;
};
#endif
