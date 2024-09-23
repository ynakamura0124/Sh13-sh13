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
//
 
#ifndef LENSLongEventAction_h
#define LENSLongEventAction_h 1

#include "G4UserEventAction.hh"

// forward declaration of NuSNS classes to use
class LENSLongAnalysisManager;
class LENSLongDetectorConstruction;

// Geant forward declarations
class G4Event;

//********************************************************************************


class LENSLongEventAction : public G4UserEventAction
{
  public:
    LENSLongEventAction();
	LENSLongEventAction( LENSLongDetectorConstruction* theDetectorConstruction );
   ~LENSLongEventAction();

  public:
    void BeginOfEventAction(const G4Event*);
    void EndOfEventAction(const G4Event*);
	
  protected:
    void StartUp();
		void CleanUp();
		G4double GetNonlinearEnergyDeposit( const G4double E, const G4double eDep );
	
	LENSLongDetectorConstruction* m_LENSLongDetectorConstruction;
	LENSLongAnalysisManager* m_AnalysisManager;
		
    G4int m_LongitudinalScintOpticalPhotonID;
    G4int m_LongitudinalScintGammaID;
	
};


#endif // LENSLongEventAction_h

    
