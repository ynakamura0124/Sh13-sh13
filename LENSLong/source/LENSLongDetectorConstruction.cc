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


#include "globals.hh"

#include "G4Material.hh"
#include "G4MaterialTable.hh"

#include "G4Box.hh"
//#include "G4Tubs.hh"
#include "G4LogicalVolume.hh"

#include "G4PVPlacement.hh"
#include "G4SDManager.hh"
#include "G4VSensitiveDetector.hh"

#include "G4VisAttributes.hh"
#include "G4Colour.hh"

#include "LENSLongDetectorConstruction.hh"
#include "LENSLongLiquidScintillator.hh"

using namespace CLHEP;


LENSLongDetectorConstruction::LENSLongDetectorConstruction()
{
	m_Vacuum = 0;
	m_CarbonFiber = 0;
	m_SiliconPutty = 0;
  m_Cu = 0;
	m_Fe = 0;
	m_Pb = 0;
	m_Cd = 0;
	m_HDPE = 0;
  m_Delrin = 0;
	m_Air = 0;
	m_Polystyrene = 0;
	m_MineralOil = 0;
	m_PMTGlass = 0;
	m_Concrete = 0;
	m_Water = 0;
	
  m_Vinyl = 0;
	m_Nylon = 0;
	m_Teflon = 0;
	m_PTFETape = 0;
	m_BC408Material = 0;
	m_Tin = 0;
	m_Aluminium = 0;
  m_LowDensityAluminium = 0;
	m_NaI = 0;
	m_NaIWithTl = 0;
	m_AluminiumOxide = 0;
	m_Textolite = 0;
	m_GermaniumCrystal = 0;
	m_BariumFlorideCrystal = 0;
	m_StainlessSteel = 0;
	m_PCB = 0;
	m_YSO = 0;
	
// Material tables
	m_Nylon_mt = 0;
	m_Teflon_mt = 0;
	m_BC408Material_mt = 0;
//Jeffout	m_IndiumFreeScintillatorMaterial_mt = 0;
//	m_Water_mt = 0;
//	m_MineralOil_mt = 0;
//	m_PMTGlass_mt = 0;

	m_pWorld_logVol = 0;
	m_pWorld_phyVol = 0;

	m_pLENSLongHall_logVol = 0;
	m_pLENSLongHall_phyVol = 0;
	m_LENSLongHallRotation = 0;

	m_LENSLongBoxScintillatorSD = 0;

	m_pFloor_logVol = 0;
	m_pFloor_phyVol = 0;
	m_FloorThickness = 0.0;
	m_UseFloor = false;
	
//	m_IronVisAtt = 0;
	m_ScintillatorVisAtt = 0;
	m_ConcreteVisAtt = 0;
	m_IndiumVisAtt = 0;
	m_TinVisAtt = 0;
// END SETTING DEFAULT INTIAL VALUES

	m_LENSLongHallRotation = 0;

	SetFloorThickness( 2.0*m );
}

LENSLongDetectorConstruction::~LENSLongDetectorConstruction()
{
	DestroyVisibilityAttributes();
	DestroyMaterials();
	
	if( m_LENSLongHallRotation != 0 )
	{
		delete m_LENSLongHallRotation;
	}
}


G4VPhysicalVolume* LENSLongDetectorConstruction::Construct()
{
	ConstructMaterials();
	ConstructVisibilityAttributes();

// Construct the WORLD Volume
  G4double worldHalf_x = 10.0*m;
  G4double worldHalf_y = 10.0*m;
  G4double worldHalf_z = 10.0*m;
	
  G4Box* world_box = new G4Box( "World_box", worldHalf_x, worldHalf_y, worldHalf_z );
  m_pWorld_logVol = new G4LogicalVolume( world_box,  m_Vacuum, "World_logical", 0, 0, 0 );
  m_pWorld_phyVol = new G4PVPlacement( 0, G4ThreeVector(0.0,0.0,0.0), m_pWorld_logVol, "World_physical", 0, 0, 0 );
	
//  ConstructLENSLongHall();
//	if( m_UsePixelatedLattice )	ConstructLENSPixelatedLatticeScintillator();
	ConstructLENSLongScintillator();

//  DumpGeometricalTree(m_pWorld_phyVol);// Get too long of an output and hence is confusing.

  return m_pWorld_phyVol;
}


void LENSLongDetectorConstruction::ConstructMaterials()
{
// If already made, do not make again.
	if( m_Vacuum != 0 ) return;

  G4double atomicMass;  // atomic mass
  G4double z;  // atomic number
  G4double density;
  G4int numberElements;

// density for these guys? Is it automatic? What about temperature? Or is the density/temp in the material class only?
  G4Element* H = new G4Element( "Hydrogen", "H", z=1., atomicMass=  1.00*g/mole );
  G4Element* B = new G4Element( "Boron"		, "B", z=5., atomicMass= 10.81*g/mole );
  G4Element* C = new G4Element( "Carbon"	, "C", z=6., atomicMass= 12.01*g/mole );
  G4Element* N = new G4Element( "Nitrogen", "N", z=7., atomicMass= 14.01*g/mole );
  G4Element* O = new G4Element( "Oxygen"  , "O", z=8., atomicMass= 16.00*g/mole );
  G4Element* F = new G4Element( "Fluorine", "F", z=9., atomicMass= 18.998*g/mole );

  G4Element* Na = new G4Element( "Sodium"		, "Na", z=11., atomicMass= 22.99*g/mole );
  G4Element* Mg = new G4Element( "Magnesium", "Mg", z=12., atomicMass= 24.31*g/mole );
  G4Element* Al = new G4Element( "Aluminium", "Al", z=13., atomicMass= 26.98*g/mole );
  G4Element* Si = new G4Element( "Silicon"  , "Si", z=14., atomicMass= 28.09*g/mole );
  G4Element* P = new G4Element( "Phosphorus", "P", z=15., atomicMass= 30.974*g/mole );
  G4Element* S = new G4Element( "Sulfur"		, "S", z=16., atomicMass= 32.065*g/mole );
  G4Element* Cl = new G4Element( "Chlorine"	, "Cl", z=17., atomicMass= 35.45*g/mole );
  G4Element* K  = new G4Element( "Potassium", "K",	z=19., atomicMass= 39.10*g/mole );
  G4Element* Ca = new G4Element( "Calcium"  , "Ca", z=20., atomicMass= 40.08*g/mole );
	G4Element* Cr = new G4Element( "Chromium"	, "Cr", z=24., atomicMass= 51.9961*g/mole );
  G4Element* Mn = new G4Element( "Manganese", "Mn", z=25., atomicMass= 54.938*g/mole );
  G4Element* Fe = new G4Element( "Iron"			, "Fe", z=26., atomicMass= 55.85*g/mole );
  G4Element* Ni = new G4Element( "Nickel"		, "Ni", z=28., atomicMass= 58.69*g/mole );
  G4Element* Ge = new G4Element( "Germanium", "I", z=32., atomicMass= 72.64*g/mole );
  G4Element* Y = new G4Element( "Yttrium", "Y", z=39., atomicMass= 88.90585*g/mole );
  G4Element* Mo = new G4Element( "Molybdenum", "Mo", z=42., atomicMass= 95.54*g/mole );

  //G4Element* In = new G4Element( "Indium"		, "In", z=49., atomicMass= 114.818*g/mole );
  G4Element* Sn = new G4Element( "Tin"			, "Sn", z=50., atomicMass= 118.710*g/mole );
  G4Element* I = new G4Element( "Iodine"		, "I", z=53., atomicMass= 126.9*g/mole );
  G4Element* Ba = new G4Element( "Barium"		, "Ba", z=56., atomicMass= 137.3*g/mole );
  G4Element* Tl = new G4Element( "Thalium"	, "Tl", z=81., atomicMass= 204.383*g/mole );

// From Novice Example N03
	m_Vacuum = new G4Material("GalacticVacuum", z=1., atomicMass= 1.01*g/mole, density = universe_mean_density, kStateGas, 3.e-18*pascal, 2.73*kelvin);

  m_Air = new G4Material("Air", density= 1.29*mg/cm3, numberElements=2);
  m_Air->AddElement(N, 70*perCent);
  m_Air->AddElement(O, 30*perCent);
	
// THIS IS FROM THE     *  Cost estimate for veto based on extruded scintillators * file on the nuSNS website
// It is assumed here that we need to cover: four times 4.9 m x 6.7 m plus 4.9 m x 4.9 m, which gives a total of 155.33 m 2. 
// For three layers of each 0.01 m thickness we arrive at a scintillator volume of 4.66 m 3. 
// The density of polystyrene is 1.04 kg/l which is 1040 kg/m 3. Following is 4.66 m3 would have a mass of 4846 kg. 
// Doping would be done with 1% PPO and 0.03% POPOP and the extrusion coextruded with a 0.25 mm layer of white (TiO2) coating.

// Scintillator
  m_Polystyrene = new G4Material("Scintillator", density= 1.040*g/cm3, numberElements=2);// I think this is all that is needed
  m_Polystyrene->AddElement(C, 9);
  m_Polystyrene->AddElement(H, 10);

// Not just G4Elements, but actual materials
	m_CarbonFiber = new G4Material( "CarbonFiber", z=6., atomicMass=12.01*g/mole, density=1.8*g/cm3 );
	m_Silicon = new G4Material( "Silicon", z=14., atomicMass=28.09*g/mole, density=2.33*g/cm3 );
//	m_SiliconPutty = new G4Material( "SiliconPutty", z=14., atomicMass=28.09*g/mole, density=2.3*g/cm3 );
	m_SiliconPutty = new G4Material( "SiliconPutty", density= 0.5*g/cm3, numberElements=2 );
  m_SiliconPutty->AddElement( Si, 1 );// Composition specified either by fractional mass or atom counts
  m_SiliconPutty->AddElement( O, 2 );// Composition specified either by fractional mass or atom counts
	
  m_Cu = new G4Material( "Copper", z=29., atomicMass=63.546*g/mole, density=8.920*g/cm3 );// Different than G4Element
  m_Fe = new G4Material( "Iron", z=26., atomicMass=55.85*g/mole, density=7.874*g/cm3 );// Different than G4Element
  m_Pb = new G4Material( "Lead", z=82., atomicMass=207.2*g/mole, density=11.34*g/cm3 );
	
	m_Cd = new G4Material( "Cadmium", z=48., atomicMass=112.411*g/mole, density=8.650*g/cm3 );
	m_HDPE = new G4Material( "High Density Polyethylene", density= 0.95*g/cm3, numberElements=2 );
  m_HDPE->AddElement( C, 2 );// Composition specified either by fractional mass or atom counts
  m_HDPE->AddElement( H, 4 );// Composition specified either by fractional mass or atom counts

  m_Delrin = new G4Material( "Delrin", density= 1.415*g/cm3, numberElements=3 );
  m_Delrin->AddElement( O, 1 );// Composition specified either by fractional mass or atom counts
  m_Delrin->AddElement( C, 1 );// Composition specified either by fractional mass or atom counts
  m_Delrin->AddElement( H, 2 );// Composition specified either by fractional mass or atom counts

	m_MineralOil = new G4Material( "MineralOil", density= 0.836*g/cm3, numberElements=2 );
  m_MineralOil->AddElement( C, 1 );// Composition specified either by fractional mass or atom counts
  m_MineralOil->AddElement( H, 2 );// Composition specified either by fractional mass or atom counts
	
// Stainless steel:	Fe, 0.05% C, 17% Cr, 6% Ni, 0.8% Mo, 1.5% Si, 1.5% Mn, 0.04% P, 0.015% S, 0.11% N
	const G4double SSCarbonFraction = 0.0005;
	const G4double SSChromiumFraction = 0.17;
	const G4double SSNickelFraction = 0.06;
	const G4double SSMolybdenumFraction = 0.008;
	const G4double SSSiliconFraction = 0.015;
	const G4double SSMangneseFraction = 0.015;
	const G4double SSPhosphorusFraction = 0.0004;
	const G4double SSSulfurFraction = 0.0015;
	const G4double SSNitrogenFraction = 0.0011;
	const G4double SSIronFraction = 1.0 - SSCarbonFraction - SSChromiumFraction - SSNickelFraction - 
		SSMolybdenumFraction - SSSiliconFraction - SSMangneseFraction - SSPhosphorusFraction - SSSulfurFraction - SSNitrogenFraction;
	
	m_StainlessSteel = new G4Material( "StainlessSteel", density= 8.0*g/cm3, numberElements = 10 );
  m_StainlessSteel->AddElement( C, SSCarbonFraction );// Composition specified either by fractional mass or atom counts
  m_StainlessSteel->AddElement( Cr, SSChromiumFraction );// Composition specified either by fractional mass or atom counts
  m_StainlessSteel->AddElement( Ni, SSNickelFraction );// Composition specified either by fractional mass or atom counts
  m_StainlessSteel->AddElement( Mo, SSMolybdenumFraction );// Composition specified either by fractional mass or atom counts
  m_StainlessSteel->AddElement( Si, SSSiliconFraction );// Composition specified either by fractional mass or atom counts
  m_StainlessSteel->AddElement( Mn, SSMangneseFraction );// Composition specified either by fractional mass or atom counts
  m_StainlessSteel->AddElement( P, SSPhosphorusFraction );// Composition specified either by fractional mass or atom counts
  m_StainlessSteel->AddElement( S, SSSulfurFraction );// Composition specified either by fractional mass or atom counts
  m_StainlessSteel->AddElement( N, SSNitrogenFraction );// Composition specified either by fractional mass or atom counts
  m_StainlessSteel->AddElement( Fe, SSIronFraction );// Composition specified either by fractional mass or atom counts
	
	m_PCB = new G4Material( "PCB", density= 1.8*g/cm3, numberElements = 5 );
  m_PCB->AddElement( Ca, .111 );// Composition specified either by fractional mass or atom counts
  m_PCB->AddElement( Si, .257 );// Composition specified either by fractional mass or atom counts
  m_PCB->AddElement( O, .522 );// Composition specified either by fractional mass or atom counts
  m_PCB->AddElement( Al, .079 );// Composition specified either by fractional mass or atom counts
  m_PCB->AddElement( B, .031 );// Composition specified either by fractional mass or atom counts
// OPTICAL PROPERTIES OF MINERAL OIL
//  const G4int numberEntries = 8;
//  G4double MineralOil_Energy[numberEntries]    = { 1.77*eV, 2.78*eV , 4.17*eV, 4.63*eV, 4.88*eV, 4.92*eV, 5.00*eV, 6.199*eV };

//  G4double MineralOil_SCINT[numberEntries] = { 0.001, 0.01, 0.3, 0.01, 0.01, 0.5, 0.01, 0.001 };
//  G4double MineralOil_RIND[numberEntries]  = { 1.470, 1.472, 1.49, 1.51, 1.52, 1.53, 1.55, 1.56 };
//  G4double MineralOil_ABSL = 20.*m; 
//  m_MineralOil_mt = new G4MaterialPropertiesTable();
// General Optical Properties
//  m_MineralOil_mt->AddProperty("RINDEX", MineralOil_Energy, MineralOil_RIND,  numberEntries);
//  m_MineralOil_mt->AddConstProperty("RINDEX", 1.5 );
//  m_MineralOil_mt->AddConstProperty("ABSLENGTH", MineralOil_ABSL );
// Scintillation properties...
//  m_MineralOil_mt->AddProperty("FASTCOMPONENT", MineralOil_Energy, MineralOil_SCINT, numberEntries);
//  m_MineralOil_mt->AddProperty("SLOWCOMPONENT", MineralOil_Energy, MineralOil_SCINT, numberEntries);
//  m_MineralOil_mt->AddConstProperty("SCINTILLATIONYIELD", 10./MeV);// ?? 
//  m_MineralOil_mt->AddConstProperty("RESOLUTIONSCALE",1.0);
//  m_MineralOil_mt->AddConstProperty("FASTTIMECONSTANT",20.*ns);
//  m_MineralOil_mt->AddConstProperty("SLOWTIMECONSTANT",100.*ns);
//  m_MineralOil_mt->AddConstProperty("YIELDRATIO",1.0);
//  m_MineralOil->SetMaterialPropertiesTable( m_MineralOil_mt );
	
//	m_MineralOil->GetMaterialPropertiesTable()->DumpTable();
//	m_MineralOil_mt->DumpTable();
// END OPTICAL PROPERTIES OF MINERAL OIL

	m_Water = new G4Material( "Water", density= 1.0*g/cm3, numberElements=2 );
  m_Water->AddElement( O, 1 );
  m_Water->AddElement( H, 2 );

// OPTICAL PROPERTIES OF WATER
//  const G4int numberWaterEntries = 6;
	// ??nm, 700 nm, 600nm, 500nm, 400nm, 300nm, 200nm, 100nm
//  G4double Water_Energy[numberWaterEntries] = { 1.77*eV, 2.066*eV, 2.480*eV, 3.100*eV, 4.133*eV, 6.199*eV };
//  G4double Water_RIND[numberWaterEntries]  = { 1.331, 1.334, 1.338, 1.345, 1.36, 1.436 };
//  G4double Water_ABSL = 50.*m; 

//  m_Water_mt = new G4MaterialPropertiesTable();
// Add General Optical Properties
//  m_Water_mt->AddProperty("RINDEX", Water_Energy, Water_RIND, numberWaterEntries );
//  m_Water_mt->AddConstProperty("ABSLENGTH", Water_ABSL );
//  m_Water->SetMaterialPropertiesTable( m_Water_mt );
// END OPTICAL PROPERTIES OF WATER
	
// Concrete depends on vendor. Example of concrete that (Yuri) used before is
// H - 0.6%, C - 10.4%, O - 50.6%, Mg-10.7%, Al 0.3%, Si - 1.2%, Ca - 22.9%, Fe -3.3%
// Density of concrete is somewhere between 1.6 and 2.4 g/cm3 ( from one of the internets )
  m_Concrete = new G4Material( "Concrete", density= 2.4*g/cm3, numberElements=9 );
  m_Concrete->AddElement( H,  0.56852*perCent ); // Is this by volume or mass? Composition specified either by fractional mass or atom counts. So Mass fraction
  m_Concrete->AddElement( O, 49.62966*perCent );
  m_Concrete->AddElement( Na, 2.17286*perCent );
  m_Concrete->AddElement( Mg, 0.25554*perCent );
  m_Concrete->AddElement( Al, 4.56009*perCent );
  m_Concrete->AddElement( Si,31.39440*perCent );
  m_Concrete->AddElement( K,  1.91699*perCent );
  m_Concrete->AddElement( Ca, 8.26617*perCent );
  m_Concrete->AddElement( Fe, 1.23576*perCent );
	

	m_PMTGlass = new G4Material( "PMTGlass", density= 2.6*g/cm3, numberElements=2 );// Approximate density. Can do better
  m_PMTGlass->AddElement( Si, 1 );
  m_PMTGlass->AddElement( O , 2 );
	
// OPTICAL PROPERTIES OF PMT GLASS
//  G4double m_PMTGlass_RIND = 1.3;
//  G4double m_PMTGlass_ABSL  = 2.*m;

//  const G4int numberGlassEntries = 8;
//  G4double PMTGlass_Energy[numberGlassEntries]    = { 1.77*eV, 2.78*eV , 4.17*eV, 4.63*eV, 4.88*eV, 4.92*eV, 5.00*eV, 6.199*eV };
//  G4double PMTGlass_RIND[numberGlassEntries]  = { 1.35, 1.372, 1.39, 1.41, 1.4, 1.43, 1.45, 1.46 };
//  G4double PMTGlass_RIND[numberGlassEntries]  = { 1.25, 1.272, 1.29, 1.31, 1.3, 1.33, 1.35, 1.36 };// Always less than water now...
//	G4double PMTGlass_ABSL = 20.*m; 

//  m_PMTGlass_mt = new G4MaterialPropertiesTable();
// General Optical Properties
//  m_PMTGlass_mt->AddProperty("RINDEX", PMTGlass_Energy, PMTGlass_RIND, numberGlassEntries );
//  m_PMTGlass_mt->AddConstProperty("ABSLENGTH", PMTGlass_ABSL );
//  m_PMTGlass->SetMaterialPropertiesTable( m_PMTGlass_mt );
	
//	m_PMTGlass->GetMaterialPropertiesTable()->DumpTable();
// END OPTICAL PROPERTIES OF PMT GLASS

  m_Vinyl = new G4Material( "Vinyl", density= 1.2*g/cm3, numberElements=3 );
  m_Vinyl->AddElement( C, 2 );
  m_Vinyl->AddElement( H, 3 );
  m_Vinyl->AddElement( Cl, 1 );

	m_Nylon = new G4Material( "Nylon", density= 1.14*g/cm3, numberElements=4 );
  m_Nylon->AddElement( H,  9.7976*perCent );
  m_Nylon->AddElement( C, 63.6856*perCent );
  m_Nylon->AddElement( N, 12.3779*perCent );
  m_Nylon->AddElement( O, 14.1389*perCent );

// OPTICAL PROPERTIES OF NYLON
//  const G4int numberNylonEntries = 2;
	// ??nm, 700 nm, 100nm
//  G4double Nylon_Energy[numberInScintEntries] = { 1.77*eV, 6.199*eV };
//  G4double Nylon_RIND[numberInScintEntries]  = { 1.45, 1.45 };
//  G4double Nylon_ABSL = 1.*m; // 1/2 abs length or i/e absorb length?

//  m_Nylon_mt = new G4MaterialPropertiesTable();
// Add General Optical Properties
//  m_Nylon_mt->AddProperty("RINDEX", Nylont_Energy, Nylon_RIND, numberInScintEntries );
//  m_Nylon_mt->AddConstProperty("ABSLENGTH", Nylon_ABSL );
//  m_Nylon->SetMaterialPropertiesTable( m_Nylon_mt );
// END OPTICAL PROPERTIES OF NYLON

	
  m_Teflon = new G4Material( "Teflon", density= 2.2*g/cm3, numberElements=2 );
  m_Teflon->AddElement( C, 2 );
  m_Teflon->AddElement( F, 4 );
	
  m_PTFETape = new G4Material( "PTFETape", density= 0.8*g/cm3, numberElements=2 );//Teflon tape with 20% density of regular Teflon. HA!
  m_PTFETape->AddElement( C, 2 );
  m_PTFETape->AddElement( F, 4 );

// OPTICAL PROPERTIES OF TEFLON
//  const G4int numberNylonEntries = 6;
	// ??nm, 700 nm, 600nm, 500nm, 400nm, 300nm, 200nm
//  G4double InScint_Energy[numberInScintEntries] = { 1.77*eV, 2.066*eV, 2.480*eV, 3.100*eV, 4.133*eV, 6.199*eV };
//  G4double InScint_RIND[numberInScintEntries]  = { 1.331, 1.334, 1.338, 1.345, 1.36, 1.436 };
//  G4double InScint_ABSL = 50.*m; // 1/2 abs length or i/e absorb length?

//  m_Teflon_mt = new G4MaterialPropertiesTable();
// Add General Optical Properties
//  m_Teflon_mt->AddProperty("RINDEX", InScint_Energy, InScint_RIND, numberInScintEntries );
//  m_Teflon_mt->AddConstProperty("ABSLENGTH", InScint_ABSL );
//  m_Teflon->SetMaterialPropertiesTable( m_Teflon_mt );
// END OPTICAL PROPERTIES OF TEFLON

//	m_IndiumScintillatorMaterial = new G4Material( "IndiumScintillator", density= .889*g/cm3, numberElements=3 );
  m_BC408Material = new G4Material( "BC408", density= 1.032*g/cm3, numberElements=2 );
  m_BC408Material->AddElement( H, 222 );
  m_BC408Material->AddElement( C, 201 );


// OPTICAL PROPERTIES OF INDIUM SCINTILATOR
//  const G4int numberInScintEntries = 2;
	// ??nm, 700 nm, 100nm
//  G4double InScint_Energy[numberInScintEntries] = { 1.77*eV, 6.199*eV };
//  G4double InScint_RIND[numberInScintEntries]  = { 1.5, 1.5 };
//  G4double InScint_ABSL = 10.*m; // 1/2 abs length or i/e absorb length?

//  m_InScintillator_mt = new G4MaterialPropertiesTable();
// Add General Optical Properties
//  m_IndiumScintillatorMaterial_mt->AddProperty("RINDEX", InScint_Energy, InScint_RIND, numberInScintEntries );
//  m_IndiumScintillatorMaterial_mt->AddConstProperty("ABSLENGTH", InScint_ABSL );
//  m_IndiumScintillatorMaterial->SetMaterialPropertiesTable( m_IndiumScintillatorMaterial_mt );
// END OPTICAL PROPERTIES OF INDIUM SCINTILATOR
	
/*Jeffout m_IndiumFreeScintillatorMaterial =  new G4Material( "IndiumFreeScintillator", density= 0.889*g/cm3, numberElements=2 );
  m_IndiumFreeScintillatorMaterial->AddElement( H, 12 );
  m_IndiumFreeScintillatorMaterial->AddElement( C, 9 );
*/
	m_Tin = new G4Material( "Tin", density= 7.31*g/cm3, numberElements=1 );
	m_Tin->AddElement( Sn, 1 );
	
	m_Aluminium = new G4Material( "Aluminium", density= 2.7*g/cm3, numberElements=1 );
	m_Aluminium->AddElement( Al, 1 );

// Used to model aluminum electrostatic guards
//	m_LowDensityAluminium = new G4Material( "LowDensityAluminium", density= 1.0*g/cm3, numberElements=1 );
	m_LowDensityAluminium = new G4Material( "LowDensityAluminium", density= 1.4*g/cm3, numberElements=1 );
	m_LowDensityAluminium->AddElement( Al, 1 );
	
	m_NaI = new G4Material( "SodiumIodide", density= 3.67*g/cm3, numberElements=2 );
	m_NaI->AddElement( Na, 1 );
	m_NaI->AddElement( I, 1 );
	
	const G4double percentThalium = 0.5 * perCent;
	const G4double percentNa = ( 15.34 * perCent ) * ( 1.0 - percentThalium);
	const G4double percentI = ( 84.66 * perCent ) * ( 1.0 - percentThalium);	
	
	m_NaIWithTl = new G4Material( "SodiumIodideWithThalium", density= 3.67*g/cm3, numberElements=3 );
	m_NaIWithTl->AddElement( Na, percentNa );
	m_NaIWithTl->AddElement( I, percentI );
	m_NaIWithTl->AddElement( Tl, percentThalium );
	
	m_AluminiumOxide = new G4Material( "AluminiumOxide", density= 3.9*g/cm3, numberElements=2 );
	m_AluminiumOxide->AddElement( Al, 2 );
	m_AluminiumOxide->AddElement( O, 3 );
	
	m_Textolite = new G4Material( "Textolite", density= 1.2*g/cm3, numberElements=3 );
	m_Textolite->AddElement( C, 36 );
	m_Textolite->AddElement( H, 44 );
	m_Textolite->AddElement( O, 18 );

	m_GermaniumCrystal = new G4Material( "GermaniumCrystal", density= 5.323*g/cm3, numberElements=1 );
	m_GermaniumCrystal->AddElement( Ge, 1 );

	m_BariumFlorideCrystal = new G4Material( "BariumFlorideCrystal", density= 4.89*g/cm3, numberElements=2 );
	m_BariumFlorideCrystal->AddElement( Ba, 1 );
	m_BariumFlorideCrystal->AddElement( F , 2 );
	
	m_YSO = new G4Material( "YSOCrystal", density = 4.44*g/cm3, numberElements = 3 );
	m_YSO->AddElement( Y, 2 );
	m_YSO->AddElement( Si, 1 );
	m_YSO->AddElement( O , 5 );

//  G4cout << G4endl << "The materials defined are : " << G4endl << G4endl;
//  G4cout << *( G4Material::GetMaterialTable() ) << G4endl;
}

void LENSLongDetectorConstruction::ConstructVisibilityAttributes()
{
	DestroyVisibilityAttributes();

//  m_IronVisAtt = new G4VisAttributes( G4Colour(0.6,0.6,0.6) );// Grey
	m_ScintillatorVisAtt = new G4VisAttributes( G4Colour(0.0,0.3,0.9) );// bluish
    m_ConcreteVisAtt = new G4VisAttributes( G4Colour(0.5,0.6,0.8) );//lite bluish
//	m_ScintillatorVisAtt->SetForceSolid(true);
	m_IndiumVisAtt = new G4VisAttributes( G4Colour(0.9,0.9,0.0) );// Yellow bluish;
	m_TinVisAtt = new G4VisAttributes( G4Colour(0.5,0.6,0.8) );//lite bluish;

}

void LENSLongDetectorConstruction::DestroyVisibilityAttributes()
{
//	if( m_IronVisAtt != 0 )
//	{
//		delete m_IronVisAtt;
//	}

	if( m_ScintillatorVisAtt != 0 )
	{
		delete m_ScintillatorVisAtt;
	}

	if( m_ConcreteVisAtt != 0 )
	{
		delete m_ConcreteVisAtt;
	}

	if( m_IndiumVisAtt != 0 )
	{
		delete m_IndiumVisAtt;
	}

	if( m_TinVisAtt != 0 )
	{
		delete m_TinVisAtt;
	}


}

void LENSLongDetectorConstruction::DestroyMaterials()
{
// Destroy all allocated elements and materials that were registered with geant4.
// I thought these were taken care of automatically by geant4? Hmmm.
  size_t i;
  G4MaterialTable* matTable = (G4MaterialTable*)G4Material::GetMaterialTable();
  for( i = 0; i < matTable->size(); i++ )
  {
		delete (*(matTable))[i];
	}
  matTable->clear();
	
  G4ElementTable* elemTable = (G4ElementTable*)G4Element::GetElementTable();
  for( i = 0; i < elemTable->size(); i++ )
  {
		delete (*(elemTable))[i];
	}
  elemTable->clear();
}

const G4double LENSLongDetectorConstruction::GetOpticalPhotonEmissionFraction() const
{
	if( m_LENSLongBoxScintillatorSD != 0 )// This is a safety check.
	{
		return m_LENSLongBoxScintillatorSD->GetOpticalPhotonEmissionFraction();
	}
	else
	{
		return 1.0;
	}
}


void LENSLongDetectorConstruction::ConstructLENSLongHall()
{
// SNS hall
// After rotation X-axis is Drawing east, the Y-Axis is drwing north, and the Z axis is up. 
// The directions are from the target building overall framing plan S1.01.30 on the SNS website
  G4double snsHallHalf_x = 3.0*m;
  G4double snsHallHalf_y = 3.0*m;
  G4double snsHallHalf_z = 3.0*m;
	
  G4Box* LENSLongHall_box = new G4Box( "snsHall_box", snsHallHalf_x, snsHallHalf_y, snsHallHalf_z );
//  m_pSNSHall_logVol = new G4LogicalVolume( snsHall_box,  m_Air, "SNSHall_logical", 0, 0, 0 );
  m_pLENSLongHall_logVol = new G4LogicalVolume( LENSLongHall_box,  m_Vacuum, "LENSLongHall_logical", 0, 0, 0 );// Makes things slightly faster
  m_pLENSLongHall_phyVol = new G4PVPlacement( m_LENSLongHallRotation, G4ThreeVector(0.0,0.0,0.0), m_pLENSLongHall_logVol, 
																					"LENSLongHall_physical", m_pWorld_logVol, false, 0 );

  if( m_UseFloor )
  {
//    G4double floorHalfDepth = 0.5 * GetFloorThickness();
//    G4double floorZOffset = -0.5 * GetBunkerHeight() - GetBunkerPadding() - floorHalfDepth + m_vBunkerCenter[2];
//    G4ThreeVector floorOffset = G4ThreeVector( 0.0*m,0.0*m, floorZOffset );

//    G4VSolid* floor_Solid = new G4Box( "ConcreteFloor", snsHallHalf_x, snsHallHalf_y, floorHalfDepth );
//    m_pFloor_logVol = new G4LogicalVolume( floor_Solid, m_Concrete, "ConcreteFloorLogical", 0, 0, 0 );
//    m_pFloor_phyVol = new G4PVPlacement( 0, floorOffset, m_pFloor_logVol, "ConcreteFloorPhysical", m_pLENSLongHall_logVol, 0, 0 );

//    m_pFloor_logVol->SetVisAttributes( m_ConcreteVisAtt );
  }
}

// Sets up sensitive detector
void LENSLongDetectorConstruction::ConstructLENSLongScintillator()
{
  G4String SDname;
  m_LENSLongBoxScintillatorSD = new LENSLongLiquidScintillator( SDname = "/LiquidScintillator" );
// The "this" pointer alows the LENSLongLiquidScintillator to get things such as BunkerWidth(), Materials, etc...
	m_LENSLongBoxScintillatorSD->ConstructLiquidDetector( this ); 
}

void LENSLongDetectorConstruction::DumpGeometricalTree(G4VPhysicalVolume* aVolume,G4int depth)
{
  for( int isp = 0; isp < depth; isp++ )
  { G4cout << "  "; }
  G4cout << aVolume->GetName() << "[" << aVolume->GetCopyNo() << "] "
         << aVolume->GetLogicalVolume()->GetName() << " "
         << aVolume->GetLogicalVolume()->GetNoDaughters() << " "
         << aVolume->GetLogicalVolume()->GetMaterial()->GetName();
  if( aVolume->GetLogicalVolume()->GetSensitiveDetector() )
  {
    G4cout << " " << aVolume->GetLogicalVolume()->GetSensitiveDetector()->GetFullPathName();
  }
  G4cout << G4endl;
  for( int i = 0; i < aVolume->GetLogicalVolume()->GetNoDaughters(); i++ )
  { DumpGeometricalTree(aVolume->GetLogicalVolume()->GetDaughter(i),depth+1); }
}


void LENSLongDetectorConstruction::GetHallRotation( G4RotationMatrix& rotation ) const
{
	rotation = m_pLENSLongHall_phyVol->GetObjectRotationValue();
}


void LENSLongDetectorConstruction::GetBeamPipeLaunchCoordinateGlobal( G4ThreeVector& center ) const
{
	if( m_LENSLongBoxScintillatorSD != 0 )
	{
		m_LENSLongBoxScintillatorSD->GetBeamPipeLaunchCoordinateGlobal( center );
	}
	else
	{
		center = G4ThreeVector(0.0,0.0,0.0);
	}
}
void LENSLongDetectorConstruction::GetHitModuleCenterGlobal( G4ThreeVector& center ) const
{
	if( m_LENSLongBoxScintillatorSD != 0 )
	{
		m_LENSLongBoxScintillatorSD->GetHitModuleCenterGlobalCoord( center );
	}
	else
	{
		center = G4ThreeVector(0.0,0.0,0.0);
	}
}


const G4double LENSLongDetectorConstruction::GetCellX() const
{
	G4double returnValue = 0.0;
	if( m_LENSLongBoxScintillatorSD != 0 )
	{
		returnValue = m_LENSLongBoxScintillatorSD->GetSodiumIodideX();
	}
	
	return returnValue;
}

const G4double LENSLongDetectorConstruction::GetCellY() const
{
	G4double returnValue = 0.0;
	if( m_LENSLongBoxScintillatorSD != 0 )
	{
		returnValue = m_LENSLongBoxScintillatorSD->GetSodiumIodideY();
	}

	return returnValue;
}

const G4double LENSLongDetectorConstruction::GetCellZ() const
{
	G4double returnValue = 0.0;
	if( m_LENSLongBoxScintillatorSD != 0 )
	{
		returnValue = m_LENSLongBoxScintillatorSD->GetSodiumIodideZ();
	}

	return returnValue;
}

const G4double LENSLongDetectorConstruction::GetCellXOffset() const
{
	G4double returnValue = 0.0;
	if( m_LENSLongBoxScintillatorSD != 0 )
	{
		returnValue = 0.0;//m_LENSLongBoxScintillatorSD->GetIndiumCellY();
	}
	
	return returnValue;
}
const G4double LENSLongDetectorConstruction::GetCellYOffset() const
{
	G4double returnValue = 0.0;
	if( m_LENSLongBoxScintillatorSD != 0 )
	{
		returnValue = 0.0;//m_LENSLongBoxScintillatorSD->GetIndiumCellY();
	}
	
	return returnValue;
}
const G4double LENSLongDetectorConstruction::GetCellZOffset() const
{
	G4double returnValue = 0.0;
	if( m_LENSLongBoxScintillatorSD != 0 )
	{
		returnValue = 0.0;//m_LENSLongBoxScintillatorSD->GetIndiumCellY();
	}
	
	return returnValue;
}

const G4double LENSLongDetectorConstruction::GetFullDetectorWidth() const
{
	G4double returnValue = 0.0;
	if( m_LENSLongBoxScintillatorSD != 0 )
	{
		returnValue = m_LENSLongBoxScintillatorSD->GetDetectorWidth();
	}
	
	return returnValue;
}

const G4double LENSLongDetectorConstruction::GetFullDetectorHeight() const
{
	G4double returnValue = 0.0;
	if( m_LENSLongBoxScintillatorSD != 0 )
	{
		returnValue = m_LENSLongBoxScintillatorSD->GetDetectorHeight();
	}
	
	return returnValue;
}

const G4double LENSLongDetectorConstruction::GetFullDetectorDepth() const
{
	G4double returnValue = 0.0;
	if( m_LENSLongBoxScintillatorSD != 0 )
	{
		returnValue = m_LENSLongBoxScintillatorSD->GetDetectorDepth();
	}
	
	return returnValue;
}




