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
// $Id: A01HodoscopeHit.cc,v 1.5 2003/10/14 18:58:22 perl Exp $
// --------------------------------------------------------------
//
#include "LENSLongLiquidScintillatorHit.hh"

#include "G4VVisManager.hh"
#include "G4Circle.hh"
#include "G4Colour.hh"
#include "G4VisAttributes.hh"
#include "G4ios.hh"

#include <stdio.h>

using namespace CLHEP;

G4Allocator<LENSLongLiquidScintillatorOpticalPhotonHit> LENSLongLiquidScintillatorOpticalPhotonHitAllocator;
G4Allocator<LENSLongLiquidScintillatorGammaHit> LENSLongLiquidScintillatorGammaHitAllocator;

LENSLongLiquidScintillatorOpticalPhotonHit::LENSLongLiquidScintillatorOpticalPhotonHit( G4int moduleIndex, G4int PMTIndex, G4double eDep, G4double time )
{
	m_ModuleIndex = moduleIndex;
  m_PMTIndex = PMTIndex;
  m_Time.push_back( time );

	m_EnergyDeposit.push_back( eDep );
	
  m_pLogV = 0;
	m_pLiquidScintillatorOpticalPhotonOutputFile = 0;
}

LENSLongLiquidScintillatorOpticalPhotonHit::~LENSLongLiquidScintillatorOpticalPhotonHit()
{
}

LENSLongLiquidScintillatorOpticalPhotonHit::LENSLongLiquidScintillatorOpticalPhotonHit(const LENSLongLiquidScintillatorOpticalPhotonHit &right) : G4VHit()
{
	m_ModuleIndex = right.m_ModuleIndex;
  m_PMTIndex = right.m_PMTIndex;
	const G4int sizeRight = right.m_Time.size();
	if( sizeRight > 0 )
	{
		m_Time.reserve( sizeRight );
		for( int i = 0; i < sizeRight; i++ )
		{
			m_Time.push_back( right.m_Time[i] );
		}
		
		m_EnergyDeposit.reserve( sizeRight );
		for( int i = 0; i < sizeRight; i++ )
		{
			m_EnergyDeposit.push_back( right.m_EnergyDeposit[i] );
		}
	}
//  m_Time = right.m_Time;// Is this bad to do for std::vectors?
  m_pLogV = right.m_pLogV;
	m_pLiquidScintillatorOpticalPhotonOutputFile = right.m_pLiquidScintillatorOpticalPhotonOutputFile;
}


void LENSLongLiquidScintillatorOpticalPhotonHit::AddHit( G4double energyDep , G4double time )
{// This only adds to a time vector that has at least one element.
//	if( m_TimeOrderHits )
//	{
//		bool done = false;
//		std::vector<double>::iterator iter = m_Time.begin();
//		const double dTime = time/ns;
//		while( iter != m_Time.end() && ! done )
//		{
//			if( *iter > dTime ) done = true;
//			else iter++;
//		}
//		m_Time.insert( iter, dTime );
//	}
//	else
//	{
		m_Time.push_back( time );

		m_EnergyDeposit.push_back( energyDep );
//	}
}

const LENSLongLiquidScintillatorOpticalPhotonHit& LENSLongLiquidScintillatorOpticalPhotonHit::operator=(const LENSLongLiquidScintillatorOpticalPhotonHit &right)
{
	m_ModuleIndex = right.m_ModuleIndex;
  m_PMTIndex = right.m_PMTIndex;
	const G4int sizeRight = right.m_Time.size();
	if( sizeRight > 0 )
	{
		m_Time.clear();
		m_Time.reserve( sizeRight );
		for( int i = 0; i < sizeRight; i++ )
		{
			m_Time.push_back( right.m_Time[i] );
		}
		
		m_EnergyDeposit.clear();
		m_EnergyDeposit.reserve( sizeRight );
		for( int i = 0; i < sizeRight; i++ )
		{
			m_EnergyDeposit.push_back( right.m_EnergyDeposit[i] );
		}
	}
//  m_Time = right.m_Time;// Is this bad to do for std::vectors?
//	m_TimeOrderHits = right.m_TimeOrderHits;
  m_pLogV = right.m_pLogV;
	m_pLiquidScintillatorOpticalPhotonOutputFile = right.m_pLiquidScintillatorOpticalPhotonOutputFile;

  return *this;
}

int LENSLongLiquidScintillatorOpticalPhotonHit::operator==(const LENSLongLiquidScintillatorOpticalPhotonHit &/*right*/) const
{
  return 0;
}

void LENSLongLiquidScintillatorOpticalPhotonHit::Draw()
{
  G4VVisManager* pVVisManager = G4VVisManager::GetConcreteInstance();
  if( pVVisManager )
  {
//    G4Transform3D trans(m_Rot.inverse(),m_Pos);
    G4VisAttributes attribs;
    const G4VisAttributes* pVA = m_pLogV->GetVisAttributes();
    if(pVA) attribs = *pVA;
    G4Colour colour(0.,1.,1.);
    attribs.SetColour(colour);
    attribs.SetForceSolid(true);
//    pVVisManager->Draw( *m_pLogV, attribs, trans );
  }
}

void LENSLongLiquidScintillatorOpticalPhotonHit::Print()
{
	const int numberHits = m_Time.size();
// Unformatted output
//  G4cout << "PMTID=" << m_PMTIndex << " Coord=" << m_Pos/m << " Number Hits=" << numberHits << G4endl;
//	for( int i = 0; i < numberHits; i++ ) G4cout << "Time=" << m_Time[i] << "(sec)" << G4endl;
// Formatted output to the current open file, if no file is open I assume it will be output to the screen.
	if( m_pLiquidScintillatorOpticalPhotonOutputFile != 0 )
	{
		fprintf( m_pLiquidScintillatorOpticalPhotonOutputFile,"%9s%3d %7s%3d %6s%6d\n", "ModuleID=", m_ModuleIndex, " PMTID=", m_PMTIndex, " Hits=", numberHits );
//		for( int i = 0; i < numberHits; i++ ) fprintf( m_pLiquidScintillatorOutputFile, "%5s%13.6E\n", "Time=", m_Time[i] );
//		for( int i = 0; i < numberHits; i++ ) fprintf( m_pLiquidScintillatorOutputFile, "%2s%13.6E\n", "T=", m_Time[i] );
		for( int i = 0; i < numberHits; i++ ) fprintf( m_pLiquidScintillatorOpticalPhotonOutputFile, "%12.6E\n", m_Time[i] );
	}
	else
	{// Print to screen
		printf( "%9s%3d %7s%3d %6s%6d\n", "ModuleID=", m_ModuleIndex, " PMTID=", m_PMTIndex, " Hits=", numberHits );
//		printf( "%3d %6d\n", m_PMTIndex, numberHits );
//		for( int i = 0; i < numberHits; i++ ) printf( "%5s%13.6E\n", "Time=", m_Time[i] );
//		for( int i = 0; i < numberHits; i++ ) printf( "%2s%13.6E\n", "T=", m_Time[i] );
		for( int i = 0; i < numberHits; i++ ) printf( "%12.6E\n", m_Time[i] );
	}
}

//***********************************************************************************************************************
//***********************************************************************************************************************

// GAMMA HIT

LENSLongLiquidScintillatorGammaHit::LENSLongLiquidScintillatorGammaHit( const int moduleID, const G4double currentKE, 
												const G4double energyDeposit, const G4double time, const G4double distance, const G4int interactionID  )
{
  m_pLogV = 0;
	m_pOutputFile = 0;
	m_ModuleID = moduleID;
	m_CurrentEnergy = currentKE;
	m_EnergyDepositedThisStep = energyDeposit;
	m_TimeOfEnergyDeposit = time;
	m_DistanceThisStep = distance;
	m_InteractionID = interactionID;
}

LENSLongLiquidScintillatorGammaHit::~LENSLongLiquidScintillatorGammaHit()
{
}

LENSLongLiquidScintillatorGammaHit::LENSLongLiquidScintillatorGammaHit(const LENSLongLiquidScintillatorGammaHit &right) : G4VHit()
{
	m_pLogV = right.m_pLogV;
	m_pOutputFile = right.m_pOutputFile;
	m_ModuleID = right.m_ModuleID;
	m_CurrentEnergy = right.m_CurrentEnergy;
	m_EnergyDepositedThisStep = right.m_EnergyDepositedThisStep;
	m_TimeOfEnergyDeposit = right.m_TimeOfEnergyDeposit;
	m_DistanceThisStep = right.m_DistanceThisStep;
	m_InteractionID = right.m_InteractionID;
}

const LENSLongLiquidScintillatorGammaHit& LENSLongLiquidScintillatorGammaHit::operator=(const LENSLongLiquidScintillatorGammaHit &right)
{
  return *this;
}

int LENSLongLiquidScintillatorGammaHit::operator==(const LENSLongLiquidScintillatorGammaHit &/*right*/) const
{
  return 0;
}

void LENSLongLiquidScintillatorGammaHit::Draw()
{
  G4VVisManager* pVVisManager = G4VVisManager::GetConcreteInstance();
  if( pVVisManager )
  {
//    G4Transform3D trans(m_Rot.inverse(),m_Pos);
    G4VisAttributes attribs;
    const G4VisAttributes* pVA = m_pLogV->GetVisAttributes();
    if(pVA) attribs = *pVA;
    G4Color color(0.,0.,0.);
    attribs.SetColor(color);
    attribs.SetForceSolid(true);
//    pVVisManager->Draw( *m_pLogV, attribs, trans );
  }
}

void LENSLongLiquidScintillatorGammaHit::Print()
{
	if( m_pOutputFile != 0 )
	{
		fprintf( m_pOutputFile, "%10s%6i%17s%13.6f \n", "HitCellID=", m_ModuleID, " EnergyDeposited=", m_EnergyDepositedThisStep/keV );
	}
	else
	{
		printf( "%10s%6i%17s%13.6f \n", "HitCellID=", m_ModuleID, " EnergyDeposited=", m_EnergyDepositedThisStep/keV );
	}
}



