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



#ifndef LENSLongLiquidScintillatorHitsCollection_h
#define LENSLongLiquidScintillatorHitsCollection_h 1

#include "G4VHit.hh"
#include "G4THitsCollection.hh"
#include "G4Allocator.hh"
#include "G4ThreeVector.hh"
#include "G4LogicalVolume.hh"
#include "G4Transform3D.hh"
#include "G4RotationMatrix.hh"

#include <stdio.h>

class G4Run;

//**************************************************************************************
// BEGIN OPTICAL HIT

class LENSLongLiquidScintillatorOpticalPhotonHit : public G4VHit
{
public:
  LENSLongLiquidScintillatorOpticalPhotonHit();
  LENSLongLiquidScintillatorOpticalPhotonHit( G4int moduleIndex, G4int PMTIndex, G4double eDep, G4double time );
  ~LENSLongLiquidScintillatorOpticalPhotonHit();
  LENSLongLiquidScintillatorOpticalPhotonHit( const LENSLongLiquidScintillatorOpticalPhotonHit &right );
	
	const LENSLongLiquidScintillatorOpticalPhotonHit& operator=( const LENSLongLiquidScintillatorOpticalPhotonHit &right );
  int operator==( const LENSLongLiquidScintillatorOpticalPhotonHit &right ) const;

  inline void *operator new(size_t);
  inline void operator delete(void*aHit);

  void Draw();
  void Print();

  private:
    G4int m_ModuleIndex;
    G4int m_PMTIndex;
    std::vector<G4double> m_Time;
    std::vector<G4double> m_EnergyDeposit;
    const G4LogicalVolume* m_pLogV;
	  
		FILE* m_pLiquidScintillatorOpticalPhotonOutputFile;
		
  public:
    void AddHit( G4double eDep, G4double time );
		G4int GetNumberHits( void ) const { return m_Time.size(); }
		G4double GetTime( const G4int index ) const { return m_Time[index]; }// No saftey checking.
		G4double GetEnergyDeposit( const G4int index ) const { return m_EnergyDeposit[index]; }// No saftey checking.
		G4int GetModuleIndex() const { return m_ModuleIndex; }
		G4int GetPMTIndex() const { return m_PMTIndex; }

		void setOutputFile( FILE* newFile ) { m_pLiquidScintillatorOpticalPhotonOutputFile = newFile; }

    void SetLogV(G4LogicalVolume* val) { m_pLogV = val; }
    const G4LogicalVolume* GetLogV() const { return m_pLogV; }
};

typedef G4THitsCollection<LENSLongLiquidScintillatorOpticalPhotonHit> LENSLongLiquidScintillatorOpticalPhotonHitsCollection;

// This is declared in LENSLongLiquidScintillatorHit.cc
extern G4Allocator<LENSLongLiquidScintillatorOpticalPhotonHit> LENSLongLiquidScintillatorOpticalPhotonHitAllocator;

inline void* LENSLongLiquidScintillatorOpticalPhotonHit::operator new( size_t )
{
  void* aHit;
  aHit = (void*) LENSLongLiquidScintillatorOpticalPhotonHitAllocator.MallocSingle();
  return aHit;
}

inline void LENSLongLiquidScintillatorOpticalPhotonHit::operator delete(void*aHit)
{
  LENSLongLiquidScintillatorOpticalPhotonHitAllocator.FreeSingle( ( LENSLongLiquidScintillatorOpticalPhotonHit* ) aHit );
}



// END OPTICAL HIT
//**************************************************************************************
//**************************************************************************************
// BEGIN OTHER HIT



class LENSLongLiquidScintillatorGammaHit : public G4VHit
{
public:
  LENSLongLiquidScintillatorGammaHit();
  LENSLongLiquidScintillatorGammaHit( const G4int moduleID, const G4double currentKE, const G4double initialEnergyDeposit, const G4double time, 
																			const G4double distance, const G4int interactionID  );
  LENSLongLiquidScintillatorGammaHit( const LENSLongLiquidScintillatorGammaHit& right );
  ~LENSLongLiquidScintillatorGammaHit();
	
	const LENSLongLiquidScintillatorGammaHit& operator=( const LENSLongLiquidScintillatorGammaHit &right );
  int operator==( const LENSLongLiquidScintillatorGammaHit &right ) const;

  inline void *operator new( size_t );
  inline void operator delete( void*aHit );

  void Draw();
  void Print();

	const G4LogicalVolume* GetLogV() const { return m_pLogV; }

	G4int GetModuleID() const{ return m_ModuleID; }
	G4double GetCurrentKE() { return m_CurrentEnergy; }

	G4double GetEnergyDeposited(){ return m_EnergyDepositedThisStep; }
	G4double GetTimeOfEDep(){ return m_TimeOfEnergyDeposit; }
	G4double GetDistanceSinceLastInteraction(){ return m_DistanceThisStep; }
	G4int GetInteractionID(){ return m_InteractionID; }

	void setOutputFile( FILE* newFile ) { m_pOutputFile = newFile; }
	void SetLogV(G4LogicalVolume* val) { m_pLogV = val; }
	
private:
	G4LogicalVolume* m_pLogV;
	FILE* m_pOutputFile;

	G4int m_ModuleID;
	G4double m_CurrentEnergy;
	G4double m_EnergyDepositedThisStep;
	G4double m_TimeOfEnergyDeposit;
	G4double m_DistanceThisStep;
	G4int m_InteractionID;
};

typedef G4THitsCollection<LENSLongLiquidScintillatorGammaHit> LENSLongLiquidScintillatorGammaHitsCollection;

// This is declared in LENSLongLiquidScintillatorHit.cc
extern G4Allocator<LENSLongLiquidScintillatorGammaHit> LENSLongLiquidScintillatorGammaHitAllocator;

inline void* LENSLongLiquidScintillatorGammaHit::operator new( size_t )
{
  void* aHit;
  aHit = (void*) LENSLongLiquidScintillatorGammaHitAllocator.MallocSingle();
  return aHit;
}

inline void LENSLongLiquidScintillatorGammaHit::operator delete( void*aHit )
{
  LENSLongLiquidScintillatorGammaHitAllocator.FreeSingle( ( LENSLongLiquidScintillatorGammaHit* ) aHit );
}

// END OTHER HIT
//**************************************************************************************


#endif // LENSLongLiquidScintillatorHitsCollection_h





