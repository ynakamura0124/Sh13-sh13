//
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************
//
//
// $Id: ExN02EventAction.cc,v 1.11 2006/06/29 17:48:05 gunter Exp $
// GEANT4 tag $Name: geant4-08-03-patch-01 $
//
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......


#include "Sh13EventAction.cc" 
// #include "ExN02EventAction.hh"
#include "ExN02TrackerSD.hh"
#include "ExN02RunAction.hh"

#include "G4Event.hh"
#include "G4EventManager.hh"
#include "G4TrajectoryContainer.hh"
#include "G4Trajectory.hh"
#include "G4ios.hh"
#include "G4SDManager.hh"
#include "G4RunManager.hh"

#include "TROOT.h"
#include "TH1.h"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

ExN02EventAction::ExN02EventAction(TTree *tree)
{
  output_tree = tree;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

ExN02EventAction::~ExN02EventAction()
{
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void ExN02EventAction::BeginOfEventAction(const G4Event *)
{
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void ExN02EventAction::EndOfEventAction(const G4Event *evt)
{
  // G4int event_id = evt->GetEventID();

  // get number of stored trajectories
  //
  G4TrajectoryContainer *trajectoryContainer = evt->GetTrajectoryContainer();
  G4int n_trajectories = 0;
  if (trajectoryContainer)
    n_trajectories = trajectoryContainer->entries();

  // periodic printing
  //
  // get information from a sensitivedetector volume.
  G4SDManager *SDManager = G4SDManager::GetSDMpointer();

  // get "Hit Collection of This Event"
  G4HCofThisEvent *HCTE = evt->GetHCofThisEvent();
  if (!HCTE)
    return;

  // get a hit collection
  static G4int idtra = -1;
  if (idtra < 0)
    idtra = SDManager->GetCollectionID("trackerCollection");
  // G4cout << idtra << G4endl;
  ExN02TrackerHitsCollection *hctra = (ExN02TrackerHitsCollection *)HCTE->GetHC(idtra);
  if (!hctra)
    return; // no hit collection

  // get hits
  G4int nhits = hctra->entries();
  // G4cout << nhits << G4endl;
  for (G4int idx = 0; idx < nhits; idx++)
  {
    G4double etotal = (*hctra)[idx]->GetEgamma();
    G4int geid = (*hctra)[idx]->GetGeID();
    // G4cout << etotal << G4endl;

    // fill a histogram
    G4RunManager *runManager = G4RunManager::GetRunManager();
    ExN02RunAction *runaction = (ExN02RunAction *)(runManager->GetUserRunAction());
    if (geid == 0)
    {
      TH1D *edep1 = runaction->GetHist1();
      edep1->Fill(etotal / CLHEP::keV);
    }
    if (geid == 1)
    {
      TH1D *edep2 = runaction->GetHist2();
      edep2->Fill(etotal / CLHEP::keV);
      // TH1D* hist_shower= (TH1D*)gROOT-> FindObject("shower");
    }
  }
}
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
