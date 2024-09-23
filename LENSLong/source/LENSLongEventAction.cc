//
// ********************************************************************
// * DISCLAIMER                                                       *
// *                                                                  *
// * The following disclaimer summarizes all the specific disclaimers *
// * of contributors to this software. The specific disclaimers,which *
// * govern, are listed with their locations in:                      *
// *   http://cern.ch/geant4/license                                  *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.                                                             *
// *                                                                  *
// * This  code  implementation is the  intellectual property  of the *
// * GEANT4 collaboration.                                            *
// * By copying,  distributing  or modifying the Program (or any work *
// * based  on  the Program)  you indicate  your  acceptance of  this *
// * statement, and all its terms.                                    *
// ********************************************************************
//


#include "G4Event.hh"
#include "G4EventManager.hh"
#include "G4HCofThisEvent.hh"
#include "G4ios.hh"
#include "G4PrimaryParticle.hh"
#include "G4SDManager.hh"
#include "G4TrajectoryContainer.hh"
#include "G4Trajectory.hh"
#include "G4VVisManager.hh"
#include "G4VHit.hh"

#include "LENSLongAnalysisManager.hh"
#include "LENSLongEventAction.hh"

#include "LENSLongDetectorConstruction.hh"

#include "LENSLongLiquidScintillator.hh"
#include "LENSLongLiquidScintillatorHit.hh"

#include <stdio.h>

using namespace CLHEP;


//********************************************************************************
 
LENSLongEventAction::LENSLongEventAction(): m_AnalysisManager(0), 
m_LongitudinalScintOpticalPhotonID(-1), m_LongitudinalScintGammaID(-1)
{
	m_LENSLongDetectorConstruction = 0;
	StartUp();
}


LENSLongEventAction::LENSLongEventAction( LENSLongDetectorConstruction* theDetectorConstruction ) : 
m_AnalysisManager(0), m_LongitudinalScintOpticalPhotonID(-1), m_LongitudinalScintGammaID(-1)
{
	m_LENSLongDetectorConstruction = theDetectorConstruction;
	StartUp();
}

//********************************************************************************
 
LENSLongEventAction::~LENSLongEventAction()
{
  CleanUp();
}

//********************************************************************************
 
void LENSLongEventAction::StartUp()
{
  CleanUp();
  m_AnalysisManager = new LENSLongAnalysisManager;
  
  G4String colName;
  G4SDManager* SDman = G4SDManager::GetSDMpointer();

  if( m_LongitudinalScintOpticalPhotonID < 0 )
  {
	colName="LiquidScintillator/OpticalPhoton";// use m_LENSLongDetectorConstruction->getOpticalPhotonDetectorName();
    m_LongitudinalScintOpticalPhotonID = SDman->GetCollectionID( colName );
//		G4cout << ( *SDman->ListTree() ) << G4endl;
//		G4cout << "Liquid Scint Optical PhotonSD ID: " <<  m_LiquidScintOpticalPhotonID << G4endl;
  }

  if( m_LongitudinalScintGammaID < 0 )
  {
	colName="LiquidScintillator/Gamma";// use m_LENSLongDetectorConstruction->getGammaDetectorName();
    m_LongitudinalScintGammaID = SDman->GetCollectionID( colName );
//		G4cout << ( *SDman->ListTree() ) << G4endl;
//		G4cout << "Liquid Scint GammaSD ID: " <<  m_LiquidScintGammaID << G4endl;
  }

}

void LENSLongEventAction::CleanUp()
{
	if( m_AnalysisManager != 0 )
	{
		delete m_AnalysisManager;
	}
}

//********************************************************************************

void LENSLongEventAction::BeginOfEventAction(const G4Event* theEvent )
{
//	theEvent
//	m_AnalysisManager->newEvent();
}

//********************************************************************************
 
void LENSLongEventAction::EndOfEventAction(const G4Event* evt)
{
  G4int event_id = evt->GetEventID();
  G4HCofThisEvent* hitCollection = evt->GetHCofThisEvent();
	
// Longitudinal Optical Photon Scintillator Hits 
  LENSLongLiquidScintillatorOpticalPhotonHitsCollection* liquidScintillatorOpticalHC = 0;

  if( hitCollection && m_LongitudinalScintOpticalPhotonID != -1 )
  {
		liquidScintillatorOpticalHC = (LENSLongLiquidScintillatorOpticalPhotonHitsCollection*)( hitCollection->GetHC( m_LongitudinalScintOpticalPhotonID ) );
	}
	
	if( liquidScintillatorOpticalHC )
	{
		const G4int eventID = evt->GetEventID();
		const G4int numberPMTsHit = liquidScintillatorOpticalHC->entries();

		G4PrimaryVertex* primaryVertex = evt->GetPrimaryVertex();
		if( ! primaryVertex ) return;

//		G4RotationMatrix rotation;
//		m_LENSLongDetectorConstruction->GetHallRotation( rotation );// Local to Global
//		const G4RotationMatrix rotationInverse( rotation.inverse() );// Global to Local
		
		G4ThreeVector scintillatorOffsetG;// Default = (0.0,0.0,0.0)
//		m_LENSLongDetectorConstruction->GetLiquidScintCenterGlobalCoord( scintillatorOffsetG );
//		G4cout << "OffsetGlobalCoordiantes: " << scintillatorOffsetG/m << G4endl;

		G4ThreeVector scintillatorOffsetL;// Default = (0.0,0.0,0.0)
//		m_LENSLongDetectorConstruction->GetLiquidScintCenterLocalCoord( scintillatorOffsetL );
//		G4cout << "OffsetLocalCoordiantes: " << scintillatorOffsetL/m << G4endl;

//  Unrotated
		G4ThreeVector particleLaunchPos = primaryVertex->GetPosition();
//		particleLaunchPos.transform( rotationInverse );// Global to Local

		G4double X0 = particleLaunchPos[0];
		G4double Y0 = particleLaunchPos[1];
		G4double Z0 = particleLaunchPos[2];
		G4double T0 = primaryVertex->GetT0();

		G4PrimaryParticle* particle1 = primaryVertex->GetPrimary();

		G4ThreeVector particleLaunchMom = particle1->GetMomentum();
		G4double P1T = sqrt( particleLaunchMom[0] * particleLaunchMom[0] + particleLaunchMom[1] * particleLaunchMom[1] + particleLaunchMom[2] * particleLaunchMom[2] );

//		particleLaunchMom.transform( rotationInverse );// Global to Local
		G4double P1X = particleLaunchMom[0];
		G4double P1Y = particleLaunchMom[1];
		G4double P1Z = particleLaunchMom[2];
		if( P1T != 0.0 )
		{
			P1X /= P1T;
			P1Y /= P1T;
			P1Z /= P1T;
		}
		
		const G4double mass = particle1->GetMass();
		const G4double initialKE = sqrt( P1T*P1T + mass * mass ) - mass;
		
		G4ParticleDefinition* particle1Def = particle1->GetG4code();
		G4String particle1Name = particle1Def->GetParticleName();
		const char *cName1 = particle1Name.data();
		const G4double particleCharge = particle1Def->GetPDGCharge();
		primaryVertex = primaryVertex->GetNext();

		G4PrimaryParticle* particle2;
		G4ParticleDefinition* particle2Def;
		G4String particle2Name;

		if( primaryVertex )
		{
			particle2 = primaryVertex->GetPrimary();
			particleLaunchMom = particle2->GetMomentum();
			particle2Def = particle2->GetG4code();
			particle2Name = particle2Def->GetParticleName();
		}
		else
		{
			particleLaunchMom = G4ThreeVector(0.0,0.0,0.0);
		}

//		particleLaunchMom.transform( rotationInverse );// Global to Local
		G4double P2X = particleLaunchMom[0];
		G4double P2Y = particleLaunchMom[1];
		G4double P2Z = particleLaunchMom[2];
		G4double P2T = sqrt( particleLaunchMom[0] * particleLaunchMom[0] + particleLaunchMom[1] * particleLaunchMom[1] + particleLaunchMom[2] * particleLaunchMom[2] );
		if( P2T != 0.0 )
		{
			P2X /= P2T;
			P2Y /= P2T;
			P2Z /= P2T;
		}
		
		const char *cName2 = particle2Name.data();
//	G4int numberDetectedOpticalPhotons = 0;

		if( false)
		{
	// Should move this to the Analysis Manager, though the call to liqScint
	// Formatted Output
			FILE* pLiquidScintillatorOutputFile = 0;
			pLiquidScintillatorOutputFile = fopen ("LiquidScintillatorOpticalOutput.txt","a");

			fprintf( pLiquidScintillatorOutputFile, "%15s%15s %15s%15s\n", "Particle1Type=", cName1, " Particle2Type=", cName2 );
	//		fprintf( pLiquidScintillatorOutputFile, "%14s%8d\n", "NumberPMTsHit=", numberPMTsHit );
	//		fprintf( pLiquidScintillatorOutputFile, "%6s%13.6E%6s%13.6E%6s%13.6E%3s%13.6E %7s\n", "PX/PT=", PX, "PY/PT=", PY, "PZ/PT=", PZ, "PT=", PT, "(none, none, none, MeV/c)" );
			fprintf( pLiquidScintillatorOutputFile, "%3s%13.6E%3s%13.6E%3s%13.6E%3s%13.6E %18s\n", "X0=", X0/cm, "Y0=", Y0/cm, "Z0=", Z0/cm, "T0=", T0/ns, "(cm, cm, cm, nsec)" );

			for( int i = 0; i < numberPMTsHit; i++)
			{
	//		numberDetectedOpticalPhotons += (*m_HitsCollection)[i]->GetNumberHits();
				(*liquidScintillatorOpticalHC)[i]->setOutputFile( pLiquidScintillatorOutputFile );
				(*liquidScintillatorOpticalHC)[i]->Print();
				(*liquidScintillatorOpticalHC)[i]->setOutputFile( 0 );
			}
			
			if( pLiquidScintillatorOutputFile != 0 )
			{
				fclose( pLiquidScintillatorOutputFile ) ;
			}
		}

//		m_AnalysisManager->AddEventInfo( eventID, particleCharge, initialKE, X0, Y0, Z0, P1X, P1Y, P1Z, P2X, P2Y, P2Z );
		

		G4int moduleID = 0;
		G4int numberHits = 0;
		G4int PMTID;
		G4double time;
		G4double photonEnergy;
		
		
		for( int i = 0; i < numberPMTsHit; i++)
		{
			moduleID = (*liquidScintillatorOpticalHC)[i]->GetModuleIndex();
			numberHits = (*liquidScintillatorOpticalHC)[i]->GetNumberHits();
			PMTID = (*liquidScintillatorOpticalHC)[i]->GetPMTIndex();
			for( int indexOfAbsorbedPhotons = 0; indexOfAbsorbedPhotons < numberHits; indexOfAbsorbedPhotons++ )
			{
//				fprintf( m_pLiquidScintillatorOpticalPhotonOutputFile, "%12.6E\n", m_Time[i] );
				time = (*liquidScintillatorOpticalHC)[i]->GetTime( indexOfAbsorbedPhotons );
//				photonEnergy = 0.0;//(*liquidScintillatorOpticalHC)[i]->GetEnergyDeposit( indexOfAbsorbedPhotons )
				photonEnergy = (*liquidScintillatorOpticalHC)[i]->GetEnergyDeposit( indexOfAbsorbedPhotons );
				m_AnalysisManager->AddOpticalPhotonAbsorbed( eventID , moduleID, PMTID, time, photonEnergy );
			}
		}

		m_AnalysisManager->endofEvent( eventID );
	}
// END Liquid Scintillator Optical photon Hits 

// BEGIN Liquid Scintillator  EMITTED ELECTRON/gamma Hits
  LENSLongLiquidScintillatorGammaHitsCollection* liquidScintillatorGammaHC = 0;

  if( hitCollection && m_LongitudinalScintGammaID != -1 )
  {
		liquidScintillatorGammaHC = (LENSLongLiquidScintillatorGammaHitsCollection*)( hitCollection->GetHC( m_LongitudinalScintGammaID ) );
	}

	if( liquidScintillatorGammaHC )
	{
		const G4int numberSteps = liquidScintillatorGammaHC->entries();
		
		G4PrimaryVertex* primaryVertex = evt->GetPrimaryVertex();

		G4ThreeVector scintillatorOffsetG;// Default = (0.0,0.0,0.0)
		G4ThreeVector scintillatorOffsetL;// Default = (0.0,0.0,0.0)

		G4ThreeVector particleLaunchPos = primaryVertex->GetPosition();

//		G4double X0 = particleLaunchPos[0]/cm;
//		G4double Y0 = particleLaunchPos[1]/cm;
//		G4double Z0 = particleLaunchPos[2]/cm;
//		G4double T0 = primaryVertex->GetT0()/ns;

//		G4int numberPrimaryParticles = primaryVertex->GetNumberOfParticle();
//		G4cout << "Number Primary Particles: " << numberPrimaryParticles << G4endl;
		
		G4PrimaryParticle* particle = primaryVertex->GetPrimary();
		G4ThreeVector particleLaunchMom = particle->GetMomentum();
		G4double PT = sqrt( particleLaunchMom[0] * particleLaunchMom[0] + particleLaunchMom[1] * particleLaunchMom[1] + particleLaunchMom[2] * particleLaunchMom[2] );

//		particleLaunchMom.transform( rotationInverse );// Global to Local
		G4double PX = particleLaunchMom[0]/MeV;
		G4double PY = particleLaunchMom[1]/MeV;
		G4double PZ = particleLaunchMom[2]/MeV;
		if( PT != 0.0 )
		{
			PX /= PT;
			PY /= PT;
			PZ /= PT;
		}
		
		G4ParticleDefinition* particleDef = particle->GetG4code();
		G4String particleName = particleDef->GetParticleName();
//		const char *cName = particleName.data();

		const G4int eventID = evt->GetEventID();
//		const int vertexCellID = m_LENSLongDetectorConstruction->GetVertexCellIndex();
		G4int cellID = -1;

		G4double initialStepKE = 0.0;
		G4double totalEnergyLostInStep = 0.0;
		G4double timeOfStep = -1.0f;
		G4double totalNLEnergyLostInStep = 0.0;
//		G4double distanceOfStep = -1.0f;
//		G4int processID;

	// Counts only one module. Doh.
		G4int numberCellsHit = 0;
		std::vector<G4int> v_HitCellList;
		std::vector<G4double> v_CellNLEnergyDepositList;//LOAD with nonlinear light production with currentKE + EDep.
		std::vector<G4double> v_CellEnergyDepositList;
		std::vector<G4double> v_CellEarliestTimeList;// Add earliest time too
		int hitCellIndex = 0;
		
		G4double totalEnergyLost = 0.0;
		G4double earliestTime = 100000.0;
		
		const int numberofMTASModules = 18;// 19 - 1 More precisely MTAS module indecies.

		for( int index = 0; index < numberSteps; ++index )
		{
			cellID = (*liquidScintillatorGammaHC)[index]->GetModuleID();

			bool cellIDFound = false;
			for( G4int cellIndex = 0; cellIndex < (G4int) v_HitCellList.size() && ! cellIDFound; cellIndex++ )
			{
				if( v_HitCellList[cellIndex] == cellID ) 
				{
					hitCellIndex = cellIndex;
					cellIDFound = true;
					break;// Stops the for loop I think. yup continue just increments the loop
				}
			}
			
			initialStepKE = (*liquidScintillatorGammaHC)[index]->GetCurrentKE();
			totalEnergyLostInStep = (*liquidScintillatorGammaHC)[index]->GetEnergyDeposited();
			timeOfStep = (*liquidScintillatorGammaHC)[index]->GetTimeOfEDep();
			
	//		if( (*liquidScintillatorGammaHC)[index]->GetInteractionID() != 7 )
			if( cellID < 19 )
			{// Nonlinear for the NaI modules.
				totalNLEnergyLostInStep = this->GetNonlinearEnergyDeposit( initialStepKE / keV,totalEnergyLostInStep / keV) * keV;
			}
			else
			{// No nonlinear for the Silicon detectors
				totalNLEnergyLostInStep = totalEnergyLostInStep;
			}
	//		else totalNLEnergyLostInStep = 0.0;
			
//			G4cout << totalEnergyLostInStep / keV << ", " << totalNLEnergyLostInStep / keV << G4endl;
//			distanceOfStep = (*liquidScintillatorGammaHC)[index]->GetDistanceSinceLastInteraction();
//			processID = (*liquidScintillatorGammaHC)[index]->GetInteractionID();
//			static int count = 0;
//			if( count < 200 )
//			{
//				G4cout << timeOfStep / ns << "," << G4endl;
//				count++;
//			}
			
			if( timeOfStep / ns < 200.0 )// Measureed MTAS cutoff time !!!HMMMMMM!!!!!
			{
				if( cellIDFound )
				{
					v_CellEnergyDepositList[hitCellIndex] += totalEnergyLostInStep;
					v_CellNLEnergyDepositList[hitCellIndex] += totalNLEnergyLostInStep;
					
					if( v_CellEarliestTimeList[hitCellIndex] > timeOfStep )
					{
						v_CellEarliestTimeList[hitCellIndex] = timeOfStep;
					}
				}
				else
				{
					v_HitCellList.push_back( cellID );
					v_CellEnergyDepositList.push_back( totalEnergyLostInStep );
					v_CellNLEnergyDepositList.push_back( totalNLEnergyLostInStep );
					v_CellEarliestTimeList.push_back( timeOfStep );
				}
				
				//m_AnalysisManager->AddStepInfo( eventID , cellID, currentKE, totalEnergyLostInStep, timeOfStep, distanceOfStep, processID );
				if( cellID <= numberofMTASModules )
				{
					totalEnergyLost += totalEnergyLostInStep;
					if( timeOfStep < earliestTime ) earliestTime = timeOfStep;
				}
			}
		}
		
		int numberSiHits = 0;
		for( int cellIndex = 0; cellIndex < (int) v_HitCellList.size(); cellIndex++ )
		{
			if( v_HitCellList[cellIndex] > numberofMTASModules ) numberSiHits++;
			
			m_AnalysisManager->AddEnergyDepositedInCell( eventID, v_HitCellList[cellIndex], 
					v_CellEnergyDepositList[cellIndex], v_CellNLEnergyDepositList[cellIndex], v_CellEarliestTimeList[cellIndex] );
		}
		
		numberCellsHit = (G4int) v_HitCellList.size() - numberSiHits;
		if( numberCellsHit > 0 && earliestTime != 100000.0 ) m_AnalysisManager->AddCellCounting( eventID, numberCellsHit, totalEnergyLost, earliestTime );
    else if( numberCellsHit <= 0 ) m_AnalysisManager->AddCellCounting( eventID, 0, 0, earliestTime );
		//END SIMPLE OUTPUT

//		G4ThreeVector particleLaunchPos = primaryVertex->GetPosition();
//		particleLaunchPos.transform( rotationInverse );// Global to Local

		G4double X0 = particleLaunchPos[0];
		G4double Y0 = particleLaunchPos[1];
		G4double Z0 = particleLaunchPos[2];
		G4double T0 = primaryVertex->GetT0();

		G4double P1T = sqrt( particleLaunchMom[0] * particleLaunchMom[0] + particleLaunchMom[1] * particleLaunchMom[1] + particleLaunchMom[2] * particleLaunchMom[2] );
		G4double P1X = particleLaunchMom[0];
		G4double P1Y = particleLaunchMom[1];
		G4double P1Z = particleLaunchMom[2];
		if( P1T != 0.0 )
		{
			P1X /= P1T;
			P1Y /= P1T;
			P1Z /= P1T;
		}
		
		const G4double mass = particle->GetMass();
		const G4double initialKE = sqrt( P1T*P1T + mass * mass ) - mass;

    m_AnalysisManager->AddEventInfo( eventID, 0.0, initialKE, X0, Y0, Z0, P1X, P1Y, P1Z, 0.0, 0.0, 0.0 );

    m_AnalysisManager->writeEvent();
	}
  


// THIS IS FOR GEANT OUTPUT AND VISUALIZATION OUT, NO HISTOGRAMS INFO DOWN HERE
// In order for this to work you must set the "/tracking/storeTrajectory 1" in the macro. 
// For us this is turned on in the vis.mac
// get number of stored trajectories
//  G4TrajectoryContainer* trajectoryContainer = evt->GetTrajectoryContainer();
//  G4int n_trajectories = 0;
//  if( trajectoryContainer ) n_trajectories = trajectoryContainer->entries();
/*
	if( false ) // BE CAREFUL, this can output a SHITLOAD of data
	{
//		G4cout << "Number of Trajectories: " << n_trajectories << G4endl;
		for( G4int i=0; i < n_trajectories; i++ ) 
		{
			G4Trajectory* trj = (G4Trajectory*)( (*( evt->GetTrajectoryContainer() ) )[i] );
			const G4String particleName = trj->GetParticleName();
			const int pdgenc = trj->GetPDGEncoding();
//			if( particleName == "gamma" )
			if( pdgenc == 22 ) // 22 = gamma, 11 = electron
			{
//				G4int pID = trj->GetParentID();
				G4ThreeVector initMom = trj->GetInitialMomentum();
				G4double initMomSq = initMom[0] * initMom[0] + initMom[1] * initMom[1] + initMom[2] * initMom[2];
				if( 40.0 * 40.0 * keV * keV < initMomSq )
				{
//					static int s_count = 0;
//					s_count++;
					G4PrimaryVertex* primaryVertex = evt->GetPrimaryVertex();
					G4PrimaryParticle* primaryParticle = primaryVertex->GetPrimary();
					G4ThreeVector particleLaunchMom = primaryParticle->GetMomentum();
					G4double PT = sqrt( particleLaunchMom[0] * particleLaunchMom[0] + particleLaunchMom[1] * particleLaunchMom[1] + particleLaunchMom[2] * particleLaunchMom[2] );
					
					const G4double initmom = sqrt( initMomSq );
//					G4cout << s_count << ": E= " << initmom / keV << G4endl;
					G4cout << PT / keV << ", " << initmom / keV << "," << G4endl;
//					G4cout << initmom / keV << "," << G4endl;
				}
//			Get the daughter particle names.
//				if( i <= pID )
//				{
//					G4Trajectory* pTrj = (G4Trajectory*)( (*( evt->GetTrajectoryContainer() ) )[pID] );
//					const G4String parentName = pTrj->GetParticleName();
//					G4cout << parentName << ": " << particleName << G4endl;
//				}
			}
		}
	}
*/

// periodic printing every 100 events
	if( event_id % 1000 == 0 )
	{
		G4cout << "Finished Running Event # " << event_id << G4endl;
	}
    
// extract the trajectories and draw them
//	if( G4VVisManager::GetConcreteInstance() )
//	{
//	   for( G4int i=0; i < n_trajectories; i++ ) 
//		{ 
//			G4Trajectory* trj = (G4Trajectory*)( (*( evt->GetTrajectoryContainer() ))[i] );
//			trj->DrawTrajectory();
//		}
//	}
}

//********************************************************************************


G4double LENSLongEventAction::GetNonlinearEnergyDeposit( const G4double E, const G4double eDep )
{

static const G4double a[] = 
{
1.6,
0.058,
0.580,
0.00490,
0.25,
0.479,
0.00494
};

G4double nonlinearEDep = ( a[0] * ( 1.0 - exp( -a[1] * E) ) + a[2] * E + a[3] * E * E ) * E;
nonlinearEDep /= (a[4] + a[5] * E + a[6] * E * E);

G4double E1 = E - eDep;
if( E1 < 0.0 ) E1 = 0.0;

G4double nonlinearEDep1 = ( a[0] * ( 1.0 - exp( -a[1] * E1) ) + a[2] * E1 + a[3] * E1 * E1 ) * E1;
nonlinearEDep1 /= (a[4] + a[5] * E1 + a[6] * E1 * E1);

return ( nonlinearEDep - nonlinearEDep1 );

}


//********************************************************************************
