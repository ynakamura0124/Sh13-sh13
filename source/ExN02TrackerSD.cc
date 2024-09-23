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
// $Id: ExN02TrackerSD.cc,v 1.9 2006/06/29 17:48:27 gunter Exp $
// GEANT4 tag $Name: geant4-08-03-patch-01 $
//
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#include "ExN02TrackerSD.hh"
#include "ExN02TrackerHit.hh"
#include "G4HCofThisEvent.hh"
#include "G4Step.hh"
#include "G4ThreeVector.hh"
#include "G4SDManager.hh"
#include "G4ios.hh"
#include "G4UnitsTable.hh"
// test
#include "G4ParticleChangeForGamma.hh"
#include "G4ParticleDefinition.hh"
#include "G4DynamicParticle.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

ExN02TrackerSD::ExN02TrackerSD(G4String name)
	: G4VSensitiveDetector(name)
{
	G4String HCname;
	collectionName.insert(HCname = "trackerCollection");
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

ExN02TrackerSD::~ExN02TrackerSD() {}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void ExN02TrackerSD::Initialize(G4HCofThisEvent *HCE)
{
	trackerCollection = new ExN02TrackerHitsCollection(SensitiveDetectorName, collectionName[0]);
	static G4int HCID = -1;
	if (HCID < 0)
	{
		HCID = G4SDManager::GetSDMpointer()->GetCollectionID(collectionName[0]);
	}
	HCE->AddHitsCollection(HCID, trackerCollection);

	totalEnergy[0] = 0.0;
	totalEnergy[1] = 0.0;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4bool ExN02TrackerSD::ProcessHits(G4Step *aStep, G4TouchableHistory *)
{
	G4double edep = aStep->GetTotalEnergyDeposit();
	// G4double edep = - aStep->GetDeltaEnergy();
	// G4cout << "ProcessHits" << G4endl;
	G4String hvolume = aStep->GetPostStepPoint()->GetTouchableHandle()->GetVolume()->GetName(); //get hit volume\
 name
	G4int hnum;

	if (hvolume == "Sensitive")
	{
		hnum = aStep->GetPostStepPoint()->GetTouchableHandle()->GetCopyNumber(); // get id for hit volume

		if (edep == 0.)
			return false;

		// Ignore gamma's edep
		// G4String particle = aStep->GetTrack()->GetDefinition()->GetParticleName();
		// if(particle=="gamma") return false;

		// Deexcitation emit test
		// G4ParticleDefinition* particleDefinition = aStep->GetTrack()->GetDefinition();
		// G4String particleName = particleDefinition->GetParticleName();
		// if(particleName=="gamma")
		//{
		//   G4ParticleChangeForGamma* pChange = new G4ParticleChangeForGamma();
		//	G4ThreeVector direction = G4ThreeVector(0.,0.,1.);
		//	G4DynamicParticle* dParticle = new G4DynamicParticle(particleDefinition, direction, edep);
		//	G4cout << "test" << G4endl;
		//	pChange->AddSecondary(dParticle);
		//	return false;
		// }

		totalEnergy[hnum] += edep;
		// G4cout << "ThisEdep : " << G4BestUnit(edep, "Energy") << G4endl;

		/*
		  ExN02TrackerHit* newHit = new ExN02TrackerHit();
		  newHit->SetTrackID  (aStep->GetTrack()->GetTrackID());
		  newHit->SetParentID  (aStep->GetTrack()->GetParentID());

		  newHit->SetEdep     (edep);
		  newHit->SetPos      (aStep->GetPostStepPoint()->GetPosition());
		  trackerCollection->insert( newHit );
		*/
		// newHit->Print();
		// newHit->Draw();
		//////////////daiki /////////////
		// G4String v1 = aStep->GetPreStepPoint()->GetPhysicalVolume()->GetName();
		// G4String v2 = aStep->GetPostStepPoint()->GetPhysicalVolume()->GetName();
		// G4cout << "v1 = " << v1 << G4endl;
		// G4cout << "v2 = " << v2 << G4endl;
		//	if(v1 == "sensitive" && v2 == "sensitive"){
		//	char stri2[1000] = "";
		// sprintf(stri2, "%f",totalEnergy);
		// FILE *fp;
		// fp = fopen("egamma.dat","a");
		// fprintf(fp,"%s\n",stri2);
		// fclose(fp);
		// }
		//////////////end////////////////
	}
	return true;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void ExN02TrackerSD::EndOfEvent(G4HCofThisEvent *)
{

	// if (verboseLevel > 0)
	//	{
	// G4int NbHits = trackerCollection->entries();
	// for (G4int i=0;i<NbHits;i++) (*trackerCollection)[i]->Print();
	for (G4int i = 0; i < 2; i++)
	{
		if (totalEnergy[i] != 0.0)
		{
			//		  G4cout
			//	  << "TotalEnergyDeposit : "
			//	  << G4BestUnit(totalEnergy, "Energy")
			//		<< G4endl;

			ExN02TrackerHit *ahit = new ExN02TrackerHit(totalEnergy[i]);
			ahit->SetGeID(i);
			trackerCollection->insert(ahit);
		}
	}

	//}
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
