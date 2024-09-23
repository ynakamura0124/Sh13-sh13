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

#ifndef LENSLongPrimaryGeneratorAction_h
#define LENSLongPrimaryGeneratorAction_h 1

#include "G4VUserPrimaryGeneratorAction.hh"

class G4ParticleGun;
class G4Event;
class MTASIsotope;
class MTASDecayManager;

class LENSLongDetectorConstruction;


class LENSLongPrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction
{
  public:
    LENSLongPrimaryGeneratorAction();
    LENSLongPrimaryGeneratorAction( LENSLongDetectorConstruction* theDetector );
    ~LENSLongPrimaryGeneratorAction();

  public:
    void GeneratePrimaries(G4Event* anEvent);

  private:
    G4ParticleGun* m_ParticleGun;
		LENSLongDetectorConstruction* m_LENSLongDetectorConstruction;
		
		G4bool m_bRandomInitialAim;

// Initial position options...
		G4bool m_RandomInitialCellLocation;
		G4bool m_RandomInitialOctantLocation;
		G4bool m_RandomInitialPlanarLocation;
		G4bool m_RandomBoxLocalizedLocation;
		G4int m_CellXTotalWidthToGenerateEventsIn;
		G4int m_CellYTotalWidthToGenerateEventsIn;// vertical
		G4int m_CellZTotalWidthToGenerateEventsIn;

// Particular types of events
		G4bool m_PointSource;
		G4bool m_bIndiumBetaDecay;
		G4bool m_BremstrahlungDecay;
		G4bool m_498GammaDecay;
		G4bool m_CascadeEvent;
		G4bool m_Cs137Event;
		G4bool m_Kr87Event;
		G4bool m_Mn54Event;
		G4bool m_Co60Event;
		G4bool m_Y88Event;
		G4bool m_Zn65Event;
		G4bool m_Xe137Event;
		G4bool m_I137Event;
		G4bool m_Y96Event;
		G4bool m_TwoGammaEvent;
		G4bool m_ArbitraryEnergySpectrumEvent;
		G4bool m_NeutronFlatEnergySpectrumEvent;
		G4bool m_FlatSingleParticleEnergySpectrumEvent;
		G4bool m_ArbitraryENSDFEvent;
		
    MTASDecayManager* m_MTASDecayManager;
//    MTASIsotope* m_isotopeFile;		
//    G4int m_NumberDecayManagers;// Used for multiple beta decays.
		
		G4double m_MinumumEnergyCutoff;
		
		G4ThreeVector m_Aim;
		G4double m_InitialMomentum;
		G4ThreeVector m_InitialPosition;

		std::vector<G4double> m_ElectronEnergySpectrum;// Energy
		std::vector<G4double> m_IntegratedElectronDecaySpectrum;// Integrated Cross section
		std::vector<G4double> m_ElectronDecaySpectrum;// Cross section

		std::vector<G4double> m_BSElectronEnergySpectrum;
		std::vector<G4double> m_IntegratedBSElectronDecaySpectrum;// BS electron Integrated Cross section
		std::vector<G4double> m_BSElectronDecaySpectrum;// Cross section

		G4bool m_BremstrahlungDataLoaded;
		std::vector<G4double> m_BremstrahlungEnergySpectrum;
		std::vector<G4double> m_BremstrahlungIntegratedSpectrum100;
		std::vector<G4double> m_BremstrahlungIntegratedSpectrum200;
		std::vector<G4double> m_BremstrahlungIntegratedSpectrum300;
		std::vector<G4double> m_BremstrahlungIntegratedSpectrum400;
		std::vector<G4double> m_BremstrahlungIntegratedSpectrum500;

		G4String m_ArbitraryEnergySpetrumFileName;
		G4double GetEnergyFromArbitrarySpectrum();
		void LoadArbitrarySpectrum();

		G4double GetIndiumBetaDecayEnergy();
		void LoadIndiumBetaDecaySpectrum();
		
		G4double GetBSIndiumBetaDecayEnergy();
		void LoadBSIndiumBetaDecaySpectrum();
		
		void GetBremstrahlungEnergy( G4double& energy, G4double& gammaEnergy );// The energy variable put in is changed to energy - gammaEnergy in this function
		void LoadBremsttrahlungDecaySpectrum();
		
		void SetUp( void );
		void  GenerateIsotropicDirectionDistribution( G4ThreeVector& direction, G4double theta0 );
};

#endif // LENSLongPrimaryGeneratorAction_h


