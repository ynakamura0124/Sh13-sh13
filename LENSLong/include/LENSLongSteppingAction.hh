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
#ifndef LENSLongSteppingAction_h
#define LENSLongSteppingAction_h 1

#include "globals.hh"
#include "G4UserSteppingAction.hh"

// Forward Declarations
class LENSLongDetectorConstruction;

class LENSLongSteppingAction : public G4UserSteppingAction
{
public:
  LENSLongSteppingAction();
  LENSLongSteppingAction( LENSLongDetectorConstruction* theDetectorConstruction );
  ~LENSLongSteppingAction();
  virtual void UserSteppingAction( const G4Step* );

  void SetOneStepPrimaries(G4bool b){oneStepPrimaries=b;}
  G4bool GetOneStepPrimaries(){return oneStepPrimaries;}
  
private:
// Stops emission of several optical photons so that only the outside cone is visible.
  G4bool oneStepPrimaries;
	
  LENSLongDetectorConstruction* m_Detector;
};

#endif // LENSLongSteppingAction_h
