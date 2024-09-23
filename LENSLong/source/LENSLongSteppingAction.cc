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
#include "LENSLongSteppingAction.hh"

#include "LENSLongDetectorConstruction.hh"
#include "LENSLongLiquidScintillator.hh"

#include "G4SteppingManager.hh"
#include "G4SDManager.hh"
#include "G4EventManager.hh"
#include "G4ProcessManager.hh"
#include "G4Track.hh"
#include "G4Step.hh"
#include "G4Event.hh"
#include "G4StepPoint.hh"
#include "G4TrackStatus.hh"
#include "G4VProcess.hh"
#include "G4VPhysicalVolume.hh"
#include "G4ParticleDefinition.hh"
#include "G4ParticleTypes.hh"
#include "G4OpBoundaryProcess.hh"

#include "G4VVisManager.hh"
#include "G4Colour.hh"
#include "G4VisAttributes.hh"
#include "G4LogicalVolume.hh"
#include "G4VPhysicalVolume.hh"

#include "GLG4Scint.hh"
#include "G4SystemOfUnits.hh"


//_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_

LENSLongSteppingAction::LENSLongSteppingAction()  : oneStepPrimaries(false)
{
	m_Detector = 0;
}
//_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_

LENSLongSteppingAction::LENSLongSteppingAction( LENSLongDetectorConstruction* theDetectorConstruction )  : oneStepPrimaries(false)
{
	m_Detector = theDetectorConstruction;
}

//_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_

LENSLongSteppingAction::~LENSLongSteppingAction()
{
}

//_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_

void LENSLongSteppingAction::UserSteppingAction(const G4Step* theStep)
{
// START IMPROVED SCINT AND OPTICAL PHOTON Travelling.
  // do scintillation photons, and also re-emission
//  if( theTrack->GetCurrentStepNumber() == 0 )
//		thePrePoint->GetProcessDefinedStep() != 0 && thePrePoint->GetProcessDefinedStep()->GetProcessName() == "Cerenkov")
//  if( theStep->GetPreStepPoint()->GetProcessDefinedStep() != 0 && 
//			theStep->GetPreStepPoint()->GetProcessDefinedStep()->GetProcessName() != "Transportation" )
  {
		G4ParticleDefinition* particleType = theStep->GetTrack()->GetDefinition();
//	if( particleType == G4OpticalPhoton::OpticalPhotonDefinition() && theStep->GetTrack()->GetCreatorProcess() != NULL )
	if( particleType == G4OpticalPhoton::OpticalPhotonDefinition()  && 
		theStep->GetTrack()->GetCurrentStepNumber() == 1 && 
		theStep->GetTrack()->GetCreatorProcess() != NULL && 
		theStep->GetTrack()->GetCreatorProcess()->GetProcessName() == "Cerenkov" )
	{
// wavelength is below 300nm (above 4.132808 * eV) optical photon can not be detected by PMT so kill it.
		if( theStep->GetTrack()->GetDynamicParticle()->GetKineticEnergy() < 4.132808 * eV )
		{
			//if( theStep->GetTrack()->GetCurrentStepNumber() == 1 ) // Why this is not 0 is a mystery to me.
			{
//			static int keepCount = 0;
//			static int killCount = 0;
// Apparently there is no need to include the random header, because it is included in one of the other header files already.
			const double fractionToKeep = m_Detector->GetOpticalPhotonEmissionFraction();// This compensates for the increase in the PMT QE
			if( G4UniformRand() > fractionToKeep )
			{
//				++killCount;
				theStep->GetTrack()->SetTrackStatus( fStopAndKill );
			}
//			else
//			{
//				++keepCount;
//			}
			
// Output for testing			
//			G4cout << "CERENKOV PHOTONS: " << keepCount + killCount 
//				<< ", KILLED CERENKOV FRACTION: " << ( killCount / (G4double)(keepCount + killCount) ) << ", KILL FRACTION: " 
//				<< fractionToKeep << G4endl;
			}
		}
		else
		{
			theStep->GetTrack()->SetTrackStatus( fStopAndKill );
		}

//		static int count = 0;
//		++count;
//		G4cout << theStep->GetTrack()->GetCreatorProcess()->GetProcessName() << G4endl;
//		G4cout << theStep->GetTrack()->GetCurrentStepNumber() << G4endl;
//		G4cout << "Cerenkov Photon Count: " << count << G4endl;
	}
//	static int count = 0;
//	++count;
//	G4cout << "Cerenkov photon Detected! Count: " << count << G4endl;
//	G4cout << theStep->GetPreStepPoint()->GetProcessDefinedStep()->GetProcessName() << G4endl;
  }

    {
      // invoke scintillation process
		G4VParticleChange * pParticleChange = GLG4Scint::GenericPostPostStepDoIt(theStep);
      // were any secondaries defined?
		G4int iSecondary= pParticleChange->GetNumberOfSecondaries();
		
// VERY TEMP FOR TESTING
//		static G4int numberSecondaries = 0;// in this case static means leave it around after the variable is created.
//		if (iSecondary > 0)
//		{
//			numberSecondaries += iSecondary;
//			G4cout << "Total Number Secondaries: " << numberSecondaries << G4endl;
//		}
// END OF VERY TEMP FOR TESTING
		
		if (iSecondary > 0)
		{
	  // add secondaries to the list
			while ( (iSecondary--) > 0 )
			{
				G4Track* tempSecondaryTrack = pParticleChange->GetSecondary(iSecondary);
				
// START VERY TEMP FOR TESTING
//				tempSecondaryTrack->SetTrackStatus( fStopAndKill );
// END VERY TEMP FOR TESTING
				fpSteppingManager->GetfSecondary()->push_back( tempSecondaryTrack );

			}
		}
      // clear ParticleChange
		pParticleChange->Clear();
    }
// END IMPROVED SCINT AND OPTICAL PHOTON Travelling.

  G4Track* theTrack = theStep->GetTrack();
//  G4StepPoint* thePrePoint = theStep->GetPreStepPoint();

  G4StepPoint* thePostPoint = theStep->GetPostStepPoint();
  G4VPhysicalVolume* thePostPV = thePostPoint->GetPhysicalVolume();
  
//  NuSNSUserEventInformation* eventInfo
//    = (NuSNSUserEventInformation*) G4EventManager::GetEventManager()->GetConstCurrentEvent()->GetUserInformation();

//  G4StepPoint* thePrePoint = theStep->GetPreStepPoint();
//  G4VPhysicalVolume* thePrePV = thePrePoint->GetPhysicalVolume();


  G4OpBoundaryProcessStatus boundaryStatus = Undefined;
  static G4OpBoundaryProcess* boundary = NULL;
  
  //find the boundary process only once
  if(!boundary)
	{
    G4ProcessManager* pm 
      = theStep->GetTrack()->GetDefinition()->GetProcessManager();
    G4int nprocesses = pm->GetProcessListLength();
    G4ProcessVector* pv = pm->GetProcessList();
    G4int i;
    for( i=0; i < nprocesses; i++ )
		{
      if( (*pv)[i]->GetProcessName() == "OpBoundary" )
			{
				boundary = (G4OpBoundaryProcess*)(*pv)[i];
				break;
      }
    }
  }

  if(theTrack->GetParentID()==0)
	{//This is a primary track
/*
    G4TrackVector* fSecondary=fpSteppingManager->GetfSecondary();   
    G4int tN2ndariesTot = fpSteppingManager->GetfN2ndariesAtRestDoIt()
      + fpSteppingManager->GetfN2ndariesAlongStepDoIt()
      + fpSteppingManager->GetfN2ndariesPostStepDoIt();
    
//If we havent already found the conversion position and there were 
//secondaries generated, then search for it

    if(!eventInformation->IsConvPosSet() && tN2ndariesTot>0 )
		{
      for( size_t lp1=(*fSecondary).size()-tN2ndariesTot; lp1 < (*fSecondary).size(); lp1++)
			{
				const G4VProcess* creator=(*fSecondary)[lp1]->GetCreatorProcess();
				if(creator)
				{
					G4String creatorName=creator->GetProcessName();
					if(creatorName=="phot"||creatorName=="compt"||creatorName=="conv")
					{	//since this is happening before the secondary is being tracked
						//the Vertex position has not been set yet(set in initial step)
						eventInformation->SetConvPos((*fSecondary)[lp1]->GetPosition());
					}
				}
      }
    }
*/

//    if(oneStepPrimaries&&thePrePV->GetName()=="scintillator")
//      theTrack->SetTrackStatus(fStopAndKill);
  }// END PRIMARY TRACK

  if(!thePostPV)
	{//out of world
    return;
  }

  G4ParticleDefinition* particleType = theTrack->GetDefinition();
  if( particleType == G4OpticalPhoton::OpticalPhotonDefinition() )
	{//Optical photon only
///force drawing of photons in WLS slab
///		if(thePrePV->GetName()=="Slab") trackInformation->SetForceDrawTrajectory(true);
///Kill photons entering expHall from something other than Slab
///    else if(thePostPV->GetName()=="expHall") theTrack->SetTrackStatus(fStopAndKill);
        
//Was the photon absorbed by the absorption process
    if( thePostPoint->GetProcessDefinedStep()->GetProcessName() == "OpAbsorption" )
		{
//      eventInformation->IncAbsorption();
//      trackInformation->AddTrackStatusFlag(absorbed);
    }
   
// 0 - 13
// enum G4OpBoundaryProcessStatus {  Undefined,
//                                  FresnelRefraction, FresnelReflection,
//                                  TotalInternalReflection,
//                                  LambertianReflection, LobeReflection,
//                                  SpikeReflection, BackScattering,
//                                  Absorption, Detection, NotAtBoundary,
//                                  SameMaterial, StepTooSmall, NoRINDEX };

		boundaryStatus=boundary->GetStatus();

//Check to see if the partcile was actually at a boundary
//Otherwise the boundary status may not be valid
//Prior to Geant4.6.0-p1 this would not have been enough to check
    if( thePostPoint->GetStepStatus() == fGeomBoundary )
		{
//			boundary->GetModel()->DumpInfo();
//			if( boundaryStatus != StepTooSmall && boundaryStatus != SpikeReflection && boundaryStatus != NoRINDEX )
//			{
//				G4cout << "Boundary Physical Volume: " << thePrePV->GetName() << G4endl;
//				G4cout << "Boundary Physical Volume: " << thePostPV->GetName() << " : " << thePostPV->GetCopyNo() << G4endl;
//			G4cout << Absorption << Detection << FresnelReflection << TotalInternalReflection << SpikeReflection << G4endl;
//				G4cout << "Boundry Status: " << boundaryStatus << G4endl;
//			}

      switch( boundaryStatus )
			{
				case Absorption:
//					trackInformation->AddTrackStatusFlag(boundaryAbsorbed);
//					eventInformation->IncBoundaryAbsorption();
//					G4SDManager* SDman = G4SDManager::GetSDMpointer();
//					G4String sdName="/LiquidScintillator/LiquidScintillator";//"/NuSNSDet/pmtSD";
//					NuSNSLiquidScintillator* liqScintSD = (NuSNSLiquidScintillator*)SDman->FindSensitiveDetector(sdName);
//					if(liqScintSD) liqScintSD->ProcessOpticalPhotonStepHits( theStep, 0);
					break;
//Note, this assumes that the volume causing detection is the photocathode because 
//it is the only one with non-zero efficiency
				case Detection: 
//Triger sensitive detector manually since photon is
//absorbed but status was Detection
				{
					G4SDManager* SDman = G4SDManager::GetSDMpointer();
//					G4cout << "Number Hits Collections: " << ( SDman->GetHCtable()->entries() ) << G4endl;
					G4String sdName="/LiquidScintillator";
					LENSLongLiquidScintillator* liqScintSD = (LENSLongLiquidScintillator*) SDman->FindSensitiveDetector(sdName);
					if( liqScintSD ) liqScintSD->ProcessOpticalPhotonStepHits( theStep, (G4TouchableHistory*) 0);

//				trackInformation->AddTrackStatusFlag(hitPMT);
				}
				break;
				
				case FresnelReflection:
				case TotalInternalReflection:
				case SpikeReflection:
//					trackInformation->IncReflections();
				break;
				default:
				break;
      }
    }
  }
  
}


















