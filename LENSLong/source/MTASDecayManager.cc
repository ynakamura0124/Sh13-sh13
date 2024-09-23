#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "Randomize.hh"
#include "MTASGamma.hh"
#include "MTASLevel.hh"
#include "MTASBetaMinus.hh"
#include "MTASBetaPlus.hh"
#include "MTASDecayManager.hh"
#include "MendelejewProvider.hh"
#include "G4ParticleDefinition.hh"
#include "G4ParticleTable.hh"
#include "G4IonTable.hh"
#include "G4SystemOfUnits.hh"

#include "G4AtomicShells.hh"
#include <assert.h>

using namespace std;


MTASDecayManager::MTASDecayManager(string nameOfFile )
{
	m_numberOfLevels=0;
	m_numberOfGammas=0;
	m_numberOfBetaMinus=0;
	m_numberOfBetaPlus=0;
	m_isomerLevel = -1;
	this->LoadFile(nameOfFile);
	this->NormalizeBetaIntensities();
	this->NormalizeGammaIntensities();
	this->SetGammasFinalLevel();
	this->SetEnergyDistributionOfBeta();
}

MTASDecayManager::~MTASDecayManager()
{
	for(int i=0; i<m_numberOfLevels; i++)
		delete m_Levels[i];
		
	for(int i=0; i<m_numberOfGammas; i++)
		delete m_Gammas[i];
		
}


void MTASDecayManager::LoadFile(string nameOfFile)
{
	
	cout<< "Attempting to Open: " << nameOfFile <<endl;
	
	ifstream in( nameOfFile.c_str() );
	const int length = 81;
	if(!in.is_open())
		cout<< "Error message: The file " << nameOfFile << " is not open!"<<endl;

	char* buffer = new char [length+1];
	string data;
	string buff;
	
//figure out the length of the file...
	in.seekg(0, ios::end);
	int fileSize = in.tellg();
	cout<< "Total FileSize (char): " << fileSize <<endl;
	in.seekg(0, ios::beg);
	
	int currentLineLoading;

	in.read (buffer,length);//read the first line - find information about atomic mass and number
	buff= string(buffer);
	
	cout << buff << endl;
	
	currentLineLoading = in.tellg() / length;
	cout << "Loading Line: " << currentLineLoading << endl;

	data = buff.substr (0,3);
	m_atomicMass=strtol(data.c_str(), NULL, 10);
	
	data = buff.substr (3,2);
	m_atomicName=data;
	
	
	while(!in.eof( ))
	{
		in.read (buffer,length);
		buff= string(buffer);
		
		currentLineLoading = in.tellg() / length;
		if( currentLineLoading != 0 )
		{
			cout<< "Loading Line: " << currentLineLoading << endl;
			cout<< buff << endl;
		}
		else cout<< "Done! " << endl;
		
		if (buffer[6]!='c' )
		{
			if(buffer[7]=='P')//parent level
			{
				m_parentLevel = new MTASParentLevel();
				
				data=buff.substr(9,10);
				double energyLevel = strtod(data.c_str(), NULL);
				data=buff.substr(19,2);
				double enUnc =  strtod(data.c_str(), NULL);
				m_parentLevel->SetLevelEnergy(energyLevel, enUnc);
			
				data=buff.substr(21,18); 
				m_parentLevel->SetSpinAndParity(data);
				
				data=buff.substr(39,9);
				string T12 = data;		
				data=buff.substr(49,5);
				double T12Unc = strtod(data.c_str(), NULL);
				m_parentLevel->SetHalfLifeTime(T12, T12Unc);
				
				data=buff.substr(64,10);
				double Q = strtod(data.c_str(), NULL);
				data=buff.substr(74,2);//QUnc
				m_parentLevel->SetGroundStateQValue(Q,strtod(data.c_str(), NULL));
				
				data=buff.substr(76,4);
				m_parentLevel->SetIonizationState(strtod(data.c_str(), NULL));
				cout<< "Parent Energy: " << energyLevel<<", T1/2: "<< T12Unc <<", Q: " << Q<< endl;
			}
			
		
			if(buffer[7]=='L') //each level
			{
				m_numberOfLevels++;
				m_Levels.push_back(new MTASLevel(m_numberOfLevels-1));

				data=buff.substr(9,10);
				double energyLevel = strtod(data.c_str(), NULL);
				data=buff.substr(19,2);
				double enUnc =  strtod(data.c_str(), NULL);
				m_Levels[m_numberOfLevels-1]->SetLevelEnergy(energyLevel, enUnc);
			
				data=buff.substr(21,14);
				m_Levels[m_numberOfLevels-1]->SetLevelSpin(data);
	
				data=buff.substr(39,9);
				string T12 = data;
				data=buff.substr(49,5);
				double T12Unc = strtod(data.c_str(), NULL);
				m_Levels[m_numberOfLevels-1]->SetHalfLifeTime(T12, T12Unc);
				
				data=buff.substr(55,9);
				m_Levels[m_numberOfLevels-1]->SetDeltaAngularMomentum(data);
				
				//there is something more here (in the *.ens file) but I don't need this information (at least so far)
				data=buff.substr(77,2);
				m_Levels[m_numberOfLevels-1]->SetIsomerFlag(data);

			}
			
			if(buffer[7]=='B' && buffer[5]==' ')
			{
				m_numberOfBetaMinus++;
				m_Levels[m_numberOfLevels-1]->SetDecayType('B');
				m_Levels[m_numberOfLevels-1]->SetBetaId(m_numberOfBetaMinus-1);
				m_BetaMinus.push_back(new MTASBetaMinus(m_numberOfBetaMinus-1));
				
				double groundStateQValue = m_parentLevel->GetGroundStateQValue();
				double levelEnergy = m_Levels[m_numberOfLevels-1]->GetLevelEnergy();
				m_BetaMinus[m_numberOfBetaMinus-1]->SetBetaEnergy(groundStateQValue - levelEnergy);//what about b+???
				
				data=buff.substr(21,8);
				double betaIntensity = strtod(data.c_str(), NULL);
				m_Levels[m_numberOfLevels-1]->SetBetaIntensity(betaIntensity);
				data=buff.substr(29,2);
				double intensityUnc = strtod(data.c_str(), NULL);
				m_BetaMinus[m_numberOfBetaMinus-1]->SetBetaIntensity(betaIntensity,intensityUnc);

				data=buff.substr(41,8);
				double betaLogft = strtod(data.c_str(), NULL);
				data=buff.substr(49,6);
				double betaLogftUnc = strtod(data.c_str(), NULL);
				m_BetaMinus[m_numberOfBetaMinus-1]->SetBetaLogft(betaLogft, betaLogftUnc);
				
				data=buff.substr(77,2);
				m_BetaMinus[m_numberOfBetaMinus-1]->SetBetaForbiddennessClasification(data);
				m_BetaMinus[m_numberOfBetaMinus-1]->SetFinalLevelId(m_numberOfLevels-1);
				
			}
			
			if(buffer[7]=='E'&& buffer[5]==' ')
			{
				m_numberOfBetaPlus++;
				m_BetaPlus.push_back(new MTASBetaPlus(m_numberOfBetaPlus-1));
				m_Levels[m_numberOfLevels-1]->SetDecayType('E');
				m_Levels[m_numberOfLevels-1]->SetBetaId(m_numberOfBetaPlus-1);
				 
				data=buff.substr(9,10);
				m_BetaPlus[m_numberOfBetaPlus-1]->SetBetaEnergy(strtod(data.c_str(), NULL));// ????????????
					
				data=buff.substr(21,8);
				double betaIntensity = strtod(data.c_str(), NULL);
				m_Levels[m_numberOfLevels-1]->SetBetaIntensity(betaIntensity);
				data = buff.substr(29,2);
				double betaIntensityUnc = strtod(data.c_str(), NULL);
				m_BetaPlus[m_numberOfBetaPlus-1]->SetBetaPlusIntensity(betaIntensity,betaIntensityUnc);

				data=buff.substr(31,8);
				double ECIntensity = strtod(data.c_str(), NULL);
				data = buff.substr(39,2);
				double ECIntensityUnc = strtod(data.c_str(), NULL);
				m_BetaPlus[m_numberOfBetaPlus-1]->SetElectronCaptureIntensity(ECIntensity,ECIntensityUnc);
				
				data=buff.substr(64,10);
				double totalIntensity = strtod(data.c_str(), NULL);
				data = buff.substr(74,2);
				double totalIntensityUnc = strtod(data.c_str(), NULL);
				m_BetaPlus[m_numberOfBetaPlus-1]->SetTotalIntensity(totalIntensity,totalIntensityUnc);
				m_Levels[m_numberOfLevels-1]->SetBetaIntensity(totalIntensity);
				
				data = buff.substr(41,8);
				double logft = strtod(data.c_str(), NULL);
				data = buff.substr(49,6);
				double logftUnc = strtod(data.c_str(), NULL);
				m_BetaPlus[m_numberOfBetaPlus-1]->SetBetaLogft(logft, logftUnc);

				data = buff.substr(77,2);
				m_BetaPlus[m_numberOfBetaPlus-1]->SetBetaForbiddennessClasification(data);
				
				m_BetaPlus[m_numberOfBetaPlus-1]->SetFinalLevelId(m_numberOfLevels-1);
				
				
			}
			
// The last check makes sure there are levels to attach the gamma.
			if(buffer[7]=='G' && buffer[5]==' ' && m_numberOfLevels > 0 )
			{
				m_numberOfGammas++;
				m_Gammas.push_back(new MTASGamma(m_numberOfGammas-1));
				
				data = buff.substr(9,10);	
				double gammaEnergy = strtod(data.c_str(), NULL);
				data = buff.substr(19,2);
				double gammaEnergyUnc = strtod(data.c_str(), NULL);
				m_Gammas[m_numberOfGammas-1]->SetGammaEnergy(gammaEnergy, gammaEnergyUnc);
			
				data = buff.substr(21,8);	
				double gammaIntensity = strtod(data.c_str(), NULL);
				data = buff.substr(29,2);
				double gammaIntensityUnc = strtod(data.c_str(), NULL);
				m_Gammas[m_numberOfGammas-1]->SetGammaIntensity(gammaIntensity,gammaIntensityUnc);
				
				data = buff.substr(31,10);			
				m_Gammas[m_numberOfGammas-1]->SetGammaMultiplicity(data);
				
				data = buff.substr(41,10);	
				double mixingRatio = strtod(data.c_str(), NULL);
				data = buff.substr(49,6);
				double mixingRatioUnc = strtod(data.c_str(), NULL);	
				m_Gammas[m_numberOfGammas-1]->SetMixingRatio(mixingRatio, mixingRatioUnc);
				
				data = buff.substr(55,7);	
				double electronConversionCoefficient = strtod(data.c_str(), NULL);
				data = buff.substr(62,2);
				double electronConversionCoefficientUnc = strtod(data.c_str(), NULL);	
				m_Gammas[m_numberOfGammas-1]->SetElectronConversionCoefficient(electronConversionCoefficient, electronConversionCoefficientUnc);//also total intensity
				//m_Gammas[m_numberOfGammas-1]->CalculateTotalIntensity();
				
				m_Levels[m_numberOfLevels-1]->SetGammasId(m_numberOfGammas-1);
				m_Gammas[m_numberOfGammas-1]->SetPrimaryLevelId(m_numberOfLevels-1);

			}
			
			if(buffer[7]=='G' && (buffer[5]=='S'||buffer[5]=='2'))//The same gamma particle- additional information
			{

				data = buff.substr(9,3);	
				string temp;
				int currIndeks;
				if(data=="KC=")//we have the line with electron conversion coefficients
				{
					data = buff.substr(12,71);//whole line
					int i=0;
					while(data[i]!='$'&& i<=71) //$ - separator between coefficients
					{
						temp[i]=data[i];
						i++;
					}
					m_Gammas[m_numberOfGammas-1]->SetKC(strtod(temp.c_str(), NULL));

					i++;
					currIndeks = i;
					temp.clear();
					while(data[i]!='$'&& i<=71) //next coefficients LC or LC+
					{
						if(data[i]=='L'||data[i]=='C'||data[i]=='+'||data[i]=='=')
							currIndeks++;
						else
						{
							temp[i-currIndeks]=data[i];
						}
						i++;
					}
					m_Gammas[m_numberOfGammas-1]->SetLC(strtod(temp.c_str(), NULL));
					
					i++;
					currIndeks = i;
					temp.clear();
					while(data[i]!='$'&& i<=71) //next coefficients MC or MC+
					{
						if(data[i]=='M'||data[i]=='C'||data[i]=='+'||data[i]=='=')
							currIndeks++;
						else
							temp[i-currIndeks]=data[i];
						i++;
					}
					m_Gammas[m_numberOfGammas-1]->SetMC(strtod(temp.c_str(), NULL));
					
					i++;
					currIndeks = i;
					temp.clear();
					while(data[i]!='$'&& i<=71) //next coefficients NC or NC+
					{
						if(data[i]=='N'||data[i]=='C'||data[i]=='+'||data[i]=='=')
							currIndeks++;
						else
							temp[i-currIndeks]=data[i];
						i++;
					}
					m_Gammas[m_numberOfGammas-1]->SetNC(strtod(temp.c_str(), NULL));
//sometimes there is more coefficients (OC, PC) but I'm not going to read it					
				}
			}
		}
	}
	
	
	cout<<"Parent Energy Level: "<< m_parentLevel->GetLevelEnergy() << endl;
	cout<<"Number Levels: "<< m_Levels.size() << endl;
	cout<<"Number Beta Decays: "<< m_BetaMinus.size() << endl;
	cout<<"Number Gammas: "<< m_Gammas.size() << endl;

	for( int i = 0; i < (int) m_Levels.size(); ++i )
	{
		cout<<"Beta Intensity: "<< m_Levels[i]->GetBetaIntensity() << endl;
		cout<<"Level Energy: "<< m_Levels[i]->GetLevelEnergy() << endl;
	}

	for( int i = 0; i < (int) m_BetaMinus.size(); ++i )
	{
		cout<<"Beta Intensity: "<< m_BetaMinus[i]->GetBetaIntensity() << endl;
		cout<<"Beta Energy: "<< m_BetaMinus[i]->GetBetaEnergy() << endl;
	}

	for( int i = 0; i < (int) m_Gammas.size(); ++i )
	{
		cout<<"Gamma Intensity: "<< m_Gammas[i]->GetGammaIntensity() << endl;
		cout<<"Gamma Energy: "<< m_Gammas[i]->GetGammaEnergy() << endl;
		cout<<"Gamma Electron Conversion Coefficent: "<< m_Gammas[i]->GetElectronConversionCoefficient() << endl;
	}

	delete buffer;
}

void MTASDecayManager::NormalizeBetaIntensities()
{
	G4double sumLevelsIntesities=0;
	G4double intensity=0;
	for( int i=0; i<m_numberOfLevels;i++)
	{
		sumLevelsIntesities+=m_Levels[i]->GetBetaIntensity();
	}
	for( int i=0; i<m_numberOfLevels;i++)
	{
		intensity=m_Levels[i]->GetBetaIntensity();
		if(sumLevelsIntesities!=0)
			m_Levels[i]->SetBetaIntensity(intensity/sumLevelsIntesities);
	}
}

	
void MTASDecayManager::NormalizeGammaIntensities()
{
	vector<double> sumGammasIntesities;
	G4double intensity=0;
	
	for( int i=0; i<m_numberOfLevels;i++)
	{
		sumGammasIntesities.push_back(0.0);
		int sizeGammasVector=(m_Levels[i]->GetGammasId()).size();
		for(int j=0; j< sizeGammasVector;j++)
		{
			int gammaId=(m_Levels[i]->GetGammasId())[j];
			
			sumGammasIntesities[i]+=m_Gammas[gammaId]->GetTotalIntensity();
		}
	}
	
	for( int i=0; i<m_numberOfLevels;i++)
	{
		int sizeGammasVector=(m_Levels[i]->GetGammasId()).size();
		for(int j=0; j< sizeGammasVector;j++)
		{
			int gammaId=(m_Levels[i]->GetGammasId())[j];
			intensity=m_Gammas[gammaId]->GetTotalIntensity();
			if(sumGammasIntesities[i]!=0)
				m_Gammas[gammaId]->SetTotalIntensity(intensity/sumGammasIntesities[i]);
		}
	}
	
}

void MTASDecayManager::SetGammasFinalLevel()//find the final level after gamma decay (with the closest energy) 
{
	G4int primaryLevelId;
	G4int finalLevelId=0;
	G4double primaryLevelEnergy;
	G4double finalLevelEnergy;
	G4double diff1;
	G4double diff2;
		for( int i=0; i<m_numberOfGammas;i++)
		{
			primaryLevelId=m_Gammas[i]->GetPrimaryLevelId();
			primaryLevelEnergy=m_Levels[primaryLevelId]->GetLevelEnergy();
			finalLevelEnergy=primaryLevelEnergy-m_Gammas[i]->GetGammaEnergy();
			finalLevelId=primaryLevelId;
				for( int j=0; j<primaryLevelId;j++)
				{
					diff1=sqrt((finalLevelEnergy-m_Levels[finalLevelId]->GetLevelEnergy())*(finalLevelEnergy-m_Levels[finalLevelId]->GetLevelEnergy()));
					diff2=sqrt((finalLevelEnergy-m_Levels[j]->GetLevelEnergy())*(finalLevelEnergy-m_Levels[j]->GetLevelEnergy()));
					if(diff2<diff1)
					{
						finalLevelId=j;
					}
				}
			m_Gammas[i]->SetFinalLevelId(finalLevelId);
		}
}


void MTASDecayManager::SetEnergyDistributionOfBeta() 
{
	m_atomicNumber=MendelejewProvider::GetAtomicNumber(m_atomicName);
	for( int i=0; i<m_numberOfBetaMinus;i++)
	{
			m_BetaMinus[i]->SetBetaEnergyDistribution(m_BetaMinus[i]->GetBetaEnergy(), m_atomicMass, m_atomicNumber);
	}	
	
	for( int i=0; i<m_numberOfBetaPlus;i++)
	{
			m_BetaPlus[i]->SetBetaEnergyDistribution(m_BetaPlus[i]->GetBetaEnergy(), m_atomicMass, m_atomicNumber);//not exist yet
	}	
}


double MTASDecayManager::FindXRaysEnergy(int k)//k - index of gamma
{
	double KC=m_Gammas[k]->GetKC(); 
	double LC=m_Gammas[k]->GetLC();
	double MC=m_Gammas[k]->GetMC();
	double NC=m_Gammas[k]->GetNC();
	double tot=m_Gammas[k]->GetElectronConversionCoefficient();
	if(KC == 0)
		KC = tot;
	else if(LC == 0)
		LC = tot-KC;
	else if(MC == 0)
		MC = tot-KC-LC;
	else
		NC = tot-KC-LC-MC;
		
	double randomNumber = G4UniformRand()*tot;
//	cout<<"KC "<<KC<<" LC "<<LC<<" MC "<<MC<<" NC "<<NC<<" tot "<<tot<<" random "<<randomNumber<<endl;
	if(randomNumber<=KC)//K - shell nr = 0
	{
		return G4AtomicShells::GetBindingEnergy(m_atomicNumber, 0) / keV;// Converts to keV from MeV
	}
		
	else if(randomNumber>KC && randomNumber<=KC+LC)// L - shell 1
		return G4AtomicShells::GetBindingEnergy(m_atomicNumber, 1) / keV;
		
	else if(randomNumber>KC+LC && randomNumber<=KC+LC+MC)// M - shell 4
		return G4AtomicShells::GetBindingEnergy(m_atomicNumber, 4) / keV;
		
	else if(randomNumber>KC+LC+MC && randomNumber<=tot)// N - shell 9
		return G4AtomicShells::GetBindingEnergy(m_atomicNumber, 9) / keV;
			
	else
	{
		cout<<"error - wrong finding X Rays energy!!!"<<endl;
		return 0;
	}
}

int MTASDecayManager::FindGammaEmitedFromLevel(int levelId)
{	
	G4double randomNumber;
	G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
	G4ParticleDefinition* gammaPD = particleTable->FindParticle("gamma");
	G4ParticleDefinition* electronPD = particleTable->FindParticle("e-");
	G4int gammaId;
	G4double sumGammasIntesities=0;
	randomNumber=G4UniformRand();
	//determine the gamma
	for( unsigned int i=0; i<(m_Levels[levelId]->GetGammasId()).size();i++)
	{
		int k= m_Levels[levelId]->GetGammasId()[i];
		if(randomNumber>sumGammasIntesities && randomNumber<=sumGammasIntesities+m_Gammas[k]->GetTotalIntensity())
		{
			gammaId=k;	

				
			//electron conversion
			randomNumber=G4UniformRand()*100.;
			if(randomNumber<m_Gammas[k]->GetElectronConversionCoefficient()*m_Gammas[k]->GetGammaIntensity())
			{
				double xRaysEnergy = FindXRaysEnergy(k);
				m_Particle.push_back(gammaPD);//X
				m_Decay.push_back(xRaysEnergy);

				m_Particle.push_back(electronPD);//electron
				m_Decay.push_back(m_Gammas[k]->GetGammaEnergy()-xRaysEnergy);					
			}
			else
			{
				m_Particle.push_back(gammaPD);
				m_Decay.push_back(m_Gammas[k]->GetGammaEnergy());
			}
				
			levelId=m_Gammas[k]->GetFinalLevelId();//set the final level
		}
		sumGammasIntesities+=m_Gammas[k]->GetTotalIntensity();
	}
	return levelId;
}

void MTASDecayManager::DecaySimulation(vector<double>* decay, vector< G4ParticleDefinition*>* particle)
{
	G4int levelId(0);
	G4double sumLevelsIntesities = 0;
	G4double timeCutoff = 10000.0;
	G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
	G4ParticleDefinition* electronPD = particleTable->FindParticle("e-");
	G4ParticleDefinition* pozytonPD = particleTable->FindParticle("e+");
	G4double randomNumber;
	randomNumber=G4UniformRand();

	m_Decay.clear();
	m_Particle.clear();

//		G4cout <<  "Isomer LEvel: " << m_isomerLevel << G4endl;
	//Check if m_isomerLevel > -1
	if( m_isomerLevel > -1 )
	{
		levelId = m_isomerLevel;
		levelId = FindGammaEmitedFromLevel( levelId );
		m_isomerLevel = -1;

		while( levelId != 0 )//do until final level will be 0 or level half lilife will be higher than 1s
		{
			if( m_Levels[levelId]->GetHalfLifeTimeInSecond() > timeCutoff )
			{
				m_isomerLevel = levelId;
				break;
			}
			else
			{
				levelId = FindGammaEmitedFromLevel(levelId);
			}
		}
	}
	else
	{
		//Determine primary level - from beta intensity
		for( int i = 0; i < m_numberOfLevels; i++ )
		{
			if(randomNumber>sumLevelsIntesities && randomNumber<sumLevelsIntesities+m_Levels[i]->GetBetaIntensity())
			{
				levelId=i;
			}
			sumLevelsIntesities+=m_Levels[i]->GetBetaIntensity();
		}

//		G4cout <<  "LEVEL ID: " << levelId << G4endl;

		//send beta with property energy - from Q
		if (m_Levels[levelId]->GetDecayType()=='B')
		{
//		G4cout <<  m_Particle[i]->GetParticleName() << ", " << particlesEnergy[i] << " keV" << G4endl;
//			G4cout <<  "A" << G4endl;
			for(int i=0; i<m_numberOfBetaMinus;i++)
			{
				if(m_Levels[levelId]->GetBetaId()==i)//BetaId = index in m_BetaMinus vector
				{
					m_Decay.push_back(m_BetaMinus[i]->GetRandomEnergy());
					m_Particle.push_back(electronPD);
				}
			}
		}
		else if(m_Levels[levelId]->GetDecayType()=='E')//not ready!
		{
			for(int i=0; i<m_numberOfBetaMinus;i++)
			{
				if(m_Levels[levelId]->GetBetaId()==i)
				{
					m_Decay.push_back(m_BetaPlus[i]->GetRandomEnergy());
					m_Particle.push_back(pozytonPD);
				}
			}
		}
		else
		{
			m_Decay.push_back(0.0);//zero as a first energy means wrong load file
			m_Particle.push_back(electronPD);
			cout<<"Error!!"<<endl;
		}
	
		while(levelId!=0)//do until final level will be 0
		{			
//			G4cout <<  "levelID: " << levelId << "Halflife: " << m_Levels[levelId]->GetHalfLifeTimeInSecond() << G4endl;
			if( m_Levels[levelId]->GetHalfLifeTimeInSecond() > timeCutoff )
			{
//        G4cout <<  "IsomerlevelID: " << levelId << "Halflife: " << m_Levels[levelId]->GetHalfLifeTimeInSecond() << G4endl;
//				m_isomerLevel=levelId;// Uncomment this line if there are actual Isomer levels
				break;
			}
			else
			{
				levelId = FindGammaEmitedFromLevel(levelId);
			}
		}
	}
	
//	for( unsigned int i = 0; i < m_Decay.size(); i++ )
//	{
//		cout << m_Particle[i]->GetParticleName() << " of energy " << m_Decay[i] << " keV created." <<endl;
//	}
	
	*decay = m_Decay;
	*particle = m_Particle;
}
