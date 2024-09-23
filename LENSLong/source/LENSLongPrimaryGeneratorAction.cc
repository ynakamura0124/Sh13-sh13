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

#include "globals.hh"
#include "G4Event.hh"
#include "G4ParticleGun.hh"
#include "G4ParticleTable.hh"
#include "G4ParticleDefinition.hh"

#include "Randomize.hh"

#include "LENSLongPrimaryGeneratorAction.hh"
#include "LENSLongDetectorConstruction.hh"
#include "LENSLongLiquidScintillator.hh"
#include "LENSLongAnalysisManager.hh"

#include "MTASDecayManager.hh"

#include <stdio.h>

using namespace CLHEP;

LENSLongPrimaryGeneratorAction::LENSLongPrimaryGeneratorAction()
{
	m_LENSLongDetectorConstruction = 0;
	SetUp();
}

LENSLongPrimaryGeneratorAction::LENSLongPrimaryGeneratorAction( LENSLongDetectorConstruction* theDetectorConstruction )
{
	m_LENSLongDetectorConstruction = theDetectorConstruction;
//	theDetectorConstruction->GetBunkerCenterGlobalCoord( location );
//	theDetectorConstruction->GetBunkerLogicalVolume();

	SetUp();
}


void LENSLongPrimaryGeneratorAction::SetUp( void )
{
	m_BremstrahlungDataLoaded = false;
	m_bRandomInitialAim = true;

// How do you want the initial position be set? Choose here.
	m_RandomInitialCellLocation = false;  //true means it will start randomly in single cell, can't I just use a 1x1x1 box?
	m_RandomInitialOctantLocation = false;// occurs over one entire octant of the detector.
	m_RandomInitialPlanarLocation = false;// occrs over one entire single plane near the center vertically.
	m_RandomBoxLocalizedLocation = false;  //true means it will start randomly over a box of x by y by z cells defined below.
	
//	m_CellXTotalWidthToGenerateEventsIn = 14;
	m_CellXTotalWidthToGenerateEventsIn = 8;
	m_CellYTotalWidthToGenerateEventsIn = 8;// vertical
	m_CellZTotalWidthToGenerateEventsIn = 8;

	m_PointSource = true;// Can set energy of particle in macros with this.
	m_bIndiumBetaDecay = false;
	m_BremstrahlungDecay = false;
	m_498GammaDecay = false;
	m_CascadeEvent = false;
	
// Need to add 65Zn...
	m_Cs137Event = false;
	m_Kr87Event = false;
	m_Mn54Event = false;
	m_Co60Event = false;
	m_Y88Event = false;
	m_Zn65Event = false;// E.C. DOES NOT WORK YET! the E capture in the ENSDF is not implemented yet.
  m_Y96Event = false;
	m_Xe137Event = false;
	m_I137Event = false;
	m_TwoGammaEvent = false;
  m_ArbitraryEnergySpectrumEvent = false;
	m_NeutronFlatEnergySpectrumEvent = false;
  m_FlatSingleParticleEnergySpectrumEvent = false;
//  m_ArbitraryEnergySpetrumFileName = G4String("DefaultArbitraryEnergySpectrum.txt");
  m_ArbitraryEnergySpetrumFileName = G4String("I137NeutronEnergySpectrum.txt");
  
  m_ArbitraryENSDFEvent = false;
//  m_NumberDecayManagers = ( 4380.0 - 680.0 ) / 10.0

	m_MinumumEnergyCutoff = 1.0 * keV;// Set to 1.0 usually. Used to set the 450 keV minimum for the A1 events.

	m_InitialMomentum = 0.*MeV ;

  G4int n_particle = 1;
  m_ParticleGun = new G4ParticleGun( n_particle );// Can I set the particle gun in non World coordinates?

//  G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
//  G4String particleName;

// THis is bad to call now in vversion 4.8.???
//  m_ParticleGun->SetParticleDefinition(particleTable->FindParticle(particleName="mu+"));

	G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
	G4ParticleDefinition* neutronPD = particleTable->FindParticle("gamma");//neutron");
//	G4ParticleDefinition* electronPD = particleTable->FindParticle("e-");

	m_ParticleGun->SetParticleDefinition(neutronPD);
	m_ParticleGun->SetParticleEnergy(100.0 * keV);// This is kinetic energy
	
	m_ParticleGun->SetParticleMomentumDirection( G4ThreeVector(0.0,-1.0,0.0) );
	m_ParticleGun->SetParticlePosition( G4ThreeVector(0.0*m, 0.0*m, 0.0*m) );
	
	
	if( m_Cs137Event )
	{
		m_MTASDecayManager = new MTASDecayManager("Isotope137Cs.ens");
	}
	else if( m_Kr87Event )
	{
		m_MTASDecayManager = new MTASDecayManager("Isotope87Kr.ens");
	}
	else if( m_Mn54Event )
	{
		m_MTASDecayManager = new MTASDecayManager("Isotope54Mn.ens");
	}
	else if( m_Co60Event )
	{
//		m_MTASDecayManager = new MTASDecayManager("Isotope60Co.ens");
//		m_MTASDecayManager = new MTASDecayManager("ENSDF/60Co_1332L_1332g.ens");
		m_MTASDecayManager = new MTASDecayManager("ENSDF/60Co_2505L_1173_1332g.ens");
    
//		m_MTASDecayManager = new MTASDecayManager("Isotope60Co_2505Feeding_2505G_Only.ens");
//		m_MTASDecayManager = new MTASDecayManager("Isotope60Co_1332FeedingOnly.ens");
	}
	else if( m_Y96Event )
	{
//		m_MTASDecayManager = new MTASDecayManager("ENSDF/96Y_ENSDF.ens");
//		m_MTASDecayManager = new MTASDecayManager("ENSDF/96Y_0L_0g.ens");
//		m_MTASDecayManager = new MTASDecayManager("ENSDF/96Y_1581L_1581g.ens");// 1581 no gamma no conversion electron just the beta
//		m_MTASDecayManager = new MTASDecayManager("ENSDF/96Y_1751L_1751g.ens");
//		m_MTASDecayManager = new MTASDecayManager("ENSDF/96Y_2226L_2226g.ens");// 1581 no gamma no conversion electron just the beta
//		m_MTASDecayManager = new MTASDecayManager("ENSDF/96Y_4838L_4838g.ens");
//		m_MTASDecayManager = new MTASDecayManager("ENSDF/96Y_4838L_3087_1751g.ens");
//		m_MTASDecayManager = new MTASDecayManager("ENSDF/96Y_4838L_2940_1897g.ens");
//		m_MTASDecayManager = new MTASDecayManager("ENSDF/96Y_4838L_1626_3213g.ens");
//		m_MTASDecayManager = new MTASDecayManager("ENSDF/96Y_4838L_1626_1462_1751g.ens");
//		m_MTASDecayManager = new MTASDecayManager("ENSDF/96Y_4838L_1626_1315_1897g.ens");
//		m_MTASDecayManager = new MTASDecayManager("ENSDF/96Y_4838L_1626_1315_147_1751g.ens");

//		m_MTASDecayManager = new MTASDecayManager("ENSDF/96Y_4838L_1912_1175_1751g.ens");
//		m_MTASDecayManager = new MTASDecayManager("ENSDF/96Y_4838L_1912_700_2226g.ens");
//		m_MTASDecayManager = new MTASDecayManager("ENSDF/96Y_4838L_1912_700_475_1751g.ens");
//		m_MTASDecayManager = new MTASDecayManager("ENSDF/96Y_4838L_1912_700_329_147_1751g.ens");
//		m_MTASDecayManager = new MTASDecayManager("ENSDF/96Y_4838L_1912_700_329_1897g.ens");

//		m_MTASDecayManager = new MTASDecayManager("ENSDF/96Y_5552L_5552g.ens");
//		m_MTASDecayManager = new MTASDecayManager("ENSDF/96Y_5552L_3801_1751g.ens");

//		m_MTASDecayManager = new MTASDecayManager("ENSDF/96Y_FakeDecays/96Y_6975L_5224_1751g.ens");
		m_MTASDecayManager = new MTASDecayManager("ENSDF/96Y_FakeDecays/96Y_6925L_5174_1751g.ens");


  }
	else if( m_Zn65Event )
	{
//		m_MTASDecayManager = new MTASDecayManager("ENSDF/Isotope134Sb_X_ENSDF.ens");
//		m_MTASDecayManager = new MTASDecayManager("ENSDF/Isotope134Sb_ENSDF.ens");
		m_MTASDecayManager = new MTASDecayManager("ENSDF/Isotope134Sn_ENSDF.ens");

  
//    m_MTASDecayManager = new MTASDecayManager("ENSDF/Isotope500kBeta.ens");
//    m_MTASDecayManager = new MTASDecayManager("Isotope65Zn.ens");

//    m_MTASDecayManager = new MTASDecayManager("ENSDF/Isotope92Sr_beta.ens");
//    m_MTASDecayManager = new MTASDecayManager("ENSDF/Isotope92Sr_MultiBeta.ens");
//    m_MTASDecayManager = new MTASDecayManager("ENSDF/Isotope92Sr_beta_1645.ens");
//    m_MTASDecayManager = new MTASDecayManager("ENSDF/Isotope92Sr_beta_1695.ens");
//    m_MTASDecayManager = new MTASDecayManager("ENSDF/Isotope92Sr_beta_1745.ens");
//    m_MTASDecayManager = new MTASDecayManager("ENSDF/Isotope92Sr_beta_1795.ens");
//    m_MTASDecayManager = new MTASDecayManager("ENSDF/Isotope92Sr_beta_1845.ens");
//    m_MTASDecayManager = new MTASDecayManager("ENSDF/Isotope92Sr_beta_1895.ens");
//    m_MTASDecayManager = new MTASDecayManager("ENSDF/Isotope92Sr_beta_1945.ens");
//    m_MTASDecayManager = new MTASDecayManager("ENSDF/Isotope92Sr_beta_1995.ens");
//    m_MTASDecayManager = new MTASDecayManager("ENSDF/Isotope92Sr_beta_2045.ens");
//    m_MTASDecayManager = new MTASDecayManager("ENSDF/Isotope92Sr_beta_2095.ens");

//    m_MTASDecayManager = new MTASDecayManager("ENSDF/Isotope92Y_1384L_1384g.ens");
//    m_MTASDecayManager = new MTASDecayManager("ENSDF/Isotope92Y_1384L_491_651_242g.ens");
//    m_MTASDecayManager = new MTASDecayManager("ENSDF/Isotope92Y_1384L_953_431g.ens");
//    m_MTASDecayManager = new MTASDecayManager("ENSDF/Isotope92Y_1384L_1142_242g.ens");

//    m_MTASDecayManager = new MTASDecayManager("ENSDF/Isotope90Zr.ens");
//    m_MTASDecayManager = new MTASDecayManager("ENSDF/Isotope92Sr_0L_0G.ens");
//    m_MTASDecayManager = new MTASDecayManager("ENSDF/Isotope92Sr_815L_815g.ens");
//    m_MTASDecayManager = new MTASDecayManager("ENSDF/Isotope92Sr_1385L_570_815G.ens");
//    m_MTASDecayManager = new MTASDecayManager("ENSDF/Isotope92Sr_1385L_1385G.ens");
//    m_MTASDecayManager = new MTASDecayManager("ENSDF/Isotope92Sr_1778L_964_815G.ens");
//    m_MTASDecayManager = new MTASDecayManager("ENSDF/Isotope92Sr_1778L_394_570_815G.ens");
//    m_MTASDecayManager = new MTASDecayManager("ENSDF/Isotope92Sr_1778L_394_1385G.ens");
//    m_MTASDecayManager = new MTASDecayManager("ENSDF/Isotope92Sr_1778L_1778G.ens");
//    m_MTASDecayManager = new MTASDecayManager("ENSDF/Isotope92Sr_2527L_1712_815g.ens");
//    m_MTASDecayManager = new MTASDecayManager("ENSDF/Isotope92Sr_2784L_1969_815g.ens");
//    m_MTASDecayManager = new MTASDecayManager("ENSDF/Isotope92Sr_2784L_1399_1385g.ens");
//    m_MTASDecayManager = new MTASDecayManager("ENSDF/Isotope92Sr_2784L_1399_570_815g.ens");
//    m_MTASDecayManager = new MTASDecayManager("ENSDF/Isotope92Sr_2821L_2006_815g.ens");
//    m_MTASDecayManager = new MTASDecayManager("ENSDF/Isotope92Sr_2821L_2821g.ens");
//    m_MTASDecayManager = new MTASDecayManager("ENSDF/Isotope92Sr_2850L_1071_394_570_815g.ens");
//    m_MTASDecayManager = new MTASDecayManager("ENSDF/Isotope92Sr_2850L_1071_394_1385g.ens");
//    m_MTASDecayManager = new MTASDecayManager("ENSDF/Isotope92Sr_2850L_1071_964_815g.ens");
//    m_MTASDecayManager = new MTASDecayManager("ENSDF/Isotope92Sr_2850L_1465_1385g.ens");
//    m_MTASDecayManager = new MTASDecayManager("ENSDF/Isotope92Sr_4638L_2860_394_1385g.ens");
//    m_MTASDecayManager = new MTASDecayManager("ENSDF/Isotope92Sr_4638L_2860_964_815g.ens");
//    m_MTASDecayManager = new MTASDecayManager("ENSDF/Isotope92Sr_4638L_3823_815g.ens");
//    m_MTASDecayManager = new MTASDecayManager("ENSDF/Isotope92Sr_4638L_1817_2821g.ens");
//    m_MTASDecayManager = new MTASDecayManager("ENSDF/Isotope92Sr_4638L_4638g.ens");
//    m_MTASDecayManager = new MTASDecayManager("ENSDF/Isotope92Sr_5054L_5054Xg.ens");
//    m_MTASDecayManager = new MTASDecayManager("ENSDF/Isotope92Sr_5054L_2232_2821g.ens");
//    m_MTASDecayManager = new MTASDecayManager("ENSDF/Isotope92Sr_5054L_2913_1326_815g.ens");
//    m_MTASDecayManager = new MTASDecayManager("ENSDF/Isotope92Sr_5054L_2913_756_570_815g.ens");
//    m_MTASDecayManager = new MTASDecayManager("ENSDF/Isotope92Sr_5054L_4240_815g.ens");
//    m_MTASDecayManager = new MTASDecayManager("ENSDF/Isotope92Sr_5584L_4769_815g.ens");
//    m_MTASDecayManager = new MTASDecayManager("ENSDF/Isotope92Sr_5584L_5584g.ens");
//    m_MTASDecayManager = new MTASDecayManager("ENSDF/Isotope92Sr_5615L_4800_815g.ens");
//    m_MTASDecayManager = new MTASDecayManager("ENSDF/Isotope92Sr_6003L_6003g.ens");
//    m_MTASDecayManager = new MTASDecayManager("ENSDF/Isotope92Sr_6003L_5188_815g.ens");
//    m_MTASDecayManager = new MTASDecayManager("ENSDF/Isotope92Sr_6030L_6030g.ens");
//    m_MTASDecayManager = new MTASDecayManager("ENSDF/Isotope92Sr_6030L_5215_815g.ens");
//    m_MTASDecayManager = new MTASDecayManager("ENSDF/Isotope92Sr_6030L_4645_1385g.ens");
//    m_MTASDecayManager = new MTASDecayManager("ENSDF/Isotope92Sr_6030L_4645_570_815g.ens");
//    m_MTASDecayManager = new MTASDecayManager("ENSDF/Isotope92Sr_6030L_3502_1712_815g.ens");
//    m_MTASDecayManager = new MTASDecayManager("ENSDF/Isotope92Sr_6030L_3502_386_1326_815g.ens");
//    m_MTASDecayManager = new MTASDecayManager("ENSDF/Isotope92Sr_6030L_3502_386_756_570_815g.ens");
//    m_MTASDecayManager = new MTASDecayManager("ENSDF/Isotope92Sr_6030L_3502_386_756_1385g.ens");
//    m_MTASDecayManager = new MTASDecayManager("ENSDF/Isotope92Sr_6116L_6116g.ens");
//    m_MTASDecayManager = new MTASDecayManager("ENSDF/Isotope92Sr_6116L_5302_815g.ens");
//    m_MTASDecayManager = new MTASDecayManager("ENSDF/Isotope92Sr_6500L_5685_815G.ens");
//    m_MTASDecayManager = new MTASDecayManager("ENSDF/Isotope92Sr_6700L_5885_815G.ens");

//    m_MTASDecayManager = new MTASDecayManager("ENSDF/Isotope92Sr_6949L_1895_4240_815g.ens");
//    m_MTASDecayManager = new MTASDecayManager("ENSDF/Isotope92Sr_6949L_1895_2232_2821g.ens");
//    m_MTASDecayManager = new MTASDecayManager("ENSDF/Isotope92Sr_6949L_4809_1326_815g.ens");

//    m_MTASDecayManager = new MTASDecayManager("ENSDF/Isotope92Sr.ens");

//    m_MTASDecayManager = new MTASDecayManager("ENSDF/Isotope92Y.ens");

//    m_MTASDecayManager = new MTASDecayManager("ENSDF/Isotope91Sr_ENSDF.ens");

//    m_MTASDecayManager = new MTASDecayManager("ENSDF/146LA_ENSDF.ens");
//    m_MTASDecayManager = new MTASDecayManager("ENSDF/144LA_ENSDF.ens");// BROKEN at line 12
//    m_MTASDecayManager = new MTASDecayManager("ENSDF/143BA_ENSDF.ens");// CURRENT
//    m_MTASDecayManager = new MTASDecayManager("ENSDF/135I_ENSDF.ens");
//    m_MTASDecayManager = new MTASDecayManager("ENSDF/96Y_ENSDF.ens");
	}
	else if( m_Xe137Event )
	{
		m_MTASDecayManager = new MTASDecayManager("ENSDF/Isotope137Xe.ens");
//		m_MTASDecayManager = new MTASDecayManager("ENSDF/Isotope137Xe_2Level.ens");
//		m_MTASDecayManager = new MTASDecayManager("ENSDF/Isotope137Xe_0L_0g.ens");
//		m_MTASDecayManager = new MTASDecayManager("ENSDF/Isotope137Xe_455L_455g.ens");
//		m_MTASDecayManager = new MTASDecayManager("ENSDF/Isotope137Xe_848LOnly.ens");
//		m_MTASDecayManager = new MTASDecayManager("ENSDF/Isotope137Xe_848L_848g.ens");
//		m_MTASDecayManager = new MTASDecayManager("ENSDF/Isotope137Xe_848L_455+393g.ens");		
//		m_MTASDecayManager = new MTASDecayManager("ENSDF/Isotope137Xe_1575L_1575g.ens");
//		m_MTASDecayManager = new MTASDecayManager("ENSDF/Isotope137Xe_1575L_1119+455g.ens");
//		m_MTASDecayManager = new MTASDecayManager("ENSDF/Isotope137Xe_1783L_1783g.ens");
//		m_MTASDecayManager = new MTASDecayManager("ENSDF/Isotope137Xe_1783L_1328+455g.ens");
//		m_MTASDecayManager = new MTASDecayManager("ENSDF/Isotope137Xe_1916L_1916g.ens");
//		m_MTASDecayManager = new MTASDecayManager("ENSDF/Isotope137Xe_1916L_1067_849g.ens");
//		m_MTASDecayManager = new MTASDecayManager("ENSDF/Isotope137Xe_1916L_1067_393_455g.ens");
//		m_MTASDecayManager = new MTASDecayManager("ENSDF/Isotope137Xe_2068L_2068g.ens");
//		m_MTASDecayManager = new MTASDecayManager("ENSDF/Isotope137Xe_2068L_1613+455g.ens");
//		m_MTASDecayManager = new MTASDecayManager("ENSDF/Isotope137Xe_2849L_2394+455g.ens");
//		m_MTASDecayManager = new MTASDecayManager("ENSDF/Isotope137Xe_2849L_2000+849g.ens");
//		m_MTASDecayManager = new MTASDecayManager("ENSDF/Isotope137Xe_2849L_2000+455+393g.ens");
//		m_MTASDecayManager = new MTASDecayManager("ENSDF/Isotope137Xe_2850L_2850g.ens");
//		m_MTASDecayManager = new MTASDecayManager("ENSDF/Isotope137Xe_2850L_1273+1577g.ens");
//		m_MTASDecayManager = new MTASDecayManager("ENSDF/Isotope137Xe_2850L_482+2368g.ens");
//		m_MTASDecayManager = new MTASDecayManager("ENSDF/Isotope137Xe_2850L_482+1519+849g.ens");
//		m_MTASDecayManager = new MTASDecayManager("ENSDF/Isotope137Xe_2850L_482+1519+393+455g.ens");
//		m_MTASDecayManager = new MTASDecayManager("ENSDF/Isotope137Xe_2850L_982+298+1109+455g.ens");
//		m_MTASDecayManager = new MTASDecayManager("ENSDF/Isotope137Xe_2850L_982+298+1564g.ens");
//		m_MTASDecayManager = new MTASDecayManager("ENSDF/Isotope137Xe_2850L_934+1067+849g.ens");
//		m_MTASDecayManager = new MTASDecayManager("ENSDF/Isotope137Xe_2850L_934+1916g.ens");
//		m_MTASDecayManager = new MTASDecayManager("ENSDF/Isotope137Xe_2850L_934+1461+455.ens");
//		m_MTASDecayManager = new MTASDecayManager("ENSDF/Isotope137Xe_2850L_1067+1783g.ens");
//		m_MTASDecayManager = new MTASDecayManager("ENSDF/Isotope137Xe_2850L_1665+1185g.ens");
//		m_MTASDecayManager = new MTASDecayManager("ENSDF/Isotope137Xe_2850L_982+595+1273g.ens");
//		m_MTASDecayManager = new MTASDecayManager("ENSDF/Isotope137Xe_2850L_982+1868g.ens");
//		m_MTASDecayManager = new MTASDecayManager("ENSDF/Isotope137Xe_2850L_982+683+1185g.ens");
//		m_MTASDecayManager = new MTASDecayManager("ENSDF/Isotope137Xe_2850L_750+2099g.ens");
//		m_MTASDecayManager = new MTASDecayManager("ENSDF/Isotope137Xe_2850L_750+1250+849g.ens");
//		m_MTASDecayManager = new MTASDecayManager("ENSDF/Isotope137Xe_2850L_750+1250+393+455g.ens");

//		m_MTASDecayManager = new MTASDecayManager("ENSDF/Isotope137Xe_2945L_2490_455g.ens");
//		m_MTASDecayManager = new MTASDecayManager("ENSDF/Isotope137Xe_2945L_2096_849g.ens");
//		m_MTASDecayManager = new MTASDecayManager("ENSDF/Isotope137Xe_2945L_2096_393_455g.ens");

//		m_MTASDecayManager = new MTASDecayManager("ENSDF/Isotope137Xe_3037L_2582+455g.ens");
//		m_MTASDecayManager = new MTASDecayManager("ENSDF/Isotope137Xe_3037L_3037g.ens");
//		m_MTASDecayManager = new MTASDecayManager("ENSDF/Isotope137Xe_3104L_1236_298_1114_455g.ens");
//		m_MTASDecayManager = new MTASDecayManager("ENSDF/Isotope137Xe_3104L_2255+849.ens");
    
//		m_MTASDecayManager = new MTASDecayManager("ENSDF/Isotope137Xe_3159L_3159g.ens");
//		m_MTASDecayManager = new MTASDecayManager("ENSDF/Isotope137Xe_3159L_1975_1185g.ens");
//		m_MTASDecayManager = new MTASDecayManager("ENSDF/Isotope137Xe_3377L_1278+2099g.ens");
//		m_MTASDecayManager = new MTASDecayManager("ENSDF/Isotope137Xe_3377L_1278+1251+849g.ens");
//		m_MTASDecayManager = new MTASDecayManager("ENSDF/Isotope137Xe_3377L_2922+455g.ens");
//		m_MTASDecayManager = new MTASDecayManager("ENSDF/Isotope137Xe_3377L_1594+1783g.ens");
//		m_MTASDecayManager = new MTASDecayManager("ENSDF/Isotope137Xe_3377L_3377g.ens");
//		m_MTASDecayManager = new MTASDecayManager("ENSDF/Isotope137Xe_3584L_2739+849g.ens");
//		m_MTASDecayManager = new MTASDecayManager("ENSDF/Isotope137Xe_3584L_3584g.ens");
//		m_MTASDecayManager = new MTASDecayManager("ENSDF/Isotope137Xe_3377L_1594+1328+455g.ens");
//		m_MTASDecayManager = new MTASDecayManager("ENSDF/Isotope137Xe_3104L_2255+849g.ens");
//		m_MTASDecayManager = new MTASDecayManager("ENSDF/Isotope137Xe_3787L_2003+1783g.ens");

//		m_MTASDecayManager = new MTASDecayManager("ENSDF/Isotope137Xe_FAKE_455.ens");
	}
	else if( m_I137Event )
	{
//		m_MTASDecayManager = new MTASDecayManager("ENSDF/Isotope137I.ens");
//		m_MTASDecayManager = new MTASDecayManager("ENSDF/Isotope137I_0L_0g.ens");
//		m_MTASDecayManager = new MTASDecayManager("ENSDF/Isotope137I_601L_601g.ens");
//		m_MTASDecayManager = new MTASDecayManager("ENSDF/Isotope137I_1218L_1218g.ens");
//		m_MTASDecayManager = new MTASDecayManager("ENSDF/Isotope137I_1220L_1220g.ens");
//		m_MTASDecayManager = new MTASDecayManager("ENSDF/Isotope137I_1303L_1303g.ens");
//		m_MTASDecayManager = new MTASDecayManager("ENSDF/Isotope137I_1303L_702_601g.ens");
//		m_MTASDecayManager = new MTASDecayManager("ENSDF/Isotope137I_1303L_316_385_601g.ens");
//		m_MTASDecayManager = new MTASDecayManager("ENSDF/Isotope137I_1512L_1512g.ens");
//		m_MTASDecayManager = new MTASDecayManager("ENSDF/Isotope137I_1534L_1534g.ens");
//		m_MTASDecayManager = new MTASDecayManager("ENSDF/Isotope137I_1809L_1809g.ens");

		m_MTASDecayManager = new MTASDecayManager("ENSDF/Isotope137I_601L_601g.ens");

//		m_MTASDecayManager = new MTASDecayManager("ENSDF/Isotope137I_3023L_3023g.ens");
//		m_MTASDecayManager = new MTASDecayManager("ENSDF/Isotope137I_3023L_1805_1218g.ens");
//		m_MTASDecayManager = new MTASDecayManager("ENSDF/Isotope137I_3023L_1720_1303g.ens");
//		m_MTASDecayManager = new MTASDecayManager("ENSDF/Isotope137I_3730L_3730g.ens");

//		m_MTASDecayManager = new MTASDecayManager("ENSDF/Isotope137I_3795L_3795g.ens");
//		m_MTASDecayManager = new MTASDecayManager("ENSDF/Isotope137I_3795L_3194_601g.ens");
//		m_MTASDecayManager = new MTASDecayManager("ENSDF/Isotope137I_3795L_1922_1873g.ens");
//		m_MTASDecayManager = new MTASDecayManager("ENSDF/Isotope137I_3795L_1922_655_1218g.ens");
//		m_MTASDecayManager = new MTASDecayManager("ENSDF/Isotope137I_3795L_1922_571_1303g.ens");
//		m_MTASDecayManager = new MTASDecayManager("ENSDF/Isotope137I_3795L_1859_1936g.ens");
//		m_MTASDecayManager = new MTASDecayManager("ENSDF/Isotope137I_3795L_1859_1335_601g.ens");
//		m_MTASDecayManager = new MTASDecayManager("ENSDF/Isotope137I_3795L_1859_950_385_601g.ens");

//		m_MTASDecayManager = new MTASDecayManager("ENSDF/Isotope137I_3863L_3262_601g.ens");
//		m_MTASDecayManager = new MTASDecayManager("ENSDF/Isotope137I_3987L_2220_1165_601g.ens");
    
//		m_MTASDecayManager = new MTASDecayManager("ENSDF/Isotope137I_3996L_3996g.ens");
//		m_MTASDecayManager = new MTASDecayManager("ENSDF/Isotope137I_3996L_3395_601g.ens");
//		m_MTASDecayManager = new MTASDecayManager("ENSDF/Isotope137I_3996L_1367_877_532_1220g.ens");
//		m_MTASDecayManager = new MTASDecayManager("ENSDF/Isotope137I_3996L_1367_2630g.ens");
//		m_MTASDecayManager = new MTASDecayManager("ENSDF/Isotope137I_3996L_2123_1873g.ens");
//		m_MTASDecayManager = new MTASDecayManager("ENSDF/Isotope137I_3996L_2123_655_1218g.ens");
//		m_MTASDecayManager = new MTASDecayManager("ENSDF/Isotope137I_3996L_2123_571_1303g.ens");
//		m_MTASDecayManager = new MTASDecayManager("ENSDF/Isotope137I_3996L_2694_1303g.ens");
//		m_MTASDecayManager = new MTASDecayManager("ENSDF/Isotope137I_3996L_2005_773_1218g.ens");
//		m_MTASDecayManager = new MTASDecayManager("ENSDF/Isotope137I_3996L_2005_1991g.ens");
//		m_MTASDecayManager = new MTASDecayManager("ENSDF/Isotope137I_4026L_2808_1218g.ens");
//		m_MTASDecayManager = new MTASDecayManager("ENSDF/Isotope137I_4026L_2491_1534g.ens");
//		m_MTASDecayManager = new MTASDecayManager("ENSDF/Isotope137I_4026L_1788_2238g.ens");
//		m_MTASDecayManager = new MTASDecayManager("ENSDF/Isotope137I_4026L_1788_726_1512g.ens");
//		m_MTASDecayManager = new MTASDecayManager("ENSDF/Isotope137I_4026L_1396_2630g.ens");
//		m_MTASDecayManager = new MTASDecayManager("ENSDF/Isotope137I_4026L_1396_877_532_1220g.ens");
//		m_MTASDecayManager = new MTASDecayManager("ENSDF/Isotope137I_4026L_1396_877_532_1220g.ens");
//		m_MTASDecayManager = new MTASDecayManager("ENSDF/Isotope137I_4026L_1396_877_532_1220g.ens");
//		m_MTASDecayManager = new MTASDecayManager("ENSDF/Isotope137I_4026L_1396_877_532_1220g.ens");
//		m_MTASDecayManager = new MTASDecayManager("ENSDF/Isotope137I_4026L_1396_877_532_1220g.ens");

//		m_MTASDecayManager = new MTASDecayManager("ENSDF/Isotope137I_4029L_2093_950_385_601g.ens");
//		m_MTASDecayManager = new MTASDecayManager("ENSDF/Isotope137I_4029L_2093_268_1067_601g.ens");
//		m_MTASDecayManager = new MTASDecayManager("ENSDF/Isotope137I_4029L_2093_268_682_385_601g.ens");
//		m_MTASDecayManager = new MTASDecayManager("ENSDF/Isotope137I_4029L_2093_1335_601g.ens");
//		m_MTASDecayManager = new MTASDecayManager("ENSDF/Isotope137I_4029L_3428_601g.ens");

//		m_MTASDecayManager = new MTASDecayManager("ENSDF/Isotope137I_4161L_4161g.ens");
//		m_MTASDecayManager = new MTASDecayManager("ENSDF/Isotope137I_4161L_2943_1218g.ens");
//		m_MTASDecayManager = new MTASDecayManager("ENSDF/Isotope137I_4277L_4277g.ens");

    
	}
	else if( m_TwoGammaEvent )
	{
    G4cout << "Generating Two Gamma Event " << G4endl;
	}
	else if( m_NeutronFlatEnergySpectrumEvent )
	{
    G4cout << "Generating Neutrons with beta Event " << G4endl;
    m_MTASDecayManager = new MTASDecayManager("ENSDF/Isotope1314kBeta.ens");
//    m_MTASDecayManager = new MTASDecayManager("ENSDF/Isotope450kBeta.ens");
	}
  else if( m_FlatSingleParticleEnergySpectrumEvent )
  {
    G4cout << "Generating a single particle defined in the macro file with a flat energy spectrum" << G4endl;
//    G4cout << "spectrum defined in the file: " << m_ArbitraryEnergySpetrumFileName << "." << G4endl;
  }
  else if( m_ArbitraryEnergySpectrumEvent )
  {
    G4cout << "Generating a particle defined in the macro file with an energy from the energy " << G4endl;
    G4cout << "spectrum defined in the file: " << m_ArbitraryEnergySpetrumFileName << "." << G4endl;
  }
  else if( m_ArbitraryENSDFEvent )
  {
    G4cout << "Generating a single particle defined in the ENSDF file" << G4endl;
//    G4cout << "spectrum defined in the file: " << m_ArbitraryEnergySpetrumFileName << "." << G4endl;
		m_MTASDecayManager = new MTASDecayManager("ENSDF/Betas137I.ens");
    G4cout << "Done Generating a single particle defined in the ENSDF file" << G4endl;
  }
  
	
//	HepRandom::getTheEngine()
//	HepRandom::setTheSeed(57);
}


LENSLongPrimaryGeneratorAction::~LENSLongPrimaryGeneratorAction()
{
  delete m_ParticleGun;
	delete m_MTASDecayManager;
}

void LENSLongPrimaryGeneratorAction::GeneratePrimaries( G4Event* anEvent )
{// In World coordinates
	if( m_PointSource )
	{
//		const G4double PI = 4.0 * atan(1.0);// Not sure where pi is defined in GEANT, but it is probably in there somewhere
//		const G4double maxPhi = 2.0 * PI;
//		const G4double minPhi = 0.0 * PI;
//		const G4double randomPhi =  G4UniformRand() * ( maxPhi - minPhi ) + minPhi;
//		const G4double cosP = cos(randomPhi);
//		const G4double sinP = sin(randomPhi);

//		const G4double angleFromBeamline = 90.0 * deg;// SET THIS
//		const G4double cosT = cos(angleFromBeamline);
//		const G4double sinT = sin(angleFromBeamline);

//		G4ThreeVector aim( sinT * cosP, sinT * sinP, -cosT );// The minus sign is due to the direction of the z axis relative to the x axis.
		G4ThreeVector aim( 0.0, -1.0, 0.0 );// The minus sign is due to the direction of the z axis relative to the x axis.
		
		if( m_bRandomInitialAim )
		{
//			G4double cosTheta = ( G4UniformRand() - 0.5 ) * 2.0;// -1. < cosTheta < 1.
      const G4double A = -1.0;
      const G4double B = 0.0;
			G4double cosTheta = G4UniformRand() * ( B - A ) + A;// A < cosTheta < B
			G4double sinTheta = sqrt( 1.0 - cosTheta * cosTheta );// 0. < sinTheta < 1.
			G4double phi = G4UniformRand() * 8.0 * atan(1.0);// 0. < phi < 2*pi

			G4double randomXaim = cos(phi) * sinTheta;
			G4double randomYaim = sin(phi) * sinTheta;
			G4double randomZaim = cosTheta;
			aim = G4ThreeVector( randomXaim, randomYaim, randomZaim );
		}
	  
//		G4ThreeVector startPos(0.0,0.0,25.0 *cm);// Defaults to (0,0,0)
		G4ThreeVector startPos(0.0,0.0,0.0 *cm);// Defaults to (0,0,0)
//		m_LENSLongDetectorConstruction->GetBeamPipeLaunchCoordinateGlobal( startPos );
		
		//startPos[1] = 7.0 * 2.54 * cm;
		//G4cout <<  startPos << G4endl;
		//G4cout <<  aim << G4endl;

//		G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
//		G4ParticleDefinition* gammaPD = particleTable->FindParticle("gamma");
		
//		m_ParticleGun->SetParticleDefinition(gammaPD);
		m_ParticleGun->SetParticlePosition( startPos );
		m_ParticleGun->SetParticleMomentumDirection( aim );
// Sets kinetic energy, not total energy. This would overwrite anything set in a macro files, so set it in the constructor instead
//		m_ParticleGun->SetParticleEnergy( 455.5 * keV );
		m_ParticleGun->GeneratePrimaryVertex(anEvent);
/*
		if( m_bRandomInitialAim )
		{
			G4double cosTheta = ( G4UniformRand() - 0.5 ) * 2.0;// -1. < cosTheta < 1.
			G4double sinTheta = sqrt( 1.0 - cosTheta * cosTheta );// 0. < sinTheta < 1.
			G4double phi = G4UniformRand() * 8.0 * atan(1.0);// 0. < phi < 2*pi

			G4double randomXaim = cos(phi) * sinTheta;
			G4double randomYaim = sin(phi) * sinTheta;
			G4double randomZaim = cosTheta;
			aim = G4ThreeVector( randomXaim, randomYaim, randomZaim );
		}
	  
		m_ParticleGun->SetParticleMomentumDirection( aim );
// Sets kinetic energy, not total energy. This would overwrite anything set in a macro files, so set it in the constructor instead
		m_ParticleGun->SetParticleEnergy( 455.5 * keV );
		m_ParticleGun->GeneratePrimaryVertex(anEvent);
*/
		
		return;
	}
	else if( m_Cs137Event )
	{
		G4ThreeVector startPos( 0.0*cm, 0.0*cm, 0.0*cm );
		m_LENSLongDetectorConstruction->GetBeamPipeLaunchCoordinateGlobal( startPos );
	
		//Direction
		G4ThreeVector direction( 1.0, 0.0, 0.0 );
		GenerateIsotropicDirectionDistribution( direction,0.0);

//*******************for decay simulations:****************************************
		vector<double> particlesEnergy;
		vector< G4ParticleDefinition*> particlesType;
		m_MTASDecayManager->DecaySimulation(&particlesEnergy,&particlesType);

		unsigned int nrOfParticlesInEvent = particlesType.size();
		int electronCount = 0;
		for(unsigned int i=0; i<nrOfParticlesInEvent;i++)
		{
			// the first electron
//			if( electronCount == 1 ) continue;// This is an Cs137 decay event, not an Ba137 IT decay
			if( electronCount == 0 && particlesType[i]->GetParticleName() == "e-" )
			{
				electronCount++;
				continue;// Ba137 IT decays, not Cs137 decay
			}
			
			GenerateIsotropicDirectionDistribution( direction, 0.0 );
			m_ParticleGun->SetParticleMomentumDirection( direction );
			m_ParticleGun->SetParticleDefinition(particlesType[i]);
			m_ParticleGun->SetParticlePosition( startPos );
			m_ParticleGun->SetParticleEnergy(particlesEnergy[i]*keV);
			m_ParticleGun->GeneratePrimaryVertex(anEvent);
			//	G4cout <<  particlesType[i]->GetParticleName() << ", " << particlesEnergy[i] / keV << " keV" << G4endl;
		}
		
		return;
	}
	else if( m_Kr87Event )
	{// A whole bunch o stuff...
		G4ThreeVector startPos( 0.0*cm, 0.0*cm, 0.0*cm );
		m_LENSLongDetectorConstruction->GetBeamPipeLaunchCoordinateGlobal( startPos );
	
		//Direction
		G4ThreeVector direction( 1.0, 0.0, 0.0 );
		GenerateIsotropicDirectionDistribution( direction,0.0);

//*******************for decay simulations:****************************************
		vector<double> particlesEnergy;
		vector< G4ParticleDefinition*> particlesType;
		m_MTASDecayManager->DecaySimulation(&particlesEnergy,&particlesType);

		unsigned int nrOfParticlesInEvent =particlesType.size();
		for(unsigned int i=0; i<nrOfParticlesInEvent;i++)
		{
				m_ParticleGun->SetParticlePosition( startPos );
				GenerateIsotropicDirectionDistribution( direction, 0.0 );
				m_ParticleGun->SetParticleMomentumDirection( direction );
				m_ParticleGun->SetParticleEnergy(particlesEnergy[i]*keV);
				m_ParticleGun->SetParticleDefinition(particlesType[i]);
				m_ParticleGun->GeneratePrimaryVertex(anEvent);
//				G4cout <<  particlesType[i]->GetParticleName() << ", " << particlesEnergy[i] << " keV" << G4endl;
		}
		
		return;
	}
	else if( m_Mn54Event )
	{// 834.85 keV gamma
		G4ThreeVector startPos( 0.0*cm, 0.0*cm, 0.0*cm );
		m_LENSLongDetectorConstruction->GetBeamPipeLaunchCoordinateGlobal( startPos );
	
		//Direction
		G4ThreeVector direction( 1.0, 0.0, 0.0 );
		GenerateIsotropicDirectionDistribution( direction,0.0);

//*******************for decay simulations:****************************************
		vector<double> particlesEnergy;
		vector< G4ParticleDefinition*> particlesType;
		m_MTASDecayManager->DecaySimulation(&particlesEnergy,&particlesType);

		unsigned int nrOfParticlesInEvent =particlesType.size();
		for(unsigned int i=0; i<nrOfParticlesInEvent;i++)
		{
				m_ParticleGun->SetParticlePosition( startPos );
				GenerateIsotropicDirectionDistribution( direction, 0.0 );
				m_ParticleGun->SetParticleMomentumDirection( direction );
				m_ParticleGun->SetParticleEnergy(particlesEnergy[i]*keV);
				m_ParticleGun->SetParticleDefinition(particlesType[i]);
				m_ParticleGun->GeneratePrimaryVertex(anEvent);
//				G4cout <<  particlesType[i]->GetParticleName() << ", " << particlesEnergy[i] << " keV" << G4endl;
		}
		
		return;
	}
	else if( m_Co60Event )
	{
		G4ThreeVector startPos( 0.0*cm, 0.0*cm, 0.0*cm );
		m_LENSLongDetectorConstruction->GetBeamPipeLaunchCoordinateGlobal( startPos );
	
		//Direction
		G4ThreeVector direction( 1.0, 0.0, 0.0 );
		GenerateIsotropicDirectionDistribution( direction,0.0);

//*******************for decay simulations:****************************************
		vector<double> particlesEnergy;
		vector< G4ParticleDefinition*> particlesType;
		m_MTASDecayManager->DecaySimulation(&particlesEnergy,&particlesType);

		unsigned int nrOfParticlesInEvent = particlesType.size();
		for(unsigned int i=0; i<nrOfParticlesInEvent;i++)
		{
				m_ParticleGun->SetParticlePosition( startPos );
				GenerateIsotropicDirectionDistribution( direction, 0.0 );
				m_ParticleGun->SetParticleMomentumDirection( direction );
				m_ParticleGun->SetParticleEnergy(particlesEnergy[i]*keV);
				m_ParticleGun->SetParticleDefinition(particlesType[i]);
				m_ParticleGun->GeneratePrimaryVertex(anEvent);
//				G4cout <<  particlesType[i]->GetParticleName() << ", " << particlesEnergy[i] << " keV" << G4endl;
		}
		
		return;
		
// 1,173.2 keV and 1,332.5 keV gammas: 99.88% both .12% 1.3325 MeV only OR equivalently: 1.1732MeV 99.85% and 1.332MeV 99.983% of the time.
// True if launching along face of detector, otherwise launched perp to sharp edge of the detector.
//		const G4bool alongFlatEdge = true;//m_LENSLongDetectorConstruction->GetLENSLongDetector()->IsSourceAlongFace();
//		const G4double PI = 4.0 * atan(1.0);// Not sure where pi is defined in GEANT, but it is probably in there somewhere
//		
//// Random aim into all 4pi, might change to do into 2pi to simulate less events but not a real speed issue since most just hit nothing.
//		const G4double cosTheta = ( G4UniformRand() - 0.5 ) * 2.0;// -1. < cosTheta < 1.
//		const G4double sinTheta = sqrt( 1.0 - cosTheta * cosTheta );// 0. < sinTheta < 1.
//		const G4double phi = G4UniformRand() * 2.0 * PI;// 0. < phi < 2*pi
//
//		const G4double randomXaim = cos(phi) * sinTheta;
//		const G4double randomYaim = sin(phi) * sinTheta;
//		const G4double randomZaim = cosTheta;
//		G4ThreeVector aim = G4ThreeVector( randomXaim, randomYaim, randomZaim );
//	  
//	// Generate over 2mm diameter, 1mm radius, circle. This means use r = d * sqrt(random1) and theta = 2 pi random2; 
//	// NOT r = Random1, theta = 2 pi random2, this produces too many near the center.
//		const G4double sourceRadius = m_LENSLongDetectorConstruction->GetLENSLongDetector()->GetSourceRadius();
//		const G4double radius = sourceRadius * sqrt( G4UniformRand() );
//		const G4double angle = 2.0 * PI * G4UniformRand();// Generate over 2mm diameter, 1mm radius, circle.
//		
//		G4ThreeVector startPos;
//		m_LENSLongDetectorConstruction->GetBeamPipeLaunchCoordinateGlobal( startPos );// Fixed accessor function.
////		startPos += G4ThreeVector( 0.0, 4.0 * 2.54 * cm, 0.0 );
//		
//		if( alongFlatEdge ) startPos += G4ThreeVector( radius * cos(angle), 0.0, radius * sin(angle) );
//		else startPos += G4ThreeVector( 0.0, radius * cos(angle), radius * sin(angle) );
//		//G4cout <<  startPos << G4endl;
//		//G4cout <<  aim << G4endl;
//
//		G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
//		G4ParticleDefinition* gammaPD = particleTable->FindParticle("gamma");
//		
//		m_ParticleGun->SetParticleDefinition(gammaPD);
//		m_ParticleGun->SetParticlePosition( startPos );
//		m_ParticleGun->SetParticleMomentumDirection( aim );
//
//// Sets kinetic energy, not total energy. This would overwrite anything set in a macro files, so set it in the constructor instead
//		m_ParticleGun->SetParticleEnergy( 1332.0 * keV );
//	// Generate the 1332 gamma
//		m_ParticleGun->GeneratePrimaryVertex(anEvent);
//		
//		
//		const G4double fractionBoth = 1.0 - 0.0012;
//		
//		if( G4UniformRand() < fractionBoth )
//		{// OK to reuse variables inside brackets, it knows to use these instead of the ones above.
//// Random aim into all 4pi, might change to do into 2pi to simulate less events but not a real speed issue since most just hit nothing.
//			const G4double cosTheta = ( G4UniformRand() - 0.5 ) * 2.0;// -1. < cosTheta < 1.
//			const G4double sinTheta = sqrt( 1.0 - cosTheta * cosTheta );// 0. < sinTheta < 1.
//			const G4double phi = G4UniformRand() * 2.0 * PI;// 0. < phi < 2*pi
//
//			const G4double randomXaim = cos(phi) * sinTheta;
//			const G4double randomYaim = sin(phi) * sinTheta;
//			const G4double randomZaim = cosTheta;
//			G4ThreeVector aim = G4ThreeVector( randomXaim, randomYaim, randomZaim );
//			// Only have to reset aim, the start position is already set as is the particle type.
//			m_ParticleGun->SetParticleMomentumDirection( aim );
//			m_ParticleGun->SetParticleEnergy( 1173.2 * keV );
//			
//	// Generate the 1173 gamma
//			m_ParticleGun->GeneratePrimaryVertex(anEvent);
//		}
//
//		
//		return;
	}
	else if( m_Y88Event )
	{// 1.8361MeV 99.2% and { 1382.2MeV 0.021% or 898keV 93.7% or 850.6keV .065% }. % are total percentages so they need to be scaled
	// OR 2.734MeV .71% and { 850.6 .065% } Is this most of the 850keV's % or almost none of it?
	}
	else if( m_Y96Event )
	{
		G4ThreeVector startPos( 0.0*cm, 0.0*cm, 0.0*cm );
		//m_LENSLongDetectorConstruction->GetBeamPipeLaunchCoordinateGlobal( startPos );
	
		//Direction
		G4ThreeVector direction( 1.0, 0.0, 0.0 );
		GenerateIsotropicDirectionDistribution( direction,0.0);

//*******************for decay simulations:****************************************
		vector<double> particlesEnergy;
		vector< G4ParticleDefinition*> particlesType;
		m_MTASDecayManager->DecaySimulation(&particlesEnergy,&particlesType);
    
//    G4cout <<  particlesType[0]->GetParticleName() << ", " << particlesEnergy[0] << " keV" << G4endl;

    unsigned int nrOfParticlesInEvent = particlesType.size();
		for(unsigned int i=0; i<nrOfParticlesInEvent;i++)
		{
//      G4cout <<  particlesType[i]->GetParticleName() << ", " << particlesEnergy[i] << " keV" << " from: " << startPos / mm << " mm" << G4endl;

      m_ParticleGun->SetParticlePosition( startPos );
      GenerateIsotropicDirectionDistribution( direction, 0.0 );

      m_ParticleGun->SetParticleMomentumDirection( direction );
      m_ParticleGun->SetParticleEnergy( particlesEnergy[i]*keV );
      m_ParticleGun->SetParticleDefinition( particlesType[i] );
      m_ParticleGun->GeneratePrimaryVertex( anEvent );
		}
    
// Used to generate 1581 level in 96Y
//    GenerateIsotropicDirectionDistribution( direction, 0.0 );
//
//    m_ParticleGun->SetParticleMomentumDirection( direction );
//    m_ParticleGun->SetParticleEnergy( 1581.0*keV );
////    m_ParticleGun->SetParticleDefinition( particlesType[i] ); // should already be an electron from beta above
//    m_ParticleGun->GeneratePrimaryVertex( anEvent );
		
		return;
  }
	else if( m_Zn65Event )
	{
		G4ThreeVector startPos( 0.0*cm, 0.0*cm, 0.0*cm );
		//m_LENSLongDetectorConstruction->GetBeamPipeLaunchCoordinateGlobal( startPos );
	
		//Direction
		G4ThreeVector direction( 1.0, 0.0, 0.0 );
		GenerateIsotropicDirectionDistribution( direction,0.0);

//*******************for decay simulations:****************************************
		vector<double> particlesEnergy;
		vector< G4ParticleDefinition*> particlesType;
		m_MTASDecayManager->DecaySimulation(&particlesEnergy,&particlesType);
    
//    G4cout <<  particlesType[0]->GetParticleName() << ", " << particlesEnergy[0] << " keV" << G4endl;

    unsigned int nrOfParticlesInEvent = particlesType.size();
		for(unsigned int i=0; i<nrOfParticlesInEvent;i++)
		{
//      G4cout <<  particlesType[i]->GetParticleName() << ", " << particlesEnergy[i] << " keV" << " from: " << startPos / mm << " mm" << G4endl;

      m_ParticleGun->SetParticlePosition( startPos );
      GenerateIsotropicDirectionDistribution( direction, 0.0 );

      m_ParticleGun->SetParticleMomentumDirection( direction );
      m_ParticleGun->SetParticleEnergy( particlesEnergy[i]*keV );
      m_ParticleGun->SetParticleDefinition( particlesType[i] );
      m_ParticleGun->GeneratePrimaryVertex( anEvent );
		}
    
// Used to generate 1581 level in 69Y
//    GenerateIsotropicDirectionDistribution( direction, 0.0 );
//
//    m_ParticleGun->SetParticleMomentumDirection( direction );
//    m_ParticleGun->SetParticleEnergy( 1581.0*keV );
////    m_ParticleGun->SetParticleDefinition( particlesType[i] ); // should already be an electron
//    m_ParticleGun->GeneratePrimaryVertex( anEvent );
		
		return;
	}
	else if( m_Xe137Event )
	{
		G4ThreeVector startPos( 0.0*cm, 0.0*cm, 0.0*cm );
		m_LENSLongDetectorConstruction->GetBeamPipeLaunchCoordinateGlobal( startPos );
	
		//Direction
		G4ThreeVector direction( 1.0, 0.0, 0.0 );
		GenerateIsotropicDirectionDistribution( direction,0.0);

//*******************for decay simulations:****************************************
		vector<double> particlesEnergy;
		vector< G4ParticleDefinition*> particlesType;
		m_MTASDecayManager->DecaySimulation(&particlesEnergy,&particlesType);

//		static int count = 0;
//		count++;
		
		unsigned int nrOfParticlesInEvent = particlesType.size();
//		G4cout << count << ": " <<  nrOfParticlesInEvent << G4endl;
		for(unsigned int i=0; i<nrOfParticlesInEvent;i++)
		{
				m_ParticleGun->SetParticlePosition( startPos );
				GenerateIsotropicDirectionDistribution( direction, 0.0 );
				m_ParticleGun->SetParticleMomentumDirection( direction );
				m_ParticleGun->SetParticleEnergy(particlesEnergy[i]*keV);
				m_ParticleGun->SetParticleDefinition(particlesType[i]);
				m_ParticleGun->GeneratePrimaryVertex(anEvent);
//				G4cout <<  particlesType[i]->GetParticleName() << ", " << particlesEnergy[i] << " keV" << G4endl;
		}
		
		return;
	}
	else if( m_I137Event )
	{
		G4ThreeVector startPos( 0.0*cm, 0.0*cm, 0.0*cm );
		m_LENSLongDetectorConstruction->GetBeamPipeLaunchCoordinateGlobal( startPos );
	
		//Direction
		G4ThreeVector direction( 1.0, 0.0, 0.0 );
		GenerateIsotropicDirectionDistribution( direction,0.0);

//*******************for decay simulations:****************************************
		vector<double> particlesEnergy;
		vector< G4ParticleDefinition*> particlesType;
		m_MTASDecayManager->DecaySimulation(&particlesEnergy,&particlesType);

		unsigned int nrOfParticlesInEvent =particlesType.size();
		for(unsigned int i=0; i<nrOfParticlesInEvent;i++)
		{
				m_ParticleGun->SetParticlePosition( startPos );
				GenerateIsotropicDirectionDistribution( direction, 0.0 );
				m_ParticleGun->SetParticleMomentumDirection( direction );
				m_ParticleGun->SetParticleEnergy(particlesEnergy[i]*keV);
				m_ParticleGun->SetParticleDefinition(particlesType[i]);
				m_ParticleGun->GeneratePrimaryVertex(anEvent);
//				G4cout <<  particlesType[i]->GetParticleName() << ", " << particlesEnergy[i] << " keV" << G4endl;
		}
		
		return;
	}
	else if( m_TwoGammaEvent )
	{
		G4ThreeVector startPos( 0.0*cm, 0.0*cm, 0.0*cm );
		m_LENSLongDetectorConstruction->GetBeamPipeLaunchCoordinateGlobal( startPos );
	
		//Direction
		G4ThreeVector direction( 1.0, 0.0, 0.0 );
		GenerateIsotropicDirectionDistribution( direction,0.0);
		G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
		G4ParticleDefinition* gammaPD = particleTable->FindParticle("gamma");

		m_ParticleGun->SetParticlePosition( startPos );
		m_ParticleGun->SetParticleMomentumDirection( direction );
//		m_ParticleGun->SetParticleEnergy(1332.0*keV);
		m_ParticleGun->SetParticleDefinition(gammaPD);
		m_ParticleGun->GeneratePrimaryVertex(anEvent);

		m_ParticleGun->SetParticlePosition( startPos );
		GenerateIsotropicDirectionDistribution( direction, 0.0 );
		m_ParticleGun->SetParticleMomentumDirection( direction );
//		m_ParticleGun->SetParticleEnergy(1173.0*keV);
		m_ParticleGun->SetParticleTime( 371.0 * ns );
		m_ParticleGun->SetParticleDefinition(gammaPD);
		m_ParticleGun->GeneratePrimaryVertex(anEvent);
	}
	else if( m_NeutronFlatEnergySpectrumEvent )
	{
		G4ThreeVector startPos( 0.0*cm, 0.0*cm, 0.0*cm );
		m_LENSLongDetectorConstruction->GetBeamPipeLaunchCoordinateGlobal( startPos );
	
		//Direction
		G4ThreeVector direction( 1.0, 0.0, 0.0 );
    
		vector<double> particlesEnergy;
		vector< G4ParticleDefinition*> particlesType;
		m_MTASDecayManager->DecaySimulation(&particlesEnergy,&particlesType);

		GenerateIsotropicDirectionDistribution( direction, 0.0 );
		G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
		G4ParticleDefinition* neutronPD = particleTable->FindParticle("neutron");
		
    const G4double deltaEnergy = 25.0 * keV;
		const G4double minEnergy = 75.0 * keV;
		const G4double maxEnergy = minEnergy + deltaEnergy;
		const G4double energy = G4UniformRand() * ( maxEnergy - minEnergy ) + minEnergy;

		m_ParticleGun->SetParticlePosition( startPos );
		m_ParticleGun->SetParticleMomentumDirection( direction );
		m_ParticleGun->SetParticleEnergy( energy );
		m_ParticleGun->SetParticleDefinition( neutronPD );
		m_ParticleGun->GeneratePrimaryVertex( anEvent );

		unsigned int nrOfParticlesInEvent =particlesType.size();
		for(unsigned int i=0; i<nrOfParticlesInEvent;i++)
		{
				m_ParticleGun->SetParticlePosition( startPos );
				GenerateIsotropicDirectionDistribution( direction, 0.0 );
				m_ParticleGun->SetParticleMomentumDirection( direction );
				m_ParticleGun->SetParticleEnergy(particlesEnergy[i]*keV);
				m_ParticleGun->SetParticleDefinition(particlesType[i]);
				m_ParticleGun->GeneratePrimaryVertex(anEvent);
//				G4cout <<  particlesType[i]->GetParticleName() << ", " << particlesEnergy[i] << " keV" << G4endl;
		}
    
	}
  else if( m_FlatSingleParticleEnergySpectrumEvent )
  {// Sets particle to whatever is set in the macro file.
		G4ThreeVector aim( 0.0, -1.0, 0.0 );// The minus sign is due to the direction of the z axis relative to the x axis.
		
//		if( m_bRandomInitialAim )
		{
			G4double cosTheta = ( G4UniformRand() - 0.5 ) * 2.0;// -1. < cosTheta < 1.
			G4double sinTheta = sqrt( 1.0 - cosTheta * cosTheta );// 0. < sinTheta < 1.
			G4double phi = G4UniformRand() * 8.0 * atan(1.0);// 0. < phi < 2*pi

			G4double randomXaim = cos(phi) * sinTheta;
			G4double randomYaim = sin(phi) * sinTheta;
			G4double randomZaim = cosTheta;
			aim = G4ThreeVector( randomXaim, randomYaim, randomZaim );
		}
	  
		G4ThreeVector startPos(0.0,0.0,0.0);// Defaults to (0,0,0)

    const G4double gamma2E = 0.0 * keV;
    const G4double gamma3E = 0.0 * keV;
    
		const G4double minEnergy = 0.0 * keV - gamma2E - gamma3E;
		const G4double maxEnergy = 7100.0 * keV - gamma2E - gamma3E;
		const G4double energy = G4UniformRand() * ( maxEnergy - minEnergy ) + minEnergy;

		m_ParticleGun->SetParticlePosition( startPos );
		m_ParticleGun->SetParticleMomentumDirection( aim );
		m_ParticleGun->SetParticleEnergy( energy );
//		m_ParticleGun->SetParticleDefinition( neutronPD );
		m_ParticleGun->GeneratePrimaryVertex( anEvent );
    
    if( gamma2E > 0.0 * keV )// make a second gamma ray, initial KE of first gamma recorded in output data
    {
			G4double cosTheta = ( G4UniformRand() - 0.5 ) * 2.0;// -1. < cosTheta < 1.
			G4double sinTheta = sqrt( 1.0 - cosTheta * cosTheta );// 0. < sinTheta < 1.
			G4double phi = G4UniformRand() * 8.0 * atan(1.0);// 0. < phi < 2*pi

			G4double randomXaim = cos(phi) * sinTheta;
			G4double randomYaim = sin(phi) * sinTheta;
			G4double randomZaim = cosTheta;

      aim = G4ThreeVector( randomXaim, randomYaim, randomZaim );

      m_ParticleGun->SetParticleMomentumDirection( aim );
      m_ParticleGun->SetParticleEnergy( gamma2E );
      m_ParticleGun->GeneratePrimaryVertex( anEvent );

      if( gamma3E > 0.0 * keV )// make a third gamma ray, initial KE of first gamma recorded in output data
      {
        G4double cosTheta = ( G4UniformRand() - 0.5 ) * 2.0;// -1. < cosTheta < 1.
        G4double sinTheta = sqrt( 1.0 - cosTheta * cosTheta );// 0. < sinTheta < 1.
        G4double phi = G4UniformRand() * 8.0 * atan(1.0);// 0. < phi < 2*pi

        G4double randomXaim = cos(phi) * sinTheta;
        G4double randomYaim = sin(phi) * sinTheta;
        G4double randomZaim = cosTheta;

        aim = G4ThreeVector( randomXaim, randomYaim, randomZaim );

        m_ParticleGun->SetParticleMomentumDirection( aim );
        m_ParticleGun->SetParticleEnergy( gamma3E );
        m_ParticleGun->GeneratePrimaryVertex( anEvent );
      }
    }
  }
	else if( m_ArbitraryEnergySpectrumEvent )
	{
		G4ThreeVector startPos( 0.0*cm, 0.0*cm, 0.0*cm );
		m_LENSLongDetectorConstruction->GetBeamPipeLaunchCoordinateGlobal( startPos );
	
		//Direction
		G4ThreeVector direction( 1.0, 0.0, 0.0 );
		this->GenerateIsotropicDirectionDistribution( direction,0.0);

	  G4double energy = GetEnergyFromArbitrarySpectrum();
  // Can set the particle and postion and direction in here if you want.
	  //m_ParticleGun->SetParticleDefinition(electronPD);
	  m_ParticleGun->SetParticlePosition( startPos );
	  m_ParticleGun->SetParticleMomentumDirection( direction );
	  m_ParticleGun->SetParticleEnergy( energy );
	  m_ParticleGun->GeneratePrimaryVertex(anEvent);
	}
  else if( m_ArbitraryENSDFEvent )
  {
//    G4cout << "XXX" << G4endl;
		G4ThreeVector startPos( 0.0*cm, 0.0*cm, 0.0*cm );
		m_LENSLongDetectorConstruction->GetBeamPipeLaunchCoordinateGlobal( startPos );
	
//    G4cout << "XXX1" << G4endl;
		//Direction
		G4ThreeVector direction( 1.0, 0.0, 0.0 );
		GenerateIsotropicDirectionDistribution( direction,0.0);

//*******************for decay simulations:****************************************
//    G4cout << "XXX2" << G4endl;

		vector<double> particlesEnergy;
		vector< G4ParticleDefinition*> particlesType;
		m_MTASDecayManager->DecaySimulation( &particlesEnergy,&particlesType );

		unsigned int nrOfParticlesInEvent = particlesType.size();
//    G4cout << "N Particles " << nrOfParticlesInEvent << G4endl;
//    while( nrOfParticlesInEvent < 1 )
//    {
//      m_MTASDecayManager->DecaySimulation( &particlesEnergy,&particlesType );
//      nrOfParticlesInEvent = particlesType.size();
//    }
    
		for( unsigned int i=0; i < nrOfParticlesInEvent; i++)
		{
				m_ParticleGun->SetParticlePosition( startPos );
				GenerateIsotropicDirectionDistribution( direction, 0.0 );
				m_ParticleGun->SetParticleMomentumDirection( direction );
				m_ParticleGun->SetParticleEnergy(particlesEnergy[i]*keV);
				m_ParticleGun->SetParticleDefinition(particlesType[i]);
				m_ParticleGun->GeneratePrimaryVertex(anEvent);
//				G4cout <<  particlesType[i]->GetParticleName() << ", " << particlesEnergy[i] << " keV" << G4endl;
		}
		
//		return;
  }

}

void LENSLongPrimaryGeneratorAction::GenerateIsotropicDirectionDistribution(G4ThreeVector& direction, G4double theta0)
{
	G4double cosTheta = ( G4UniformRand() - 0.5 ) * 2.0 * cos(theta0);// -cos(Theta0) < cosTheta < cos(Theta0)
	G4double sinTheta = sqrt( 1.0 - cosTheta * cosTheta );
	G4double phi = G4UniformRand() * 2.0 * M_PI;// 0. < phi < 2*pi

	direction = G4ThreeVector (cos(phi) *sinTheta, sin(phi) * sinTheta, cosTheta);
}



G4double LENSLongPrimaryGeneratorAction::GetIndiumBetaDecayEnergy()
{
	if( m_IntegratedElectronDecaySpectrum.size() == 0 ) LoadIndiumBetaDecaySpectrum();
	
	const G4double random1 = G4UniformRand();// 0.0 <= x < 1.0, I think
	G4int energyIndex = 1;

// Use this if uneven input widths
	while( random1 > m_IntegratedElectronDecaySpectrum[energyIndex] ) energyIndex++;

// Saftey checks
	const G4int arraySize = m_IntegratedElectronDecaySpectrum.size();
	if( arraySize <= energyIndex ) energyIndex = arraySize;// Should I record this???
//	if( energyIndex < 1 ) energyIndex = 1;// Can not happen if the index starts at 1.
	
// Might could add an interpolation between values?
// INTERPOLATION OF ENERGY HERE
	const G4double pMax = m_ElectronDecaySpectrum[energyIndex] > m_ElectronDecaySpectrum[energyIndex-1] ? m_ElectronDecaySpectrum[energyIndex] : m_ElectronDecaySpectrum[energyIndex-1];
	const G4double slope = ( m_ElectronDecaySpectrum[energyIndex] - m_ElectronDecaySpectrum[energyIndex-1] ) / ( m_ElectronEnergySpectrum[energyIndex] - m_ElectronEnergySpectrum[energyIndex-1] );

	G4double pCompare = G4UniformRand() * pMax;
	G4double energyInterp = G4UniformRand() * ( m_ElectronEnergySpectrum[energyIndex] - m_ElectronEnergySpectrum[energyIndex-1] );
	G4double pE = m_ElectronDecaySpectrum[energyIndex-1] + slope * energyInterp;
	bool done = ( pCompare <= pE );
	
	while( ! done )
	{
		pCompare = G4UniformRand() * pMax;
		energyInterp = G4UniformRand() * ( m_ElectronEnergySpectrum[energyIndex] - m_ElectronEnergySpectrum[energyIndex-1] );
		pE = m_ElectronDecaySpectrum[energyIndex-1] + slope * energyInterp;
		if( pCompare <= pE) done = true; 
	}

	const G4double energy = m_ElectronEnergySpectrum[energyIndex-1] + energyInterp;
	
	return energy;
//	return m_ElectronEnergySpectrum[energyIndex];
}

void LENSLongPrimaryGeneratorAction::LoadIndiumBetaDecaySpectrum()
{
	char line[81];
	double spect[2];
	FILE *fp = fopen("PfeifferIndiumDecaySpectrum.txt","r");
	if( fp != NULL )
	{		
		while( fgets( line, 19, fp ) ) // Highly dependent on file structure of data
		{
			sscanf(line,"%lf%le", &spect[0], &spect[1] );// If spect are doubles
			m_ElectronEnergySpectrum.push_back( spect[0] * keV );
			m_ElectronDecaySpectrum.push_back( spect[1] );
		}
		fclose(fp);
			
		const int sizeOfData = m_ElectronEnergySpectrum.size();
	// This assumes constant step size...
		G4double deltaE = 1.0;
		
		if( sizeOfData > 1 ) deltaE = m_ElectronEnergySpectrum[1] - m_ElectronEnergySpectrum[0];
		G4double totalCrossSection = 0.0;
		G4double prevElectronDecaySpect = 0.0;
		for( int i = 0; i < sizeOfData; i++ )
		{
			totalCrossSection += ( m_ElectronDecaySpectrum[i] + prevElectronDecaySpect ) * deltaE * 0.5;
			prevElectronDecaySpect = m_ElectronDecaySpectrum[i];
		}
			
		m_IntegratedElectronDecaySpectrum.reserve( sizeOfData );
		G4double prevIntegratedCS = 0.0;
		prevElectronDecaySpect = 0.0;
//		m_IntegratedElectronDecaySpectrum.push_back( prevIntegratedCS );
		for( int i = 0; i < sizeOfData; i++ )
		{
			prevIntegratedCS += ( m_ElectronDecaySpectrum[i] + prevElectronDecaySpect ) * deltaE * 0.5 / totalCrossSection;
			prevElectronDecaySpect = m_ElectronDecaySpectrum[i];
			m_IntegratedElectronDecaySpectrum.push_back( prevIntegratedCS );
		}
// Check the data...
//		FILE *fpCHECK = fopen("PfeifferIndiumDecaySpectrumCHECKOUT.txt","w");
//		G4double crossSection = 0.0;
//		G4double prevIntegratedElectronDecaySpectrum = 0.0;
//		fprintf(fpCHECK, "Number Elements: %i \n", sizeOfData );
//		for( int i = 0; i < sizeOfData; i++ )
//		{
//			crossSection = m_IntegratedElectronDecaySpectrum[i] - prevIntegratedElectronDecaySpectrum;
//			crossSection *= totalCrossSection / deltaE;
//			prevIntegratedElectronDecaySpectrum = m_IntegratedElectronDecaySpectrum[i];
//			fprintf(fpCHECK,"Energy: %lf, DiffCrossSection: %lf, Integrated Cross Section: %lf \n", m_ElectronEnergySpectrum[i]/keV, m_ElectronDecaySpectrum[i], m_IntegratedElectronDecaySpectrum[i] );// If spect are doubles
//		}
//		fprintf( fpCHECK, "{ ");
//		for( int i = 0; i < sizeOfData; i++ )
//		{
//			fprintf( fpCHECK, "%lf, ", m_ElectronEnergySpectrum[i]/keV );
//		}
//		fprintf(fpCHECK, "}\n");
//		fprintf( fpCHECK, "{ ");
//		for( int i = 0; i < sizeOfData; i++ )
//		{
//			fprintf( fpCHECK, "%lf, ", m_ElectronDecaySpectrum[i] );
//		}
//		fprintf( fpCHECK, "}\n");
//		fprintf( fpCHECK, "{ ");
//		for( int i = 0; i < sizeOfData; i++ )
//		{
//			fprintf( fpCHECK, "%lf, ", m_IntegratedElectronDecaySpectrum[i] );
//		}
//		fprintf( fpCHECK, "}\n");		
//		fclose(fpCHECK);
// END CHECK
	}
	else
	{
		G4cout << "NO Input file for Charged Current electron found, it should be titled 'PfeifferIndiumDecaySpectrum.txt'." << G4endl;
		m_ElectronEnergySpectrum.push_back( 116.0 * keV );
		m_IntegratedElectronDecaySpectrum.push_back( 1.1 );// Always accept this single value, i.e. any value bigger than 1 here will do.
	}

}

G4double LENSLongPrimaryGeneratorAction::GetBSIndiumBetaDecayEnergy()
{
	if( m_IntegratedBSElectronDecaySpectrum.size() == 0 ) LoadBSIndiumBetaDecaySpectrum();
	
	const G4double random1 = G4UniformRand();// 0.0 <= x < 1.0, I think
	G4int energyIndex = 1;

// Use this if uneven input widths
	while( random1 > m_IntegratedBSElectronDecaySpectrum[energyIndex] ) energyIndex++;

// Saftey checks
	const G4int arraySize = m_IntegratedBSElectronDecaySpectrum.size();
	if( arraySize <= energyIndex ) energyIndex = arraySize;// Should I record this???
//	if( energyIndex < 1 ) energyIndex = 1;// Can not happen if the index starts at 1.
	
// Might could add an interpolation between values?
// INTERPOLATION OF ENERGY HERE, for now there is only the integrated BS spectrum. Can change to spectrum or could use Integrated Spectrum to calculate things.
/*	const G4double pMax = m_ElectronDecaySpectrum[energyIndex] > m_ElectronDecaySpectrum[energyIndex-1] ? m_ElectronDecaySpectrum[energyIndex] : m_ElectronDecaySpectrum[energyIndex-1];
	const G4double slope = ( m_ElectronDecaySpectrum[energyIndex] - m_ElectronDecaySpectrum[energyIndex-1] ) / ( m_ElectronEnergySpectrum[energyIndex] - m_ElectronEnergySpectrum[energyIndex-1] );

	G4double pCompare = G4UniformRand() * pMax;
	G4double energyInterp = G4UniformRand() * ( m_ElectronEnergySpectrum[energyIndex] - m_ElectronEnergySpectrum[energyIndex-1] );
	G4double pE = m_ElectronDecaySpectrum[energyIndex-1] + slope * energyInterp;
	bool done = ( pCompare <= pE );
	
	while( ! done )
	{
		pCompare = G4UniformRand() * pMax;
		energyInterp = G4UniformRand() * ( m_ElectronEnergySpectrum[energyIndex] - m_ElectronEnergySpectrum[energyIndex-1] );
		pE = m_ElectronDecaySpectrum[energyIndex-1] + slope * energyInterp;
		if( pCompare <= pE) done = true; 
	}
*/
	const G4double energy = m_BSElectronEnergySpectrum[energyIndex-1];// + energyInterp;
	
	return energy;
//	return m_ElectronEnergySpectrum[energyIndex];
}

void LENSLongPrimaryGeneratorAction::LoadBSIndiumBetaDecaySpectrum()
{
	char line[81];
	double spect[2];
	FILE *fp = fopen("IndiumBSIntegratedSpectrum.txt","r");
	if( fp != NULL )
	{		
		while( fgets( line, 22, fp ) ) // Highly dependent on file structure of data
		{
			sscanf(line,"%lf%le", &spect[0], &spect[1] );// If spect are doubles
			m_BSElectronEnergySpectrum.push_back( spect[0] * keV );
			m_IntegratedBSElectronDecaySpectrum.push_back( spect[1] );
		}
		fclose(fp);
/*			
		const int sizeOfData = m_BSElectronEnergySpectrum.size();
	// This assumes constant step size...
		G4double deltaE = 1.0;
		if( sizeOfData > 1 ) deltaE = m_BSElectronEnergySpectrum[1] - m_BSElectronEnergySpectrum[0];
		G4double prob = 0.0;
		G4double prevElectronDecaySpect = 0.0;
		m_BSElectronDecaySpectrum.push_back(0.0);

		for( int i = 1; i < sizeOfData; i++ )
		{
			prob = 2.0;
			prob *= m_IntegratedBSElectronDecaySpectrum[i] - m_IntegratedBSElectronDecaySpectrum[i-1];
			prob /= m_BSElectronEnergySpectrum[i] - m_BSElectronEnergySpectrum[i-1];

			prob -= m_BSElectronDecaySpectrum[i-1];
			m_BSElectronDecaySpectrum.push_back( prob );
		}
*/
// Check the data...
/*		FILE *fpCHECK = fopen("BSIndiumDecaySpectrumCHECKOUT.txt","w");
		G4double crossSection = 0.0;
		G4double prevIntegratedElectronDecaySpectrum = 0.0;

		const G4double totalCrossSection = 1.0;
		const G4double deltaE = m_BSElectronEnergySpectrum[1] - m_BSElectronEnergySpectrum[0];
		const int sizeOfData = m_BSElectronEnergySpectrum.size();
		
//		fprintf(fpCHECK, "Number Elements: %i \n", sizeOfData );
//		for( int i = 0; i < sizeOfData; i++ )
//		{
//			crossSection = m_IntegratedBSElectronDecaySpectrum[i] - prevIntegratedElectronDecaySpectrum;
//			crossSection *= totalCrossSection / deltaE;
//			prevIntegratedElectronDecaySpectrum = m_IntegratedBSElectronDecaySpectrum[i];
//			fprintf(fpCHECK,"Energy: %lf, DiffCrossSection: %lf, Integrated Cross Section: %lf \n", m_BSElectronEnergySpectrum[i]/keV, m_ElectronDecaySpectrum[i], m_IntegratedElectronDecaySpectrum[i] );
//			fprintf(fpCHECK,"Energy: %lf, Integrated Cross Section: %lf \n", m_BSElectronEnergySpectrum[i]/keV, m_IntegratedBSElectronDecaySpectrum[i] );
//		}
		fprintf( fpCHECK, "{\n");
		for( int i = 0; i < sizeOfData; i++ )
		{
			fprintf( fpCHECK, "%lf, ", m_BSElectronEnergySpectrum[i]/keV );
		}
		fprintf(fpCHECK, "\n}\n");
//		fprintf( fpCHECK, "{ ");
//		for( int i = 0; i < sizeOfData; i++ )
//		{
//			fprintf( fpCHECK, "%lf, ", m_ElectronDecaySpectrum[i] );
//		}
//		fprintf( fpCHECK, "}\n");
		fprintf( fpCHECK, "{\n");
		for( int i = 0; i < sizeOfData; i++ )
		{
			fprintf( fpCHECK, "%lf, ", m_IntegratedBSElectronDecaySpectrum[i] );
		}
		fprintf( fpCHECK, "\n}\n");		
		fclose(fpCHECK);
*/
// END CHECK
	}
	else
	{
		G4cout << "NO Input file for Charged Current electron found, it should be titled 'IndiumBSIntegratedSpectrum.txt'." << G4endl;
		m_BSElectronEnergySpectrum.push_back( 116.0 * keV );
		m_IntegratedBSElectronDecaySpectrum.push_back( 1.1 );// Always accept this single value, i.e. any value bigger than 1 here will do.
	}

}


void LENSLongPrimaryGeneratorAction::GetBremstrahlungEnergy( G4double& energy, G4double& gammaEnergy )
{// The energy variable put in is changed to energy - gammaEnergy in this function
	const G4double minE = 41.0 * keV;
	if( ! m_BremstrahlungDataLoaded ) LoadBremsttrahlungDecaySpectrum();
	
	if( m_BremstrahlungIntegratedSpectrum100.size() == 0 )
	{
		G4cout << "Trouble Loading the Bremstrahlung Data" << G4endl;
		gammaEnergy = minE;
		energy -= gammaEnergy;
		return;
	}
	
	const G4double random1 = G4UniformRand();// 0.0 <= x < 1.0, I think
	G4int energyIndex = 41;// Safe as long as below minimum E.

	if( energy <= 100.0 * keV )
	{
		while( random1 > m_BremstrahlungIntegratedSpectrum100[energyIndex] ) energyIndex++;
	// Saftey checks
		const G4int arraySize = m_BremstrahlungIntegratedSpectrum100.size();
		if( arraySize <= energyIndex ) energyIndex = arraySize;// Should I record this???
		const G4double defaultEnergySpreadFraction = ( energy - minE ) / ( 100.0 * keV - minE );
		
		gammaEnergy = minE + ( m_BremstrahlungEnergySpectrum[energyIndex] - minE ) * defaultEnergySpreadFraction;
		energy -= gammaEnergy;
	}
	else if( energy <= 200.0 * keV )
	{
		while( random1 > m_BremstrahlungIntegratedSpectrum200[energyIndex] ) energyIndex++;
	// Saftey checks
		const G4int arraySize = m_BremstrahlungIntegratedSpectrum200.size();
		if( arraySize <= energyIndex ) energyIndex = arraySize;// Should I record this???
		const G4double defaultEnergySpreadFraction = ( energy - minE ) / ( 200.0 * keV - minE );
		
		gammaEnergy = minE + ( m_BremstrahlungEnergySpectrum[energyIndex] - minE ) * defaultEnergySpreadFraction;
		energy -= gammaEnergy;
	}
	else if( energy <= 300.0 * keV )
	{
		while( random1 > m_BremstrahlungIntegratedSpectrum300[energyIndex] ) energyIndex++;
	// Saftey checks
		const G4int arraySize = m_BremstrahlungIntegratedSpectrum300.size();
		if( arraySize <= energyIndex ) energyIndex = arraySize;// Should I record this???
		const G4double defaultEnergySpreadFraction = ( energy - minE ) / ( 300.0 * keV - minE );
		
		gammaEnergy = minE + ( m_BremstrahlungEnergySpectrum[energyIndex] - minE ) * defaultEnergySpreadFraction;
		energy -= gammaEnergy;
	}
	else if( energy <= 400.0 * keV )
	{
		while( random1 > m_BremstrahlungIntegratedSpectrum400[energyIndex] ) energyIndex++;
	// Saftey checks
		const G4int arraySize = m_BremstrahlungIntegratedSpectrum400.size();
		if( arraySize <= energyIndex ) energyIndex = arraySize;// Should I record this???
		const G4double defaultEnergySpreadFraction = ( energy - minE ) / ( 400.0 * keV - minE );
		
		gammaEnergy = minE + ( m_BremstrahlungEnergySpectrum[energyIndex] - minE ) * defaultEnergySpreadFraction;
		energy -= gammaEnergy;
	}
	else
	{
		while( random1 > m_BremstrahlungIntegratedSpectrum500[energyIndex] ) energyIndex++;
	// Saftey checks
		const G4int arraySize = m_BremstrahlungIntegratedSpectrum500.size();
		if( arraySize <= energyIndex ) energyIndex = arraySize;// Should I record this???
		const G4double defaultEnergySpreadFraction = ( energy - minE ) / ( 500.0 * keV - minE );
		
		gammaEnergy = minE + ( m_BremstrahlungEnergySpectrum[energyIndex] - minE ) * defaultEnergySpreadFraction;
		energy -= gammaEnergy;
	}

// For testing purposes only...
//	if( gammaEnergy > 400.0 * keV ) G4cout << energy / keV << ", " << gammaEnergy / keV << "," << G4endl;
//	G4cout << energyIndex << "," << G4endl;
}


void LENSLongPrimaryGeneratorAction::LoadBremsttrahlungDecaySpectrum()
{
	char line[30];
	double spect[2];
	FILE* pGammaInputFile = 0;
	pGammaInputFile = fopen ("GammaSummedProb100keV.txt","r");
//fprintf( pLiquidScintGammaOutputFile, "%5.1F %7.6F \n", (Float_t) i, sum );// How the info is written.
	if( pGammaInputFile != 0 )
	{		
		while( fgets( line, 19, pGammaInputFile ) ) // Highly dependent on file structure of data
		{
			sscanf(line,"%lf%lf", &spect[0], &spect[1] );// If spect are doubles
//			m_BremstrahlungEnergySpectrum.push_back( spect[0] * keV );
			m_BremstrahlungIntegratedSpectrum100.push_back( spect[1] );
		}
	}
	if( pGammaInputFile != 0 )
	{
		fclose( pGammaInputFile );
		pGammaInputFile = 0;
	}

	pGammaInputFile = fopen ("GammaSummedProb200keV.txt","r");
//fprintf( pLiquidScintGammaOutputFile, "%5.1F %7.6F \n", (Float_t) i, sum );// How the info is written.
	if( pGammaInputFile != 0 )
	{		
		while( fgets( line, 19, pGammaInputFile ) ) // Highly dependent on file structure of data
		{
			sscanf(line,"%lf%lf", &spect[0], &spect[1] );// If spect are doubles
//			m_BremstrahlungEnergySpectrum.push_back( spect[0] * keV );// This already exists...
			m_BremstrahlungIntegratedSpectrum200.push_back( spect[1] );
		}
	}
	if( pGammaInputFile != 0 )
	{
		fclose( pGammaInputFile );
		pGammaInputFile = 0;
	}

	pGammaInputFile = fopen ("GammaSummedProb300keV.txt","r");
//fprintf( pLiquidScintGammaOutputFile, "%5.1F %7.6F \n", (Float_t) i, sum );// How the info is written.
	if( pGammaInputFile != 0 )
	{		
		while( fgets( line, 19, pGammaInputFile ) ) // Highly dependent on file structure of data
		{
			sscanf(line,"%lf%lf", &spect[0], &spect[1] );// If spect are doubles
//			m_BremstrahlungEnergySpectrum.push_back( spect[0] * keV );// This already exists...
			m_BremstrahlungIntegratedSpectrum300.push_back( spect[1] );
		}
	}
	if( pGammaInputFile != 0 )
	{
		fclose( pGammaInputFile );
		pGammaInputFile = 0;
	}

	pGammaInputFile = fopen ("GammaSummedProb400keV.txt","r");
//fprintf( pLiquidScintGammaOutputFile, "%5.1F %7.6F \n", (Float_t) i, sum );// How the info is written.
	if( pGammaInputFile != 0 )
	{		
		while( fgets( line, 19, pGammaInputFile ) ) // Highly dependent on file structure of data
		{
			sscanf(line,"%lf%lf", &spect[0], &spect[1] );// If spect are doubles
//			m_BremstrahlungEnergySpectrum.push_back( spect[0] * keV );// This already exists...
			m_BremstrahlungIntegratedSpectrum400.push_back( spect[1] );
		}
	}
	if( pGammaInputFile != 0 )
	{
		fclose( pGammaInputFile );
		pGammaInputFile = 0;
	}

	pGammaInputFile = fopen ("GammaSummedProb500keV.txt","r");
//fprintf( pLiquidScintGammaOutputFile, "%5.1F %7.6F \n", (Float_t) i, sum );// How the info is written.
	if( pGammaInputFile != 0 )
	{		
		while( fgets( line, 19, pGammaInputFile ) ) // Highly dependent on file structure of data
		{
			sscanf(line,"%lf%lf", &spect[0], &spect[1] );// If spect are doubles
			m_BremstrahlungEnergySpectrum.push_back( spect[0] * keV );// This already exists...
			m_BremstrahlungIntegratedSpectrum500.push_back( spect[1] );
		}
	}
	if( pGammaInputFile != 0 )
	{
		fclose( pGammaInputFile );
		pGammaInputFile = 0;
	}

	if( false ) for( int i = 0; i < (int) m_BremstrahlungEnergySpectrum.size(); i++ )
	{
		G4cout << m_BremstrahlungEnergySpectrum[i] / keV << 
		", " << m_BremstrahlungIntegratedSpectrum100[i] << 
		", " << m_BremstrahlungIntegratedSpectrum200[i] << 
		", " << m_BremstrahlungIntegratedSpectrum300[i] << 
		", " << m_BremstrahlungIntegratedSpectrum400[i] << 
		", " << m_BremstrahlungIntegratedSpectrum500[i] << G4endl;
	}
	
	m_BremstrahlungDataLoaded = true;
}

G4double LENSLongPrimaryGeneratorAction::GetEnergyFromArbitrarySpectrum()
{
  if( m_IntegratedElectronDecaySpectrum.size() == 0 ) LoadArbitrarySpectrum();
	
  const G4double random1 = G4UniformRand();// 0.0 <= x < 1.0, I think
  G4int energyIndex = 1;

// Use this if uneven input widths
  while( random1 > m_IntegratedElectronDecaySpectrum[energyIndex] ) energyIndex++;

// Saftey checks
  const G4int arraySize = m_IntegratedElectronDecaySpectrum.size();
  if( arraySize <= energyIndex ) energyIndex = arraySize;// Should I record this???
//	if( energyIndex < 1 ) energyIndex = 1;// Can not happen if the index starts at 1.
	
// Might could add an interpolation between values?
// INTERPOLATION OF ENERGY HERE
  const G4double pMax = m_ElectronDecaySpectrum[energyIndex] > m_ElectronDecaySpectrum[energyIndex-1] ? 
    m_ElectronDecaySpectrum[energyIndex] : m_ElectronDecaySpectrum[energyIndex-1];
  const G4double slope = ( m_ElectronDecaySpectrum[energyIndex] - m_ElectronDecaySpectrum[energyIndex-1] ) / 
    ( m_ElectronEnergySpectrum[energyIndex] - m_ElectronEnergySpectrum[energyIndex-1] );

  G4double pCompare = G4UniformRand() * pMax;
  G4double energyInterp = G4UniformRand() * 
    ( m_ElectronEnergySpectrum[energyIndex] - m_ElectronEnergySpectrum[energyIndex-1] );
  G4double pE = m_ElectronDecaySpectrum[energyIndex-1] + slope * energyInterp;
  bool done = ( pCompare <= pE );
	
  while( ! done )
  {
    pCompare = G4UniformRand() * pMax;
    energyInterp = G4UniformRand() * 
      ( m_ElectronEnergySpectrum[energyIndex] - m_ElectronEnergySpectrum[energyIndex-1] );
    pE = m_ElectronDecaySpectrum[energyIndex-1] + slope * energyInterp;
    if( pCompare <= pE) done = true; 
  }

  const G4double energy = m_ElectronEnergySpectrum[energyIndex-1] + energyInterp;
	
  return energy;
}


void LENSLongPrimaryGeneratorAction::LoadArbitrarySpectrum()
{
  double energy, probability;
  std::ifstream myfile;
  myfile.open ( m_ArbitraryEnergySpetrumFileName.c_str(), std::ios::in );
  

  if( myfile.is_open() )
  {
    while (! myfile.eof() )
    {
      //      getline( myfile, inputLine );
      myfile >> energy;
      myfile >> probability;
//			std::cout << energy << ", " << probability << std::endl;

      if( m_ElectronEnergySpectrum.size() == 0 && energy == 0.0 )
      {
				m_ElectronEnergySpectrum.push_back( energy * keV );
				m_ElectronDecaySpectrum.push_back( probability );
				
//				std::cout << energy << ", " << probability << std::endl;
      }
      else if( m_ElectronEnergySpectrum.size() > 0 && energy * keV != m_ElectronEnergySpectrum.back() )
      {
				m_ElectronEnergySpectrum.push_back( energy * keV );
				m_ElectronDecaySpectrum.push_back( probability );

//				std::cout << energy << ", " << probability << std::endl;
      }
    }

    myfile.close();

    const int sizeOfData = m_ElectronEnergySpectrum.size();
    // This assumes constant step size...
    G4double deltaE = 1.0;
		
    if( sizeOfData > 1 ) deltaE = m_ElectronEnergySpectrum[1] - m_ElectronEnergySpectrum[0];
    G4double totalCrossSection = 0.0;
    G4double prevElectronDecaySpect = 0.0;
    for( int i = 1; i < sizeOfData; i++ )
    {
			deltaE = m_ElectronEnergySpectrum[i] - m_ElectronEnergySpectrum[i-1];
      totalCrossSection += ( m_ElectronDecaySpectrum[i] + prevElectronDecaySpect ) * deltaE * 0.5;
      prevElectronDecaySpect = m_ElectronDecaySpectrum[i];
    }
			
    m_IntegratedElectronDecaySpectrum.reserve( sizeOfData );
    G4double prevIntegratedCS = 0.0;
    prevElectronDecaySpect = 0.0;
		
		m_IntegratedElectronDecaySpectrum.push_back( 0.0 );
    for( int i = 1; i < sizeOfData; i++ )
    {
			deltaE = m_ElectronEnergySpectrum[i] - m_ElectronEnergySpectrum[i-1];
      prevIntegratedCS += ( m_ElectronDecaySpectrum[i] + prevElectronDecaySpect ) * deltaE * 0.5 / totalCrossSection;
      prevElectronDecaySpect = m_ElectronDecaySpectrum[i];
      m_IntegratedElectronDecaySpectrum.push_back( prevIntegratedCS );
			std::cout << m_ElectronDecaySpectrum[i] << ", " << prevIntegratedCS << std::endl;
    }
// Check the data...
/*    FILE *fpCHECK = fopen("ArbitraryEnergySpectrumCHECKOUT.txt","w");
    G4double crossSection = 0.0;
    G4double prevIntegratedElectronDecaySpectrum = 0.0;
    fprintf(fpCHECK, "Number Elements: %i \n", sizeOfData );
    for( int i = 0; i < sizeOfData; i++ )
    {
      crossSection = m_IntegratedElectronDecaySpectrum[i] - prevIntegratedElectronDecaySpectrum;
      crossSection *= totalCrossSection / deltaE;
      prevIntegratedElectronDecaySpectrum = m_IntegratedElectronDecaySpectrum[i];
      fprintf(fpCHECK,"Energy: %lf, DiffCrossSection: %lf, Integrated Cross Section: %lf \n", m_ElectronEnergySpectrum[i]/keV, m_ElectronDecaySpectrum[i], m_IntegratedElectronDecaySpectrum[i] );// If spect are doubles
    }
    fprintf( fpCHECK, "{ ");
    for( int i = 0; i < sizeOfData; i++ )
    {
      fprintf( fpCHECK, "%lf, ", m_ElectronEnergySpectrum[i]/keV );
    }
    fprintf(fpCHECK, "}\n");
    fprintf( fpCHECK, "{ ");
    for( int i = 0; i < sizeOfData; i++ )
    {
      fprintf( fpCHECK, "%lf, ", m_ElectronDecaySpectrum[i] );
    }
    fprintf( fpCHECK, "}\n");
    fprintf( fpCHECK, "{ ");
    for( int i = 0; i < sizeOfData; i++ )
    {
      fprintf( fpCHECK, "%lf, ", m_IntegratedElectronDecaySpectrum[i] );
    }
    fprintf( fpCHECK, "}\n");		
    fclose(fpCHECK);*/
// END CHECK
  }
  else
  {
    G4cout << "NO Input file for Arbitrary Energy Spectrum found!. You named it: " <<
	   m_ArbitraryEnergySpetrumFileName << G4endl;

    m_ElectronEnergySpectrum.push_back( 10.0 * keV );// Put in an energy that probably be noticed as odd.
    m_ElectronEnergySpectrum.push_back( 30.0 * keV );// Put in an energy that probably be noticed as odd.

    m_IntegratedElectronDecaySpectrum.push_back( 0.5 );
// Always accept this single value, i.e. any value bigger than 1 here will do.
    m_IntegratedElectronDecaySpectrum.push_back( 1.1 );
  }
}
