
#include "MTASGamma.hh"

MTASGamma::MTASGamma(int id)
{
	m_gammaId=id;
	m_mixingRatio =0.;
	m_electronConversionCoefficient =0.;
	m_KC =0.;
	m_LC =0.;
	m_MC =0.;
	m_NC =0.;
}

MTASGamma::~MTASGamma()
{
}

void MTASGamma::SetElectronConversionCoefficient(double electronConversionCoefficient, double electronConversionCoefficientUnc)
{
	m_electronConversionCoefficient = electronConversionCoefficient;
	m_electronConversionCoefficientUnc = electronConversionCoefficientUnc;
	m_totalIntensity = m_gammaIntensity*(1+electronConversionCoefficient);
	
}
