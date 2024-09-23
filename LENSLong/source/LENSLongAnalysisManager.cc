/*
 *  LENSLongAnalysisManager.cpp
 *  
 *
 *  Created by Charlie Rasco on 10/5/04.
 *  Copyright 2004 __MyCompanyName__. All rights reserved.
 *
 */

#include "G4ios.hh"
#include "G4SDManager.hh"
#include "G4Run.hh"
#include "G4Event.hh"
#include "G4HCofThisEvent.hh"

#include "LENSLongAnalysisManager.hh"

// Correct way to do this? i.e. how do I get the compiler to look in /Users/Charlie/root/include/
// automatically?
//#include "/Users/Charlie/root/include/TFile.h"
//#include "/Users/Charlie/root/include/TF1.h"
//#include "/Users/Charlie/root/include/TH1F.h"
//#include "/Users/Charlie/root/include/TNtuple.h"
//#include "/Users/Charlie/root/include/THStack.h"

#include "TFile.h"
#include "TH1F.h"
#include "TNtuple.h"
#include "TTree.h"

//#include <fstream>
//#include <stdio.h>

using namespace CLHEP;

//********************************************************************

void PMTTimeInfo::clear()
{
	m_SeesPhotons = false;
	for( int i = 0; i < m_NumberTimes; i++ )
	{
		m_Times[i] = 0.0;
	}
}

void PMTTimeInfo::cleanUp()
{
	delete [] m_Times;
}

//********************************************************************

void ModuleTimeInfo::clear()
{
	for( int i = 0; i < m_NumberPMT; i++ )
	{
		m_PMTTimeInfo[i].clear();
	}
}

void ModuleTimeInfo::cleanUp()
{
	for( int i = 0; i < m_NumberPMT; i++ )
	{
		m_PMTTimeInfo[i].cleanUp();
	}
	
	delete [] m_PMTTimeInfo;
}


//********************************************************************

void TotalEventInfo::clear()
{
  m_EventID = -1;
  m_InitialNumberParticles = 0;
  mv_KE0.clear();
  mv_X0.clear();
  mv_P0.clear();
  m_TotalMTASEDep = 0.0;// MTAS Only
	m_TotalTriggerEDep = 0.0;// Silicon Only
  
	m_NumberModulesHit = -1;// Modules include 2 Si trigger detectors
  mv_ModulesIDs.clear();
  mv_ModuleEnergies.clear();
  mv_ModuleNLEnergies.clear();
  mv_ModuleEarliestTime.clear();
}

//********************************************************************************

LENSLongAnalysisManager::LENSLongAnalysisManager()
: m_HistogramList(0), m_RootOutputFile(0), m_pTree(0), m_NormalizeOutput(true)
{
  m_pCellHitHistogram = 0;

	m_pCellHitsNTuple = 0;
	m_pCellsHitNTuple = 0;
  m_pTotalEventNTuple = 0;
  
  m_TotalEventInfo.clear();

  m_pOpticalPMTNTuple = 0;
	m_p10nsWaveformPMTNTuple = 0;
  m_pEventInfoNTuple = 0;
	
	m_pLiquidScintillatorOutputFile = 0;
	
	m_Output10nsWaveform = false;
	//m_Output10nsWaveform = true;
	m_NumberTimes = 650;
  m_NumberModules = 19;

	if( m_Output10nsWaveform )
	{
		m_aModuleInfo = new ModuleTimeInfo[m_NumberModules];
		for( int i = 0; i < m_NumberModules; i++ )
		{
			m_aModuleInfo[i].m_ModuleID = i;
			
			if( i < 18 )
			{
				m_aModuleInfo[i].m_NumberPMT = 2;
				m_aModuleInfo[i].m_PMTTimeInfo = new PMTTimeInfo[m_aModuleInfo[i].m_NumberPMT];
			}
			else
			{
				m_aModuleInfo[i].m_NumberPMT = 12;
				m_aModuleInfo[i].m_PMTTimeInfo = new PMTTimeInfo[m_aModuleInfo[i].m_NumberPMT];
			}
			
			for( int j = 0; j < m_aModuleInfo[i].m_NumberPMT; j++ )
			{
				m_aModuleInfo[i].m_PMTTimeInfo[j].m_NumberTimes = m_NumberTimes;
				m_aModuleInfo[i].m_PMTTimeInfo[j].m_Times = new G4float[m_NumberTimes];
				m_aModuleInfo[i].m_PMTTimeInfo[j].m_PMTID = j;
				
				m_aModuleInfo[i].m_PMTTimeInfo[j].clear();
			}
		}
	}
	
  SetUp();
}

// Destructor
LENSLongAnalysisManager::~LENSLongAnalysisManager()
{	
//	MakeAndOpenOutputFile();

// No need to write to file until the end? Even if it is REALLY BIG?
  //MakeAndOpenOutputFile();
	if( m_RootOutputFile != 0 )
	{
// Create an array of Histograms
//  m_HistogramList = new TObjArray;
		TObjArray histogramList;// = new TObjArray;
//		THStack histStack( "hs", "Temp Histograms" );

//		histStack.Add( m_pHistograms[0] );
//		histStack.Add( m_pHistograms[1] );

// Useless until used to actually draw the stack of histograms.
//  histStack.Draw("nostack");
//  histStack.GetXaxis()->SetTitle("Muon Energy Lost (MeV)");
//		histogramList.Add( &histStack );
  
//		histogramList.Add( m_pHistograms[0] );
//		histogramList.Add( m_pHistograms[1] );

//		histogramList.Add( m_pTuples[0] );
//		histogramList.Add( m_pTuples[1] );

//		histogramList.Add( m_pCellHitsNTuple );
//		histogramList.Add( m_pCellsHitNTuple );
//		histogramList.Add( m_pOpticalPMTNTuple );
//		histogramList.Add( m_pEventInfoNTuple );
//		histogramList.Add( m_p10nsWaveformPMTNTuple );
    
    histogramList.Add( m_pTotalEventNTuple );
				
		histogramList.Write();// Writes to the current open file
	}

//	if( m_pLiquidScintillatorOutputFile != 0 )
//	{
//		float n[3] = { .0000457, 124507.,7.2 };
//		fprintf (m_pLiquidScintillatorOutputFile, "%-E %E\n",n[0],n[1]);
//		fprintf (m_pLiquidScintillatorOutputFile, "%-.4E \n",n[1]);
//		fprintf (m_pLiquidScintillatorOutputFile, "%.3E  \n",n[2]);
//	}
  CloseOutputFile();
  
	if( m_Output10nsWaveform )
	{
		for( int i = 0; i < m_NumberModules; i++ )
		{
			m_aModuleInfo[i].cleanUp();
		}
		
		delete [] m_aModuleInfo;
	}
	
  CleanUp();
}

//********************************************************************

void LENSLongAnalysisManager::SetUp()
{
  CleanUp();// Make sure all are valid NULL pointers

// Create an array of Histograms
//  m_HistogramList = new TObjArray;

// Color Defintions are at http://root.cern.ch/root/html/gif/colors.gif
// 1 black, 2 red, 3 green, 4 blue, 5 yellow, 6 lavender, 7 cyan, 
// 13-17 dark to lite blue-grey. 11 grey. Up to 50 is a valid color.
//	m_pHistograms[0] = new TH1F("ELost", "#mu Energy Lost In Scintillator Top", 100, 0, 50 );
//  m_pHistograms[0]->GetXaxis()->SetTitle("#mu Energy Lost (MeV)");
//  m_pHistograms[0]->SetLineColor(2);

//  m_pHistograms[1] = new TH1F("ELost1", "#mu Energy Lost In Scintillator Top", 100, 0, 50 );
//  m_pHistograms[1]->GetXaxis()->SetTitle("#mu Energy Lost (MeV)");
//  m_pHistograms[1]->SetLineColor(3);

//  m_pTuples[0] = new TNtuple("TestTuple0","TestTuple 1","ELostInScint");
//  m_pTuples[1] = new TNtuple("TestTuple1","TestTuple 1","ELostInScint");

//  m_HistogramList->Add( m_pCellHitNTuple );
//  m_HistogramList->Add( m_pHistograms[0] );
//  m_HistogramList->Add( m_pHistograms[1] );
//  m_HistogramList->Add( m_pTuples[0] );
//  m_HistogramList->Add( m_pTuples[1] );
  
  m_temp = 0;
}

//********************************************************************

void LENSLongAnalysisManager::CleanUp()
{
	CloseOutputFile();
// Close then delete the file in memeory
	if( m_RootOutputFile != 0 )
	{
	  
	  delete m_RootOutputFile;
	}

	if( m_pLiquidScintillatorOutputFile != 0 )
	{
	  
	  delete m_pLiquidScintillatorOutputFile;
	}

// Delete histograms
	if( m_pCellHitHistogram != 0 )
	{
		delete m_pCellHitHistogram;
	}

// Delete touple used to save things to root file.
	if( m_pCellsHitNTuple != 0 )
	{
		delete m_pCellsHitNTuple;
	}

	if( m_pCellHitsNTuple != 0 )
	{
		delete m_pCellHitsNTuple;
	}

	if( m_pOpticalPMTNTuple != 0 )
	{
		delete m_pOpticalPMTNTuple;
	}
	if( m_pEventInfoNTuple != 0 )
	{
		delete m_pEventInfoNTuple;
	}

// Get rid of the list of ROOT objects
	if( m_HistogramList != 0 )
	{
		delete m_HistogramList;
	}
}


//********************************************************************


void LENSLongAnalysisManager::MakeAndOpenOutputFile()
{
  m_RootOutputFile = new TFile( "OpticalPhotonInfo.root","recreate" );// This opens the file too

//	if( m_pLiquidScintillatorOutputFile == 0 )
//	{
//		m_pLiquidScintillatorOutputFile  = fopen ("ScintillatorOutput1.txt","w");
//	}

/*
// Copy a file with <fstream>
  char * buffer;
  long size;
  ifstream infile ("test.txt",ifstream::binary);
  ofstream outfile ("new.txt",ofstream::binary);
// get size of file
  infile.seekg(0,ifstream::end);
  size=infile.tellg();
  infile.seekg(0);
// allocate memory for file content
  buffer = new char [size];
// read content of infile
  infile.read (buffer,size);
// write to outfile
  outfile.write (buffer,size);
// release dynamically-allocated memory
  delete[] buffer;
	outfile.close();
	infile.close();
*/
}

//********************************************************************

void LENSLongAnalysisManager::CloseOutputFile()
{
//  if( ! m_RootOutputFile->IsOpen() ) return;
	if( m_RootOutputFile != 0 )
	{
//		m_RootOutputFile->Write();
		m_RootOutputFile->Close();
	}
	
	if( m_pLiquidScintillatorOutputFile != 0 )
	{
		fclose (m_pLiquidScintillatorOutputFile);
	}
}

//********************************************************************

void LENSLongAnalysisManager::AddFakeData()
{
//  m_pHistograms[0]->Fill(m_temp);
  m_temp += 1;
}
//********************************************************************

void LENSLongAnalysisManager::AddStepInfo( const G4int eventID , const G4int cellID, const G4double currentKE,
										const G4double totalEnergyLostInStep, const G4double timeOfStep, 
										const G4double distanceOfStep, const G4int processID )
{
	if( m_pCellsHitNTuple == 0 )
	{
		m_pCellsHitNTuple = new TNtuple("stepInfo","Per Step Info","eventID:cellID:currentEnergy:energyDeposit:EDepTime:Distance:processID", 32000 );
	}

	m_pCellsHitNTuple->Fill( eventID, cellID, currentKE/keV, totalEnergyLostInStep/keV, timeOfStep / ns * 1000000.0, distanceOfStep / nm, processID );
}


//********************************************************************

void LENSLongAnalysisManager::AddEnergyDepositedInCell( const G4int eventID , const G4int cellID, G4double energyDepositInCell,
																												G4double nonlinearEnergyDepositInCell, G4double time )
{
//	if( m_pCellsHitNTuple == 0 )
//	{
//		m_pCellsHitNTuple = new TNtuple("cellshitInfo","Cell Hit With Energy Deposit","eventID:cellID:energydeposit:nlenergydeposit:earliestEDeptime", 32000 );
//	}
//
//	m_pCellsHitNTuple->Fill( eventID, cellID, energyDepositInCell/keV, nonlinearEnergyDepositInCell/keV, time / ns );

//  m_TotalEventInfo.m_EventID;
  if( cellID <= 18 ) m_TotalEventInfo.m_TotalMTASEDep += energyDepositInCell/keV;// MTAS module
  else if( cellID <= 20 ) m_TotalEventInfo.m_TotalTriggerEDep += energyDepositInCell/keV;// Si
  
  m_TotalEventInfo.mv_ModulesIDs.push_back(cellID);
  m_TotalEventInfo.mv_ModuleEnergies.push_back(energyDepositInCell/keV);
  m_TotalEventInfo.mv_ModuleNLEnergies.push_back(nonlinearEnergyDepositInCell/keV);
  m_TotalEventInfo.mv_ModuleEarliestTime.push_back(time/ns);
}


void LENSLongAnalysisManager::AddCellCounting( const int eventID , const int numberCellsHit, const G4double totalEnergyDepositedInEvent, G4double time  )
{
//	if( m_pCellHitsNTuple == 0 )
//	{
//		m_pCellHitsNTuple = new TNtuple("cellhitsInfo","Number Cells Hit in Event","eventID:numbercellshits:totalenergydeposit:earliestEDeptime", 32000 );
//	}
//
//	m_pCellHitsNTuple->Fill( eventID, numberCellsHit, totalEnergyDepositedInEvent/keV, time / ns );
}

void LENSLongAnalysisManager::AddPixelOpticalPhotonAbsorbed( const G4int eventID ,  const G4int PMTID, 
			const G4double x, const G4double y, const G4double z, const G4double time, const G4double photonEnergy )
{
	if( m_pOpticalPMTNTuple == 0 )
	{
		m_pOpticalPMTNTuple = new TNtuple("PMTHitInfo","Optical Photons detected by PMT","eventID:pmtID:x:y:z:time:photonenergy", 32000 );
	}
	m_pOpticalPMTNTuple->Fill( eventID, PMTID, x/m, y/m, z/m, time/ns, photonEnergy/eV );
}

void LENSLongAnalysisManager::AddOpticalPhotonAbsorbed( const int eventID , const int moduleID, const int PMTID, const G4double time, const G4double energy )
{
	if( m_Output10nsWaveform )
	{
		G4int timeIndex = ((G4int)time ) / 10;
		
//		static int kkk = 0;
//		if( kkk < 30 )
//		{
//		G4cout << "Time: " << time << G4endl;
//		G4cout << "TimeIndex: " << timeIndex << G4endl;
//		kkk++;
//		}
//		G4cout << "Module ID: " << moduleID << G4endl;
//		G4cout << "PMT ID: " << PMTID << G4endl;
		if( timeIndex >= 0 && timeIndex < m_NumberTimes )
		{
			m_aModuleInfo[moduleID].m_PMTTimeInfo[PMTID].m_SeesPhotons = true;
			m_aModuleInfo[moduleID].m_PMTTimeInfo[PMTID].m_Times[timeIndex] += 1.0;
		}
	}
	else
	{
		if( m_pOpticalPMTNTuple == 0 )
		{
			m_pOpticalPMTNTuple = new TNtuple("PMTHitInfo","Optical Photons detected by PMT","eventID:moduleID:pmtID:time:photonenergy", 32000 );
		}
		m_pOpticalPMTNTuple->Fill( eventID, moduleID, PMTID, time/ns, energy/eV );
	}
}

void LENSLongAnalysisManager::AddEventInfo( const int eventID, const G4double particleCharge, const G4double initialKE,
																						const G4double x0, const G4double y0, const G4double z0,
																						const G4double aimx1, const G4double aimy1, const G4double aimz1,
																						const G4double aimx2, const G4double aimy2, const G4double aimz2 )
{
//	if( m_pEventInfoNTuple == 0 )
//	{
//		m_pEventInfoNTuple = new TNtuple("EventInfo","Event Information","eventID:charge:KE0:x0:y0:z0:aimx1:aimy1:aimz1:aimx2:aimy2:aimz2", 32000 );
//	}
//	m_pEventInfoNTuple->Fill( eventID, particleCharge, initialKE/keV, x0/m, y0/m, z0/m, aimx1, aimy1, aimz1, aimx2, aimy2, aimz2 );

  m_TotalEventInfo.m_EventID = eventID;
  m_TotalEventInfo.mv_KE0.push_back( initialKE );
  m_TotalEventInfo.mv_X0.push_back( G4ThreeVector(x0,y0,z0) );
  m_TotalEventInfo.mv_P0.push_back( G4ThreeVector(aimx1,aimy1,aimz1) );// Only handle one particle for now...
//  if( aimx2 != 0.0 && aimy2 != 0.0 && aimz2 != 0.0 ) m_TotalEventInfo.mv_P0.push_back( G4ThreeVector(aimx2,aimy2,aimz2) );

}

void LENSLongAnalysisManager::writeEvent()
{
  m_TotalEventInfo.m_InitialNumberParticles = m_TotalEventInfo.mv_KE0.size();
  m_TotalEventInfo.m_NumberModulesHit = m_TotalEventInfo.mv_ModulesIDs.size();
  
	if( m_pTotalEventNTuple == 0 )
	{
//		m_pTotalEventNTuple = new TNtuple("eventData","Total Event Info", "eventID:numParticles:KE0:MTAS_E:Si_E:numMods:modID[numMods]:modE[numMods]:modNLE[numMods]:modT[numMods]", 32000 );
    m_pTotalEventNTuple = new TTree("eventData","TotalEventInfo");
    
    m_pTotalEventNTuple->Branch("eventID",&m_TotalEventInfo.m_EventID,"eventID/I");
    m_pTotalEventNTuple->Branch("numPart",&m_TotalEventInfo.m_InitialNumberParticles,"numPart/I");
    m_pTotalEventNTuple->Branch("KE0",m_TotalEventInfo.mvd_KE0,"KE0[numPart]/F");
//    m_pTotalEventNTuple->Branch("x0",m_TotalEventInfo.mv_X0[0][0],"X0[numPart]/F");
//    m_pTotalEventNTuple->Branch("y0",m_TotalEventInfo.mv_X0[0][1],"Y0[numPart]/F");
//    m_pTotalEventNTuple->Branch("z0",m_TotalEventInfo.mv_X0[0][2],"Z0[numPart]/F");

    m_pTotalEventNTuple->Branch("MTAS_E", &m_TotalEventInfo.m_TotalMTASEDep ,"MTAS_E/F");
    m_pTotalEventNTuple->Branch("Si_E",&m_TotalEventInfo.m_TotalTriggerEDep ,"Si_E/F");

    m_pTotalEventNTuple->Branch("numMod",&m_TotalEventInfo.m_NumberModulesHit,"numMod/I");
    m_pTotalEventNTuple->Branch("modID", m_TotalEventInfo.mvd_ModulesIDs,"modID[numMod]/I");
    m_pTotalEventNTuple->Branch("modE", m_TotalEventInfo.mvd_ModuleEnergies,"modE[numMod]/F");
    m_pTotalEventNTuple->Branch("modNLE", m_TotalEventInfo.mvd_ModuleNLEnergies,"modNLE[numMod]/F");
    m_pTotalEventNTuple->Branch("modTime", m_TotalEventInfo.mvd_ModuleEarliestTime,"modTime[numMod]/F");
	}

  for( G4int ii = 0; ii < m_TotalEventInfo.m_InitialNumberParticles; ii++ )
  {
    m_TotalEventInfo.mvd_KE0[ii] = m_TotalEventInfo.mv_KE0[ii];
//    mvd_ModuleEnergies[ii] = m_TotalEventInfo.mv_ModuleEnergies[ii];
//    mvd_ModuleNLEnergies[ii] = m_TotalEventInfo.mv_ModuleNLEnergies[ii];
//    mvd_ModuleEarliestTime[ii] = m_TotalEventInfo.mv_ModuleEarliestTime[ii];
  }
  
  for( G4int ii = 0; ii < m_TotalEventInfo.m_NumberModulesHit; ii++ )
  {
    m_TotalEventInfo.mvd_ModulesIDs[ii] = m_TotalEventInfo.mv_ModulesIDs[ii];
    m_TotalEventInfo.mvd_ModuleEnergies[ii] = m_TotalEventInfo.mv_ModuleEnergies[ii];
    m_TotalEventInfo.mvd_ModuleNLEnergies[ii] = m_TotalEventInfo.mv_ModuleNLEnergies[ii];
    m_TotalEventInfo.mvd_ModuleEarliestTime[ii] = m_TotalEventInfo.mv_ModuleEarliestTime[ii];
  }

	m_pTotalEventNTuple->Fill();

//  G4ThreeVector&  X = m_TotalEventInfo.mv_X0.at(0);
//  G4ThreeVector&  P = m_TotalEventInfo.mv_P0.at(0);

//	m_pTotalEventNTuple->Fill(m_TotalEventInfo.m_EventID, 
//                            m_TotalEventInfo.m_InitialNumberParticles,
//                            m_TotalEventInfo.mv_KE0[0],
//                            X[0], X[1], X[2],
//                            P[0], P[1], P[2],
//                            m_TotalEventInfo.m_NumberModulesHit,
//                            m_TotalEventInfo.mv_ModulesIDs[0],
//                            m_TotalEventInfo.mv_ModuleEnergies[0],
//                            m_TotalEventInfo.mv_ModuleNLEnergies[0],
//                            m_TotalEventInfo.mv_ModuleEarliestTime[0]
//                            );
  

//	if( m_pTotalEventNTuple == 0 )
//	{
//		m_pTotalEventNTuple = new TNtuple("eventData","Total_Event_Info", "eventID:numParticles:KE0:MTAS_E:Si_E:numMods", 32000 );
//	}
//	m_pTotalEventNTuple->Fill(m_TotalEventInfo.m_EventID, 
//                            m_TotalEventInfo.m_InitialNumberParticles,
//                            m_TotalEventInfo.mv_KE0.at(0) / keV,
//                            m_TotalEventInfo.m_TotalMTASEDep / keV,// MTAS Only
//                            m_TotalEventInfo.m_TotalTriggerEDep / keV,// Silicon Only
//                            m_TotalEventInfo.m_NumberModulesHit
//                            );
                            
  m_TotalEventInfo.clear();
}

void LENSLongAnalysisManager::nextEvent()
{
	// Not really needed?
}

void LENSLongAnalysisManager::endofEvent( G4int eventID )
{	
	if( ! m_Output10nsWaveform ) return;
	
	static G4float *s_NumberPE;
	static G4int failure = 3717;
//	static G4int s_NumberTimes = m_NumberTimes;
	
	if( m_p10nsWaveformPMTNTuple == 0 )
	{
//			m_p10nsWaveformPMTNTuple = new TNtuple("PMTHitWaveform","Optical Photons detected by PMT 10ns Waveform",
//				"eventID:moduleID:pmtID:numberTimes:times[numberTimes]:photonenergies[numberTimes]", 32000 );
//		m_p10nsWaveformPMTNTuple = new TNtuple("PMTHitWaveform","Optical Photons detected as PMT 10ns Waveform");
		m_p10nsWaveformPMTNTuple = new TTree("PMTHitWaveform","Optical Photons detected by PMT 10ns Waveform",32000);
		m_p10nsWaveformPMTNTuple->Branch("eventID", &failure, "eventID/I" );
		m_p10nsWaveformPMTNTuple->Branch("moduleID", &failure, "moduleID/I" );
		m_p10nsWaveformPMTNTuple->Branch("pmtID", &failure, "pmtID/I" );
//		m_p10nsWaveformPMTNTuple->Branch("times", &failure, "times[650]" );
		m_p10nsWaveformPMTNTuple->Branch("numberPEperTime", s_NumberPE, "numberPEperTime[650]" );
		
//		m_p10nsWaveformPMTNTuple = new TTree("PMTHitWaveform","Optical Photons detected by PMT 10ns Waveform",
//			"eventID/I:moduleID/I:pmtID/I:numberPE[650]/I", 32000 );
	}
	
	for( int i = 0; i < m_NumberModules; i++ )
	{
		for( int j = 0; j < m_aModuleInfo[i].m_NumberPMT; j++ )
		{
			if( m_aModuleInfo[i].m_PMTTimeInfo[j].m_SeesPhotons )
			{
//				for( int timeIndex = 0; timeIndex<m_NumberTimes; timeIndex++ )
				{
//					eventID, m_aModuleInfo[i].m_ModuleID, m_aModuleInfo[i].m_PMTTimeInfo[j].m_PMTID, 
//						 (m_aModuleInfo[i].m_PMTTimeInfo[j].m_Times[0])
					s_NumberPE = &( m_aModuleInfo[i].m_PMTTimeInfo[j].m_Times[0] );
					TBranch* eventBranch = m_p10nsWaveformPMTNTuple->FindBranch("eventID");
					TBranch* moduleBranch = m_p10nsWaveformPMTNTuple->FindBranch("moduleID");
					TBranch* pmtBranch = m_p10nsWaveformPMTNTuple->FindBranch("moduleID");
					TBranch* numberPEBranch = m_p10nsWaveformPMTNTuple->FindBranch("numberPEperTime");
					m_p10nsWaveformPMTNTuple->SetBranchAddress("eventID", (void*)&eventID, &eventBranch );
					m_p10nsWaveformPMTNTuple->SetBranchAddress("moduleID",(void*)&m_aModuleInfo[i].m_ModuleID, &moduleBranch );
					m_p10nsWaveformPMTNTuple->SetBranchAddress("pmtID",(void*) &m_aModuleInfo[i].m_PMTTimeInfo[j].m_PMTID, &pmtBranch );
					m_p10nsWaveformPMTNTuple->SetBranchAddress("numberPEperTime",(void*) m_aModuleInfo[i].m_PMTTimeInfo[j].m_Times, &numberPEBranch );
					m_p10nsWaveformPMTNTuple->Fill();
				}
			}
		}
		m_aModuleInfo[i].clear();
	}
	
	
		
}

//*******************************************************************************************
/*
// example of macro to read data from an ascii file and  
// create a root file with an histogram and a TTree.     
gROOT->Reset();
// the structure to hold the variables for the branch
struct staff_t {
Int_t cat;
Int_t division;
Int_t flag;
Int_t age;
Int_t service;
Int_t children;
Int_t grade;
Int_t step;
Int_t nation;
Int_t hrweek;
Int_t cost;
};
staff_t staff;
// open the ASCII file
FILE *fp = fopen("staff.dat","r");
char line[81];
// create a new ROOT file
TFile *f = new TFile("staff.root","RECREATE");
// create a TTree
TTree *tree = new TTree("tree","staff data from ascii file");
// create one branch with all information from the stucture
tree->Branch("staff",&staff.cat,"cat/I:division:flag:age:service:children:grade:step:nation:hrweek:cost");
// fill the tree from the values in ASCII file
while (fgets(&line,80,fp))
{
sscanf(&line[0],"%d%d%d%d",&staff.cat,&staff.division,&staff.flag,&staff.age);
sscanf(&line[13],"%d%d%d%d",&staff.service,&staff.children, &staff.grade,&staff.step);
sscanf(&line[24],"%d%d%d",&staff.nation,&staff.hrweek, &staff.cost);
tree->Fill();
}
// check what the tree looks like
tree->Print();
fclose(fp);
f->Write();
}*/
//*****************************************************************

