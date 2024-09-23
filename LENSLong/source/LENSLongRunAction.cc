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

#include "LENSLongRunAction.hh"

#include "G4Run.hh"
#include "G4VisManager.hh"


LENSLongRunAction::LENSLongRunAction()
{
}


LENSLongRunAction::~LENSLongRunAction()
{
}


void LENSLongRunAction::BeginOfRunAction(const G4Run* aRun)
{
  G4cout << "### Run " << aRun->GetRunID() << " start." << G4endl;

// There is probably a better way and place to do this.
  G4VVisManager* visMan = G4VVisManager::GetConcreteInstance();
//  G4VViewer* openGLViewer = ((G4VisManager*)visMan)->GetViewer("OGLIX");
  G4VViewer* openGLViewer = ((G4VisManager*)visMan)->GetCurrentViewer();// This is probably a bad bad thing.
//  G4cout << openGLViewer << G4endl;
  if( openGLViewer != 0 )
  {
  G4ViewParameters viewParams = openGLViewer->GetViewParameters();
  viewParams.SetBackgroundColour(G4Colour(1.0,1.0,1.0,1.0) );
  openGLViewer->SetViewParameters( viewParams );
  }
}

void LENSLongRunAction::EndOfRunAction(const G4Run*)
{
}





