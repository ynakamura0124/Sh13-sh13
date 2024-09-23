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



#ifndef LENSLongAnalysisManger_h
#define LENSLongAnalysisManger_h 1

#include <stdio.h>
#include <globals.hh>
#include <G4ThreeVector.hh>
#include <TObject.h>
#include <TFile.h>
#include <TTree.h>
#include <TH1.h>
#include <TNtuple.h>

// Forward declarations of Geant4 classes
class G4Run;
class G4Event;
class G4Step;

// forward declaration of root classes to use
class TObjArray;// List of ROOT objects
class TFile;
class TTree;// TTree in TTree.h
class TH1F;// TH1F in TH1F.h
class TNtuple; //TNtupleD in TNtupleD.h OR TNtuple in TNtuple.h for f's


class PMTTimeInfo
{
public:
	G4bool m_SeesPhotons;
	G4float *m_Times;
	G4float *m_Energies;
	G4int m_PMTID;
	G4int m_NumberTimes;
	
	void clear();
	void cleanUp();
};

class ModuleTimeInfo
{
public:
	G4int m_ModuleID;
	G4int m_NumberPMT;
	
	PMTTimeInfo *m_PMTTimeInfo;
	
	void clear();
	void cleanUp();
};

class TotalEventInfo
{
public:
	G4int m_EventID;
	G4int m_InitialNumberParticles;
  std::vector<G4float> mv_KE0;
  std::vector<G4ThreeVector> mv_X0;//LOAD with nonlinear light production with currentKE + EDep.
  std::vector<G4ThreeVector> mv_P0;//LOAD with nonlinear light production with currentKE + EDep.
//  G4float *KE0;
//  G4ThreeVector *m_P0;
  
	G4float m_TotalMTASEDep;// MTAS Only
	G4float m_TotalTriggerEDep;// Silicon Only
  
	G4int m_NumberModulesHit;// Modules include 2 Si trigger detectors
  std::vector<G4int> mv_ModulesIDs;
  std::vector<G4float> mv_ModuleEnergies;
  std::vector<G4float> mv_ModuleNLEnergies;
  std::vector<G4float> mv_ModuleEarliestTime;
//	G4int *m_ModulesIDs;
//	G4float *m_ModuleEnergies;
//	G4float *m_ModuleNLEnergies;
//	G4float *m_ModuleEarliestTime;
  G4float mvd_KE0[21];
//  std::vector<G4ThreeVector> mv_X0;//LOAD with nonlinear light production with currentKE + EDep.
//  std::vector<G4ThreeVector> mv_P0;//LOAD with nonlinear light production with currentKE + EDep.
//  G4float *KE0;
//  G4ThreeVector *m_P0;
  
  G4int mvd_ModulesIDs[21];
  G4float mvd_ModuleEnergies[21];
  G4float mvd_ModuleNLEnergies[21];
  G4float mvd_ModuleEarliestTime[21];
  
	void clear();
//	void cleanUp();
};

class LENSLongAnalysisManager
{
public:
  LENSLongAnalysisManager();
  virtual ~LENSLongAnalysisManager();

public:
  void AddFakeData();
	void AddStepInfo( const G4int eventID , const G4int cellID, const G4double currentKE, 
					const G4double totalEnergyLostInStep, const G4double timeOfStep, 
					const G4double distanceOfStep, const G4int processID );
  void AddEnergyDepositedInCell( const G4int eventID , const G4int numberCellsHit, G4double energyDepositInCell,
																 G4double nonlinearEnergyDepositInCell, G4double time );
  void AddCellCounting( const int eventID , const int numberCellsHit, const G4double totalEnergyDepositedInEvent, G4double time );
	void AddPixelOpticalPhotonAbsorbed( const G4int eventID ,  const G4int PMTID, 
					const G4double x, const G4double y, const G4double z, const G4double time, const G4double photonEnergy );
  void AddOpticalPhotonAbsorbed( const int eventID, const int moduleIndex, const int PMTIndex, const G4double time, const G4double energy );
	void AddEventInfo( const int eventID , const G4double particleCharge,  const G4double initialKE,
										 const G4double x0, const G4double y0, const G4double z0,
										 const G4double aimx1, const G4double aimy1, const G4double aimz1,
										 const G4double aimx2, const G4double aimy2, const G4double aimz2 );
	
	void writeEvent();
	void nextEvent();
	void endofEvent( G4int eventID );

protected:

  TObjArray* m_HistogramList;
  TFile* m_RootOutputFile;
  TTree* m_pTree;
// These are an array of pointers. 
// Should do dynamically i.e. allocate the array or make into a std::vector of pointers? 
  TH1F* m_pCellHitHistogram;
	TNtuple* m_pCellHitsNTuple;
  TNtuple* m_pCellsHitNTuple;
  TTree* m_pTotalEventNTuple;

// OpticalStuff
// Change to handle many different PMTS...
  TNtuple* m_pOpticalPMTNTuple;
  TTree* m_p10nsWaveformPMTNTuple;
  TNtuple* m_pEventInfoNTuple;

	FILE* m_pLiquidScintillatorOutputFile;

  void SetUp();
  void CleanUp();
  virtual void MakeAndOpenOutputFile();
  void CloseOutputFile();
  
  G4int m_temp;
  G4bool m_NormalizeOutput;
	
	G4bool m_Output10nsWaveform;
	G4int m_NumberTimes;
	G4int m_NumberModules;
	ModuleTimeInfo *m_aModuleInfo;
  
  TotalEventInfo m_TotalEventInfo;
	
};


#endif // LENSLongAnalysisManger_h


