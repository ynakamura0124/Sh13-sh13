#include "Randomize.hh"
#include "MTASBetaPlus.hh"

MTASBetaPlus::MTASBetaPlus(int id)
{
	m_betaId=id;
	m_betaEnergyDistributionProb = new double [m_betaEnergyDistributionSize];
	m_betaEnergyDistributionValue = new double [m_betaEnergyDistributionSize];
}

MTASBetaPlus::~MTASBetaPlus()
{
	delete [] m_betaEnergyDistributionProb;
	delete [] m_betaEnergyDistributionValue;
}

double MTASBetaPlus::GetRandomEnergy()
{

//find maximum probability
	double maxProbability = 0;
	int randomIndex=1;
	int randomProbability=0;
	for(int i=0; i<m_betaEnergyDistributionSize;i++)
		if(m_betaEnergyDistributionProb[i]>maxProbability)
			maxProbability=m_betaEnergyDistributionProb[i];

	while(randomProbability<m_betaEnergyDistributionProb[randomIndex])
	{
		randomIndex=(int)(G4UniformRand()* m_betaEnergyDistributionSize);
		randomProbability= (G4UniformRand()*maxProbability);
	}

	return m_betaEnergyDistributionValue[randomIndex];
}
