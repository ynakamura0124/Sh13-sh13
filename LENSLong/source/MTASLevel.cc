

#include "globals.hh"
#include <sstream>
#include "MTASLevel.hh"

MTASLevel::MTASLevel(int id)
{
	m_levelId=id;
	m_betaIntensity=0.;
	m_betaEnergy=0.;
}

MTASLevel::~MTASLevel()
{
}

void MTASLevel::SetLevelEnergy(double levelEnergy, double energyUncertainty)
{
	m_levelEnergy = levelEnergy;
	m_energyUncertainty = energyUncertainty;
}

void MTASLevel::SetHalfLifeTime(string halfLife, double halfLifeTimeUncertainty)//sprawdzenie czy nie 0????
{
	istringstream iss(halfLife);
	double halfLifeTimeValue;
	char halfLifeTimeUnit;
	iss >> halfLifeTimeValue;
	
	G4cout << "Halflife TIME Unit: " <<  halfLifeTimeValue << G4endl;
	
	if (halfLifeTimeValue == 0)
	{
			m_halfLifeTimeInSeconds = 0.;
			m_halfLifeTimeUncertainty = 0.;
	}
	else
	{
		iss >> halfLifeTimeUnit;
		
		G4cout << "Halflife Unit: " <<  halfLifeTimeUnit << G4endl;
	
		if(halfLifeTimeUnit == 'S')
		{
			m_halfLifeTimeInSeconds = halfLifeTimeValue;
			m_halfLifeTimeUncertainty = halfLifeTimeUncertainty;
		}
		else if(halfLifeTimeUnit == 'M')// Screws up with MS??
		{
			m_halfLifeTimeInSeconds = halfLifeTimeValue*60.;
			m_halfLifeTimeUncertainty = halfLifeTimeUncertainty*60.;
		}
		else if(halfLifeTimeUnit == 'H')
		{
			m_halfLifeTimeInSeconds = halfLifeTimeValue*3600.;
			m_halfLifeTimeUncertainty = halfLifeTimeUncertainty*3600.;
		}
		else if(halfLifeTimeUnit == 'Y')
		{
			m_halfLifeTimeInSeconds = halfLifeTimeValue* 31556926.;
			m_halfLifeTimeUncertainty = halfLifeTimeUncertainty * 31556926.;
		}
		else if(halfLifeTimeUnit == 'N')// NS becomes N for nanoseconds
		{// Also need PS, FS, MS(micro), mS(milli)?
			m_halfLifeTimeInSeconds = halfLifeTimeValue * 0.000000001;
			m_halfLifeTimeUncertainty = halfLifeTimeUncertainty * 0.000000001;
		}
		else if(halfLifeTimeUnit == 'P')// PS becomes P for picoseconds
		{// Also need PS, FS, MS(micro), mS(milli)?
			m_halfLifeTimeInSeconds = halfLifeTimeValue * 0.000000000001;
			m_halfLifeTimeUncertainty = halfLifeTimeUncertainty * 0.000000000001;
		}
		else
		{
			G4cout << "ERROR UNKNOWN Halflife Unit: " <<  halfLifeTimeUnit << G4endl;
			G4cout << "Halflife set to 0.0" << G4endl;
			
			m_halfLifeTimeInSeconds = 0.0;
			m_halfLifeTimeUncertainty = 0.0;
		}
	}
}

MTASParentLevel::MTASParentLevel()
{
	m_levelEnergy=0.;
	m_energyUncertainty = 0.;
	m_halfLifeTimeUncertainty = 0.;
	m_halfLifeTimeInSeconds = 0.;
	m_groundStateQValue = 0.;
	m_groundStateQUncertainty = 0.;
	m_ionisationState = 0.;
}

void MTASParentLevel::SetLevelEnergy(double levelEnergy, double energyUncertainty)
{
	m_levelEnergy = levelEnergy;
	m_energyUncertainty = energyUncertainty;
}

void MTASParentLevel::SetHalfLifeTime(string halfLife, double halfLifeTimeUncertainty)
{
	istringstream iss(halfLife);
	double halfLifeTimeValue;
	char halfLifeTimeUnit;
	iss >> halfLifeTimeValue;
	iss >> halfLifeTimeUnit;
	
	if(halfLifeTimeUnit == 'S')
	{
		m_halfLifeTimeInSeconds = halfLifeTimeValue;
		m_halfLifeTimeUncertainty = halfLifeTimeUncertainty;
	}
	else if(halfLifeTimeUnit == 'M')
	{
		m_halfLifeTimeInSeconds = halfLifeTimeValue*60.;
		m_halfLifeTimeUncertainty = halfLifeTimeUncertainty*60.;
	}
	else if(halfLifeTimeUnit == 'H')
	{
		m_halfLifeTimeInSeconds = halfLifeTimeValue*3600.;
		m_halfLifeTimeUncertainty = halfLifeTimeUncertainty*3600.;
	}
	else //if(halfLifeTimeUnit == 'Y')
	{
		m_halfLifeTimeInSeconds = halfLifeTimeValue* 31556926.;
		m_halfLifeTimeUncertainty = halfLifeTimeUncertainty * 31556926.;
	}
}

void MTASParentLevel::SetGroundStateQValue(double groundStateQValue, double groundStateQUncertainty)
{
	m_groundStateQValue = groundStateQValue;
	m_groundStateQUncertainty = groundStateQUncertainty;
}

