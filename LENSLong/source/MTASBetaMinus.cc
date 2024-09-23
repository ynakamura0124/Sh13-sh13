
#include "MTASBetaMinus.hh"
#include "G4BetaFermiFunction.hh"
#include "G4BetaDecayType.hh"
#include "G4BetaDecayCorrections.hh"
#include "G4SystemOfUnits.hh"

#include "Randomize.hh"

MTASBetaMinus::MTASBetaMinus(int id)
{
	m_betaId=id;
	m_betaEnergyDistributionProb = new double [m_betaEnergyDistributionSize];
	m_betaEnergyDistributionValue = new double [m_betaEnergyDistributionSize];
}

MTASBetaMinus::~MTASBetaMinus()
{
	delete [] m_betaEnergyDistributionProb;
	delete [] m_betaEnergyDistributionValue;
}

void MTASBetaMinus::SetBetaEnergyDistribution(double betaEnergy, int A, int Z)
{
	double c = betaEnergy/1000.;//Max beta (kinetic?) energy in MeV
	double e0;
//	double n(1.0);
	// to work out the Fermi function normalization factor first
	G4BetaFermiFunction* aBetaFermiFunction = new G4BetaFermiFunction( A, (Z+1) );
	G4BetaDecayCorrections* aBetaCorrections = new G4BetaDecayCorrections( A, (Z+1) );
	
	e0 = c*MeV/0.511;//Q in electron mass units
	G4cout <<  "Q Value " << e0 / MeV << G4endl;

//	n = aBetaFermiFunction->GetFFN(e0);
	 // now to work out the histogram and initialise the random generator
	G4int npti = m_betaEnergyDistributionSize;				
	//G4double* pdf = new G4double[npti];
	G4int ptn;
	G4double g,ke,ee,f,p;
	G4double shape = 1.0;
	ee = e0+1.;//Total E for decay
	for (ptn=0; ptn < npti; ptn++)
	{
		      // e =e0*(ptn+1.)/102.;
		      // bug fix (#662) (flei, 22/09/2004)
		ke =e0*(ptn+0.5)/(double)npti;// KE of electron
		g = ke+1.;// Total E of electron
		p = std::sqrt(g*g-1.0);// momentum of electron
		f = p*(ee-g)*(ee-g)*g;
		// Special treatment for K-40 (problem #1068) (flei,06/05/2010) 
		if (A==40&&Z==19)
			f *= (std::pow((g*g-1),3)+std::pow((ee-g),6)+7*(g*g-1)*std::pow((ee-g),2)*(g*g-1+std::pow((ee-g),2)));
		// allowed = false, first forbidden, etc... = true for now.
		if( false ) shape = aBetaCorrections->ShapeFactor( firstForbidden, p, ee - g );
//		if( false ) shape = aBetaCorrections->ShapeFactor( uniqueFirstForbidden, p, ee - g );
//		G4cout <<  p << ", " << ke << ", " << e0 - ke << ", " << shape << G4endl;
		m_betaEnergyDistributionProb[ptn] = f * shape * aBetaFermiFunction->GetFF(ke);
		m_betaEnergyDistributionValue[ptn] = ke*0.511*1000;
	}		

	
	delete aBetaFermiFunction;
	delete aBetaCorrections;
	//delete [] pdf;
}

double MTASBetaMinus::GetShape( double energy, double energyMax )
{
	G4double factor = 1.0;
// This is the 210Bi decay, not good for other decays...	
//	const G4double c1 = 0.578;
//	const G4double c2 = 28.466;
//	const G4double c3 = -0.658;
//
//	const G4double p_e = sqrt( energy*energy - 1.0 );
//
//	const G4double w = std::sqrt(1. + p_e*p_e);
//	factor = 1. + c1*w + c2/w + c3*w*w;
	
	return factor;
}

double MTASBetaMinus::GetRandomEnergy()
{

//find maximum probability
	double maxProbability = 0;
	int randomIndex=1;

	for(int i=0; i<m_betaEnergyDistributionSize;i++)
	{
//		G4cout <<  i << ", " << m_betaEnergyDistributionProb[i] << G4endl;
		if( m_betaEnergyDistributionProb[i] > maxProbability)
			maxProbability=m_betaEnergyDistributionProb[i];
	}
	
	double randomProbability=maxProbability+1.; //to go inside "while"
	
	while( randomProbability > m_betaEnergyDistributionProb[randomIndex])
	{
		randomIndex=(int)(G4UniformRand()* m_betaEnergyDistributionSize);
		randomProbability= (G4UniformRand()*maxProbability);
	}

//	G4cout <<  maxProbability << ", " << randomProbability << G4endl;

	return m_betaEnergyDistributionValue[randomIndex];
}

