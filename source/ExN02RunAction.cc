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
// $Id: ExN02RunAction.cc,v 1.9 2006/06/29 17:48:16 gunter Exp $
// GEANT4 tag $Name: geant4-08-03-patch-01 $
//
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#include "ExN02RunAction.hh"
// #include "ExN02PrimaryGeneratorAction.hh"

#include "G4Run.hh"

#include "TROOT.h"
#include "TH1.h"
#include "TFile.h"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

ExN02RunAction::ExN02RunAction()
{
  // initialize root
  gROOT->Reset();

  // define histograms
  edep1 = new TH1D("edep1", "energy1", 1600, 0.0, 1600.);
  edep2 = new TH1D("edep2", "energy2", 1600, 0.0, 1600.);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

ExN02RunAction::~ExN02RunAction()
{
  delete edep1, edep2;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void ExN02RunAction::BeginOfRunAction(const G4Run *aRun)
{
  G4cout << "### Run " << aRun->GetRunID() << " start." << G4endl;

  edep1->Reset();
  edep2->Reset();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void ExN02RunAction::EndOfRunAction(const G4Run *)
{
  TFile *file = new TFile("Ge1.root",
                          "RECREATE", "Geant4 ROOT analysis");
  edep1->Write();
  edep2->Write();
  file->Close();
  delete file;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
