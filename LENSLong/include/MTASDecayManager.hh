using namespace std;

#ifndef MTASDecayManager_h
#define MTASDecayManager_h 1

#include <vector>
#include <string>
#include "globals.hh"
class MTASParentLevel;
class MTASLevel;
class MTASGamma;
class MTASBetaMinus;
class MTASBetaPlus;
class G4ParticleDefinition;

class MTASEvent
{
	public:
	MTASEvent(){};
	~MTASEvent(){};
	void AddParticle(G4ParticleDefinition* particleType, double particleEnergy)
	{
		m_ParticleType.push_back(particleType);
		m_ParticleEnergy.push_back(particleEnergy);
	}
	vector<G4ParticleDefinition*> GetPatricleTypes(){return m_ParticleType;}
	vector<double> GetPatricleEnergy(){return m_ParticleEnergy;}
	private:
	vector<G4ParticleDefinition*> m_ParticleType; 
	vector<double> m_ParticleEnergy;
};


class MTASDecayManager
{
	public:
	MTASDecayManager(string);
	~MTASDecayManager();
	void LoadFile(string);
	void NormalizeBetaIntensities();
	void NormalizeGammaIntensities();
	void SetGammasFinalLevel();
	void SetEnergyDistributionOfBeta();
	void DecaySimulation(vector<double>*, vector<G4ParticleDefinition*>*);
	double FindXRaysEnergy(int k);
	int FindGammaEmitedFromLevel(int levelId);
	
	int GetNumbersOfLevels(){return m_numberOfLevels;}
	void SetAtomicName(char* atomicName){m_atomicName=atomicName;}
	int GetAtomicMass(){return m_atomicMass;}
	int GetAtomicNumber(){return m_atomicNumber;}
	
	private:
	MTASParentLevel* m_parentLevel;
	vector<MTASLevel*> m_Levels;
	vector<MTASBetaMinus*> m_BetaMinus;
	vector<MTASBetaPlus*> m_BetaPlus;
	vector<MTASGamma*> m_Gammas;
	vector <G4ParticleDefinition*> m_Particle;
	vector<double> m_Decay;
	//vector<MTASEvent*> m_Events;
	int m_numberOfLevels;
	int m_numberOfGammas;
	int m_numberOfBetaMinus;
	int m_numberOfBetaPlus;
	int m_atomicMass;
	std::string m_atomicName;
	int m_atomicNumber;
	int m_isomerLevel;
	
	
};
#endif
