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
// $Id: ExN02TrackerHit.cc,v 1.10 2006/06/29 17:48:24 gunter Exp $
// GEANT4 tag $Name: geant4-08-03-patch-01 $
//
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#include "ExN02TrackerHit.hh"
#include "G4UnitsTable.hh"
#include "G4VVisManager.hh"
#include "G4Circle.hh"
#include "G4Colour.hh"
#include "G4VisAttributes.hh"

#include "G4ios.hh"

G4Allocator<ExN02TrackerHit> ExN02TrackerHitAllocator;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

ExN02TrackerHit::ExN02TrackerHit()
    : egamma(0.)
{
}

ExN02TrackerHit::ExN02TrackerHit(G4double aetotal)
    : egamma(aetotal)
{
}
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

ExN02TrackerHit::~ExN02TrackerHit() {}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

ExN02TrackerHit::ExN02TrackerHit(const ExN02TrackerHit &right)
    : G4VHit()
{
  // trackID   = right.trackID;
  // parentID  = right.parentID;
  // etotal      = right.etotal;
  // pos       = right.pos;
  GeID = right.GeID;
  egamma = right.egamma;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

const ExN02TrackerHit &ExN02TrackerHit::operator=(const ExN02TrackerHit &right)
{
  //  trackID = right.trackID;
  //  parentID = right.parentID;
  //  etotal = right.etotal;
  //  pos = right.pos;
  GeID = right.GeID;
  egamma = right.egamma;

  return *this;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4int ExN02TrackerHit::operator==(const ExN02TrackerHit &right) const
{
  return (this == &right) ? 1 : 0;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void ExN02TrackerHit::Draw()
{
  G4VVisManager *pVVisManager = G4VVisManager::GetConcreteInstance();
  if (pVVisManager)
  {
    // G4Circle circle(pos);
    // circle.SetScreenSize(2.);
    // circle.SetFillStyle(G4Circle::filled);
    // G4Colour colour(1., 0., 0.);
    // G4VisAttributes attribs(colour);
    // circle.SetVisAttributes(attribs);
    // pVVisManager->Draw(circle);
  }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void ExN02TrackerHit::Print()
{
  //  G4cout << "energy deposit: " << G4BestUnit(etotal, "Energy")
  //         << " position: " << G4BestUnit(pos, "Length")
  //         << " trackID: " << trackID
  //         << " parentID: " << parentID
  //         << G4endl;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
