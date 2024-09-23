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
// $Id: A01Hodoscope.cc,v 1.6 2003/08/20 16:32:50 duns Exp $
// --------------------------------------------------------------
//

#include "LENSLongLiquidScintillator.hh"
//#include "LENSLongLiquidScintillatorHit.hh"
#include "LENSLongDetectorConstruction.hh"

#include "G4HCofThisEvent.hh"
#include "G4TouchableHistory.hh"
#include "G4Track.hh"
#include "G4Step.hh"
#include "G4SDManager.hh"
#include "G4ios.hh"

#include "G4Box.hh"
#include "G4Cons.hh"
#include "G4CutTubs.hh"
#include "G4Tubs.hh"
#include "G4Trd.hh"
#include "G4Torus.hh"
#include "G4Sphere.hh"
#include "G4Polyhedra.hh"
#include "G4SubtractionSolid.hh"
#include "G4UnionSolid.hh"
#include "G4IntersectionSolid.hh"

#include "G4LogicalBorderSurface.hh"
#include "G4LogicalVolume.hh"
#include "G4Material.hh"
#include "G4MaterialTable.hh"
#include "G4OpBoundaryProcess.hh"

#include "G4VisAttributes.hh"
#include "G4Colour.hh"

#include "G4PVPlacement.hh"
#include "G4SDManager.hh"
#include "G4VSensitiveDetector.hh"

#include <stdio.h>
#include <math.h>

using namespace CLHEP;


LENSLongLiquidScintillator::LENSLongLiquidScintillator(G4String name): G4VSensitiveDetector(name)
, m_OpticalPhotonHitsCollection(0)
, m_GammaHitsCollection(0)
, m_SingleModuleOnly(false)
{
// Output info or not.
	m_OutputInfo = true;
// Track Optical photons(Really? Is that what this means? WOW)
	m_TrackOpticalPhotons = false;
	m_UseRoughSurfaces = true;
	
// This is the fraction to reduce the number of scintillator optical photons related to the actual PMT QE. 
// It also reduces the number of cerenkov optical photons.
// If this is set to 1.0 then all of the photons are created
 	m_PhotonEmissionFraction = 0.3175;// Should be 25.4%?
	
// Sensitive detector stuff
  G4String HCname;
  collectionName.insert( HCname="OpticalPhoton" );// Is this the "collectionName" variable in G4VSensitiveDetector? Yes.
  collectionName.insert( HCname="Gamma" );// Is this the "collectionName" variable in G4VSensitiveDetector? Yes.

	m_OpticalPhotonHitsCollectionID = -1;
	m_GammaHitsCollectionID = -1;

	m_NaI_mt = 0;
	m_Teflon_mt = 0;
	m_PMT_mt = 0;
	m_Air_mt = 0;
	
	m_GasVisAtt = 0;
	m_CellWallVisAtt = 0;
	m_IndiumCellVisAtt = 0;
	m_PMTGlassVisAtt = 0;
	m_PMTHitGlassVisAtt = 0;
	
//	const G4double BEGeHeight = 30.0 * mm;
//	const G4double BEGeDiamter = 85.0 * mm;
//
//	m_SodiumIodideDimensions.push_back( G4ThreeVector( BEGeDiamter, BEGeDiamter, BEGeHeight ) );

//	const G4double NIST_GeHeight = 55.0 * mm;
//	const G4double NIST_GeDiamter = 2.0 * 62.5 * mm;

	const G4double NIST_GeHeight = 30.0 * mm;
	const G4double NIST_GeDiamter = 85.0 * mm;
	m_SodiumIodideDimensions.push_back( G4ThreeVector( NIST_GeDiamter, NIST_GeDiamter, NIST_GeHeight ) );
// KEEP THE ORDER OF THESE, IT REALLY COUNTS FOR SOMETHING....

	m_SourceLocation = G4ThreeVector( 0.0, 0.0, 0.0 );
	//m_SourceLocation = G4ThreeVector( 0.0, 0.5 * hexagonHeight + 1.0 * 2.54 * cm, 0.0 * moduleHalfLength );// SET: m_SourceAlongFace = true;
	//m_SourceLocation = G4ThreeVector( sqrt(3.0) * hexagonHeight, 0.0, 0.0 * moduleHalfLength );// SET: m_SourceAlongFace = false;
	
	m_SourceRadius = 1.0 * mm;
// if source is located along X axis (along an edge) this is false, if the source is along the y axis (along a module face) this is true.
	
	m_Layer0Thickness = 0.81 * mm;//0.4 * mm;//0.762 * mm;// Carbon Fiber
	m_Layer1Thickness = 0.04*mm;// Stainless Steel
	m_Layer2Thickness = 2.025 * mm;// Silicon Putty
	m_Layer3Thickness = 0.5 * mm;// Teflon

//	m_Layer0Thickness = 0.5 * mm;//Aluminum
//	m_Layer1Thickness = 0.5 * mm;//Aluminum
//	m_Layer2Thickness = 1.5875 * mm;// Silicon Putty
//	m_Layer3Thickness = 0.5 * mm;// Teflon
	
	m_Layer0InCenterHoleThickness = m_Layer0Thickness;//0.8128 * mm;//0.4 * mm;//0.762 * mm;// Carbon Fiber// Outside
	m_Layer1InCenterHoleThickness = m_Layer1Thickness;// Stainless Steel
	m_Layer2InCenterHoleThickness = 0.68*mm;//m_Layer2Thickness;//0.682*mm;//1.5875 * mm;// Silicon Putty
	m_Layer3InCenterHoleThickness = m_Layer3Thickness;//0.5 * mm;// Teflon // touches NaI

	m_GlassWindowThickness = 0.5 * 2.54 * cm;// 1/2"
	m_AluminumEndPieceThickness = m_GlassWindowThickness + m_PMTLength;

	m_CadmiumThickness = 1.0 * mm;
	m_HDPEThickness = 5.0 * mm;

	m_NaIHoleRadius = 3.175 * cm;// 2.5" hole
	
//  m_PipeOuterRadius = 2.54 * cm; // 2" Pipe
  //  m_PipeOuterRadius = 2.54 * cm+1.0*mm; // 2" Pipe
  m_PipeOuterRadius = 1.1 * 2.54 * cm;// - 0.25 *mm; // 2" Pipe - .5 mm shaved off
	m_PipeThickness = 1.0*mm;//0.8 * mm;// Thickness of 2" Pipe .76 to 1.mm for Ola
	
	m_UseSingleHoledCrystal = false;// Places center holed crystal in simulation.
	m_UseNeutronAbsorber = false;
	
	m_RollerRadius = 1.0 * mm;
	m_RollerLength = 0.7 * 2.54 * cm;
}

LENSLongLiquidScintillator::~LENSLongLiquidScintillator()
{
// Clean up the rotations.
	const G4int numberRotations = (G4int) ( m_Rotations.size() );
	for( G4int i = 0; i < numberRotations; ++i )
	{
		delete m_Rotations[i];
	}
	
	m_Rotations.clear();
	
	unsigned int numberVisAtt = m_vVisAttributes.size();
	
	for( unsigned int i = 0; i < numberVisAtt; i++ )
	{
		delete m_vVisAttributes[i];
	}
	
// Destroy visiblity attributes
	if( m_ReflectiveFoilVisAtt != 0 )
	{
		delete m_ReflectiveFoilVisAtt;
	}
	if( m_GasVisAtt != 0 )
	{
		delete m_GasVisAtt;
	}
	if( m_CellWallVisAtt != 0 )
	{
		delete m_CellWallVisAtt;
	}
	if( m_IndiumCellVisAtt != 0 )
	{
		delete m_IndiumCellVisAtt;
	}
	if( m_PMTGlassVisAtt != 0 )
	{
		delete m_PMTGlassVisAtt;
	}

	if( m_PMTHitGlassVisAtt != 0 )
	{
		delete m_PMTHitGlassVisAtt;
	}
	
}

void LENSLongLiquidScintillator::GetHitModuleCenterGlobalCoord( G4ThreeVector& centerG ) const
{
	centerG[0] = 0.0;
	centerG[1] = 0.0;
	centerG[2] = 0.0;
}


void LENSLongLiquidScintillator::ConstructLiquidDetector( LENSLongDetectorConstruction* mainDetectorConstructor )
{

	m_MainDetectorConstructor = mainDetectorConstructor;
	
// Start making stuff...
	
// Make the Air around the detector...
	this->makeAirAroundDetector();
//	this->makeTable();// Use with Single module only!
// MUST MAKE THE MODULES FIRST
	this->makeBasicModules();// Makes, but does not place, Modules and PMTs in world, that is done next.
	
	//this->makePipe();// Need to be called after the makeBasicModule, since that determines the size of the detector.
	// Moved to inside of makeBasicModule() in order to place inside of messed up module.

// ONLY USE ONE OF THESE. I am not kidding, if you use more than one they will all be made and overlap each other 
// with random crap results.
	this->makeDefaultSingleModule();
  
	// OR use this (which strangely counts as one)...
//	this->makeSingleFourLayer();
//  this->makeTwoFourLayer();
//  this->makeThreeFourLayer();

//  this->makeSingleEightLayer();
//  this->makeTwoEightLayer();

// 		this->makeBEGeCube();
//		this->makeCloverCube();
//		this->makeSquareOrthocupola();
    
//    this->makeCloverSquareOrthocupola();
//    this->makeCloverRhombicuboctahedron();

//    this->makeGRIFFINCube();
//    this->makeGRIFFINSquareOrthocupola();
//    this->makeGRIFFINRhombicuboctahedron();
    
// END OF USE ONLY ONE

// Make this or not, it should not overlap with the other parts of the detector.
//	this->makeTargetChamber();

// SET UP AS A SENSITIVE DETECTOR
	G4SDManager* SDman = G4SDManager::GetSDMpointer();
	SDman->AddNewDetector( this );
	G4int number = m_NaI_LogicalVolumes.size();
// Set volumes to record eDep in. All NaI crystals.
	for( int i = 0; i < number; ++i ) m_NaI_LogicalVolumes[i]->SetSensitiveDetector( this );

// Add later if needed. May confuse total summing of NaI modules.
	number = m_Si_LogicalVolumes.size();
	for( int i = 0; i < number; ++i ) m_Si_LogicalVolumes[i]->SetSensitiveDetector( this );

// Vis. Attributes.
//	m_ReflectiveFoilVisAtt = new G4VisAttributes( G4Colour(0.9,0.0,0.5) );// purple
	m_ReflectiveFoilVisAtt = new G4VisAttributes( G4Colour(0.3,0.4,0.4,0.005) );// gray
	//m_ReflectiveFoilVisAtt->SetForceAuxEdgeVisible(true);
	//m_ReflectiveFoilVisAtt->SetLineWidth(2.0);
	number = m_ModuleTotal_LogicalVolumes.size();
	for( int i = 0; i < number; ++i )
	{
//	  m_ModuleTotal_LogicalVolumes[i]->SetVisAttributes( G4VisAttributes::GetInvisible() );
	}
	
	number = m_Layer0_LogicalVolumes.size();
	for( int i = 0; i < number; ++i )
	{
		m_Layer0_LogicalVolumes[i]->SetVisAttributes( m_ReflectiveFoilVisAtt );//G4VisAttributes::GetInvisible() );
	}
	
	number = m_Layer1_LogicalVolumes.size();
	for( int i = 0; i < number; ++i )
	{
		m_Layer1_LogicalVolumes[i]->SetVisAttributes( m_ReflectiveFoilVisAtt );//G4VisAttributes::GetInvisible() );
	}
	
	number = m_Layer2_LogicalVolumes.size();
	for( int i = 0; i < number; ++i )
	{
		m_Layer2_LogicalVolumes[i]->SetVisAttributes( m_ReflectiveFoilVisAtt );//G4VisAttributes::GetInvisible() );
	}
	
	number = m_Layer3_LogicalVolumes.size();
	for( int i = 0; i < number; ++i )
	{
		m_Layer3_LogicalVolumes[i]->SetVisAttributes( m_ReflectiveFoilVisAtt );//G4VisAttributes::GetInvisible() );
	}	
	
	//	for( int i = 0; i < number; ++i ) m_ModuleTotal_LogicalVolumes[i]->SetVisAttributes( G4VisAttributes::GetInvisible() );
	
	m_GasVisAtt = new G4VisAttributes( G4Colour(0.5,0.5,0.99 ) );//, 0.75) );// light blue
	//m_GasVisAtt->SetForceAuxEdgeVisible(true);
//	m_GasVisAtt->SetForceSolid(true);
	number = m_Gas_LogicalVolumes.size();
	m_Gas_LogicalVolumes[0]->SetVisAttributes( G4VisAttributes::GetInvisible() );
	for( int i = 1; i < number; ++i ) m_Gas_LogicalVolumes[i]->SetVisAttributes( m_GasVisAtt );
//	for( int i = 1; i < number; ++i ) m_Gas_LogicalVolumes[i]->SetVisAttributes( G4VisAttributes::GetInvisible() );

	//for( int i = 0; i < number; ++i ) m_Gas_LogicalVolumes[i]->SetVisAttributes( G4VisAttributes::GetInvisible() );

//	m_CellWallVisAtt = new G4VisAttributes( G4Colour(0.8,0.8,0.2) );// vomit yellow
	m_CellWallVisAtt = new G4VisAttributes( G4Colour(0.1,0.81,0.1,0.5) );// green
//	m_CellWallVisAtt->SetForceSolid(true);
	number = m_NaI_LogicalVolumes.size();
	for( int i = 0; i < number; ++i ) m_NaI_LogicalVolumes[i]->SetVisAttributes( m_CellWallVisAtt );
	
	m_PMTGlassVisAtt = new G4VisAttributes( G4Colour(0.9,0.1,0.1) );// redish gray
//	m_PMTGlassVisAtt->SetForceAuxEdgeVisible(true);// Can see outline when drawn with lines
	m_PMTGlassVisAtt->SetForceSolid(true);
	number = m_PMT_LogicalVolumes.size();
	for( int i = 0; i < number; ++i ) m_PMT_LogicalVolumes[i]->SetVisAttributes( m_PMTGlassVisAtt );

	G4VisAttributes* nextVisAtt = new G4VisAttributes( G4Colour(0.5,0.5,0.5,0.2) );// light gray
	nextVisAtt->SetForceSolid(true);
	m_vVisAttributes.push_back(nextVisAtt);
	
	number = m_Pipe_LogicalVolumes.size();
	for( int i = 0; i < number; ++i ) m_Pipe_LogicalVolumes[i]->SetVisAttributes( nextVisAtt );
//	for( int i = 0; i < number; ++i ) m_Pipe_LogicalVolumes[i]->SetVisAttributes( G4VisAttributes::GetInvisible() );
	
	nextVisAtt = new G4VisAttributes( G4Colour(0.6,0.35,0.05) );// brownish
	nextVisAtt->SetForceSolid(true);
	m_vVisAttributes.push_back(nextVisAtt);// This keeps track of it and deletes it at the end of the day.
	
	number = m_Si_LogicalVolumes.size();
	for( int i = 0; i < number; ++i ) m_Si_LogicalVolumes[i]->SetVisAttributes( nextVisAtt );
	
// Shield around source	
	nextVisAtt = new G4VisAttributes( G4Colour(0.2,0.2,0.3) );// Dark blue grey
	//nextVisAtt->SetForceSolid(true);
	m_vVisAttributes.push_back(nextVisAtt);// This keeps track of it and deletes it at the end of the day.

	number = m_Shield_LogicalVolumes.size();
	for( int i = 0; i < number; ++i ) m_Shield_LogicalVolumes[i]->SetVisAttributes( nextVisAtt );
	
	nextVisAtt = new G4VisAttributes( G4Colour(0.3,0.3,0.9,0.5) );// Blue grey
	nextVisAtt->SetForceSolid(true);
	m_vVisAttributes.push_back(nextVisAtt);// This keeps track of it and deletes it at the end of the day.
	number = m_PMT_Shield_LogicalVolumes.size();
	for( int i = 0; i < number; ++i ) m_PMT_Shield_LogicalVolumes[i]->SetVisAttributes( nextVisAtt );

// Wire Housing	
	nextVisAtt = new G4VisAttributes( G4Colour(0.75,0.6,0.1,1.0) );// YELLOW
//  nextVisAtt->SetForceSolid(true);
	m_vVisAttributes.push_back(nextVisAtt);// This keeps track of it and deletes it at the end of the day.

	number = m_Wire_LogicalVolumes.size();
	for( int i = 0; i < number; ++i ) m_Wire_LogicalVolumes[i]->SetVisAttributes( nextVisAtt );

// Cadmium	
	nextVisAtt = new G4VisAttributes( G4Colour(0.9,0.1,0.1) );// RED!
	//nextVisAtt->SetForceSolid(true);
	m_vVisAttributes.push_back(nextVisAtt);// This keeps track of it and deletes it at the end of the day.

	number = m_Cd_LogicalVolumes.size();
	for( int i = 0; i < number; ++i ) m_Cd_LogicalVolumes[i]->SetVisAttributes( nextVisAtt );
	
// HDPE
	nextVisAtt = new G4VisAttributes( G4Colour(0.9,0.9,0.9) );// light gray!
	//nextVisAtt->SetForceSolid(true);
	m_vVisAttributes.push_back(nextVisAtt);// This keeps track of it and deletes it at the end of the day.

	number = m_HDPE_LogicalVolumes.size();
	for( int i = 0; i < number; ++i ) m_HDPE_LogicalVolumes[i]->SetVisAttributes( nextVisAtt );
	
	// Glass window 
	nextVisAtt = new G4VisAttributes( G4Colour(0.7,0.7,0.1) );//,0.05) );// light yellow!
	//nextVisAtt->SetForceSolid(true);
	m_vVisAttributes.push_back(nextVisAtt);// This keeps track of it and deletes it at the end of the day.

	number = m_Glass_LogicalVolumes.size();
	for( int i = 0; i < number; ++i ) m_Glass_LogicalVolumes[i]->SetVisAttributes( nextVisAtt );
	
	G4LogicalVolume* worldLogical = m_MainDetectorConstructor->GetWorldLogicalVolume();
	worldLogical->SetVisAttributes( G4VisAttributes::GetInvisible() );
}


//****************************************************************************************

void LENSLongLiquidScintillator::makeAirAroundDetector()
{
	G4Material* gasMaterial = m_MainDetectorConstructor->GetAir();
	G4LogicalVolume* worldLogical = m_MainDetectorConstructor->GetWorldLogicalVolume();

	const G4double airTotalX = 300.0*cm;
	const G4double airTotalY = 300.0*cm;
	const G4double airTotalZ = 300.0*cm;

	const G4double halfAirTotalX = airTotalX * 0.5;
	const G4double halfAirTotalY = airTotalY * 0.5;
	const G4double halfAirTotalZ = airTotalZ * 0.5;
	
	const G4ThreeVector totalAirDimension( airTotalX, airTotalY, airTotalZ );
	
	// Make total cell entirely of reflective foil then fill in the inside structure.
	G4VSolid* air_Solid = new G4Box( "Air_Solid", halfAirTotalX, halfAirTotalY, halfAirTotalZ );
	G4LogicalVolume* air_logVol = new G4LogicalVolume( air_Solid, gasMaterial, "Air_Logical");
	m_Gas_LogicalVolumes.push_back( air_logVol );
	
	const G4ThreeVector zero( 0.0, 0.0, 0.0 );
	
	new G4PVPlacement( 0, zero, m_Gas_LogicalVolumes[0], "AirVolume", worldLogical, 0, 0 );
	
}

void LENSLongLiquidScintillator::makeTable()
{
	const G4double halfTableX = 1.0*m; 
	const G4double halfTableY = 0.25*cm; 
	const G4double halfTableZ = 1.0*m;
	 
	const G4double offsetYTable = -0.5 * ( m_SodiumIodideDimensions[0][1] + 
				2.0 * ( m_Layer0Thickness + m_Layer1Thickness + m_Layer2Thickness + m_Layer3Thickness ) ) - halfTableY;
				
	G4Material* tableMaterial = m_MainDetectorConstructor->GetAluminium();
	G4Box	*table_Solid = new G4Box( "table_Solid", halfTableX, halfTableY, halfTableZ );
	
	G4LogicalVolume* table_LogVol = new G4LogicalVolume( table_Solid, tableMaterial, "table_Logical");
	m_Pipe_LogicalVolumes.push_back( table_LogVol );
	
	G4ThreeVector placement = G4ThreeVector( 0.0, offsetYTable, 0.0 );
		new G4PVPlacement( 0, placement, table_LogVol, "table_Physical", m_Gas_LogicalVolumes[0], 0, 0 );
}


void  LENSLongLiquidScintillator::makePipe()
{// place in m_Gas_LogicalVolumes[0]
	G4Material* vacuumMaterial = m_MainDetectorConstructor->GetVacuum();
	G4Material* pipeMaterial = m_MainDetectorConstructor->GetAluminium();
	const G4ThreeVector zero( 0.0, 0.0, 0.0 );

	const G4double innerRadius = m_PipeOuterRadius - m_PipeThickness;
	const G4double outerRadius = m_PipeOuterRadius;
	const G4double halfLength = 0.5 * this->GetDetectorWidth();// Makes it same size as the detector
	const G4double startPhi = 0.0 * degree;
	const G4double deltaPhi = 360.0 * degree;
	const G4double pipeHalfThickness = m_PipeThickness * 0.5;
		
	G4VSolid* pipe_Cylinder =  new G4Tubs("PipeCylinder_Solid", innerRadius, outerRadius, halfLength, startPhi, deltaPhi);	
	G4LogicalVolume* pipe_LogVol = new G4LogicalVolume( pipe_Cylinder, pipeMaterial, "Pipe_Logical");
	m_Pipe_LogicalVolumes.push_back( pipe_LogVol );

	G4VSolid* pipeEnd_Cylinder =  new G4Tubs("PipeEndCylinder_Solid", 0.0, innerRadius, pipeHalfThickness, startPhi, deltaPhi);	
	G4LogicalVolume* pipeEnd_LogVol = new G4LogicalVolume( pipeEnd_Cylinder, pipeMaterial, "PipeEnd_Logical");
	m_Pipe_LogicalVolumes.push_back( pipeEnd_LogVol );	
	G4ThreeVector placement( 0.0, 0.0, -halfLength + pipeHalfThickness );
	
	G4VSolid* vacuum_Cylinder =  new G4Tubs("VacuumCylinder_Solid", 0.0, innerRadius, halfLength, startPhi, deltaPhi);	
	G4LogicalVolume* vacuum_LogVol = new G4LogicalVolume( vacuum_Cylinder, vacuumMaterial, "Vacuum_Logical");
	m_Gas_LogicalVolumes.push_back( vacuum_LogVol );
	
	new G4PVPlacement( 0, placement, pipeEnd_LogVol, "PipeVacuum_Physical", vacuum_LogVol, 0, 0 );

	new G4PVPlacement( 0, zero, vacuum_LogVol, "PipeVacuum_Physical", pipe_LogVol, 0, 0 );

	G4RotationMatrix* rot = new G4RotationMatrix();//(matrixColumn1, matrixColumn2, matrixColumn3);
	rot->rotateY( 90 * degree );
	m_Rotations.push_back(rot);// automatically deletes it later...
	
	const G4double offsetFromCenter = 6.35 * mm;//1/4 inch
	placement = G4ThreeVector(-halfLength + offsetFromCenter,0.0,0.0);
	new G4PVPlacement( rot, placement, pipe_LogVol, "Pipe_Physical", m_Gas_LogicalVolumes[0], 0, 0 );

}

void  LENSLongLiquidScintillator::makeBasicModules()
{
//	G4LogicalVolume* gasLogical = m_Gas_LogicalVolumes[0];//m_MainDetectorConstructor->GetWorldLogicalVolume();

	const G4int numberDetectorTypes = 6;//m_SodiumIodideDimensions.size();
	//const G4int numberNaIDimensions = m_SodiumIodideDimensions.size();
	if( numberDetectorTypes < 1 )
	{
		G4cout << "No Detector sizes are set! Much Badness" << G4endl;
		return;
	}

	const G4ThreeVector zero( 0.0, 0.0, 0.0 );
	G4ThreeVector placement(zero);

	G4Material* vacuumMaterial = m_MainDetectorConstructor->GetVacuum();
	G4Material* alMaterial = m_MainDetectorConstructor->GetAluminium();
//	G4Material* airMaterial = m_MainDetectorConstructor->GetAir();
	G4Material* germaniumMaterial = m_MainDetectorConstructor->GetGermaniumCrystal();
	G4Material* cuMaterial = m_MainDetectorConstructor->GetCopper();
  G4Material* carbonFiberMaterial = m_MainDetectorConstructor->GetCarbonFiber();

//	G4Material* cadmium_Material = m_MainDetectorConstructor->GetCadmium();
//	G4Material* HDPE_Material = m_MainDetectorConstructor->GetAluminium();//->GetHDPE();
//	G4Material* Si_Material = m_MainDetectorConstructor->GetSilicon();
//	G4Material* PCB_Material = m_MainDetectorConstructor->GetDelrin();//->GetPCB();
	
	const G4double  startPhi = 0.0 * degree;
	const G4double  deltaPhi = 360.0 * degree;

	// Used to line up all of the cylinders in the detector along the x-axis
	G4RotationMatrix* rotY90 = new G4RotationMatrix();//(matrixColumn1, matrixColumn2, matrixColumn3);
	rotY90->rotateY( 90 * degree );
	m_Rotations.push_back(rotY90);

	G4RotationMatrix* rotY180 = new G4RotationMatrix();//(matrixColumn1, matrixColumn2, matrixColumn3);
	rotY180->rotateY( 180 * degree );
	m_Rotations.push_back(rotY180);
		
	for( int i = 0; i < numberDetectorTypes; ++i )
	{
	// START BASIC GEBe Detector
		if( i == 0 )
		{
      const G4double cuThick = 0.50 * mm;
      const G4double geDeadLayerThick = 0.0050 * mm;
      const G4double vacuumThick = 1.0 * cm;
      const G4double vacuumFrontThick = 1.0 * cm;
      const G4double alThick = 0.5 * mm;
      const G4double alFrontThick = 0.6 * mm;// Used fro Carbon fiber front thickness.
      
      //The aluminum can
      const G4double canhalfZ = 0.5 * 40.0 * cm;
      const G4double canInnerRad = 0.0 * mm;
      const G4double canOuterRad = 0.5 * m_SodiumIodideDimensions[0][0] + cuThick + vacuumThick + alThick;
      G4VSolid* alOuter_Can_Solid =  new G4Tubs("Al_Can_Solid", canInnerRad, canOuterRad, canhalfZ, startPhi, deltaPhi);	
      G4LogicalVolume* alOuter_Can_LV = new G4LogicalVolume( alOuter_Can_Solid, alMaterial, "Al_Can_Logical");
      m_Pipe_LogicalVolumes.push_back( alOuter_Can_LV );// USED FOR DRAWING
      m_ModuleTotal_LogicalVolumes.push_back( alOuter_Can_LV );// USED FOR DRAWING and OVERALL PLACEMENT

      // Place the vacuum in the can
      const G4double vachalfZ = canhalfZ - 0.5 * alFrontThick;
      const G4double vacInnerRad = 0.0 * mm;
      const G4double vacOuterRad = canOuterRad - alThick;
      G4VSolid* vacuum_Can_Solid =  new G4Tubs("Vacuum_Can_Solid", vacInnerRad, vacOuterRad, vachalfZ, startPhi, deltaPhi);	
      G4LogicalVolume* vacuume_Can_LV = new G4LogicalVolume( vacuum_Can_Solid, vacuumMaterial, "Vacuum_Can_Logical");
      m_Gas_LogicalVolumes.push_back( vacuume_Can_LV );// USED FOR DRAWING
      
      placement = G4ThreeVector( 0.0*mm, 0.0*mm, canhalfZ - alFrontThick - vachalfZ );
      new G4PVPlacement( 0, placement, vacuume_Can_LV, "Vacuum_in_Can_Volume", m_ModuleTotal_LogicalVolumes[0], 0, 0 );
      
      // Make the carbon fiber front window
      const G4double cFhalfZ = 0.5 * alFrontThick;
      const G4double cFInnerRad = 0.0 * mm;
      const G4double cFOuterRad = canOuterRad;// - alThick;
      G4VSolid* cF_Can_Solid =  new G4Tubs("Vacuum_Can_Solid", cFInnerRad, cFOuterRad, cFhalfZ, startPhi, deltaPhi);	
      G4LogicalVolume* cF_Front_LV = new G4LogicalVolume( cF_Can_Solid, carbonFiberMaterial, "CFm_Can_Logical");
      
      m_PMT_LogicalVolumes.push_back( cF_Front_LV );// USED FOR DRAWING

      placement = G4ThreeVector( 0.0*mm, 0.0*mm, canhalfZ - cFhalfZ );
      new G4PVPlacement( 0, placement, cF_Front_LV, "Vacuum_in_Can_Volume", m_ModuleTotal_LogicalVolumes[0], 0, 0 );

      //the copper cooling pipe
      const G4double cuHalfZ = vachalfZ - 0.5 * vacuumFrontThick;
      const G4double cuInnerRad = 0.0 * mm;
      const G4double cuOuterRad = vacOuterRad - vacuumThick;
      G4VSolid* cu_Support_Solid =  new G4Tubs("Cu_Support_Solid", cuInnerRad, cuOuterRad, cuHalfZ, startPhi, deltaPhi);	
      G4LogicalVolume* cu_Support_LV = new G4LogicalVolume( cu_Support_Solid, cuMaterial, "Cu_Support_Logical");
      m_Wire_LogicalVolumes.push_back( cu_Support_LV );// USED FOR DRAWING
      
      placement[2] = vachalfZ - vacuumFrontThick - cuHalfZ;
      new G4PVPlacement( 0, placement, cu_Support_LV, "Cu_Support_Volume", vacuume_Can_LV, 0, 0 );

      // Includes dead layer, so not an active ge detector yet.
      const G4double geHalfZ = 0.5 * m_SodiumIodideDimensions[0][2];//vachalfZ - 0.5 * vacuumFrontThick;
      const G4double geInnerRad = 0.0 * mm;
      const G4double geOuterRad = 0.5 * m_SodiumIodideDimensions[0][0];
      G4VSolid* ge_Solid =  new G4Tubs("Ge_Solid", geInnerRad, geOuterRad, geHalfZ, startPhi, deltaPhi);	
      G4LogicalVolume* ge_LV = new G4LogicalVolume( ge_Solid, germaniumMaterial, "Ge_Logical");
      m_PMT_Shield_LogicalVolumes.push_back( ge_LV );// USED FOR DRAWING
      
      placement[2] = cuHalfZ - geHalfZ;
      new G4PVPlacement( 0, placement, ge_LV, "Ge_Volume", cu_Support_LV, 0, 0 );

      const G4double vac1HalfZ = cuHalfZ - geHalfZ;// Vacuum inside copper sleave
      const G4double vac1InnerRad = geInnerRad;
      const G4double vac1OuterRad = geOuterRad;
      G4VSolid* vac1_Solid =  new G4Tubs("Vacuum1_Solid", vac1InnerRad, vac1OuterRad, vac1HalfZ, startPhi, deltaPhi);	
      G4LogicalVolume* vac1_LV = new G4LogicalVolume( vac1_Solid, vacuumMaterial, "Vacuum1_Logical");
      m_Gas_LogicalVolumes.push_back( vac1_LV );// USED FOR DRAWING
      
      placement[2] = cuHalfZ - 2.0 * geHalfZ - vac1HalfZ;
      new G4PVPlacement( 0, placement, vac1_LV, "Vacuum1_Volume", cu_Support_LV, 0, 0 );

      const G4double geActiveHalfZ = geHalfZ - geDeadLayerThick;
      const G4double geActiveInnerRad = geInnerRad;
      const G4double geActiveOuterRad = geOuterRad - geDeadLayerThick;
      G4VSolid* geActive_Solid =  new G4Tubs("GeActive_Solid", geActiveInnerRad, geActiveOuterRad, geActiveHalfZ, startPhi, deltaPhi);	
      G4LogicalVolume* geActive_LV = new G4LogicalVolume( geActive_Solid, germaniumMaterial, "GeActive_Logical");
      m_NaI_LogicalVolumes.push_back( geActive_LV );// USED FOR DRAWING and Recording energy!
      
//      placement[2] = cuHalfZ - geHalfZ;
      new G4PVPlacement( 0, zero, geActive_LV, "Ge_Volume", ge_LV, 0, 0 );
    }
    else if( i == 1 )
    {
// START Clover HPGe
      const bool ptype = false;// decides if hole in front/back and deadlayer thicknesses 
      const G4double d1 = 0.01 * mm;//0.5 * mm;//dead layer 1
      const G4double d2 = 0.01 * mm;//1.0 * mm;// dead layer 2
      
// Remember to change positioning of module when changing length of module.
      const G4double geZ = ( 80.0 - 0.0 ) * mm;// minus dead layer, dead layer handled below now...
      const G4double geInnerRad = 0.0 * mm;
      const G4double geOuterRad = 0.5 * ( 50.0 - 0.0 ) * mm;// minus removes dead layer, dead layer handled below now...
      
      const G4double geFlatX = 21.50 * mm;// Defined by offset distance assumed to be 7 mm.

      const G4double holeR = 5.0 * mm;// Set
      const G4double holeHalfZ = 0.5 * geZ * 50.0 / 80.0;// CHange based on total HPGeZ length

      const G4double canAlThickness = 1.5 * mm;
      const G4double canAlThicknessZ = 1.5 * mm;
      const G4double vacuumThickness = 7.0 * mm;
      const G4double canExtraZ = 200.0 * mm;

      const G4double backcatcherR = geOuterRad + geFlatX;
      const G4double backcatcherZ = 80.0 * mm;
      const G4double backcatcherHalfZ = 0.5 * backcatcherZ;

      const G4double canhalfZ = 0.5 * ( geZ + 2.0 * vacuumThickness + 2.0 * canAlThicknessZ + canExtraZ );//
//      G4cout << "Can Z=" << 2.0 * canhalfZ << G4endl;
      const G4double canOuterRad = geOuterRad + geFlatX + vacuumThickness + canAlThickness;// 0.5 * 100.0 * mm;// Make wider to fit larger can.

      G4VSolid* alOuter_Can_Solid =  new G4Box("Al_Can_Solid", canOuterRad, canOuterRad, canhalfZ );	
      G4LogicalVolume* alOuter_Can_LV = new G4LogicalVolume( alOuter_Can_Solid, alMaterial, "Al_Can_Logical");
      m_Pipe_LogicalVolumes.push_back( alOuter_Can_LV );// USED FOR DRAWING
      m_ModuleTotal_LogicalVolumes.push_back( alOuter_Can_LV );// USED FOR DRAWING and OVERALL PLACEMENT

      const G4double vacuumhalfZ = canhalfZ - canAlThicknessZ;
      const G4double vacuumOuterRad = canOuterRad - canAlThickness;
      G4VSolid* vacuum_Can_Solid =  new G4Box("Al_Can_Solid", vacuumOuterRad, vacuumOuterRad, vacuumhalfZ );	
      G4LogicalVolume* vacuume_Can_LV = new G4LogicalVolume( vacuum_Can_Solid, vacuumMaterial, "Vacuum_Logical");
      m_Gas_LogicalVolumes.push_back( vacuume_Can_LV );// USED FOR DRAWING

      new G4PVPlacement( 0, zero, vacuume_Can_LV, "Vacuum_in_Can_Volume", alOuter_Can_LV, 0, 0 );
      
      const G4double geHalfZ = 0.5 * geZ;
      
      const G4double spaceX = 0.5 * mm;// offset, so total space = 2 * this
      
      G4ThreeVector delta1( geFlatX + spaceX, 0.0, 0.0 );
      G4ThreeVector delta2( 0.0, geFlatX + spaceX, 0.0 );
      
      G4ThreeVector boxOffset( geOuterRad - geFlatX, geOuterRad - geFlatX,0.0);

      G4VSolid* ge1AaClover_Solid = new G4Tubs("Ge1Aa_Solid", geInnerRad, geOuterRad, geHalfZ, startPhi, deltaPhi );      
      G4VSolid* ge1AbClover_Solid = new G4Box("Ge1Ac_Solid", geOuterRad, geOuterRad, geHalfZ );
      
      G4VSolid* ge1AabClover_Solid = new G4IntersectionSolid("Ge1Aab_Solid", ge1AaClover_Solid, ge1AbClover_Solid, 0, boxOffset );

      G4VSolid* vacHole_Solid = new G4Tubs("VacuumHole_Solid", 0.0, holeR, holeHalfZ, startPhi, deltaPhi );

      placement[0] = 0.0;
      placement[1] = 0.0;
      placement[2] = -geHalfZ + holeHalfZ;
      if( ptype ) placement[2] *= -1.0;
      
      G4VSolid* ge1AabHoleClover_Solid = new G4SubtractionSolid("Ge1AabcH_Solid", ge1AabClover_Solid, vacHole_Solid, 0, placement );
      G4VSolid* ge1Clover_Solid = ge1AabHoleClover_Solid;//ge1ABCDEClover_Solid;      
      
      G4LogicalVolume* ge1_LV = new G4LogicalVolume( ge1Clover_Solid, germaniumMaterial, "Ge1_Logical");
      m_Wire_LogicalVolumes.push_back( ge1_LV );// USED FOR DRAWING

      const G4double geAZ = ptype ? geZ - d1 - d2 : geZ - d1 - d1;// minus dead layer
      const G4double geAOuterRad = ptype ? geOuterRad - d2 : geOuterRad - d1;// minus removes dead layer
      
      const G4double geAFlatX = ptype ? geFlatX - d2: geFlatX - d1;// Defined by offset distance assumed to be 7 mm.

      const G4double holeAR = ptype ? holeR + d1 : holeR + d2;// Set
      const G4double holeAHalfZ = holeHalfZ;//ptype ? holeHalfZ + 0.5 * d1 : holeHalfZ + 0.5 * d2;// CHange based on total HPGeZ length

      const G4double geAHalfZ = 0.5 * geAZ;

      G4ThreeVector boxAOffset( geAOuterRad - geAFlatX, geAOuterRad - geAFlatX,0.0);

      G4VSolid* geA1AaClover_Solid = new G4Tubs("GeA1Aa_Solid", geInnerRad, geAOuterRad, geAHalfZ, startPhi, deltaPhi );
      
      G4VSolid* geA1AbClover_Solid = new G4Box("GeA1Ac_Solid", geAOuterRad, geAOuterRad, geAHalfZ );
      
      G4VSolid* geA1AabClover_Solid = new G4IntersectionSolid("GeA1Aab_Solid", geA1AaClover_Solid, geA1AbClover_Solid, 0, boxAOffset );

      G4VSolid* vacAHole_Solid = new G4Tubs("VacuumAHole_Solid", 0.0, holeAR, holeAHalfZ, startPhi, deltaPhi );

      placement[0] = 0.0;
      placement[1] = 0.0;
      placement[2] = -geAHalfZ + holeAHalfZ;
      if( ptype ) placement[2] *= -1.0;
      
      G4VSolid* geA1AabcHoleClover_Solid = new G4SubtractionSolid("Ge1AabcH_Solid", geA1AabClover_Solid, vacAHole_Solid, 0, placement );
      G4VSolid* geA1Clover_Solid = geA1AabcHoleClover_Solid;//ge1ABCDEClover_Solid;      
      
      G4LogicalVolume* geA1_LV = new G4LogicalVolume( geA1Clover_Solid, germaniumMaterial, "GeA1_Logical");
      m_NaI_LogicalVolumes.push_back( geA1_LV );// USED FOR DRAWING and Recording energy!

      placement[0] = 0.0;
      placement[1] = 0.0;
      placement[2] = -geHalfZ + geAHalfZ + d1;
      if( ptype ) placement[2] *= -1.0;
      new G4PVPlacement( 0, placement, geA1_LV, "GeA_Volume", ge1_LV, 0, 0 );

// Place the four detectors with same numbering as ORNL clovers CCW from upper left = 0. 
// For some reason I place them clockwise from upper right.
      placement[0] = delta1[0];
      placement[1] = delta2[1];
      placement[2] = vacuumhalfZ - vacuumThickness - geHalfZ;//geActiveHalfZ - geActiveTaperHalfZ;
      new G4PVPlacement( 0, placement, ge1_LV, "Ge_Volume", vacuume_Can_LV, 0, 1 );

      placement[0] = -delta1[0];
      placement[1] = delta2[1];
//      placement[2] = 0.0 * mm;

      G4RotationMatrix* rot10 = new G4RotationMatrix();//(matrixColumn1, matrixColumn2, matrixColumn3);
      rot10->rotateZ( 270 * degree );
      m_Rotations.push_back(rot10);// automatically deletes it later...

      new G4PVPlacement( rot10, placement, ge1_LV, "Ge_Volume", vacuume_Can_LV, 0, 0 );

      placement[0] = -delta1[0];
      placement[1] = -delta2[1];
//      placement[2] = 0.0 * mm;

      G4RotationMatrix* rot40 = new G4RotationMatrix();//(matrixColumn1, matrixColumn2, matrixColumn3);
      rot40->rotateZ( 180 * degree );
      m_Rotations.push_back(rot40);// automatically deletes it later...

      new G4PVPlacement( rot40, placement, ge1_LV, "Ge_Volume", vacuume_Can_LV, 0, 3 );

      placement[0] = delta1[0];
      placement[1] = -delta2[1];
//      placement[2] = 0.0 * mm;

      G4RotationMatrix* rot30 = new G4RotationMatrix();//(matrixColumn1, matrixColumn2, matrixColumn3);
      rot30->rotateZ( 90 * degree );
      m_Rotations.push_back(rot30);// automatically deletes it later...

      new G4PVPlacement( rot30, placement, ge1_LV, "Ge_Volume", vacuume_Can_LV, 0, 2 );
      
      G4VSolid* backcatcher_Solid =  new G4Box("BackCatcher_Solid", backcatcherR, backcatcherR, backcatcherHalfZ );	
      G4LogicalVolume* backcatcher_LV = new G4LogicalVolume( backcatcher_Solid, germaniumMaterial, "Backcatcher_Logical");
      m_NaI_LogicalVolumes.push_back( backcatcher_LV );// USED FOR DRAWING and Recording energy!

      placement[0] = 0.0 * mm;
      placement[1] = 0.0 * mm;
      placement[2] = vacuumhalfZ - vacuumThickness - geZ - 10.0 * mm - backcatcherHalfZ;

 //     new G4PVPlacement( 0, placement, backcatcher_LV, "Backcatcher_Volume", vacuume_Can_LV, 0, 5 );

// END Clover HPGe
    }
    else if( i == 2 )
    {
// START GRIFFIN Clover HPGe
      const G4double geActiveZ = ( 90.0 - 1.0 ) * mm;// minus dead layer
      const G4double geInnerRad = 0.0 * mm;
      const G4double geHoleInnerRad = 5.0 * mm;
      const G4double geOuterRad = 0.5 * ( 60.0 - 0.5 ) * mm;

      const G4double geFlatX = 24.0 * mm;
      const G4double cosT = geFlatX / geOuterRad;
      const G4double theta = acos( cosT );

      const G4double geFlatX1 = 21.3 * mm;
      const G4double cosT1 = geFlatX1 / geOuterRad;
      const G4double theta1 = acos( cosT1 );

      const G4double canAlThickness = 1.5 * mm;
      const G4double canAlThicknessZ = 1.5 * mm;
      const G4double vacuumThickness = 7.0 * mm;

      const G4double canhalfZ = 0.5 * ( geActiveZ + 2.0 * vacuumThickness + 2.0 * canAlThicknessZ );
      //      const G4double canInnerRad = 0.0 * mm; // Needed for cylinders
      const G4double canOuterRad = 0.5 * 120.0 * mm;// Make wider to fit larger can, but front would still be small

      G4VSolid* alOuter_Can_Solid =  new G4Box("Al_Can_Solid", canOuterRad, canOuterRad, canhalfZ );	
      G4LogicalVolume* alOuter_Can_LV = new G4LogicalVolume( alOuter_Can_Solid, alMaterial, "Al_Can_Logical");
      m_Pipe_LogicalVolumes.push_back( alOuter_Can_LV );// USED FOR DRAWING
      m_ModuleTotal_LogicalVolumes.push_back( alOuter_Can_LV );// USED FOR DRAWING and OVERALL PLACEMENT

      const G4double vacuumhalfZ = canhalfZ - canAlThicknessZ;
      const G4double vacuumOuterRad = canOuterRad - canAlThickness;
      G4VSolid* vacuum_Can_Solid =  new G4Box("Al_Can_Solid", vacuumOuterRad, vacuumOuterRad, vacuumhalfZ );	
      G4LogicalVolume* vacuume_Can_LV = new G4LogicalVolume( vacuum_Can_Solid, vacuumMaterial, "Vacuum_Logical");
      m_Gas_LogicalVolumes.push_back( vacuume_Can_LV );// USED FOR DRAWING

      new G4PVPlacement( 0, zero, vacuume_Can_LV, "Vacuum_in_Can_Volume", alOuter_Can_LV, 0, 0 );
      
//      const G4double geActiveZ = 90.0 * mm;
      const G4double geActiveTaperZ =  ( geOuterRad - geFlatX1 ) / tan( 22.0 * deg );
//      G4cout << "R=" << ( geOuterRad ) << G4endl; //Does not work for some reason
//      G4cout << "X/2=" << ( geFlatX1 ) << G4endl; //Does not work for some reason
//      G4cout << "Y=" << ( geOuterRad - ( geFlatX1 ) ) << G4endl; //Does not work for some reason
//      G4cout << "D=" << ( geOuterRad - ( geFlatX1 ) ) / tan( 0.3840 ) << G4endl; //Does not work for some reason
      const G4double geActiveHoleZ = 50.0 * mm;
      const G4double geActiveNoHoleZ = geActiveZ - geActiveTaperZ - geActiveHoleZ;
//      G4cout << "Z_h=" << ( geActiveNoHoleZ ) << G4endl;
      const G4double geActiveHalfZ = 0.5 * geActiveZ;
      const G4double geActiveTaperHalfZ = 0.5 * geActiveTaperZ;
      const G4double geActiveHoleHalfZ = 0.5 * geActiveHoleZ;
      const G4double geActiveNoHoleHalfZ = 0.5 * geActiveNoHoleZ;
      
      const G4double phiA0 = -theta1;//theta - halfpi;
//      const G4double phiAb0 = -theta;
      const G4double phiB0 = pi + theta;
      const G4double phiC0 = pi - theta;
      const G4double phiD0 = 3.0 * halfpi - theta;
      const G4double phiE0 = halfpi - theta1;
      const G4double phiF0 = -halfpi + theta;
      const G4double phiG0 = theta1;
      const G4double phiH0 = halfpi + theta1;
      const G4double phiI0 = -halfpi + theta;//theta - halfpi;
      
//      const G4double dPhiAa = 3.0 * halfpi - 2.0 * theta;
      const G4double dPhiA = 2.0 * theta1;
      const G4double dPhiB = halfpi - 2.0 * theta;
      const G4double dPhiC = 2.0 * theta;
      const G4double dPhiD = 2.0 * theta;
      const G4double dPhiE = 2.0 * theta1;
      const G4double dPhiF = halfpi - theta - theta1;
      const G4double dPhiG = halfpi - theta1 - theta1;
      const G4double dPhiH = halfpi - theta - theta1;
      const G4double dPhiI = pi - theta - ( -halfpi + theta );
      
      const G4double spaceX = 0.5 * mm;// offset, so total space = 2 * this
      
      const G4int num1Sides = 1;
      const G4int num1ZPlanes = 2;
      
      const G4double r1Inner[] = { 0.0, 0.0 };
      const G4double r1Outer[] = { geFlatX, geFlatX };
      const G4double r1aOuter[] = { geOuterRad, geFlatX1 };
      const G4double z1Planes[] = { -geActiveTaperHalfZ, geActiveTaperHalfZ };

      const G4double r2Inner[] = { 0.0, 0.0 };
      const G4double r2Outer[] = { geFlatX, geFlatX };
      const G4double z2Planes[] = { -geActiveNoHoleHalfZ, geActiveNoHoleHalfZ };

      const G4double r3Inner[] = { geHoleInnerRad * cosT, geHoleInnerRad * cosT };
      const G4double r3Outer[] = { geFlatX, geFlatX };
      const G4double z3Planes[] = { -geActiveHoleHalfZ, geActiveHoleHalfZ };

      G4ThreeVector delta1( geFlatX+spaceX, 0.0, 0.0 );
      G4ThreeVector delta2( 0.0, geFlatX+spaceX, 0.0 );

      G4VSolid* ge1AaClover_Solid = new G4Cons("GeActive1Aa_Solid", geInnerRad, geOuterRad, geInnerRad, geOuterRad, 
                                                                  geActiveTaperHalfZ, phiA0, dPhiA );
      
      G4VSolid* ge1AbClover_Solid = new G4Polyhedra("GeActive1Ab_Solid", phiA0, dPhiA, num1Sides, num1ZPlanes, z1Planes, r1Inner, r1aOuter );
      
      G4VSolid* ge1AClover_Solid = new G4IntersectionSolid("GeActive1Aa_Solid", ge1AaClover_Solid, ge1AbClover_Solid);


      G4VSolid* ge1BClover_Solid = 0;// = new G4Cons("GeActive1B_Solid", geInnerRad, geOuterRad, geInnerRad, geOuterRad,  
                                   //                               geActiveTaperHalfZ, phiB0, dPhiB );
                                   
      if( dPhiB > 0.0 )
      {
        ge1BClover_Solid = new G4Cons("GeActive1B_Solid", geInnerRad, geOuterRad, geInnerRad, geOuterRad,  
                                                                  geActiveTaperHalfZ, phiB0, dPhiB );
      }
      else
      {
      }
                                                                
      G4VSolid* ge1CClover_Solid = new G4Polyhedra("GeActive1C_Solid", phiC0, dPhiC, num1Sides, num1ZPlanes, z1Planes, r1Inner, r1Outer );
      G4VSolid* ge1DClover_Solid = new G4Polyhedra("GeActive1D_Solid", phiD0, dPhiD, num1Sides, num1ZPlanes, z1Planes, r1Inner, r1Outer );

      G4VSolid* ge1EaClover_Solid = new G4Cons("GeActive1Ea_Solid", geInnerRad, geOuterRad, geInnerRad, geOuterRad, 
                                                                  geActiveTaperHalfZ, phiE0, dPhiE );
      
      G4VSolid* ge1EbClover_Solid = new G4Polyhedra("GeActive1Eb_Solid", phiE0, dPhiE, num1Sides, num1ZPlanes, z1Planes, r1Inner, r1aOuter );
      
      G4VSolid* ge1EClover_Solid = new G4IntersectionSolid("GeActive1Ea_Solid", ge1EaClover_Solid, ge1EbClover_Solid);
      
      G4VSolid* ge1FClover_Solid = 0;
      if( dPhiF > 0.0 )
      {
        ge1FClover_Solid = new G4Cons("GeActive1F_Solid", geInnerRad, geOuterRad, geInnerRad, geOuterRad,  
                                                                  geActiveTaperHalfZ, phiF0, dPhiF );
      }
      
      G4VSolid* ge1GClover_Solid = new G4Cons("GeActive1G_Solid", geInnerRad, geOuterRad, geInnerRad, geOuterRad,  
                                                                  geActiveTaperHalfZ, phiG0, dPhiG );

      G4VSolid* ge1HClover_Solid = new G4Cons("GeActive1H_Solid", geInnerRad, geOuterRad, geInnerRad, geOuterRad,  
                                                                  geActiveTaperHalfZ, phiH0, dPhiH );


      G4VSolid* ge1ABClover_Solid = 0;//new G4UnionSolid("GeActive1AB_Solid", ge1AClover_Solid, ge1BClover_Solid, 0, zero );
      G4VSolid* ge1ABCClover_Solid = 0;// new G4UnionSolid("GeActive1ABC_Solid", ge1ABClover_Solid, ge1CClover_Solid, 0, zero );
      if( dPhiB > 0.0 )
      {
        ge1ABClover_Solid = new G4UnionSolid("GeActive1AB_Solid", ge1AClover_Solid, ge1BClover_Solid, 0, zero );
        ge1ABCClover_Solid = new G4UnionSolid("GeActive1ABC_Solid", ge1ABClover_Solid, ge1CClover_Solid, 0, zero );
      }
      else
      {
        ge1ABCClover_Solid = new G4UnionSolid("GeActive1ABC_Solid", ge1AClover_Solid, ge1CClover_Solid, 0, zero );
      }
      
      G4VSolid* ge1ABCDClover_Solid = new G4UnionSolid("GeActive1ABCD_Solid", ge1ABCClover_Solid, ge1DClover_Solid, 0, zero );
      G4VSolid* ge1ABCDEClover_Solid = new G4UnionSolid("GeActive1ABCD_Solid", ge1ABCDClover_Solid, ge1EClover_Solid, 0, zero );
      
      G4VSolid* ge1ABCDEFClover_Solid = 0;//new G4UnionSolid("GeActive1ABCDF_Solid", ge1ABCDEClover_Solid, ge1FClover_Solid, 0, zero );
      G4VSolid* ge1ABCDEFGClover_Solid = 0;//new G4UnionSolid("GeActive1ABCDFG_Solid", ge1ABCDEFClover_Solid, ge1GClover_Solid, 0, zero );

      if( dPhiF > 0.0 )
      {
        ge1ABCDEFClover_Solid = new G4UnionSolid("GeActive1ABCDF_Solid", ge1ABCDEClover_Solid, ge1FClover_Solid, 0, zero );
        ge1ABCDEFGClover_Solid = new G4UnionSolid("GeActive1ABCDFG_Solid", ge1ABCDEFClover_Solid, ge1GClover_Solid, 0, zero );
      }
      else
      {
//        G4VSolid* ge1ABCDEFClover_Solid = new G4UnionSolid("GeActive1ABCDF_Solid", ge1ABCDEClover_Solid, ge1FClover_Solid, 0, zero );
        ge1ABCDEFGClover_Solid = new G4UnionSolid("GeActive1ABCDFG_Solid", ge1ABCDEClover_Solid, ge1GClover_Solid, 0, zero );
      }
      
      G4VSolid* ge1ABCDEFGHClover_Solid = new G4UnionSolid("GeActive1ABCDFGH_Solid", ge1ABCDEFGClover_Solid, ge1HClover_Solid, 0, zero );
      

      G4VSolid* ge1Clover_Solid = ge1ABCDEFGHClover_Solid;      
      
//      G4VSolid* geActive_Solid =  new G4UnionSolid("GeActive4_Solid", geActive2Clover_Solid, geActive2Clover_Solid, 0, delta2 );	

      G4LogicalVolume* ge1Active_LV = new G4LogicalVolume( ge1Clover_Solid, germaniumMaterial, "Ge1_Logical");
      m_NaI_LogicalVolumes.push_back( ge1Active_LV );// USED FOR DRAWING and Recording energy!
      
      placement[0] = delta1[0];
      placement[1] = delta2[1];
      placement[2] = geActiveHalfZ - geActiveTaperHalfZ;
      new G4PVPlacement( 0, placement, ge1Active_LV, "Ge_Volume", vacuume_Can_LV, 0, 1 );

      placement[0] = -delta1[0];
      placement[1] = delta2[1];
//      placement[2] = 0.0 * mm;

      G4RotationMatrix* rot10 = new G4RotationMatrix();//(matrixColumn1, matrixColumn2, matrixColumn3);
      rot10->rotateZ( 270 * degree );
      m_Rotations.push_back(rot10);// automatically deletes it later...

      new G4PVPlacement( rot10, placement, ge1Active_LV, "Ge_Volume", vacuume_Can_LV, 0, 0 );

      placement[0] = -delta1[0];
      placement[1] = -delta2[1];
//      placement[2] = 0.0 * mm;

      G4RotationMatrix* rot40 = new G4RotationMatrix();//(matrixColumn1, matrixColumn2, matrixColumn3);
      rot40->rotateZ( 180 * degree );
      m_Rotations.push_back(rot40);// automatically deletes it later...

      new G4PVPlacement( rot40, placement, ge1Active_LV, "Ge_Volume", vacuume_Can_LV, 0, 3 );

      placement[0] = delta1[0];
      placement[1] = -delta2[1];
//      placement[2] = 0.0 * mm;

      G4RotationMatrix* rot30 = new G4RotationMatrix();//(matrixColumn1, matrixColumn2, matrixColumn3);
      rot30->rotateZ( 90 * degree );
      m_Rotations.push_back(rot30);// automatically deletes it later...

      new G4PVPlacement( rot30, placement, ge1Active_LV, "Ge_Volume", vacuume_Can_LV, 0, 2 );
// END Tapered approximation section
// Start no hole section
      G4VSolid* ge2AClover_Solid = new G4Cons("GeActive2A_Solid", geInnerRad, geOuterRad, geInnerRad, geOuterRad, 
                                                                  geActiveNoHoleHalfZ, phiI0, dPhiI );
      G4VSolid* ge2BClover_Solid = new G4Cons("GeActive2B_Solid", geInnerRad, geOuterRad, geInnerRad, geOuterRad,  
                                                                  geActiveNoHoleHalfZ, phiB0, dPhiB );

      G4VSolid* ge2CClover_Solid = new G4Polyhedra("GeActive2C_Solid", phiC0, dPhiC, num1Sides, num1ZPlanes, z2Planes, r2Inner, r2Outer );
      G4VSolid* ge2DClover_Solid = new G4Polyhedra("GeActive2D_Solid", phiD0, dPhiD, num1Sides, num1ZPlanes, z2Planes, r2Inner, r2Outer );


      G4VSolid* ge2ABClover_Solid = new G4UnionSolid("GeActive2AB_Solid", ge2AClover_Solid, ge2BClover_Solid, 0, zero );
      G4VSolid* ge2ABCClover_Solid = new G4UnionSolid("GeActive2ABC_Solid", ge2ABClover_Solid, ge2CClover_Solid, 0, zero );
      G4VSolid* ge2ABCDClover_Solid = new G4UnionSolid("GeActive2ABCD_Solid", ge2ABCClover_Solid, ge2DClover_Solid, 0, zero );
      
      G4VSolid* ge2Clover_Solid = ge2ABCDClover_Solid;//ge1ABCClover_Solid;      
      
//      G4VSolid* geActive_Solid =  new G4UnionSolid("GeActive4_Solid", geActive2Clover_Solid, geActive2Clover_Solid, 0, delta2 );	

      G4LogicalVolume* ge2Active_LV = new G4LogicalVolume( ge2Clover_Solid, germaniumMaterial, "Ge2_Logical");
      m_NaI_LogicalVolumes.push_back( ge2Active_LV );// USED FOR DRAWING and Recording energy!
      
      placement[0] = delta1[0];
      placement[1] = delta2[1];
      placement[2] = geActiveHalfZ - geActiveTaperZ - geActiveNoHoleHalfZ;
      new G4PVPlacement( 0, placement, ge2Active_LV, "Ge_Volume", vacuume_Can_LV, 0, 1 );

      placement[0] = -delta1[0];
      placement[1] = delta2[1];
//      placement[2] = 0.0 * mm; 

      new G4PVPlacement( rot10, placement, ge2Active_LV, "Ge_Volume", vacuume_Can_LV, 0, 0 );

      placement[0] = -delta1[0];
      placement[1] = -delta2[1];
//      placement[2] = 0.0 * mm;

      new G4PVPlacement( rot40, placement, ge2Active_LV, "Ge_Volume", vacuume_Can_LV, 0, 3 );

      placement[0] = delta1[0];
      placement[1] = -delta2[1];
//      placement[2] = 0.0 * mm;

      new G4PVPlacement( rot30, placement, ge2Active_LV, "Ge_Volume", vacuume_Can_LV, 0, 2 );
// END no hole section
// Start hole section
      G4VSolid* ge3AClover_Solid = new G4Cons("GeActive3A_Solid", geHoleInnerRad, geOuterRad, geHoleInnerRad, geOuterRad, 
                                                                  geActiveHoleHalfZ, phiI0, dPhiI );
      G4VSolid* ge3BClover_Solid = new G4Cons("GeActive3B_Solid", geHoleInnerRad, geOuterRad, geHoleInnerRad, geOuterRad,  
                                                                  geActiveHoleHalfZ, phiB0, dPhiB );

      G4VSolid* ge3CClover_Solid = new G4Polyhedra("GeActive3C_Solid", phiC0, dPhiC, num1Sides, num1ZPlanes, z3Planes, r3Inner, r3Outer );
      G4VSolid* ge3DClover_Solid = new G4Polyhedra("GeActive3D_Solid", phiD0, dPhiD, num1Sides, num1ZPlanes, z3Planes, r3Inner, r3Outer );


      G4VSolid* ge3ABClover_Solid = new G4UnionSolid("GeActive3AB_Solid", ge3AClover_Solid, ge3BClover_Solid, 0, zero );
      G4VSolid* ge3ABCClover_Solid = new G4UnionSolid("GeActive3ABC_Solid", ge3ABClover_Solid, ge3CClover_Solid, 0, zero );
      G4VSolid* ge3ABCDClover_Solid = new G4UnionSolid("GeActive3ABCD_Solid", ge3ABCClover_Solid, ge3DClover_Solid, 0, zero );
      
      G4VSolid* ge3Clover_Solid = ge3ABCDClover_Solid;//ge1ABCClover_Solid;      
      
//      G4VSolid* geActive_Solid =  new G4UnionSolid("GeActive4_Solid", geActive2Clover_Solid, geActive2Clover_Solid, 0, delta2 );	

      G4LogicalVolume* ge3Active_LV = new G4LogicalVolume( ge3Clover_Solid, germaniumMaterial, "Ge3_Logical");
      m_NaI_LogicalVolumes.push_back( ge3Active_LV );// USED FOR DRAWING and Recording energy!
      
      placement[0] = delta1[0];
      placement[1] = delta2[1];
      placement[2] = -geActiveHalfZ + geActiveHoleHalfZ;
      new G4PVPlacement( 0, placement, ge3Active_LV, "Ge_Volume", vacuume_Can_LV, 0, 1 );
      
      placement[0] = -delta1[0];
      placement[1] = delta2[1];
//      placement[2] = 0.0 * mm; 

      new G4PVPlacement( rot10, placement, ge3Active_LV, "Ge_Volume", vacuume_Can_LV, 0, 0 );

      placement[0] = -delta1[0];
      placement[1] = -delta2[1];
//      placement[2] = 0.0 * mm;

      new G4PVPlacement( rot40, placement, ge3Active_LV, "Ge_Volume", vacuume_Can_LV, 0, 3 );

      placement[0] = delta1[0];
      placement[1] = -delta2[1];
//      placement[2] = 0.0 * mm;

      new G4PVPlacement( rot30, placement, ge3Active_LV, "Ge_Volume", vacuume_Can_LV, 0, 2 );
// END hole section
// END GRIFFIN Clover HPGe
    }
    else if( i == 3 )
    {
// START GRIFFIN Clover HPGe
      const G4double geActiveZ = ( 90.0 - 0.0 ) * mm;
      const G4double geInnerRad = 0.0 * mm;
//      const G4double geHoleInnerRad = 5.0 * mm;
      const G4double geOuterRad = 0.5 * ( 60.0 - 0.0 ) * mm;// minus removes dead layer

      const G4double geFlatX = 24.50 * mm;
      const G4double cosT = geFlatX / geOuterRad;
      const G4double theta = acos( cosT );

      const G4double geFlatX1 = sqrt( geOuterRad * geOuterRad - geFlatX * geFlatX );//- 19.0 * mm;
//      G4cout << "X1=" << geFlatX1 << G4endl;
      const G4double tanT1 = geFlatX / geFlatX1;
      const G4double theta1 = atan( tanT1 );
 //     G4cout << "Theta1=" << theta1 << G4endl;
//      const G4double tanT2 = geFlatX1 / geFlatX;
//      const G4double theta2 = atan( tanT2 );

      const G4double canAlThickness = 1.5 * mm;
      const G4double canAlThicknessZ = 0.5 * mm;
      const G4double vacuumThickness = 7.0 * mm;

      const G4double canhalfZ = 0.5 * ( geActiveZ + 2.0 * vacuumThickness );// 90.0 mm gives a 5 mm gap, too close? Make 100 * mm?
//      const G4double canInnerRad = 0.0 * mm; // Needed for cylinders
      const G4double canOuterRad = 0.5 * 120.0 * mm;// Make wider to fit larger can, but front would still be small

      G4VSolid* alOuter_Can_Solid =  new G4Box("Al_Can_Solid", canOuterRad, canOuterRad, canhalfZ );	
      G4LogicalVolume* alOuter_Can_LV = new G4LogicalVolume( alOuter_Can_Solid, alMaterial, "Al_Can_Logical");
      m_Pipe_LogicalVolumes.push_back( alOuter_Can_LV );// USED FOR DRAWING
      m_ModuleTotal_LogicalVolumes.push_back( alOuter_Can_LV );// USED FOR DRAWING and OVERALL PLACEMENT

      const G4double vacuumhalfZ = canhalfZ - canAlThicknessZ;
      const G4double vacuumOuterRad = canOuterRad - canAlThickness;
      G4VSolid* vacuum_Can_Solid =  new G4Box("Al_Can_Solid", vacuumOuterRad, vacuumOuterRad, vacuumhalfZ );	
      G4LogicalVolume* vacuume_Can_LV = new G4LogicalVolume( vacuum_Can_Solid, vacuumMaterial, "Vacuum_Logical");
      m_Gas_LogicalVolumes.push_back( vacuume_Can_LV );// USED FOR DRAWING

      new G4PVPlacement( 0, zero, vacuume_Can_LV, "Vacuum_in_Can_Volume", alOuter_Can_LV, 0, 0 );
      
//      const G4double geActiveZ = 90.0 * mm;
      const G4double geActiveTaperZ =  ( geOuterRad - geFlatX1 ) / tan( 22.0 * deg );
//      G4cout << "D=" << ( geOuterRad - ( geFlatX1 ) ) / tan( 0.3840 ) << G4endl;
//      const G4double geActiveHoleZ = 50.0 * mm;
//      const G4double geActiveNoHoleZ = geActiveZ - geActiveTaperZ - geActiveHoleZ;
//      G4cout << "Z_h=" << ( geActiveNoHoleZ ) << G4endl;
      const G4double geActiveHalfZ = 0.5 * geActiveZ;
      const G4double geActiveTaperHalfZ = 0.5 * geActiveTaperZ;
//      const G4double geActiveHoleHalfZ = 0.5 * geActiveHoleZ;
//      const G4double geActiveNoHoleHalfZ = 0.5 * geActiveNoHoleZ;
      
      const G4double phiA0 = -theta1;//theta - halfpi;
//      const G4double phiAb0 = -theta;
      const G4double phiB0 = pi + theta;
      const G4double phiC0 = pi - theta;
      const G4double phiD0 = 3.0 * halfpi - theta;
      const G4double phiE0 = 0.5 * halfpi;//halfpi - theta1;//0.5 * halfpi;// 
      
//      const G4double dPhiAa = 3.0 * halfpi - 2.0 * theta;
      const G4double dPhiA = 0.5 * halfpi + theta1;//2.0 * theta1;//
      const G4double dPhiB = halfpi - 2.0 * theta;
      const G4double dPhiC = theta + theta;
      const G4double dPhiD = 2.0 * theta;
      const G4double dPhiE = 0.5 * halfpi + theta1;//2.0 * theta1;//
      
      const G4double spaceX = 1.0 * mm;// offset, so total space = 2 * this
      
      const G4int num1Sides = 1;
      const G4int num1ZPlanes = 2;
      
      const G4double r1Inner[] = { 0.0, 0.0 };
      const G4double r1Outer[] = { geFlatX, geFlatX };
//      const G4double r1aOuter[] = { geOuterRad, geFlatX1 };
      const G4double z1Planes[] = { -geActiveTaperHalfZ, geActiveTaperHalfZ };

//      const G4double r2Inner[] = { 0.0, 0.0 };
//      const G4double r2Outer[] = { geFlatX, geFlatX };
//      const G4double z2Planes[] = { -geActiveNoHoleHalfZ, geActiveNoHoleHalfZ };

//      const G4double r3Inner[] = { geHoleInnerRad * cosT, geHoleInnerRad * cosT };
//      const G4double r3Outer[] = { geFlatX, geFlatX };
//      const G4double z3Planes[] = { -geActiveHoleHalfZ, geActiveHoleHalfZ };

      G4ThreeVector delta1( geFlatX+spaceX, 0.0, 0.0 );
      G4ThreeVector delta2( 0.0, geFlatX+spaceX, 0.0 );

      G4RotationMatrix* rotY22 = new G4RotationMatrix();//(matrixColumn1, matrixColumn2, matrixColumn3);
      rotY22->rotateY( 22 * degree );
      m_Rotations.push_back(rotY22);// automatically deletes it later...

      G4RotationMatrix* rotX22 = new G4RotationMatrix();//(matrixColumn1, matrixColumn2, matrixColumn3);
      rotX22->rotateX( -22 * degree );
      m_Rotations.push_back(rotX22);// automatically deletes it later...

      G4VSolid* ge1AaClover_Solid = new G4Cons("GeActive1Aa_Solid", geInnerRad, geOuterRad, geInnerRad, geOuterRad, 
                                                                  geActiveTaperHalfZ, phiA0, dPhiA );
      
//      G4VSolid* ge1AbClover_Solid = new G4Polyhedra("GeActive1Ab_Solid", phiA0, dPhiA, num1Sides, num1ZPlanes, z1Planes, r1Inner, r1aOuter );

      G4VSolid* ge1AbClover_Solid = new G4Box("GeActive1Ab_Solid", geFlatX1 / cos(22.0*deg), geOuterRad, geOuterRad );
      
      G4VSolid* ge1AClover_Solid = new G4IntersectionSolid("GeActive1Aa_Solid", ge1AaClover_Solid, ge1AbClover_Solid, rotY22, zero );


      G4VSolid* ge1BClover_Solid = ge1BClover_Solid = new G4Cons("GeActive1B_Solid", geInnerRad, geOuterRad, geInnerRad, geOuterRad,  
                                                                  geActiveTaperHalfZ, phiB0, dPhiB );
                                                                
      G4VSolid* ge1CClover_Solid = new G4Polyhedra("GeActive1C_Solid", phiC0, dPhiC, num1Sides, num1ZPlanes, z1Planes, r1Inner, r1Outer );
      G4VSolid* ge1DClover_Solid = new G4Polyhedra("GeActive1D_Solid", phiD0, dPhiD, num1Sides, num1ZPlanes, z1Planes, r1Inner, r1Outer );

      G4VSolid* ge1EaClover_Solid = new G4Cons("GeActive1Ea_Solid", geInnerRad, geOuterRad, geInnerRad, geOuterRad, 
                                                                  geActiveTaperHalfZ, phiE0, dPhiE );
      
//      G4VSolid* ge1EbClover_Solid = new G4Polyhedra("GeActive1Eb_Solid", phiE0, dPhiE, num1Sides, num1ZPlanes, z1Planes, r1Inner, r1aOuter );
      G4VSolid* ge1EbClover_Solid = new G4Box("GeActive1Ab_Solid", geOuterRad, geFlatX1 / cos(22.0*deg), geOuterRad );
      G4VSolid* ge1EClover_Solid = new G4IntersectionSolid("GeActive1Ea_Solid", ge1EaClover_Solid, ge1EbClover_Solid, rotX22, zero );
      

      G4VSolid* ge1ABClover_Solid = new G4UnionSolid("GeActive1AB_Solid", ge1AClover_Solid, ge1BClover_Solid, 0, zero );
      G4VSolid* ge1ABCClover_Solid = new G4UnionSolid("GeActive1ABC_Solid", ge1ABClover_Solid, ge1CClover_Solid, 0, zero );
      
      G4VSolid* ge1ABCDClover_Solid = new G4UnionSolid("GeActive1ABCD_Solid", ge1ABCClover_Solid, ge1EClover_Solid, 0, zero );
      G4VSolid* ge1ABCDEClover_Solid = new G4UnionSolid("GeActive1ABCDE_Solid", ge1ABCDClover_Solid, ge1DClover_Solid, 0, zero );
//      

      G4VSolid* ge1Clover_Solid = ge1ABCDEClover_Solid;//ge1ABCDEClover_Solid;      
      
      G4LogicalVolume* ge1Active_LV = new G4LogicalVolume( ge1Clover_Solid, germaniumMaterial, "Ge1_Logical");
      m_NaI_LogicalVolumes.push_back( ge1Active_LV );// USED FOR DRAWING and Recording energy!
      
      placement[0] = delta1[0];
      placement[1] = delta2[1];
      placement[2] = geActiveHalfZ - geActiveTaperHalfZ;
      new G4PVPlacement( 0, placement, ge1Active_LV, "Ge_Volume", vacuume_Can_LV, 0, 1 );
    }
    else if( i == 4 )
    {
// START GRIFFIN Clover HPGe
      const G4double geActiveZ = ( 90.0 - 1.0 ) * mm;
      const G4double geInnerRad = 0.0 * mm;
//      const G4double geHoleInnerRad = 5.0 * mm;
      const G4double geOuterRad = 0.5 * ( 60.0 - 1.0 ) * mm;// minus removes dead layer
      const G4double trdAngle = 22.5 * deg;

      const G4double geFlatX = 23.0 * mm;
//      const G4double cosT = geFlatX / geOuterRad;
//      const G4double theta = acos( cosT );

      const G4double geFlatX1 = 18.0;// This is the dist to trd face//sqrt( geOuterRad * geOuterRad - geFlatX * geFlatX );//- 19.0 * mm;
      G4cout << "X1=" << geFlatX1 << G4endl;
//      const G4double tanT1 = geFlatX / geFlatX1;
//      const G4double theta1 = atan( tanT1 );
//      G4cout << "Theta1=" << theta1 << G4endl;
//      const G4double tanT2 = geFlatX1 / geFlatX;
//      const G4double theta2 = atan( tanT2 );

      const G4double canAlThickness = 1.5 * mm;
      const G4double canAlThicknessZ = 1.5 * mm;
      const G4double vacuumThickness = 7.0 * mm;

      const G4double canhalfZ = 0.5 * ( geActiveZ + 2.0 * vacuumThickness + 2.0 * canAlThicknessZ );// 90.0 mm gives a 5 mm gap, too close? Make 100 * mm?
      G4cout << "Can Z=" << 2.0 * canhalfZ << G4endl;
//      const G4double canInnerRad = 0.0 * mm; // Needed for cylinders
      const G4double canOuterRad = 0.5 * 140.0 * mm;// Make wider to fit larger can, but front would still be small

      G4VSolid* alOuter_Can_Solid =  new G4Box("Al_Can_Solid", canOuterRad, canOuterRad, canhalfZ );	
      G4LogicalVolume* alOuter_Can_LV = new G4LogicalVolume( alOuter_Can_Solid, alMaterial, "Al_Can_Logical");
      m_Pipe_LogicalVolumes.push_back( alOuter_Can_LV );// USED FOR DRAWING
      m_ModuleTotal_LogicalVolumes.push_back( alOuter_Can_LV );// USED FOR DRAWING and OVERALL PLACEMENT

      const G4double vacuumhalfZ = canhalfZ - canAlThicknessZ;
      const G4double vacuumOuterRad = canOuterRad - canAlThickness;
      G4VSolid* vacuum_Can_Solid =  new G4Box("Al_Can_Solid", vacuumOuterRad, vacuumOuterRad, vacuumhalfZ );	
      G4LogicalVolume* vacuume_Can_LV = new G4LogicalVolume( vacuum_Can_Solid, vacuumMaterial, "Vacuum_Logical");
      m_Gas_LogicalVolumes.push_back( vacuume_Can_LV );// USED FOR DRAWING

      new G4PVPlacement( 0, zero, vacuume_Can_LV, "Vacuum_in_Can_Volume", alOuter_Can_LV, 0, 0 );
      
//      const G4double geActiveZ = 90.0 * mm;
      const G4double geActiveTaperZ =  ( geOuterRad - geFlatX1 ) / tan( trdAngle );
//      G4cout << "D=" << ( geOuterRad - ( geFlatX1 ) ) / tan( 0.3840 ) << G4endl;
//      const G4double geActiveHoleZ = 50.0 * mm;
//      const G4double geActiveNoHoleZ = geActiveZ - geActiveTaperZ - geActiveHoleZ;
//      G4cout << "Z_h=" << ( geActiveNoHoleZ ) << G4endl;
      const G4double geActiveHalfZ = 0.5 * geActiveZ;
//      const G4double geActiveTaperHalfZ = 0.5 * geActiveTaperZ;
//      const G4double geActiveHoleHalfZ = 0.5 * geActiveHoleZ;
//      const G4double geActiveNoHoleHalfZ = 0.5 * geActiveNoHoleZ;
      
      const G4double spaceX = 1.0 * mm;// offset, so total space = 2 * this
      
      G4ThreeVector delta1( geFlatX+spaceX, 0.0, 0.0 );
      G4ThreeVector delta2( 0.0, geFlatX+spaceX, 0.0 );
      
      const G4double trdR0 = 0.5 * ( 124.0 - 2.0 * 1.5 *mm - 2.0 * 7.0 ) * mm;
      const G4double trdZ = trdR0 / tan(trdAngle) + geActiveZ - geActiveTaperZ;// geOuterRad / tan(trdAngle) + geActiveZ - geActiveTaperZ;
      const G4double trdR = trdZ * tan(trdAngle);
      
      const G4double halfTrdZ = 0.5 * trdZ;

      G4cout << "R0_trd=" << ( trdR0 ) << G4endl;
      G4cout << "R_trd=" << ( trdR ) << G4endl;
      G4cout << "Z_trd=" << ( trdZ ) << G4endl;

      G4ThreeVector trdOffset( 0.0, 0.0, halfTrdZ - geActiveHalfZ );
      trdOffset -= delta1 + delta2;
      G4ThreeVector boxOffset( geOuterRad - geFlatX, geOuterRad - geFlatX,0.0);

      G4VSolid* ge1AaClover_Solid = new G4Tubs("GeActive1Aa_Solid", geInnerRad, geOuterRad, geActiveHalfZ, startPhi, deltaPhi );
      
      G4VSolid* ge1AbClover_Solid = new G4Trd("GeActive1Ab_Solid", trdR, 0.0, trdR, 0.0, halfTrdZ );
      G4VSolid* ge1AcClover_Solid = new G4Box("GeActive1Ac_Solid", geOuterRad, geOuterRad, geActiveHalfZ );
      
      G4VSolid* ge1AabClover_Solid = new G4IntersectionSolid("GeActive1Aab_Solid", ge1AaClover_Solid, ge1AbClover_Solid, 0, trdOffset );
      G4VSolid* ge1AabcClover_Solid = new G4IntersectionSolid("GeActive1Aabc_Solid", ge1AabClover_Solid, ge1AcClover_Solid, 0, boxOffset );

      G4VSolid* ge1Clover_Solid = ge1AabcClover_Solid;//ge1ABCDEClover_Solid;      
      
      G4LogicalVolume* ge1Active_LV = new G4LogicalVolume( ge1Clover_Solid, germaniumMaterial, "Ge1_Logical");
      m_NaI_LogicalVolumes.push_back( ge1Active_LV );// USED FOR DRAWING and Recording energy!
      
      const G4double holeR = 5.0 * mm;
      const G4double holeHalfZ = 0.5 * geActiveZ * 50.0 / 80.0;
      
      G4VSolid* vacHole_Solid = new G4Tubs("VacuumHole_Solid", 0.0, holeR, holeHalfZ, startPhi, deltaPhi );
      G4LogicalVolume* hole_LV = new G4LogicalVolume( vacHole_Solid, vacuumMaterial, "Hole_Logical");
      m_Gas_LogicalVolumes.push_back( hole_LV );// USED FOR DRAWING only
      placement[0] = 0.0;
      placement[1] = 0.0;
      placement[2] = -geActiveHalfZ + holeHalfZ;
      new G4PVPlacement( 0, placement, hole_LV, "Ge_Volume", ge1Active_LV, 0, 1 );


      placement[0] = delta1[0];
      placement[1] = delta2[1];
      placement[2] = 0.0;//geActiveHalfZ - geActiveTaperHalfZ;
      new G4PVPlacement( 0, placement, ge1Active_LV, "Ge_Volume", vacuume_Can_LV, 0, 1 );

      placement[0] = -delta1[0];
      placement[1] = delta2[1];
//      placement[2] = 0.0 * mm;

      G4RotationMatrix* rot10 = new G4RotationMatrix();//(matrixColumn1, matrixColumn2, matrixColumn3);
      rot10->rotateZ( 270 * degree );
      m_Rotations.push_back(rot10);// automatically deletes it later...

      new G4PVPlacement( rot10, placement, ge1Active_LV, "Ge_Volume", vacuume_Can_LV, 0, 0 );

      placement[0] = -delta1[0];
      placement[1] = -delta2[1];
//      placement[2] = 0.0 * mm;

      G4RotationMatrix* rot40 = new G4RotationMatrix();//(matrixColumn1, matrixColumn2, matrixColumn3);
      rot40->rotateZ( 180 * degree );
      m_Rotations.push_back(rot40);// automatically deletes it later...

      new G4PVPlacement( rot40, placement, ge1Active_LV, "Ge_Volume", vacuume_Can_LV, 0, 3 );

      placement[0] = delta1[0];
      placement[1] = -delta2[1];
//      placement[2] = 0.0 * mm;

      G4RotationMatrix* rot30 = new G4RotationMatrix();//(matrixColumn1, matrixColumn2, matrixColumn3);
      rot30->rotateZ( 90 * degree );
      m_Rotations.push_back(rot30);// automatically deletes it later...

      new G4PVPlacement( rot30, placement, ge1Active_LV, "Ge_Volume", vacuume_Can_LV, 0, 2 );
    }
    else if( i == 5 )
    {
// START 7x7,8,9 Clovers HPGe
      const bool ptype = false;// decides if hole in front/back and deadlayer thicknesses 
      const G4double d1 = 0.01 * mm;//0.5 * mm;//dead layer 1
      const G4double d2 = 0.01 * mm;//1.0 * mm;// dead layer 2
      
// Remember to change positioning of module when changing length of module.
      const G4double geZ = ( 90.0 - 0.0 ) * mm;// minus dead layer, dead layer handled below now...
      const G4double geInnerRad = 0.0 * mm;
      const G4double geOuterRad = 0.5 * ( 70.0 - 0.0 ) * mm;// minus removes dead layer, dead layer handled below now...
      const G4double trdAngle = 22.5 * deg;
      const G4double cosTrdA = cos( trdAngle );
      G4cout << "trdAngle=" << trdAngle << G4endl;
      G4cout << "cosA=" << cosTrdA << G4endl;

//      const G4double sinTrdA = sin( trdAngle / deg );
//      const G4double d1cos22 = d1 * co;
//      const G4double d2cos22 = 1.0 * mm;
      
      const G4double geFlatX = 28.0 * mm;// Defined by offset distance assumed to be 7 mm.
      const G4double geFlatX1 = 23.0 * mm;//20.0 * mm;// //20.0 and 23.0 are the two choices //distance to trd face
//      G4cout << "X1=" << geFlatX1 << G4endl;

      const G4double holeR = 5.0 * mm;// Set
      const G4double holeHalfZ = 0.5 * geZ * 50.0 / 80.0;// CHange based on total HPGeZ length

      const G4double canAlThickness = 1.5 * mm;
      const G4double canAlThicknessZ = 1.5 * mm;
      const G4double vacuumThickness = 7.0 * mm;

      const G4double canhalfZ = 0.5 * ( geZ + 2.0 * vacuumThickness + 2.0 * canAlThicknessZ );//
//      G4cout << "Can Z=" << 2.0 * canhalfZ << G4endl;
      const G4double canOuterRad = 0.5 * 140.0 * mm;// Make wider to fit larger can.

      G4VSolid* alOuter_Can_Solid =  new G4Box("Al_Can_Solid", canOuterRad, canOuterRad, canhalfZ );	
      G4LogicalVolume* alOuter_Can_LV = new G4LogicalVolume( alOuter_Can_Solid, alMaterial, "Al_Can_Logical");
      m_Pipe_LogicalVolumes.push_back( alOuter_Can_LV );// USED FOR DRAWING
      m_ModuleTotal_LogicalVolumes.push_back( alOuter_Can_LV );// USED FOR DRAWING and OVERALL PLACEMENT

      const G4double vacuumhalfZ = canhalfZ - canAlThicknessZ;
      const G4double vacuumOuterRad = canOuterRad - canAlThickness;
      G4VSolid* vacuum_Can_Solid =  new G4Box("Al_Can_Solid", vacuumOuterRad, vacuumOuterRad, vacuumhalfZ );	
      G4LogicalVolume* vacuume_Can_LV = new G4LogicalVolume( vacuum_Can_Solid, vacuumMaterial, "Vacuum_Logical");
      m_Gas_LogicalVolumes.push_back( vacuume_Can_LV );// USED FOR DRAWING

      new G4PVPlacement( 0, zero, vacuume_Can_LV, "Vacuum_in_Can_Volume", alOuter_Can_LV, 0, 0 );
      
      const G4double geTaperZ =  ( geOuterRad - geFlatX1 ) / tan( trdAngle );
//      G4cout << "D=" << ( geOuterRad - ( geFlatX1 ) ) / tan( 0.3840 ) << G4endl;
      const G4double geHalfZ = 0.5 * geZ;
      
      const G4double spaceX = 0.5 * mm;// offset, so total space = 2 * this
      
      G4ThreeVector delta1( geFlatX + spaceX, 0.0, 0.0 );
      G4ThreeVector delta2( 0.0, geFlatX + spaceX, 0.0 );
      
      const G4double trdR0 = 0.5 * ( 144.0 * mm - 2.0 * canAlThickness - 2.0 * vacuumThickness );
//      const G4double trdR0 = 0.5 * ( 2.0 * geFlatX1 + 2.0 * geFlatX ) ;// + 2.0 * 7.0 * mm);
      const G4double trdZ = trdR0 / tan(trdAngle) + geZ - geTaperZ;// geOuterRad / tan(trdAngle) + geActiveZ - geActiveTaperZ;
      const G4double trdR = trdZ * tan(trdAngle);
      
      const G4double halfTrdZ = 0.5 * trdZ;

      G4cout << "R0_trd=" << ( trdR0 ) << G4endl;
      G4cout << "R_trd=" << ( trdR ) << G4endl;
      G4cout << "Z_trd=" << ( trdZ ) << G4endl;

      G4ThreeVector trdOffset( 0.0, 0.0, halfTrdZ - geHalfZ );
      trdOffset -= delta1 + delta2;
      G4ThreeVector boxOffset( geOuterRad - geFlatX, geOuterRad - geFlatX,0.0);

      G4VSolid* ge1AaClover_Solid = new G4Tubs("Ge1Aa_Solid", geInnerRad, geOuterRad, geHalfZ, startPhi, deltaPhi );
      
      G4VSolid* ge1AbClover_Solid = new G4Trd("Ge1Ab_Solid", trdR, 0.0, trdR, 0.0, halfTrdZ );
      G4VSolid* ge1AcClover_Solid = new G4Box("Ge1Ac_Solid", geOuterRad, geOuterRad, geHalfZ );
      
      G4VSolid* ge1AabClover_Solid = new G4IntersectionSolid("Ge1Aab_Solid", ge1AaClover_Solid, ge1AbClover_Solid, 0, trdOffset );
      G4VSolid* ge1AabcClover_Solid = new G4IntersectionSolid("Ge1Aabc_Solid", ge1AabClover_Solid, ge1AcClover_Solid, 0, boxOffset );

      G4VSolid* vacHole_Solid = new G4Tubs("VacuumHole_Solid", 0.0, holeR, holeHalfZ, startPhi, deltaPhi );

      placement[0] = 0.0;
      placement[1] = 0.0;
      placement[2] = -geHalfZ + holeHalfZ;
      if( ptype ) placement[2] *= -1.0;
      
      G4VSolid* ge1AabcHoleClover_Solid = new G4SubtractionSolid("Ge1AabcH_Solid", ge1AabcClover_Solid, vacHole_Solid, 0, placement );
      G4VSolid* ge1Clover_Solid = ge1AabcHoleClover_Solid;//ge1ABCDEClover_Solid;      
      
      G4LogicalVolume* ge1_LV = new G4LogicalVolume( ge1Clover_Solid, germaniumMaterial, "Ge1_Logical");
      m_Wire_LogicalVolumes.push_back( ge1_LV );// USED FOR DRAWING

      const G4double geAZ = ptype ? geZ - d1 - d2 : geZ - d1 - d1;// minus dead layer
      const G4double geAOuterRad = ptype ? geOuterRad - d2 : geOuterRad - d1;// minus removes dead layer
      
      const G4double geAFlatX = ptype ? geFlatX - d2: geFlatX - d1;// Difined by offset distance assumed to be 7 mm.
      const G4double geAFlatX1 = ptype ? geFlatX1 - d2: geFlatX1 - d1;// //19.0 and 22.0 are the two choices //distance to trd face
//      G4cout << "X1=" << geAFlatX1 << G4endl;

      const G4double holeAR = ptype ? holeR + d1 : holeR + d2;// Set
      const G4double holeAHalfZ = holeHalfZ;//ptype ? holeHalfZ + 0.5 * d1 : holeHalfZ + 0.5 * d2;// CHange based on total HPGeZ length

      const G4double geATaperZ =  ( geAOuterRad - geAFlatX1 ) / tan( trdAngle );
//      G4cout << "D=" << ( geOuterRad - ( geFlatX1 ) ) / tan( 0.3840 ) << G4endl;
      const G4double geAHalfZ = 0.5 * geAZ;

      const G4double trdAR0 = ptype ?  trdR0 - d2 * cosTrdA : trdR0 - d1 * cosTrdA;
      const G4double trdAZ = trdAR0 / tan(trdAngle) + geAZ - geATaperZ;
      const G4double trdAR = trdAZ * tan(trdAngle);
      
      const G4double halfTrdAZ = 0.5 * trdAZ;

      G4cout << "R0_trdA=" << ( trdAR0 ) << G4endl;
      G4cout << "R_trdA=" << ( trdAR ) << G4endl;
      G4cout << "Z_trdA=" << ( trdAZ ) << G4endl;
      
      G4ThreeVector trdAOffset( 0.0, 0.0, halfTrdAZ - geAHalfZ );
      trdAOffset -= delta1 + delta2;
      G4ThreeVector boxAOffset( geAOuterRad - geAFlatX, geAOuterRad - geAFlatX,0.0);

      G4VSolid* geA1AaClover_Solid = new G4Tubs("GeA1Aa_Solid", geInnerRad, geAOuterRad, geAHalfZ, startPhi, deltaPhi );
      
      G4VSolid* geA1AbClover_Solid = new G4Trd("GeA1Ab_Solid", trdAR, 0.0, trdAR, 0.0, halfTrdAZ );// FIX...
      G4VSolid* geA1AcClover_Solid = new G4Box("GeA1Ac_Solid", geAOuterRad, geAOuterRad, geAHalfZ );
      
      G4VSolid* geA1AabClover_Solid = new G4IntersectionSolid("GeA1Aab_Solid", geA1AaClover_Solid, geA1AbClover_Solid, 0, trdAOffset );
      G4VSolid* geA1AabcClover_Solid = new G4IntersectionSolid("GeA1Aabc_Solid", geA1AabClover_Solid, geA1AcClover_Solid, 0, boxAOffset );

      G4VSolid* vacAHole_Solid = new G4Tubs("VacuumAHole_Solid", 0.0, holeAR, holeAHalfZ, startPhi, deltaPhi );

      placement[0] = 0.0;
      placement[1] = 0.0;
      placement[2] = -geAHalfZ + holeAHalfZ;
      if( ptype ) placement[2] *= -1.0;
      
      G4VSolid* geA1AabcHoleClover_Solid = new G4SubtractionSolid("Ge1AabcH_Solid", geA1AabcClover_Solid, vacAHole_Solid, 0, placement );
      G4VSolid* geA1Clover_Solid = geA1AabcHoleClover_Solid;//ge1ABCDEClover_Solid;      
      
      G4LogicalVolume* geA1_LV = new G4LogicalVolume( geA1Clover_Solid, germaniumMaterial, "GeA1_Logical");
      m_NaI_LogicalVolumes.push_back( geA1_LV );// USED FOR DRAWING and Recording energy!

      placement[0] = 0.0;
      placement[1] = 0.0;
      placement[2] = -geHalfZ + geAHalfZ + d1;
      if( ptype ) placement[2] *= -1.0;
      new G4PVPlacement( 0, placement, geA1_LV, "GeA_Volume", ge1_LV, 0, 0 );

// Place the four detectors with same numbering as ORNL clovers CCW from upper left = 0. 
// For some reason I place them clockwise from upper right.
      placement[0] = delta1[0];
      placement[1] = delta2[1];
      placement[2] = 0.0;//geActiveHalfZ - geActiveTaperHalfZ;
      new G4PVPlacement( 0, placement, ge1_LV, "Ge_Volume", vacuume_Can_LV, 0, 1 );

      placement[0] = -delta1[0];
      placement[1] = delta2[1];
//      placement[2] = 0.0 * mm;

      G4RotationMatrix* rot10 = new G4RotationMatrix();//(matrixColumn1, matrixColumn2, matrixColumn3);
      rot10->rotateZ( 270 * degree );
      m_Rotations.push_back(rot10);// automatically deletes it later...

      new G4PVPlacement( rot10, placement, ge1_LV, "Ge_Volume", vacuume_Can_LV, 0, 0 );

      placement[0] = -delta1[0];
      placement[1] = -delta2[1];
//      placement[2] = 0.0 * mm;

      G4RotationMatrix* rot40 = new G4RotationMatrix();//(matrixColumn1, matrixColumn2, matrixColumn3);
      rot40->rotateZ( 180 * degree );
      m_Rotations.push_back(rot40);// automatically deletes it later...

      new G4PVPlacement( rot40, placement, ge1_LV, "Ge_Volume", vacuume_Can_LV, 0, 3 );

      placement[0] = delta1[0];
      placement[1] = -delta2[1];
//      placement[2] = 0.0 * mm;

      G4RotationMatrix* rot30 = new G4RotationMatrix();//(matrixColumn1, matrixColumn2, matrixColumn3);
      rot30->rotateZ( 90 * degree );
      m_Rotations.push_back(rot30);// automatically deletes it later...

      new G4PVPlacement( rot30, placement, ge1_LV, "Ge_Volume", vacuume_Can_LV, 0, 2 );
    }		// END OF BASIC Modules
//    else if( i == 6 )
//		{
//      const G4double ysoX = 50.0 * mm;
//      const G4double ysoY = 50.0 * mm;
//      const G4double ysoZ = 5.0 * mm;
//
//      const G4double ysoXhalf = 0.5 * ysoX;
//      const G4double ysoYhalf = 0.5 * ysoY;
//      const G4double ysoZhalf = 0.5 * ysoZ;
//
//
//      G4VSolid* yso_Solid =  new G4Box("YSO_Solid", ysoXhalf, ysoYhalf, ysoZhalf );	
//      G4LogicalVolume* yso_LV = new G4LogicalVolume( yso_Solid, scintMaterial, "YSO_Logical");
////      m_Pipe_LogicalVolumes.push_back( yso_LV );// USED FOR DRAWING
//      m_ModuleTotal_LogicalVolumes.push_back( yso_LV );// USED FOR DRAWING and OVERALL PLACEMENT
////      m_NaI_LogicalVolumes.push_back( yso_LV );// USED FOR DRAWING and Recording energy!
//    }

	}
}


//************************************************************************************
//Specific setups

void LENSLongLiquidScintillator::makeDefaultSingleModule()
{
// Default single module
	m_SingleModuleOnly = true;
	
	//G4LogicalVolume* worldLogical = m_MainDetectorConstructor->GetWorldLogicalVolume();
	
  // GRIFFIN Modules are not centered on total GRIFFIN volume, but is offset to center of back part of module
//	const G4ThreeVector placement( 0.0, 0.0, -11.0 * cm - 0.5 * ( 107.0 ) * mm );
  // Clover Modules offset
//	const G4ThreeVector placement( 0.0, 0.0, -11.0 * cm -0.5 * ( 100.0 ) * mm );// For Our colvers

//	const G4ThreeVector placement( 0.0, 0.0, -11.0 * cm -0.5 * ( 103.0 ) * mm );// Offset for GRIFFIN
// Offset for 7x7 = -2.0 * 10, 7x8 = -1.0 * 10.0, 7x9.0 = -0.0 * 10.0	
  // 12.45 cm to can face = 13.3 cm to crystal face
  // 13.35 cm to can face = 14.0 cm to crystal face
//  const G4ThreeVector placement( 0.0, 0.0, -11.46 * cm -0.5 * ( 107.0 - 2.0 * 10.0 ) * mm );// 7 cm length
//  const G4ThreeVector placement( 0.0, 0.0, -13.35 * cm -0.5 * ( 107.0 - 2.0 * 10.0 ) * mm );// 7 cm length
//  const G4ThreeVector placement( 0.0, 0.0, -13.35 * cm -0.5 * ( 107.0 - 1.0 * 10.0 ) * mm );// 8 cm length
//  const G4ThreeVector placement( 0.0, 0.0, -13.35 * cm -0.5 * ( 107.0 - 0.0 * 10.0 ) * mm );// 9 cm length
	// R = -2.4142 * ( OuterRad - trdface ) + 16.899

//  const G4ThreeVector placement( 0.0, 0.0, -4.15 * cm -0.5 * ( 107.0 - 0.0 * 10.0 ) * mm );// 9 cm length vs distance
// For ORNL Clovers
//  const G4ThreeVector placement( 0.0 * cm, 0.0, -12.15 * cm -0.5 * ( 297.0 ) * mm );// ORNL Clovers with 8.5 to front face
  const G4double dist = 3.5 * cm;
  G4ThreeVector placement( 2.25 * cm, 0.0 * cm, -dist -0.5 * ( 297.0 ) * mm );// ORNL Clovers with 8.5 to front face

	G4cout << "Number of Total Module Logical Volumes is: " << m_ModuleTotal_LogicalVolumes.size() << G4endl;
// Need to change modID calculator at bottom of file...
	new G4PVPlacement( 0, placement, m_ModuleTotal_LogicalVolumes[1], "SingleModule_Volume", m_Gas_LogicalVolumes[0], 0, 0 );

    if( true )// place YS0
		{
      G4Material* scintMaterial = m_MainDetectorConstructor->GetYSO(); // Or GetBC408();
      
      const G4double ysoX = 5.0 * mm;
      const G4double ysoY = 50.0 * mm;
      const G4double ysoZ = 50.0 * mm;

      const G4double ysoXhalf = 0.5 * ysoX;
      const G4double ysoYhalf = 0.5 * ysoY;
      const G4double ysoZhalf = 0.5 * ysoZ;


      G4VSolid* yso_Solid =  new G4Box("YSO_Solid", ysoXhalf, ysoYhalf, ysoZhalf );	
      G4LogicalVolume* yso_LV = new G4LogicalVolume( yso_Solid, scintMaterial, "YSO_Logical");
//      m_Pipe_LogicalVolumes.push_back( yso_LV );// USED FOR DRAWING
      m_ModuleTotal_LogicalVolumes.push_back( yso_LV );// USED FOR DRAWING and OVERALL PLACEMENT
//      m_NaI_LogicalVolumes.push_back( yso_LV );// USED FOR DRAWING and Recording energy!

      placement = G4ThreeVector( -ysoXhalf, 0.0 * cm, -dist + ysoZhalf + 1.5 * cm );
      new G4PVPlacement( 0, placement, yso_LV, "YSO_Volume", m_Gas_LogicalVolumes[0], 0, 1 );
    }
}

// ************************************************************************************

void LENSLongLiquidScintillator::makeSingleFourLayer()
{
	G4RotationMatrix* rot1 = new G4RotationMatrix();//(matrixColumn1, matrixColumn2, matrixColumn3);
	rot1->rotateY( 90 * degree );
	m_Rotations.push_back(rot1);// automatically deletes it later...

	G4RotationMatrix* rot2 = new G4RotationMatrix();//(matrixColumn1, matrixColumn2, matrixColumn3);
	rot2->rotateY( 180 * degree );
	m_Rotations.push_back(rot2);// automatically deletes it later...

	G4RotationMatrix* rot3 = new G4RotationMatrix();//(matrixColumn1, matrixColumn2, matrixColumn3);
	rot3->rotateY( 270 * degree );
	m_Rotations.push_back(rot3);// automatically deletes it later...
  
  // Make INTO MEMBER VARIABLES!
  const G4double cuThick = 0.50 * mm;
//  const G4double geDeadLayerThick = 0.0050 * mm;
  const G4double vacuumThick = 1.0 * cm;
//  const G4double vacuumFrontThick = 1.0 * cm;
  const G4double alThick = 0.5 * mm;
//  const G4double alFrontThick = 0.6 * mm;// Used fro Carbon fiber front thickness.

  const G4double housingRadius = 0.5 * m_SodiumIodideDimensions[0][0] + cuThick + vacuumThick + alThick;

	G4ThreeVector placement( 0.0, 0.0, -housingRadius - 20.0 * cm );// 20 cm is half the length of the al tube
	
//	G4cout << "Number of Total Module Logical Volumes is: " << m_ModuleTotal_LogicalVolumes.size() << G4endl;

	new G4PVPlacement( 0, placement, m_ModuleTotal_LogicalVolumes[0], "SingleModule_Volume", m_Gas_LogicalVolumes[0], 0, 0 );
  
  placement = G4ThreeVector( housingRadius + 20.0 * cm, 0.0 * cm, 0.0 * cm );

	new G4PVPlacement( rot1, placement, m_ModuleTotal_LogicalVolumes[0], "SingleModule_Volume", m_Gas_LogicalVolumes[0], 0, 1 );

  placement = G4ThreeVector( 0.0, 0.0, housingRadius + 20.0 * cm  );

	new G4PVPlacement( rot2, placement, m_ModuleTotal_LogicalVolumes[0], "SingleModule_Volume", m_Gas_LogicalVolumes[0], 0, 2 );

  placement = G4ThreeVector( -housingRadius - 20.0 * cm, 0.0 * cm, 0.0 * cm );

	new G4PVPlacement( rot3, placement, m_ModuleTotal_LogicalVolumes[0], "SingleModule_Volume", m_Gas_LogicalVolumes[0], 0, 3 );


}

// ************************************************************************************

void LENSLongLiquidScintillator::makeBEGeCube()
{//5 of 6 sides
	G4RotationMatrix* rot1 = new G4RotationMatrix();//(matrixColumn1, matrixColumn2, matrixColumn3);
	rot1->rotateY( 90 * degree );
	m_Rotations.push_back(rot1);// automatically deletes it later...

	G4RotationMatrix* rot2 = new G4RotationMatrix();//(matrixColumn1, matrixColumn2, matrixColumn3);
	rot2->rotateY( 180 * degree );
	m_Rotations.push_back(rot2);// automatically deletes it later...

	G4RotationMatrix* rot3 = new G4RotationMatrix();//(matrixColumn1, matrixColumn2, matrixColumn3);
	rot3->rotateY( 270 * degree );
	m_Rotations.push_back(rot3);// automatically deletes it later...

	G4RotationMatrix* rot4 = new G4RotationMatrix();//(matrixColumn1, matrixColumn2, matrixColumn3);
	rot4->rotateX( -90 * degree );
	m_Rotations.push_back(rot4);// automatically deletes it later...
  
  // Make INTO MEMBER VARIABLES!
  const G4double cuThick = 0.50 * mm;
//  const G4double geDeadLayerThick = 0.0050 * mm;
  const G4double vacuumThick = 1.0 * cm;
//  const G4double vacuumFrontThick = 1.0 * cm;
  const G4double alThick = 0.5 * mm;
//  const G4double alFrontThick = 0.6 * mm;// Used fro Carbon fiber front thickness.

  const G4double housingRadius = 0.5 * m_SodiumIodideDimensions[0][0] + cuThick + vacuumThick + alThick;

	G4ThreeVector placement( 0.0, 0.0, -housingRadius - 20.0 * cm );// 20 cm is half the length of the al tube
	
//	G4cout << "Number of Total Module Logical Volumes is: " << m_ModuleTotal_LogicalVolumes.size() << G4endl;

	new G4PVPlacement( 0, placement, m_ModuleTotal_LogicalVolumes[0], "SingleModule_Volume", m_Gas_LogicalVolumes[0], 0, 0 );
  
  placement = G4ThreeVector( housingRadius + 20.0 * cm, 0.0 * cm, 0.0 * cm );

	new G4PVPlacement( rot1, placement, m_ModuleTotal_LogicalVolumes[0], "SingleModule_Volume", m_Gas_LogicalVolumes[0], 0, 1 );

  placement = G4ThreeVector( 0.0, 0.0, housingRadius + 20.0 * cm  );

	new G4PVPlacement( rot2, placement, m_ModuleTotal_LogicalVolumes[0], "SingleModule_Volume", m_Gas_LogicalVolumes[0], 0, 2 );

  placement = G4ThreeVector( -housingRadius - 20.0 * cm, 0.0 * cm, 0.0 * cm );

	new G4PVPlacement( rot3, placement, m_ModuleTotal_LogicalVolumes[0], "SingleModule_Volume", m_Gas_LogicalVolumes[0], 0, 3 );

  placement = G4ThreeVector( 0.0 * cm, housingRadius + 20.0 * cm, 0.0 * cm );

	new G4PVPlacement( rot4, placement, m_ModuleTotal_LogicalVolumes[0], "SingleModule_Volume", m_Gas_LogicalVolumes[0], 0, 4 );

}

// ************************************************************************************

void LENSLongLiquidScintillator::makeCloverCube()
{//5 of 6 sides
	G4RotationMatrix* rot1 = new G4RotationMatrix();//(matrixColumn1, matrixColumn2, matrixColumn3);
	rot1->rotateY( 90 * degree );
	m_Rotations.push_back(rot1);// automatically deletes it later...

	G4RotationMatrix* rot2 = new G4RotationMatrix();//(matrixColumn1, matrixColumn2, matrixColumn3);
	rot2->rotateY( 180 * degree );
	m_Rotations.push_back(rot2);// automatically deletes it later...

	G4RotationMatrix* rot3 = new G4RotationMatrix();//(matrixColumn1, matrixColumn2, matrixColumn3);
	rot3->rotateY( 270 * degree );
	m_Rotations.push_back(rot3);// automatically deletes it later...

	G4RotationMatrix* rot4 = new G4RotationMatrix();//(matrixColumn1, matrixColumn2, matrixColumn3);
	rot4->rotateX( -90 * degree );
	m_Rotations.push_back(rot4);// automatically deletes it later...
  
  // Make INTO MEMBER VARIABLES!
//  const G4double cuThick = 0.50 * mm;
////  const G4double geDeadLayerThick = 0.0050 * mm;
//  const G4double vacuumThick = 1.0 * cm;
////  const G4double vacuumFrontThick = 1.0 * cm;
//  const G4double alThick = 0.5 * mm;
//  const G4double alFrontThick = 0.6 * mm;// Used fro Carbon fiber front thickness.

  const G4double housingRadius = 0.0 * cm;//0.5 * m_SodiumIodideDimensions[0][0] + cuThick + vacuumThick + alThick;
  const G4double cloverOffset = 9.7 * cm;
  
	G4ThreeVector placement( 0.0, 0.0, -housingRadius - cloverOffset );// 20 cm is half the length of the al tube
	
//	G4cout << "Number of Total Module Logical Volumes is: " << m_ModuleTotal_LogicalVolumes.size() << G4endl;

	new G4PVPlacement( 0, placement, m_ModuleTotal_LogicalVolumes[1], "SingleModule_Volume", m_Gas_LogicalVolumes[0], 0, 0 );
  
  placement = G4ThreeVector( housingRadius + cloverOffset, 0.0 * cm, 0.0 * cm );

	new G4PVPlacement( rot1, placement, m_ModuleTotal_LogicalVolumes[1], "SingleModule_Volume", m_Gas_LogicalVolumes[0], 0, 1 );

  placement = G4ThreeVector( 0.0, 0.0, housingRadius + cloverOffset  );

	new G4PVPlacement( rot2, placement, m_ModuleTotal_LogicalVolumes[1], "SingleModule_Volume", m_Gas_LogicalVolumes[0], 0, 2 );

  placement = G4ThreeVector( -housingRadius - cloverOffset, 0.0 * cm, 0.0 * cm );

	new G4PVPlacement( rot3, placement, m_ModuleTotal_LogicalVolumes[1], "SingleModule_Volume", m_Gas_LogicalVolumes[0], 0, 3 );

// Add fifth face if wanted...
//  placement = G4ThreeVector( 0.0 * cm, housingRadius + cloverOffset, 0.0 * cm );
//
//	new G4PVPlacement( rot4, placement, m_ModuleTotal_LogicalVolumes[1], "SingleModule_Volume", m_Gas_LogicalVolumes[0], 0, 4 );

}

// ************************************************************************************

void LENSLongLiquidScintillator::makeSquareOrthocupola()
{
	G4RotationMatrix* rot1 = new G4RotationMatrix();//(matrixColumn1, matrixColumn2, matrixColumn3);
	rot1->rotateX( -45 * degree );
	m_Rotations.push_back(rot1);// automatically deletes it later...

	G4RotationMatrix* rot2 = new G4RotationMatrix();//(matrixColumn1, matrixColumn2, matrixColumn3);
	rot2->rotateY( 180 * degree );
	m_Rotations.push_back(rot2);// automatically deletes it later...

	G4RotationMatrix* rot3 = new G4RotationMatrix();//(matrixColumn1, matrixColumn2, matrixColumn3);
	rot3->rotateX( -135 * degree );
	m_Rotations.push_back(rot3);// automatically deletes it later...

	G4RotationMatrix* rot4 = new G4RotationMatrix();//(matrixColumn1, matrixColumn2, matrixColumn3);
	rot4->rotateX( -90 * degree );
	m_Rotations.push_back(rot4);// automatically deletes it later...

	G4RotationMatrix* rot5 = new G4RotationMatrix();//(matrixColumn1, matrixColumn2, matrixColumn3);
	rot5->rotateX( 45 * degree );
	m_Rotations.push_back(rot5);// automatically deletes it later...

	G4RotationMatrix* rot6 = new G4RotationMatrix();//(matrixColumn1, matrixColumn2, matrixColumn3);
	rot6->rotateX( 135 * degree );
	m_Rotations.push_back(rot6);// automatically deletes it later...
  
	G4RotationMatrix* rot7 = new G4RotationMatrix();//(matrixColumn1, matrixColumn2, matrixColumn3);
	rot7->rotateY( -45 * degree );
	m_Rotations.push_back(rot7);// automatically deletes it later...

	G4RotationMatrix* rot8 = new G4RotationMatrix();//(matrixColumn1, matrixColumn2, matrixColumn3);
	rot8->rotateY( -135 * degree );
	m_Rotations.push_back(rot8);// automatically deletes it later...

	G4RotationMatrix* rot9 = new G4RotationMatrix();//(matrixColumn1, matrixColumn2, matrixColumn3);
	rot9->rotateY( 45 * degree );
	m_Rotations.push_back(rot9);// automatically deletes it later...

	G4RotationMatrix* rot10 = new G4RotationMatrix();//(matrixColumn1, matrixColumn2, matrixColumn3);
	rot10->rotateY( 135 * degree );
	m_Rotations.push_back(rot10);// automatically deletes it later...

  // Make INTO MEMBER VARIABLES!
//  const G4double cuThick = 0.50 * mm;
////  const G4double geDeadLayerThick = 0.0050 * mm;
//  const G4double vacuumThick = 1.0 * cm;
////  const G4double vacuumFrontThick = 1.0 * cm;
//  const G4double alThick = 0.5 * mm;
//  const G4double alFrontThick = 0.6 * mm;// Used fro Carbon fiber front thickness.

  const G4double housingRadius = 7.07 * cm;//0.5 * m_SodiumIodideDimensions[0][0] + cuThick + vacuumThick + alThick;
  const G4double cloverOffset = 9.7 * cm;
  
	G4ThreeVector placement( 0.0, 0.0, -housingRadius - 20.1 * cm );// 20 cm is half the length of the al tube
	
//	G4cout << "Number of Total Module Logical Volumes is: " << m_ModuleTotal_LogicalVolumes.size() << G4endl;

	new G4PVPlacement( 0, placement, m_ModuleTotal_LogicalVolumes[0], "SingleModule_Volume", m_Gas_LogicalVolumes[0], 0, 0 );
  
  placement = G4ThreeVector( 0.0, 0.0, housingRadius + 20.1 * cm  );

	new G4PVPlacement( rot2, placement, m_ModuleTotal_LogicalVolumes[0], "SingleModule_Volume", m_Gas_LogicalVolumes[0], 0, 1 );

  
  placement = G4ThreeVector( 0.0 * cm, 0.721 * ( housingRadius + cloverOffset), -0.403 * ( housingRadius + cloverOffset) );

	new G4PVPlacement( rot1, placement, m_ModuleTotal_LogicalVolumes[1], "SingleModule_Volume", m_Gas_LogicalVolumes[0], 0, 2 );

  placement = G4ThreeVector( 0.0 * cm, 0.721 * ( housingRadius + cloverOffset), 0.403 * ( housingRadius + cloverOffset) );

	new G4PVPlacement( rot3, placement, m_ModuleTotal_LogicalVolumes[1], "SingleModule_Volume", m_Gas_LogicalVolumes[0], 0, 3 );

  placement = G4ThreeVector( 0.0 * cm, -0.721 * ( housingRadius + cloverOffset), -0.403 * ( housingRadius + cloverOffset) );

	new G4PVPlacement( rot5, placement, m_ModuleTotal_LogicalVolumes[1], "SingleModule_Volume", m_Gas_LogicalVolumes[0], 0, 4 );

  placement = G4ThreeVector( 0.0 * cm, -0.721 * ( housingRadius + cloverOffset), 0.403 * ( housingRadius + cloverOffset) );

	new G4PVPlacement( rot6, placement, m_ModuleTotal_LogicalVolumes[1], "SingleModule_Volume", m_Gas_LogicalVolumes[0], 0, 5 );


  placement = G4ThreeVector( -0.721 * ( housingRadius + cloverOffset), 0.0 * cm, -0.403 * ( housingRadius + cloverOffset) );

	new G4PVPlacement( rot7, placement, m_ModuleTotal_LogicalVolumes[1], "SingleModule_Volume", m_Gas_LogicalVolumes[0], 0, 6 );

  placement = G4ThreeVector( -0.721 * ( housingRadius + cloverOffset), 0.0 * cm, 0.403 * ( housingRadius + cloverOffset) );

	new G4PVPlacement( rot8, placement, m_ModuleTotal_LogicalVolumes[1], "SingleModule_Volume", m_Gas_LogicalVolumes[0], 0, 7 );

  placement = G4ThreeVector( 0.721 * ( housingRadius + cloverOffset), 0.0 * cm, -0.403 * ( housingRadius + cloverOffset) );

	new G4PVPlacement( rot9, placement, m_ModuleTotal_LogicalVolumes[1], "SingleModule_Volume", m_Gas_LogicalVolumes[0], 0, 8 );

  placement = G4ThreeVector( 0.721 * ( housingRadius + cloverOffset), 0.0 * cm, 0.403 * ( housingRadius + cloverOffset) );

	new G4PVPlacement( rot10, placement, m_ModuleTotal_LogicalVolumes[1], "SingleModule_Volume", m_Gas_LogicalVolumes[0], 0, 9 );

}

// ************************************************************************************

void LENSLongLiquidScintillator::makeCloverSquareOrthocupola()
{
	G4RotationMatrix* rot1 = new G4RotationMatrix();//(matrixColumn1, matrixColumn2, matrixColumn3);
	rot1->rotateX( -45 * degree );
	m_Rotations.push_back(rot1);// automatically deletes it later...

	G4RotationMatrix* rot2 = new G4RotationMatrix();//(matrixColumn1, matrixColumn2, matrixColumn3);
	rot2->rotateY( 180 * degree );
	m_Rotations.push_back(rot2);// automatically deletes it later...

	G4RotationMatrix* rot3 = new G4RotationMatrix();//(matrixColumn1, matrixColumn2, matrixColumn3);
	rot3->rotateX( -135 * degree );
	m_Rotations.push_back(rot3);// automatically deletes it later...

	G4RotationMatrix* rot4 = new G4RotationMatrix();//(matrixColumn1, matrixColumn2, matrixColumn3);
	rot4->rotateX( -90 * degree );
	m_Rotations.push_back(rot4);// automatically deletes it later...

	G4RotationMatrix* rot5 = new G4RotationMatrix();//(matrixColumn1, matrixColumn2, matrixColumn3);
	rot5->rotateX( 45 * degree );
	m_Rotations.push_back(rot5);// automatically deletes it later...

	G4RotationMatrix* rot6 = new G4RotationMatrix();//(matrixColumn1, matrixColumn2, matrixColumn3);
	rot6->rotateX( 135 * degree );
	m_Rotations.push_back(rot6);// automatically deletes it later...
  
	G4RotationMatrix* rot7 = new G4RotationMatrix();//(matrixColumn1, matrixColumn2, matrixColumn3);
	rot7->rotateY( -45 * degree );
	m_Rotations.push_back(rot7);// automatically deletes it later...

	G4RotationMatrix* rot8 = new G4RotationMatrix();//(matrixColumn1, matrixColumn2, matrixColumn3);
	rot8->rotateY( -135 * degree );
	m_Rotations.push_back(rot8);// automatically deletes it later...

	G4RotationMatrix* rot9 = new G4RotationMatrix();//(matrixColumn1, matrixColumn2, matrixColumn3);
	rot9->rotateY( 45 * degree );
	m_Rotations.push_back(rot9);// automatically deletes it later...

	G4RotationMatrix* rot10 = new G4RotationMatrix();//(matrixColumn1, matrixColumn2, matrixColumn3);
	rot10->rotateY( 135 * degree );
	m_Rotations.push_back(rot10);// automatically deletes it later...

  // Make INTO MEMBER VARIABLES!
//  const G4double cuThick = 0.50 * mm;
////  const G4double geDeadLayerThick = 0.0050 * mm;
//  const G4double vacuumThick = 1.0 * cm;
////  const G4double vacuumFrontThick = 1.0 * cm;
//  const G4double alThick = 0.5 * mm;
//  const G4double alFrontThick = 0.6 * mm;// Used fro Carbon fiber front thickness.

  const G4double housingRadius = 7.07 * cm;//0.5 * m_SodiumIodideDimensions[0][0] + cuThick + vacuumThick + alThick;
  const G4double cloverOffset = 9.7 * cm;
  const G4double zCloverOffset = 0.5 * 101.0 * mm;
  
	G4ThreeVector placement( 0.0, 0.0, -housingRadius - zCloverOffset );// 
	
//	G4cout << "Number of Total Module Logical Volumes is: " << m_ModuleTotal_LogicalVolumes.size() << G4endl;

	new G4PVPlacement( 0, placement, m_ModuleTotal_LogicalVolumes[1], "SingleModule_Volume", m_Gas_LogicalVolumes[0], 0, 0 );
  
  placement = G4ThreeVector( 0.0, 0.0, housingRadius + zCloverOffset );

	new G4PVPlacement( rot2, placement, m_ModuleTotal_LogicalVolumes[1], "SingleModule_Volume", m_Gas_LogicalVolumes[0], 0, 1 );

  
  placement = G4ThreeVector( 0.0 * cm, 0.721 * ( housingRadius + cloverOffset), -0.403 * ( housingRadius + cloverOffset) );

	new G4PVPlacement( rot1, placement, m_ModuleTotal_LogicalVolumes[1], "SingleModule_Volume", m_Gas_LogicalVolumes[0], 0, 2 );

  placement = G4ThreeVector( 0.0 * cm, 0.721 * ( housingRadius + cloverOffset), 0.403 * ( housingRadius + cloverOffset) );

	new G4PVPlacement( rot3, placement, m_ModuleTotal_LogicalVolumes[1], "SingleModule_Volume", m_Gas_LogicalVolumes[0], 0, 3 );

  placement = G4ThreeVector( 0.0 * cm, -0.721 * ( housingRadius + cloverOffset), -0.403 * ( housingRadius + cloverOffset) );

	new G4PVPlacement( rot5, placement, m_ModuleTotal_LogicalVolumes[1], "SingleModule_Volume", m_Gas_LogicalVolumes[0], 0, 4 );

  placement = G4ThreeVector( 0.0 * cm, -0.721 * ( housingRadius + cloverOffset), 0.403 * ( housingRadius + cloverOffset) );

	new G4PVPlacement( rot6, placement, m_ModuleTotal_LogicalVolumes[1], "SingleModule_Volume", m_Gas_LogicalVolumes[0], 0, 5 );


  placement = G4ThreeVector( -0.721 * ( housingRadius + cloverOffset), 0.0 * cm, -0.403 * ( housingRadius + cloverOffset) );

	new G4PVPlacement( rot7, placement, m_ModuleTotal_LogicalVolumes[1], "SingleModule_Volume", m_Gas_LogicalVolumes[0], 0, 6 );

  placement = G4ThreeVector( -0.721 * ( housingRadius + cloverOffset), 0.0 * cm, 0.403 * ( housingRadius + cloverOffset) );

	new G4PVPlacement( rot8, placement, m_ModuleTotal_LogicalVolumes[1], "SingleModule_Volume", m_Gas_LogicalVolumes[0], 0, 7 );

  placement = G4ThreeVector( 0.721 * ( housingRadius + cloverOffset), 0.0 * cm, -0.403 * ( housingRadius + cloverOffset) );

	new G4PVPlacement( rot9, placement, m_ModuleTotal_LogicalVolumes[1], "SingleModule_Volume", m_Gas_LogicalVolumes[0], 0, 8 );

  placement = G4ThreeVector( 0.721 * ( housingRadius + cloverOffset), 0.0 * cm, 0.403 * ( housingRadius + cloverOffset) );

	new G4PVPlacement( rot10, placement, m_ModuleTotal_LogicalVolumes[1], "SingleModule_Volume", m_Gas_LogicalVolumes[0], 0, 9 );

}

// ************************************************************************************

void LENSLongLiquidScintillator::makeCloverRhombicuboctahedron()
{
	G4RotationMatrix* rot1 = new G4RotationMatrix();//(matrixColumn1, matrixColumn2, matrixColumn3);
	rot1->rotateX( -45 * degree );
	m_Rotations.push_back(rot1);// automatically deletes it later...

	G4RotationMatrix* rot2 = new G4RotationMatrix();//(matrixColumn1, matrixColumn2, matrixColumn3);
	rot2->rotateY( 180 * degree );
	m_Rotations.push_back(rot2);// automatically deletes it later...

	G4RotationMatrix* rot3 = new G4RotationMatrix();//(matrixColumn1, matrixColumn2, matrixColumn3);
	rot3->rotateX( -135 * degree );
	m_Rotations.push_back(rot3);// automatically deletes it later...

	G4RotationMatrix* rot4 = new G4RotationMatrix();//(matrixColumn1, matrixColumn2, matrixColumn3);
	rot4->rotateX( -90 * degree );
	rot4->rotateY( -90 * degree );
	m_Rotations.push_back(rot4);// automatically deletes it later...

	G4RotationMatrix* rot5 = new G4RotationMatrix();//(matrixColumn1, matrixColumn2, matrixColumn3);
	rot5->rotateX( 45 * degree );
	m_Rotations.push_back(rot5);// automatically deletes it later...

	G4RotationMatrix* rot6 = new G4RotationMatrix();//(matrixColumn1, matrixColumn2, matrixColumn3);
	rot6->rotateX( 135 * degree );
	m_Rotations.push_back(rot6);// automatically deletes it later...
  
	G4RotationMatrix* rot7 = new G4RotationMatrix();//(matrixColumn1, matrixColumn2, matrixColumn3);
	rot7->rotateY( -45 * degree );
	m_Rotations.push_back(rot7);// automatically deletes it later...

	G4RotationMatrix* rot8 = new G4RotationMatrix();//(matrixColumn1, matrixColumn2, matrixColumn3);
	rot8->rotateY( -135 * degree );
	m_Rotations.push_back(rot8);// automatically deletes it later...

	G4RotationMatrix* rot9 = new G4RotationMatrix();//(matrixColumn1, matrixColumn2, matrixColumn3);
	rot9->rotateY( 45 * degree );
	m_Rotations.push_back(rot9);// automatically deletes it later...

	G4RotationMatrix* rot10 = new G4RotationMatrix();//(matrixColumn1, matrixColumn2, matrixColumn3);
	rot10->rotateY( 135 * degree );
	m_Rotations.push_back(rot10);// automatically deletes it later...

	G4RotationMatrix* rot11 = new G4RotationMatrix();//(matrixColumn1, matrixColumn2, matrixColumn3);
	rot11->rotateX( -90 * degree );
	rot11->rotateY( -135* degree );
	m_Rotations.push_back( rot11 );// automatically deletes it later...

	G4RotationMatrix* rot12 = new G4RotationMatrix();//(matrixColumn1, matrixColumn2, matrixColumn3);
	rot12->rotateX( -90 * degree );
	rot12->rotateY( -180* degree );
	m_Rotations.push_back( rot12 );// automatically deletes it later...

	G4RotationMatrix* rot13 = new G4RotationMatrix();//(matrixColumn1, matrixColumn2, matrixColumn3);
	rot13->rotateX( -90 * degree );
	rot13->rotateY( -225* degree );
	m_Rotations.push_back( rot13 );// automatically deletes it later...

	G4RotationMatrix* rot14 = new G4RotationMatrix();//(matrixColumn1, matrixColumn2, matrixColumn3);
	rot14->rotateX( -90 * degree );
	rot14->rotateY( 90* degree );
	m_Rotations.push_back( rot14 );// automatically deletes it later...

	G4RotationMatrix* rot15 = new G4RotationMatrix();//(matrixColumn1, matrixColumn2, matrixColumn3);
	rot15->rotateX( -90 * degree );
	rot15->rotateY( 45* degree );
	m_Rotations.push_back( rot15 );// automatically deletes it later...

	G4RotationMatrix* rot16 = new G4RotationMatrix();//(matrixColumn1, matrixColumn2, matrixColumn3);
	rot16->rotateX( -90 * degree );
//	rot16->rotateY( 45* degree );
	m_Rotations.push_back( rot16 );// automatically deletes it later...

	G4RotationMatrix* rot17 = new G4RotationMatrix();//(matrixColumn1, matrixColumn2, matrixColumn3);
	rot17->rotateX( -90 * degree );
	rot17->rotateY( -45* degree );
	m_Rotations.push_back( rot17 );// automatically deletes it later...

  // Make INTO MEMBER VARIABLES!
//  const G4double cuThick = 0.50 * mm;
////  const G4double geDeadLayerThick = 0.0050 * mm;
//  const G4double vacuumThick = 1.0 * cm;
////  const G4double vacuumFrontThick = 1.0 * cm;
//  const G4double alThick = 0.5 * mm;
//  const G4double alFrontThick = 0.6 * mm;// Used fro Carbon fiber front thickness.

  const G4double housingRadius = 7.07 * cm;//0.5 * m_SodiumIodideDimensions[0][0] + cuThick + vacuumThick + alThick;
  const G4double cloverOffset = 9.7 * cm;
  const G4double zOffset = 0.5 * 101.0 * mm;
  
  G4LogicalVolume* logVol = m_ModuleTotal_LogicalVolumes[1];
	
  G4bool setAll = true;// used to set 7 together ~half coverage

  
//	G4cout << "Number of Total Module Logical Volumes is: " << m_ModuleTotal_LogicalVolumes.size() << G4endl;
	G4ThreeVector placement( 0.0, 0.0, -housingRadius - cloverOffset );
	if( setAll ) new G4PVPlacement( 0, placement, logVol, "SingleModule_Volume", m_Gas_LogicalVolumes[0], 0, 0 );
  
  placement = G4ThreeVector( 0.0, 0.0, housingRadius + cloverOffset );
  new G4PVPlacement( rot2, placement, logVol, "SingleModule_Volume", m_Gas_LogicalVolumes[0], 0, 1 );
  
  placement = G4ThreeVector( 0.0 * cm, 0.721 * ( housingRadius + cloverOffset), -0.403 * ( housingRadius + cloverOffset) - zOffset );
	if( setAll ) new G4PVPlacement( rot1, placement, logVol, "SingleModule_Volume", m_Gas_LogicalVolumes[0], 0, 2 );

  placement = G4ThreeVector( 0.0 * cm, 0.721 * ( housingRadius + cloverOffset), 0.403 * ( housingRadius + cloverOffset) + zOffset );
  new G4PVPlacement( rot3, placement, logVol, "SingleModule_Volume", m_Gas_LogicalVolumes[0], 0, 3 );

  placement = G4ThreeVector( 0.0 * cm, -0.721 * ( housingRadius + cloverOffset), -0.403 * ( housingRadius + cloverOffset) - zOffset );
  if( setAll ) new G4PVPlacement( rot5, placement, logVol, "SingleModule_Volume", m_Gas_LogicalVolumes[0], 0, 4 );

  placement = G4ThreeVector( 0.0 * cm, -0.721 * ( housingRadius + cloverOffset), 0.403 * ( housingRadius + cloverOffset) + zOffset );
  new G4PVPlacement( rot6, placement, logVol, "SingleModule_Volume", m_Gas_LogicalVolumes[0], 0, 5 );

  placement = G4ThreeVector( -0.721 * ( housingRadius + cloverOffset), 0.0 * cm, -0.403 * ( housingRadius + cloverOffset) - zOffset );
  if( setAll ) new G4PVPlacement( rot7, placement, logVol, "SingleModule_Volume", m_Gas_LogicalVolumes[0], 0, 6 );

  placement = G4ThreeVector( -0.721 * ( housingRadius + cloverOffset), 0.0 * cm, 0.403 * ( housingRadius + cloverOffset) + zOffset );
  new G4PVPlacement( rot8, placement, logVol, "SingleModule_Volume", m_Gas_LogicalVolumes[0], 0, 7 );

  placement = G4ThreeVector( 0.721 * ( housingRadius + cloverOffset), 0.0 * cm, -0.403 * ( housingRadius + cloverOffset) - zOffset );
  if( setAll ) new G4PVPlacement( rot9, placement, logVol, "SingleModule_Volume", m_Gas_LogicalVolumes[0], 0, 8 );

  placement = G4ThreeVector( 0.721 * ( housingRadius + cloverOffset), 0.0 * cm, 0.403 * ( housingRadius + cloverOffset) + zOffset);
  new G4PVPlacement( rot10, placement, logVol, "SingleModule_Volume", m_Gas_LogicalVolumes[0], 0, 9 );

// Place middle ring every 45 degrees

  placement = G4ThreeVector( -housingRadius - cloverOffset, 0.0, 0.0 );
	new G4PVPlacement( rot4, placement, logVol, "SingleModule_Volume", m_Gas_LogicalVolumes[0], 0, 10 );

  placement = G4ThreeVector( -0.707 * ( housingRadius + cloverOffset ), -0.707 * ( housingRadius + cloverOffset ), 0.0 );
	new G4PVPlacement( rot11, placement, logVol, "SingleModule_Volume", m_Gas_LogicalVolumes[0], 0, 11 );

  placement = G4ThreeVector( 0.0, -housingRadius - cloverOffset, 0.0 );
	new G4PVPlacement( rot12, placement, logVol, "SingleModule_Volume", m_Gas_LogicalVolumes[0], 0, 12 );

  placement = G4ThreeVector( 0.707 * ( housingRadius + cloverOffset ), -0.707 * ( housingRadius + cloverOffset ), 0.0 );
	if( setAll ) new G4PVPlacement( rot13, placement, logVol, "SingleModule_Volume", m_Gas_LogicalVolumes[0], 0, 13 );

  placement = G4ThreeVector( housingRadius + cloverOffset, 0.0, 0.0 );
	if( setAll ) new G4PVPlacement( rot14, placement, logVol, "SingleModule_Volume", m_Gas_LogicalVolumes[0], 0, 14 );

  placement = G4ThreeVector( 0.707 * ( housingRadius + cloverOffset ), 0.707 * ( housingRadius + cloverOffset ), 0.0 );
	if( setAll ) new G4PVPlacement( rot15, placement, logVol, "SingleModule_Volume", m_Gas_LogicalVolumes[0], 0, 15 );

  placement = G4ThreeVector( 0.0, housingRadius + cloverOffset, 0.0 );
	if( setAll ) new G4PVPlacement( rot16, placement, logVol, "SingleModule_Volume", m_Gas_LogicalVolumes[0], 0, 16 );

  placement = G4ThreeVector( -0.707 * ( housingRadius + cloverOffset ), 0.707 * ( housingRadius + cloverOffset ), 0.0 );
	if( setAll ) new G4PVPlacement( rot17, placement, logVol, "SingleModule_Volume", m_Gas_LogicalVolumes[0], 0, 17 );
}

// ************************************************************************************


void LENSLongLiquidScintillator::makeGRIFFINCube()
{//5 of 6 sides
	G4RotationMatrix* rot1 = new G4RotationMatrix();//(matrixColumn1, matrixColumn2, matrixColumn3);
	rot1->rotateY( 90 * degree );
	m_Rotations.push_back(rot1);// automatically deletes it later...

	G4RotationMatrix* rot2 = new G4RotationMatrix();//(matrixColumn1, matrixColumn2, matrixColumn3);
	rot2->rotateY( 180 * degree );
	m_Rotations.push_back(rot2);// automatically deletes it later...

	G4RotationMatrix* rot3 = new G4RotationMatrix();//(matrixColumn1, matrixColumn2, matrixColumn3);
	rot3->rotateY( 270 * degree );
	m_Rotations.push_back(rot3);// automatically deletes it later...

	G4RotationMatrix* rot4 = new G4RotationMatrix();//(matrixColumn1, matrixColumn2, matrixColumn3);
	rot4->rotateX( -90 * degree );
	m_Rotations.push_back(rot4);// automatically deletes it later...
  
  // Make INTO MEMBER VARIABLES!
//  const G4double cuThick = 0.50 * mm;
////  const G4double geDeadLayerThick = 0.0050 * mm;
//  const G4double vacuumThick = 1.0 * cm;
////  const G4double vacuumFrontThick = 1.0 * cm;
//  const G4double alThick = 0.5 * mm;
//  const G4double alFrontThick = 0.6 * mm;// Used fro Carbon fiber front thickness.

  const G4double housingRadius = 0.0 * cm;//0.5 * m_SodiumIodideDimensions[0][0] + cuThick + vacuumThick + alThick;
  const G4double cloverOffset = 14.2 * cm;
  
	G4ThreeVector placement( 0.0, 0.0, -housingRadius - cloverOffset );// 20 cm is half the length of the al tube
	
//	G4cout << "Number of Total Module Logical Volumes is: " << m_ModuleTotal_LogicalVolumes.size() << G4endl;

	new G4PVPlacement( 0, placement, m_ModuleTotal_LogicalVolumes[2], "SingleModule_Volume", m_Gas_LogicalVolumes[0], 0, 0 );
  
  placement = G4ThreeVector( housingRadius + cloverOffset, 0.0 * cm, 0.0 * cm );

	new G4PVPlacement( rot1, placement, m_ModuleTotal_LogicalVolumes[2], "SingleModule_Volume", m_Gas_LogicalVolumes[0], 0, 1 );

  placement = G4ThreeVector( 0.0, 0.0, housingRadius + cloverOffset  );

	new G4PVPlacement( rot2, placement, m_ModuleTotal_LogicalVolumes[2], "SingleModule_Volume", m_Gas_LogicalVolumes[0], 0, 2 );

  placement = G4ThreeVector( -housingRadius - cloverOffset, 0.0 * cm, 0.0 * cm );

	new G4PVPlacement( rot3, placement, m_ModuleTotal_LogicalVolumes[2], "SingleModule_Volume", m_Gas_LogicalVolumes[0], 0, 3 );

// Add fifth face if wanted...
//  placement = G4ThreeVector( 0.0 * cm, housingRadius + cloverOffset, 0.0 * cm );
//
//	new G4PVPlacement( rot4, placement, m_ModuleTotal_LogicalVolumes[2], "SingleModule_Volume", m_Gas_LogicalVolumes[0], 0, 4 );

}

void LENSLongLiquidScintillator::makeGRIFFINSquareOrthocupola()
{
	G4RotationMatrix* rot1 = new G4RotationMatrix();//(matrixColumn1, matrixColumn2, matrixColumn3);
	rot1->rotateX( -45 * degree );
	m_Rotations.push_back(rot1);// automatically deletes it later...

	G4RotationMatrix* rot2 = new G4RotationMatrix();//(matrixColumn1, matrixColumn2, matrixColumn3);
	rot2->rotateY( 180 * degree );
	m_Rotations.push_back(rot2);// automatically deletes it later...

	G4RotationMatrix* rot3 = new G4RotationMatrix();//(matrixColumn1, matrixColumn2, matrixColumn3);
	rot3->rotateX( -135 * degree );
	m_Rotations.push_back(rot3);// automatically deletes it later...

	G4RotationMatrix* rot4 = new G4RotationMatrix();//(matrixColumn1, matrixColumn2, matrixColumn3);
	rot4->rotateX( -90 * degree );
	m_Rotations.push_back(rot4);// automatically deletes it later...

	G4RotationMatrix* rot5 = new G4RotationMatrix();//(matrixColumn1, matrixColumn2, matrixColumn3);
	rot5->rotateX( 45 * degree );
	m_Rotations.push_back(rot5);// automatically deletes it later...

	G4RotationMatrix* rot6 = new G4RotationMatrix();//(matrixColumn1, matrixColumn2, matrixColumn3);
	rot6->rotateX( 135 * degree );
	m_Rotations.push_back(rot6);// automatically deletes it later...
  
	G4RotationMatrix* rot7 = new G4RotationMatrix();//(matrixColumn1, matrixColumn2, matrixColumn3);
	rot7->rotateY( -45 * degree );
	m_Rotations.push_back(rot7);// automatically deletes it later...

	G4RotationMatrix* rot8 = new G4RotationMatrix();//(matrixColumn1, matrixColumn2, matrixColumn3);
	rot8->rotateY( -135 * degree );
	m_Rotations.push_back(rot8);// automatically deletes it later...

	G4RotationMatrix* rot9 = new G4RotationMatrix();//(matrixColumn1, matrixColumn2, matrixColumn3);
	rot9->rotateY( 45 * degree );
	m_Rotations.push_back(rot9);// automatically deletes it later...

	G4RotationMatrix* rot10 = new G4RotationMatrix();//(matrixColumn1, matrixColumn2, matrixColumn3);
	rot10->rotateY( 135 * degree );
	m_Rotations.push_back(rot10);// automatically deletes it later...

  // Make INTO MEMBER VARIABLES!
//  const G4double cuThick = 0.50 * mm;
////  const G4double geDeadLayerThick = 0.0050 * mm;
//  const G4double vacuumThick = 1.0 * cm;
////  const G4double vacuumFrontThick = 1.0 * cm;
//  const G4double alThick = 0.5 * mm;
//  const G4double alFrontThick = 0.6 * mm;// Used fro Carbon fiber front thickness.

  const G4double housingRadius = 10.5 * cm;//7.07 * cm;//0.5 * m_SodiumIodideDimensions[0][0] + cuThick + vacuumThick + alThick;
  const G4double cloverOffset = 14.3 * cm;
  const G4double zCloverOffset = 0.5 * 11.0 * cm;
  
	G4ThreeVector placement( 0.0, 0.0, -housingRadius - zCloverOffset );// 
	
//	G4cout << "Number of Total Module Logical Volumes is: " << m_ModuleTotal_LogicalVolumes.size() << G4endl;

	new G4PVPlacement( 0, placement, m_ModuleTotal_LogicalVolumes[2], "SingleModule_Volume", m_Gas_LogicalVolumes[0], 0, 0 );
  
  placement = G4ThreeVector( 0.0, 0.0, housingRadius + zCloverOffset );

	new G4PVPlacement( rot2, placement, m_ModuleTotal_LogicalVolumes[2], "SingleModule_Volume", m_Gas_LogicalVolumes[0], 0, 1 );

  
  placement = G4ThreeVector( 0.0 * cm, 0.595 * ( housingRadius + cloverOffset), -0.405 * ( housingRadius + cloverOffset) );

	new G4PVPlacement( rot1, placement, m_ModuleTotal_LogicalVolumes[2], "SingleModule_Volume", m_Gas_LogicalVolumes[0], 0, 2 );

  placement = G4ThreeVector( 0.0 * cm, 0.595 * ( housingRadius + cloverOffset), 0.405 * ( housingRadius + cloverOffset) );

	new G4PVPlacement( rot3, placement, m_ModuleTotal_LogicalVolumes[2], "SingleModule_Volume", m_Gas_LogicalVolumes[0], 0, 3 );

  placement = G4ThreeVector( 0.0 * cm, -0.595 * ( housingRadius + cloverOffset), -0.405 * ( housingRadius + cloverOffset) );

	new G4PVPlacement( rot5, placement, m_ModuleTotal_LogicalVolumes[2], "SingleModule_Volume", m_Gas_LogicalVolumes[0], 0, 4 );

  placement = G4ThreeVector( 0.0 * cm, -0.595 * ( housingRadius + cloverOffset), 0.405 * ( housingRadius + cloverOffset) );

	new G4PVPlacement( rot6, placement, m_ModuleTotal_LogicalVolumes[2], "SingleModule_Volume", m_Gas_LogicalVolumes[0], 0, 5 );


  placement = G4ThreeVector( -0.595 * ( housingRadius + cloverOffset), 0.0 * cm, -0.405 * ( housingRadius + cloverOffset) );

	new G4PVPlacement( rot7, placement, m_ModuleTotal_LogicalVolumes[2], "SingleModule_Volume", m_Gas_LogicalVolumes[0], 0, 6 );

  placement = G4ThreeVector( -0.595 * ( housingRadius + cloverOffset), 0.0 * cm, 0.405 * ( housingRadius + cloverOffset) );

	new G4PVPlacement( rot8, placement, m_ModuleTotal_LogicalVolumes[2], "SingleModule_Volume", m_Gas_LogicalVolumes[0], 0, 7 );

  placement = G4ThreeVector( 0.595 * ( housingRadius + cloverOffset), 0.0 * cm, -0.405 * ( housingRadius + cloverOffset) );

	new G4PVPlacement( rot9, placement, m_ModuleTotal_LogicalVolumes[2], "SingleModule_Volume", m_Gas_LogicalVolumes[0], 0, 8 );

  placement = G4ThreeVector( 0.595 * ( housingRadius + cloverOffset), 0.0 * cm, 0.405 * ( housingRadius + cloverOffset) );

	new G4PVPlacement( rot10, placement, m_ModuleTotal_LogicalVolumes[2], "SingleModule_Volume", m_Gas_LogicalVolumes[0], 0, 9 );
}

void LENSLongLiquidScintillator::makeGRIFFINRhombicuboctahedron()
{
	G4RotationMatrix* rot1 = new G4RotationMatrix();//(matrixColumn1, matrixColumn2, matrixColumn3);
	rot1->rotateX( -45 * degree );
	m_Rotations.push_back(rot1);// automatically deletes it later...

	G4RotationMatrix* rot2 = new G4RotationMatrix();//(matrixColumn1, matrixColumn2, matrixColumn3);
	rot2->rotateY( 180 * degree );
	m_Rotations.push_back(rot2);// automatically deletes it later...

	G4RotationMatrix* rot3 = new G4RotationMatrix();//(matrixColumn1, matrixColumn2, matrixColumn3);
	rot3->rotateX( -135 * degree );
	m_Rotations.push_back(rot3);// automatically deletes it later...

	G4RotationMatrix* rot4 = new G4RotationMatrix();//(matrixColumn1, matrixColumn2, matrixColumn3);
	rot4->rotateX( -90 * degree );
	rot4->rotateY( -90 * degree );
	m_Rotations.push_back(rot4);// automatically deletes it later...

	G4RotationMatrix* rot5 = new G4RotationMatrix();//(matrixColumn1, matrixColumn2, matrixColumn3);
	rot5->rotateX( 45 * degree );
	m_Rotations.push_back(rot5);// automatically deletes it later...

	G4RotationMatrix* rot6 = new G4RotationMatrix();//(matrixColumn1, matrixColumn2, matrixColumn3);
	rot6->rotateX( 135 * degree );
	m_Rotations.push_back(rot6);// automatically deletes it later...
  
	G4RotationMatrix* rot7 = new G4RotationMatrix();//(matrixColumn1, matrixColumn2, matrixColumn3);
	rot7->rotateY( -45 * degree );
	m_Rotations.push_back(rot7);// automatically deletes it later...

	G4RotationMatrix* rot8 = new G4RotationMatrix();//(matrixColumn1, matrixColumn2, matrixColumn3);
	rot8->rotateY( -135 * degree );
	m_Rotations.push_back(rot8);// automatically deletes it later...

	G4RotationMatrix* rot9 = new G4RotationMatrix();//(matrixColumn1, matrixColumn2, matrixColumn3);
	rot9->rotateY( 45 * degree );
	m_Rotations.push_back(rot9);// automatically deletes it later...

	G4RotationMatrix* rot10 = new G4RotationMatrix();//(matrixColumn1, matrixColumn2, matrixColumn3);
	rot10->rotateY( 135 * degree );
	m_Rotations.push_back(rot10);// automatically deletes it later...

	G4RotationMatrix* rot11 = new G4RotationMatrix();//(matrixColumn1, matrixColumn2, matrixColumn3);
	rot11->rotateX( -90 * degree );
	rot11->rotateY( -135* degree );
	m_Rotations.push_back( rot11 );// automatically deletes it later...

	G4RotationMatrix* rot12 = new G4RotationMatrix();//(matrixColumn1, matrixColumn2, matrixColumn3);
	rot12->rotateX( -90 * degree );
	rot12->rotateY( -180* degree );
	m_Rotations.push_back( rot12 );// automatically deletes it later...

	G4RotationMatrix* rot13 = new G4RotationMatrix();//(matrixColumn1, matrixColumn2, matrixColumn3);
	rot13->rotateX( -90 * degree );
	rot13->rotateY( -225* degree );
	m_Rotations.push_back( rot13 );// automatically deletes it later...

	G4RotationMatrix* rot14 = new G4RotationMatrix();//(matrixColumn1, matrixColumn2, matrixColumn3);
	rot14->rotateX( -90 * degree );
	rot14->rotateY( 90* degree );
	m_Rotations.push_back( rot14 );// automatically deletes it later...

	G4RotationMatrix* rot15 = new G4RotationMatrix();//(matrixColumn1, matrixColumn2, matrixColumn3);
	rot15->rotateX( -90 * degree );
	rot15->rotateY( 45* degree );
	m_Rotations.push_back( rot15 );// automatically deletes it later...

	G4RotationMatrix* rot16 = new G4RotationMatrix();//(matrixColumn1, matrixColumn2, matrixColumn3);
	rot16->rotateX( -90 * degree );
//	rot16->rotateY( 45* degree );
	m_Rotations.push_back( rot16 );// automatically deletes it later...

	G4RotationMatrix* rot17 = new G4RotationMatrix();//(matrixColumn1, matrixColumn2, matrixColumn3);
	rot17->rotateX( -90 * degree );
	rot17->rotateY( -45* degree );
	m_Rotations.push_back( rot17 );// automatically deletes it later...

  // Make INTO MEMBER VARIABLES!
//  const G4double cuThick = 0.50 * mm;
////  const G4double geDeadLayerThick = 0.0050 * mm;
//  const G4double vacuumThick = 1.0 * cm;
////  const G4double vacuumFrontThick = 1.0 * cm;
//  const G4double alThick = 0.5 * mm;
//  const G4double alFrontThick = 0.6 * mm;// Used fro Carbon fiber front thickness.

//  const G4double housingRadius = 10.2 * cm;//0.5 * m_SodiumIodideDimensions[0][0] + cuThick + vacuumThick + alThick;
//  const G4double cloverOffset = 14.2 * cm;
//  const G4double zOffset = 0.5 * 88.5 * mm;
  
  const G4double housingRadius = 10.5 * cm;//7.07 * cm;//0.5 * m_SodiumIodideDimensions[0][0] + cuThick + vacuumThick + alThick;
  const G4double cloverOffset = 10.5 * cm;
//  const G4double zOffset = 0.5 * 10.5 * cm;
  
  const G4double xyScale = 0.707;
  const G4double perpScale = 1.0;
  
	G4ThreeVector placement( 0.0, 0.0, -perpScale * ( housingRadius + cloverOffset ) );
	
//	G4cout << "Number of Total Module Logical Volumes is: " << m_ModuleTotal_LogicalVolumes.size() << G4endl;

// Uncomment to make full 18 Rhombi...
//	new G4PVPlacement( 0, placement, m_ModuleTotal_LogicalVolumes[2], "SingleModule_Volume", m_Gas_LogicalVolumes[0], 0, 0 );
//  
//  placement = G4ThreeVector( 0.0, 0.0, perpScale * ( housingRadius + cloverOffset ) );
//
//	new G4PVPlacement( rot2, placement, m_ModuleTotal_LogicalVolumes[2], "SingleModule_Volume", m_Gas_LogicalVolumes[0], 0, 1 );

  
  placement = G4ThreeVector( 0.0 * cm, xyScale * ( housingRadius + cloverOffset), -xyScale * ( housingRadius + cloverOffset) );

	new G4PVPlacement( rot1, placement, m_ModuleTotal_LogicalVolumes[2], "SingleModule_Volume", m_Gas_LogicalVolumes[0], 0, 2 );

  placement = G4ThreeVector( 0.0 * cm, xyScale * ( housingRadius + cloverOffset), xyScale * ( housingRadius + cloverOffset) );

	new G4PVPlacement( rot3, placement, m_ModuleTotal_LogicalVolumes[2], "SingleModule_Volume", m_Gas_LogicalVolumes[0], 0, 3 );

  placement = G4ThreeVector( 0.0 * cm, -xyScale * ( housingRadius + cloverOffset), -xyScale * ( housingRadius + cloverOffset) );

	new G4PVPlacement( rot5, placement, m_ModuleTotal_LogicalVolumes[2], "SingleModule_Volume", m_Gas_LogicalVolumes[0], 0, 4 );

  placement = G4ThreeVector( 0.0 * cm, -xyScale * ( housingRadius + cloverOffset), xyScale * ( housingRadius + cloverOffset) );

	new G4PVPlacement( rot6, placement, m_ModuleTotal_LogicalVolumes[2], "SingleModule_Volume", m_Gas_LogicalVolumes[0], 0, 5 );


  placement = G4ThreeVector( -xyScale * ( housingRadius + cloverOffset), 0.0 * cm, -xyScale * ( housingRadius + cloverOffset) );

	new G4PVPlacement( rot7, placement, m_ModuleTotal_LogicalVolumes[2], "SingleModule_Volume", m_Gas_LogicalVolumes[0], 0, 6 );

  placement = G4ThreeVector( -xyScale * ( housingRadius + cloverOffset), 0.0 * cm, xyScale * ( housingRadius + cloverOffset) );

	new G4PVPlacement( rot8, placement, m_ModuleTotal_LogicalVolumes[2], "SingleModule_Volume", m_Gas_LogicalVolumes[0], 0, 7 );

  placement = G4ThreeVector( xyScale * ( housingRadius + cloverOffset), 0.0 * cm, -xyScale * ( housingRadius + cloverOffset) );

	new G4PVPlacement( rot9, placement, m_ModuleTotal_LogicalVolumes[2], "SingleModule_Volume", m_Gas_LogicalVolumes[0], 0, 8 );

  placement = G4ThreeVector( xyScale * ( housingRadius + cloverOffset), 0.0 * cm, xyScale * ( housingRadius + cloverOffset) );

	new G4PVPlacement( rot10, placement, m_ModuleTotal_LogicalVolumes[2], "SingleModule_Volume", m_Gas_LogicalVolumes[0], 0, 9 );
//  
//// Place middle ring every 45 degrees

  placement = G4ThreeVector( -perpScale * ( housingRadius + cloverOffset ), 0.0, 0.0 );

	new G4PVPlacement( rot4, placement, m_ModuleTotal_LogicalVolumes[2], "SingleModule_Volume", m_Gas_LogicalVolumes[0], 0, 10 );

  placement = G4ThreeVector( -xyScale * ( housingRadius + cloverOffset ), -xyScale * ( housingRadius + cloverOffset ), 0.0 );

	new G4PVPlacement( rot11, placement, m_ModuleTotal_LogicalVolumes[2], "SingleModule_Volume", m_Gas_LogicalVolumes[0], 0, 11 );

  placement = G4ThreeVector( 0.0, -perpScale * ( housingRadius + cloverOffset ), 0.0 );

	new G4PVPlacement( rot12, placement, m_ModuleTotal_LogicalVolumes[2], "SingleModule_Volume", m_Gas_LogicalVolumes[0], 0, 12 );

  placement = G4ThreeVector( xyScale * ( housingRadius + cloverOffset ), -xyScale * ( housingRadius + cloverOffset ), 0.0 );

	new G4PVPlacement( rot13, placement, m_ModuleTotal_LogicalVolumes[2], "SingleModule_Volume", m_Gas_LogicalVolumes[0], 0, 13 );

  placement = G4ThreeVector( perpScale * ( housingRadius + cloverOffset ), 0.0, 0.0 );

	new G4PVPlacement( rot14, placement, m_ModuleTotal_LogicalVolumes[2], "SingleModule_Volume", m_Gas_LogicalVolumes[0], 0, 14 );

  placement = G4ThreeVector( xyScale * ( housingRadius + cloverOffset ), xyScale * ( housingRadius + cloverOffset ), 0.0 );

	new G4PVPlacement( rot15, placement, m_ModuleTotal_LogicalVolumes[2], "SingleModule_Volume", m_Gas_LogicalVolumes[0], 0, 15 );

  placement = G4ThreeVector( 0.0, perpScale * ( housingRadius + cloverOffset ), 0.0 );

	new G4PVPlacement( rot16, placement, m_ModuleTotal_LogicalVolumes[2], "SingleModule_Volume", m_Gas_LogicalVolumes[0], 0, 16 );

  placement = G4ThreeVector( -xyScale * ( housingRadius + cloverOffset ), xyScale * ( housingRadius + cloverOffset ), 0.0 );

	new G4PVPlacement( rot17, placement, m_ModuleTotal_LogicalVolumes[2], "SingleModule_Volume", m_Gas_LogicalVolumes[0], 0, 17 );
}

void LENSLongLiquidScintillator::makeTwoFourLayer()
{
	G4RotationMatrix* rot1 = new G4RotationMatrix();//(matrixColumn1, matrixColumn2, matrixColumn3);
	rot1->rotateY( 90 * degree );
	m_Rotations.push_back(rot1);// automatically deletes it later...

	G4RotationMatrix* rot2 = new G4RotationMatrix();//(matrixColumn1, matrixColumn2, matrixColumn3);
	rot2->rotateY( 180 * degree );
	m_Rotations.push_back(rot2);// automatically deletes it later...

	G4RotationMatrix* rot3 = new G4RotationMatrix();//(matrixColumn1, matrixColumn2, matrixColumn3);
	rot3->rotateY( 270 * degree );
	m_Rotations.push_back(rot3);// automatically deletes it later...
  
  // Make INTO MEMBER VARIABLES!
  const G4double cuThick = 0.50 * mm;
//  const G4double geDeadLayerThick = 0.0050 * mm;
  const G4double vacuumThick = 1.0 * cm;
//  const G4double vacuumFrontThick = 1.0 * cm;
  const G4double alThick = 0.5 * mm;
//  const G4double alFrontThick = 0.6 * mm;// Used fro Carbon fiber front thickness.

  const G4double housingRadius = 0.5 * m_SodiumIodideDimensions[0][0] + cuThick + vacuumThick + alThick;

// Top Set of Four
	G4ThreeVector placement( 0.0, 1.0 * housingRadius, -housingRadius - 20.0 * cm );// 20 cm is half the length of the al tube
	
//	G4cout << "Number of Total Module Logical Volumes is: " << m_ModuleTotal_LogicalVolumes.size() << G4endl;

	new G4PVPlacement( 0, placement, m_ModuleTotal_LogicalVolumes[0], "SingleModule_Volume", m_Gas_LogicalVolumes[0], 0, 0 );
  
  placement = G4ThreeVector( housingRadius + 20.0 * cm, 1.0 * housingRadius, 0.0 * cm );

	new G4PVPlacement( rot1, placement, m_ModuleTotal_LogicalVolumes[0], "SingleModule_Volume", m_Gas_LogicalVolumes[0], 0, 1 );

  placement = G4ThreeVector( 0.0, 1.0 * housingRadius, housingRadius + 20.0 * cm  );

	new G4PVPlacement( rot2, placement, m_ModuleTotal_LogicalVolumes[0], "SingleModule_Volume", m_Gas_LogicalVolumes[0], 0, 2 );

  placement = G4ThreeVector( -housingRadius - 20.0 * cm, 1.0 * housingRadius, 0.0 * cm );

	new G4PVPlacement( rot3, placement, m_ModuleTotal_LogicalVolumes[0], "SingleModule_Volume", m_Gas_LogicalVolumes[0], 0, 3 );

// Bottom set of four
	placement = G4ThreeVector( 0.0, -1.0 * housingRadius, -housingRadius - 20.0 * cm );// 20 cm is half the length of the al tube
	
//	G4cout << "Number of Total Module Logical Volumes is: " << m_ModuleTotal_LogicalVolumes.size() << G4endl;

	new G4PVPlacement( 0, placement, m_ModuleTotal_LogicalVolumes[0], "SingleModule_Volume", m_Gas_LogicalVolumes[0], 0, 4 );
  
  placement = G4ThreeVector( housingRadius + 20.0 * cm, -1.0 * housingRadius, 0.0 * cm );

	new G4PVPlacement( rot1, placement, m_ModuleTotal_LogicalVolumes[0], "SingleModule_Volume", m_Gas_LogicalVolumes[0], 0, 5 );

  placement = G4ThreeVector( 0.0, -1.0 * housingRadius, housingRadius + 20.0 * cm  );

	new G4PVPlacement( rot2, placement, m_ModuleTotal_LogicalVolumes[0], "SingleModule_Volume", m_Gas_LogicalVolumes[0], 0, 6 );

  placement = G4ThreeVector( -housingRadius - 20.0 * cm, -1.0 * housingRadius, 0.0 * cm );

	new G4PVPlacement( rot3, placement, m_ModuleTotal_LogicalVolumes[0], "SingleModule_Volume", m_Gas_LogicalVolumes[0], 0, 7 );


}

// ************************************************************************************

void LENSLongLiquidScintillator::makeThreeFourLayer()
{
	G4RotationMatrix* rot1 = new G4RotationMatrix();//(matrixColumn1, matrixColumn2, matrixColumn3);
	rot1->rotateY( 90 * degree );
	m_Rotations.push_back(rot1);// automatically deletes it later...

	G4RotationMatrix* rot2 = new G4RotationMatrix();//(matrixColumn1, matrixColumn2, matrixColumn3);
	rot2->rotateY( 180 * degree );
	m_Rotations.push_back(rot2);// automatically deletes it later...

	G4RotationMatrix* rot3 = new G4RotationMatrix();//(matrixColumn1, matrixColumn2, matrixColumn3);
	rot3->rotateY( 270 * degree );
	m_Rotations.push_back(rot3);// automatically deletes it later...
  
  // Make INTO MEMBER VARIABLES!
  const G4double cuThick = 0.50 * mm;
//  const G4double geDeadLayerThick = 0.0050 * mm;
  const G4double vacuumThick = 1.0 * cm;
//  const G4double vacuumFrontThick = 1.0 * cm;
  const G4double alThick = 0.5 * mm;
//  const G4double alFrontThick = 0.6 * mm;// Used fro Carbon fiber front thickness.

  const G4double housingRadius = 0.5 * m_SodiumIodideDimensions[0][0] + cuThick + vacuumThick + alThick;

// Top Set of Four
	G4ThreeVector placement( 0.0, 2.0 * housingRadius, -housingRadius - 20.0 * cm );// 20 cm is half the length of the al tube
	
//	G4cout << "Number of Total Module Logical Volumes is: " << m_ModuleTotal_LogicalVolumes.size() << G4endl;

	new G4PVPlacement( 0, placement, m_ModuleTotal_LogicalVolumes[0], "SingleModule_Volume", m_Gas_LogicalVolumes[0], 0, 0 );
  
  placement = G4ThreeVector( housingRadius + 20.0 * cm, 2.0 * housingRadius, 0.0 * cm );

	new G4PVPlacement( rot1, placement, m_ModuleTotal_LogicalVolumes[0], "SingleModule_Volume", m_Gas_LogicalVolumes[0], 0, 1 );

  placement = G4ThreeVector( 0.0, 2.0 * housingRadius, housingRadius + 20.0 * cm  );

	new G4PVPlacement( rot2, placement, m_ModuleTotal_LogicalVolumes[0], "SingleModule_Volume", m_Gas_LogicalVolumes[0], 0, 2 );

  placement = G4ThreeVector( -housingRadius - 20.0 * cm, 2.0 * housingRadius, 0.0 * cm );

	new G4PVPlacement( rot3, placement, m_ModuleTotal_LogicalVolumes[0], "SingleModule_Volume", m_Gas_LogicalVolumes[0], 0, 3 );

// Middle set of four
	placement = G4ThreeVector( 0.0, 0.0 * housingRadius, -housingRadius - 20.0 * cm );// 20 cm is half the length of the al tube
	
//	G4cout << "Number of Total Module Logical Volumes is: " << m_ModuleTotal_LogicalVolumes.size() << G4endl;

	new G4PVPlacement( 0, placement, m_ModuleTotal_LogicalVolumes[0], "SingleModule_Volume", m_Gas_LogicalVolumes[0], 0, 4 );
  
  placement = G4ThreeVector( housingRadius + 20.0 * cm, 0.0 * housingRadius, 0.0 * cm );

	new G4PVPlacement( rot1, placement, m_ModuleTotal_LogicalVolumes[0], "SingleModule_Volume", m_Gas_LogicalVolumes[0], 0, 5 );

  placement = G4ThreeVector( 0.0, 0.0 * housingRadius, housingRadius + 20.0 * cm  );

	new G4PVPlacement( rot2, placement, m_ModuleTotal_LogicalVolumes[0], "SingleModule_Volume", m_Gas_LogicalVolumes[0], 0, 6 );

  placement = G4ThreeVector( -housingRadius - 20.0 * cm, 0.0 * housingRadius, 0.0 * cm );

	new G4PVPlacement( rot3, placement, m_ModuleTotal_LogicalVolumes[0], "SingleModule_Volume", m_Gas_LogicalVolumes[0], 0, 7 );

// Bottom set of four
	placement = G4ThreeVector( 0.0, -2.0 * housingRadius, -housingRadius - 20.0 * cm );// 20 cm is half the length of the al tube
	
//	G4cout << "Number of Total Module Logical Volumes is: " << m_ModuleTotal_LogicalVolumes.size() << G4endl;

	new G4PVPlacement( 0, placement, m_ModuleTotal_LogicalVolumes[0], "SingleModule_Volume", m_Gas_LogicalVolumes[0], 0, 8 );
  
  placement = G4ThreeVector( housingRadius + 20.0 * cm, -2.0 * housingRadius, 0.0 * cm );

	new G4PVPlacement( rot1, placement, m_ModuleTotal_LogicalVolumes[0], "SingleModule_Volume", m_Gas_LogicalVolumes[0], 0, 9 );

  placement = G4ThreeVector( 0.0, -2.0 * housingRadius, housingRadius + 20.0 * cm  );

	new G4PVPlacement( rot2, placement, m_ModuleTotal_LogicalVolumes[0], "SingleModule_Volume", m_Gas_LogicalVolumes[0], 0, 10 );

  placement = G4ThreeVector( -housingRadius - 20.0 * cm, -2.0 * housingRadius, 0.0 * cm );

	new G4PVPlacement( rot3, placement, m_ModuleTotal_LogicalVolumes[0], "SingleModule_Volume", m_Gas_LogicalVolumes[0], 0, 11 );


}

// ************************************************************************************

void LENSLongLiquidScintillator::makeSingleEightLayer()
{
	G4RotationMatrix* rot1 = new G4RotationMatrix();//(matrixColumn1, matrixColumn2, matrixColumn3);
	rot1->rotateY( 90 * degree );
	m_Rotations.push_back(rot1);// automatically deletes it later...

	G4RotationMatrix* rot2 = new G4RotationMatrix();//(matrixColumn1, matrixColumn2, matrixColumn3);
	rot2->rotateY( 180 * degree );
	m_Rotations.push_back(rot2);// automatically deletes it later...

	G4RotationMatrix* rot3 = new G4RotationMatrix();//(matrixColumn1, matrixColumn2, matrixColumn3);
	rot3->rotateY( 270 * degree );
	m_Rotations.push_back(rot3);// automatically deletes it later...
  
  // Make INTO MEMBER VARIABLES!
  const G4double cuThick = 0.50 * mm;
//  const G4double geDeadLayerThick = 0.0050 * mm;
  const G4double vacuumThick = 1.0 * cm;
//  const G4double vacuumFrontThick = 1.0 * cm;
  const G4double alThick = 0.5 * mm;
//  const G4double alFrontThick = 0.6 * mm;// Used fro Carbon fiber front thickness.

  const G4double housingRadius = 0.5 * m_SodiumIodideDimensions[0][0] + cuThick + vacuumThick + alThick;

//	G4cout << "Number of Total Module Logical Volumes is: " << m_ModuleTotal_LogicalVolumes.size() << G4endl;

	G4ThreeVector placement( -housingRadius, 0.0, -2.0 * housingRadius - 20.0 * cm );// 20 cm is half the length of the al tube
	new G4PVPlacement( 0, placement, m_ModuleTotal_LogicalVolumes[0], "SingleModule_Volume", m_Gas_LogicalVolumes[0], 0, 0 );

  placement = G4ThreeVector( housingRadius, 0.0, -2.0 * housingRadius - 20.0 * cm );
	new G4PVPlacement( 0, placement, m_ModuleTotal_LogicalVolumes[0], "SingleModule_Volume", m_Gas_LogicalVolumes[0], 0, 1 );
  
  placement = G4ThreeVector( 2.0 * housingRadius + 20.0 * cm, 0.0 * cm, -housingRadius );
	new G4PVPlacement( rot1, placement, m_ModuleTotal_LogicalVolumes[0], "SingleModule_Volume", m_Gas_LogicalVolumes[0], 0, 2 );

  placement = G4ThreeVector( 2.0 * housingRadius + 20.0 * cm, 0.0 * cm, housingRadius );
	new G4PVPlacement( rot1, placement, m_ModuleTotal_LogicalVolumes[0], "SingleModule_Volume", m_Gas_LogicalVolumes[0], 0, 3 );

  placement = G4ThreeVector( housingRadius, 0.0, 2.0 * housingRadius + 20.0 * cm  );
	new G4PVPlacement( rot2, placement, m_ModuleTotal_LogicalVolumes[0], "SingleModule_Volume", m_Gas_LogicalVolumes[0], 0, 4 );

  placement = G4ThreeVector( -housingRadius, 0.0, 2.0 * housingRadius + 20.0 * cm  );
	new G4PVPlacement( rot2, placement, m_ModuleTotal_LogicalVolumes[0], "SingleModule_Volume", m_Gas_LogicalVolumes[0], 0, 5 );

  placement = G4ThreeVector( -2.0 * housingRadius - 20.0 * cm, 0.0 * cm, housingRadius );
	new G4PVPlacement( rot3, placement, m_ModuleTotal_LogicalVolumes[0], "SingleModule_Volume", m_Gas_LogicalVolumes[0], 0, 6 );

  placement = G4ThreeVector( -2.0 * housingRadius - 20.0 * cm, 0.0 * cm, -housingRadius );
	new G4PVPlacement( rot3, placement, m_ModuleTotal_LogicalVolumes[0], "SingleModule_Volume", m_Gas_LogicalVolumes[0], 0, 7 );


}

// ************************************************************************************

void LENSLongLiquidScintillator::makeTwoEightLayer()
{
	G4RotationMatrix* rot1 = new G4RotationMatrix();//(matrixColumn1, matrixColumn2, matrixColumn3);
	rot1->rotateY( 90 * degree );
	m_Rotations.push_back(rot1);// automatically deletes it later...

	G4RotationMatrix* rot2 = new G4RotationMatrix();//(matrixColumn1, matrixColumn2, matrixColumn3);
	rot2->rotateY( 180 * degree );
	m_Rotations.push_back(rot2);// automatically deletes it later...

	G4RotationMatrix* rot3 = new G4RotationMatrix();//(matrixColumn1, matrixColumn2, matrixColumn3);
	rot3->rotateY( 270 * degree );
	m_Rotations.push_back(rot3);// automatically deletes it later...
  
  // Make INTO MEMBER VARIABLES!
  const G4double cuThick = 0.50 * mm;
//  const G4double geDeadLayerThick = 0.0050 * mm;
  const G4double vacuumThick = 1.0 * cm;
//  const G4double vacuumFrontThick = 1.0 * cm;
  const G4double alThick = 0.5 * mm;
//  const G4double alFrontThick = 0.6 * mm;// Used fro Carbon fiber front thickness.

  const G4double housingRadius = 0.5 * m_SodiumIodideDimensions[0][0] + cuThick + vacuumThick + alThick;

//	G4cout << "Number of Total Module Logical Volumes is: " << m_ModuleTotal_LogicalVolumes.size() << G4endl;

	G4ThreeVector placement( -housingRadius, housingRadius, -2.0 * housingRadius - 20.0 * cm );// 20 cm is half the length of the al tube
	new G4PVPlacement( 0, placement, m_ModuleTotal_LogicalVolumes[0], "SingleModule_Volume", m_Gas_LogicalVolumes[0], 0, 0 );

  placement = G4ThreeVector( housingRadius, housingRadius, -2.0 * housingRadius - 20.0 * cm );
	new G4PVPlacement( 0, placement, m_ModuleTotal_LogicalVolumes[0], "SingleModule_Volume", m_Gas_LogicalVolumes[0], 0, 1 );
  
  placement = G4ThreeVector( 2.0 * housingRadius + 20.0 * cm, housingRadius, -housingRadius );
	new G4PVPlacement( rot1, placement, m_ModuleTotal_LogicalVolumes[0], "SingleModule_Volume", m_Gas_LogicalVolumes[0], 0, 2 );

  placement = G4ThreeVector( 2.0 * housingRadius + 20.0 * cm, housingRadius, housingRadius );
	new G4PVPlacement( rot1, placement, m_ModuleTotal_LogicalVolumes[0], "SingleModule_Volume", m_Gas_LogicalVolumes[0], 0, 3 );

  placement = G4ThreeVector( housingRadius, housingRadius, 2.0 * housingRadius + 20.0 * cm  );
	new G4PVPlacement( rot2, placement, m_ModuleTotal_LogicalVolumes[0], "SingleModule_Volume", m_Gas_LogicalVolumes[0], 0, 4 );

  placement = G4ThreeVector( -housingRadius, housingRadius, 2.0 * housingRadius + 20.0 * cm  );
	new G4PVPlacement( rot2, placement, m_ModuleTotal_LogicalVolumes[0], "SingleModule_Volume", m_Gas_LogicalVolumes[0], 0, 5 );

  placement = G4ThreeVector( -2.0 * housingRadius - 20.0 * cm, housingRadius, housingRadius );
	new G4PVPlacement( rot3, placement, m_ModuleTotal_LogicalVolumes[0], "SingleModule_Volume", m_Gas_LogicalVolumes[0], 0, 6 );

  placement = G4ThreeVector( -2.0 * housingRadius - 20.0 * cm, housingRadius, -housingRadius );
	new G4PVPlacement( rot3, placement, m_ModuleTotal_LogicalVolumes[0], "SingleModule_Volume", m_Gas_LogicalVolumes[0], 0, 7 );

// Bottom detectors
	placement = G4ThreeVector( -housingRadius, -housingRadius, -2.0 * housingRadius - 20.0 * cm );// 20 cm is half the length of the al tube
	new G4PVPlacement( 0, placement, m_ModuleTotal_LogicalVolumes[0], "SingleModule_Volume", m_Gas_LogicalVolumes[0], 0, 8 );

  placement = G4ThreeVector( housingRadius, -housingRadius, -2.0 * housingRadius - 20.0 * cm );
	new G4PVPlacement( 0, placement, m_ModuleTotal_LogicalVolumes[0], "SingleModule_Volume", m_Gas_LogicalVolumes[0], 0, 9 );
  
  placement = G4ThreeVector( 2.0 * housingRadius + 20.0 * cm, -housingRadius, -housingRadius );
	new G4PVPlacement( rot1, placement, m_ModuleTotal_LogicalVolumes[0], "SingleModule_Volume", m_Gas_LogicalVolumes[0], 0, 10 );

  placement = G4ThreeVector( 2.0 * housingRadius + 20.0 * cm, -housingRadius, housingRadius );
	new G4PVPlacement( rot1, placement, m_ModuleTotal_LogicalVolumes[0], "SingleModule_Volume", m_Gas_LogicalVolumes[0], 0, 11 );

  placement = G4ThreeVector( housingRadius, -housingRadius, 2.0 * housingRadius + 20.0 * cm  );
	new G4PVPlacement( rot2, placement, m_ModuleTotal_LogicalVolumes[0], "SingleModule_Volume", m_Gas_LogicalVolumes[0], 0, 12 );

  placement = G4ThreeVector( -housingRadius, -housingRadius, 2.0 * housingRadius + 20.0 * cm  );
	new G4PVPlacement( rot2, placement, m_ModuleTotal_LogicalVolumes[0], "SingleModule_Volume", m_Gas_LogicalVolumes[0], 0, 13 );

  placement = G4ThreeVector( -2.0 * housingRadius - 20.0 * cm, -housingRadius, housingRadius );
	new G4PVPlacement( rot3, placement, m_ModuleTotal_LogicalVolumes[0], "SingleModule_Volume", m_Gas_LogicalVolumes[0], 0, 14 );

  placement = G4ThreeVector( -2.0 * housingRadius - 20.0 * cm, -housingRadius, -housingRadius );
	new G4PVPlacement( rot3, placement, m_ModuleTotal_LogicalVolumes[0], "SingleModule_Volume", m_Gas_LogicalVolumes[0], 0, 15 );
}


// ************************************************************************************
// SENSITIVE DETECTOR STUFF

void LENSLongLiquidScintillator::Initialize( G4HCofThisEvent* HCE )
{
	if( ! m_OutputInfo ) return;
// Where is this deleted? By the G4VSensitiveDetector? Hmmm.
//	if( m_HitsCollection != 0 ) delete m_HitsCollection;// See what happens. Double deletion happens.
  m_OpticalPhotonHitsCollection = new LENSLongLiquidScintillatorOpticalPhotonHitsCollection( SensitiveDetectorName, collectionName[0] );

  if( m_OpticalPhotonHitsCollectionID < 0 )
  {
		m_OpticalPhotonHitsCollectionID = G4SDManager::GetSDMpointer()->GetCollectionID( m_OpticalPhotonHitsCollection );
		G4cout << "HITS COLLECTION ID: " << m_OpticalPhotonHitsCollectionID << G4endl;
		G4cout << SensitiveDetectorName << "/" << collectionName[0] << G4endl;
//		G4cout << collectionName[0] << G4endl;
  }

  HCE->AddHitsCollection( m_OpticalPhotonHitsCollectionID, m_OpticalPhotonHitsCollection );

  m_GammaHitsCollection = new LENSLongLiquidScintillatorGammaHitsCollection( SensitiveDetectorName, collectionName[1] );
  if( m_GammaHitsCollectionID < 0 )
  { 
	m_GammaHitsCollectionID = G4SDManager::GetSDMpointer()->GetCollectionID( m_GammaHitsCollection );
	G4cout << "HITS COLLECTION ID: " << m_GammaHitsCollectionID << G4endl;
	G4cout << SensitiveDetectorName << "/" << collectionName[1] << G4endl;
//	G4cout << collectionName[1] << G4endl;
  }

  HCE->AddHitsCollection( m_GammaHitsCollectionID, m_GammaHitsCollection );
}


G4bool LENSLongLiquidScintillator::ProcessHits( G4Step* aStep, G4TouchableHistory* /*ROhist*/)
{
	if( ! m_OutputInfo ) return true;
//  G4double edep = aStep->GetTotalEnergyDeposit();
//  if( edep == 0. ) return true;// Nothing happend so nothing to process. Not true for optical photons
//  G4cout << aStep->GetTrack()->GetDynamicParticle()->GetDefinition()->GetParticleName() << G4endl;
  const G4String particleName = aStep->GetTrack()->GetDynamicParticle()->GetDefinition()->GetParticleName();
//  if( "mu+" != particleName ) return true;
//  if( "neutron" != particleName ) return true;
//  if( "gamma" == particleName ) ...

	if( "opticalphoton" == particleName )
	{
//		G4ThreeVector momentumDirection = aStep->GetTrack()->GetDynamicParticle()->GetMomentumDirection();
// At these angles this is the fraction of photons eliminated; 
// But these are the photons trapped in the tube for  long times, processing time is sped up by roughly 20 times. Yay.
//		const G4double cutoffFraction = 0.02;// 1/2 a degree
//		const G4double cutoffFraction = 0.0;// 0 degrees, NO CUTOFF
//		if( abs( momentumDirection[2] ) > cutoffFraction ) aStep->GetTrack()->SetTrackStatus(fStopAndKill);

		return false;// Handled in ProcessOpticalPhotonStepHits(...) called in SteppingAction.hh
  }

	if( false ) // BE CAREFULL LOTS OF STUFF CAN COME OUT...
	{
		const G4VProcess* maker = aStep->GetTrack()->GetCreatorProcess();
		G4String empty = " the primary process";
		const G4String& makerName = maker != 0 ? maker->GetProcessName() : empty;
		
		const G4VProcess* postStepProcess = aStep->GetPostStepPoint()->GetProcessDefinedStep();
		const G4String& output = postStepProcess != 0 ? postStepProcess->GetProcessName(): "User defined Step";
		
		G4cout << "This " << particleName << " is made by " << makerName << " and deposits "
		<< aStep->GetTotalEnergyDeposit()/keV << " keV, and loses " << -aStep->GetDeltaEnergy()/keV << " keV" << G4endl;
		G4cout << "This step ends with process: " << output << G4endl;
	}
	
//#define NUETRONCAPTUREIGNORE 1 

#ifdef NUETRONCAPTUREIGNORE
	if( particleName == "127I[0.0]" || particleName == "Na23[0.0]" )
	{
		return true;// No Sodium or Iodine energy deposit detection.
	}
//	if( particleName == "127I[0.0]" && aStep->GetTotalEnergyDeposit() < 25.0 * keV ) return true;// No Iodine energy deposit detection.
//	if( particleName == "Na23[0.0]" && aStep->GetTotalEnergyDeposit() < 25.0 * keV  ) return true;// No Sodium energy deposit detection.
//	if( aStep->GetTotalEnergyDeposit()/keV < 1.0 ) bad idea
//	if( particleName == "proton" ) aStep->GetTrack()->SetTrackStatus(fStopAndKill);
#endif

//	const G4int parentID = aStep->GetTrack()->GetParentID();
//	const G4VProcess* creatorProcess = aStep->GetTrack()->GetCreatorProcess();
//	if( creatorProcess == 0 )
//	if( "gamma" == particleName && parentID == 0 )
//	if( false )
	{ 
		G4StepPoint* preStepPoint = aStep->GetPreStepPoint();
		G4TouchableHistory* thePreTouchable = (G4TouchableHistory*)( preStepPoint->GetTouchable() );
		
		G4int moduleIndex = GetModuleIndex( thePreTouchable );
//		G4cout << "ModuleIndex: " << moduleIndex << G4endl;
		
		G4double edep = aStep->GetTotalEnergyDeposit();
//		if( edep == 0.0 ) return true;
//		G4double edep = -aStep->GetDeltaEnergy();
//		if( edep == 0. ) return true;// Nothing happend so nothing to process.

// These are commented out for speed purposes...
		const G4double currentEnergy = aStep->GetPreStepPoint()->GetKineticEnergy();
		const G4double time = preStepPoint->GetGlobalTime();//aStep->GetPostStepPoint()->GetGlobalTime() - preStepPoint->GetGlobalTime();
		const G4double dist = 0.0;//aStep->GetStepLength();
		G4int processID = -1;
		
	//	if( "e-" == particleName )
//		{
//			const G4VProcess* postStepPointProcess = aStep->GetPostStepPoint()->GetProcessDefinedStep();
//			const G4String& outputProcessName = postStepPointProcess != 0 ? postStepPointProcess->GetProcessName(): "User defined Step";
//			// Low energy stuff is no longer used...
//			if( "eIoni" == outputProcessName || "LowEnergyIoni" == outputProcessName )
//			{
//				processID = 0;
//			}
//			else if( "eBrem" == outputProcessName || "LowEnBrem" == outputProcessName )
//			{
//				processID = 1;
//			}
//			else if( "phot" == outputProcessName || "LowEnPhotoElec" == outputProcessName )
//			{
//				processID = 2;
//			}
//			else if( "compt" == outputProcessName || "LowEnCompton" == outputProcessName )
//			{
//				processID = 3;
//			}
//			else if( "msc" == outputProcessName )
//			{
//				processID = 4;
//			}
//			else if( "annihil" == outputProcessName )
//			{
//				processID = 5;
//			}
//			else if( "Transportation" == outputProcessName )
//			{
//				processID = 6;
//			}
//			else if( "conv" == outputProcessName || "LowEnConversion" == outputProcessName)
//			{
//				processID = 7;
//			}
//			else if( "Rayl" == outputProcessName )
//			{
//				processID = 8;
//			}
//			else
//			{
//				processID = 9;
//				G4cout << outputProcessName << G4endl;
//			}
//			
//			G4cout << particleName << ", " << outputProcessName <<  ", " << edep / keV <<  ", " << dist / nm <<  ", " << edep / keV / dist * nm  
//			<< ", " << time / ns * 1000000.0 << ", " << aStep->GetTrack()->GetCurrentStepNumber() << ", " <<  aStep->GetTrack()->GetTrackID() 
//			<< ", " <<  aStep->GetTrack()->GetVertexPosition() / nm << G4endl;
//		}
		

//		G4StepPoint* postStepPoint = aStep->GetPostStepPoint();
//		G4TouchableHistory* thePostTouchable = (G4TouchableHistory*) ( postStepPoint->GetTouchable() );
		G4VPhysicalVolume* startVolume = thePreTouchable->GetVolume();
		G4LogicalVolume* logicalVolume = startVolume->GetLogicalVolume();
		G4String logicalVolumeName = logicalVolume->GetName();
		
//		const G4int postCopyNo = thePostTouchable->GetVolume()->GetCopyNo();
//		const G4double hitTime = postStepPoint->GetGlobalTime();

		LENSLongLiquidScintillatorGammaHit* aHit = new LENSLongLiquidScintillatorGammaHit( moduleIndex, currentEnergy, edep, time, dist, processID);
		G4VPhysicalVolume* thePrePhysical = thePreTouchable->GetVolume();
		aHit->SetLogV( thePrePhysical->GetLogicalVolume() );
		m_GammaHitsCollection->insert( aHit );

	}

  return true;
}

G4bool LENSLongLiquidScintillator::ProcessOpticalPhotonStepHits( const G4Step* aStep, G4TouchableHistory* /*ROhist*/)
{	// This function assumes the it is a PMT volume that is hit. Nothing else absorbs optical photon and this function 
	//is called only when that process occurs.
	if( ! m_OutputInfo ) return true;

//	const G4double edep = aStep->GetTotalEnergyDeposit();
	G4String particleName = aStep->GetTrack()->GetDynamicParticle()->GetDefinition()->GetParticleName();
//  if( "mu+" != particleName ) return true;
//  if( "neutron" != particleName ) return true;
//  if( "gamma" == particleName ) ...
//	G4cout << "Particle Name: " << particleName << G4endl;

	if( "opticalphoton" != particleName ) return true;
	
  G4Track* theTrack = aStep->GetTrack();
	G4ThreeVector momentumDir = theTrack->GetDynamicParticle()->GetMomentumDirection();
	if( fabs( momentumDir[2] ) < M_SQRT1_2 )// angle greater than 45 degrees from normal
	{
		G4double angle = acos( fabs( momentumDir[2] ) );
		G4double random = G4UniformRand();
		if( random > -4.0 * angle / M_PI + 2.0 ) return true;// Reject the counting...
	}
	
	G4StepPoint* postStepPoint = aStep->GetPostStepPoint();
	G4TouchableHistory* thePostTouchable = (G4TouchableHistory*) ( postStepPoint->GetTouchable() );

// Check for PMT Volume
//  G4VPhysicalVolume* physVol = thePostTouchable->GetVolume(0);
// I have no idea what the 0 means. It means get the current physical volume's index, a 1 would mean its parent's index, a 2 its parent's 
//parent's index, etc...
//  G4int pmtIndex = thePostTouchable->GetReplicaNumber(0);
  G4int pmtIndex = thePostTouchable->GetVolume()->GetCopyNo();

  G4StepPoint* preStepPoint = aStep->GetPreStepPoint();
  G4TouchableHistory* thePreTouchable = (G4TouchableHistory*)( preStepPoint->GetTouchable() );
//  G4int copyNo = thePreTouchable->GetVolume()->GetCopyNo();
  G4double hitTime = postStepPoint->GetGlobalTime();
	G4double eDep = aStep->GetTotalEnergyDeposit();
	
	G4int moduleIndex = GetModuleIndex( thePreTouchable );
	
	const G4int numberPMTsHit = m_OpticalPhotonHitsCollection->entries();
	int i = 0;
	G4bool done = ( numberPMTsHit == 0 );
	while( ! done )
	{
		if( ! ( moduleIndex == (*m_OpticalPhotonHitsCollection)[i]->GetModuleIndex() )  )
		{
			i++;
			if( i == numberPMTsHit ) done = true;
		}
		else
		{
			if( (*m_OpticalPhotonHitsCollection)[i]->GetPMTIndex() == pmtIndex )
			{
				(*m_OpticalPhotonHitsCollection)[i]->AddHit( eDep, hitTime );
				done = true;
			}
			else
			{
				i++;
				if( i == numberPMTsHit ) done = true;
			}
		}
	}
	
	if( i == numberPMTsHit )
	{
		LENSLongLiquidScintillatorOpticalPhotonHit* aHit = new LENSLongLiquidScintillatorOpticalPhotonHit( moduleIndex, pmtIndex, eDep, hitTime );
		G4VPhysicalVolume* thePhysical = thePostTouchable->GetVolume();// This is the volume that absorbs the photon, happens in stepping function.
		aHit->SetLogV( thePhysical->GetLogicalVolume() );

		m_OpticalPhotonHitsCollection->insert( aHit );
	}
	
  return true;
}


void LENSLongLiquidScintillator::EndOfEvent( G4HCofThisEvent* HCE )
{
//	printf("HELLO");
}


// G4TouchableHistory* thePostTouchable is the physical volume where the particle crosses into and hence is absorbed by
const G4int LENSLongLiquidScintillator::GetModuleIndex( G4TouchableHistory* thePostTouchable ) const
{
//  return 0;
  
//	if( m_SingleModuleOnly ) return 0;
	
//	if( m_Si_LogicalVolumes.size() > 0 && ( m_Si_LogicalVolumes[0] == thePostTouchable->GetVolume()->GetLogicalVolume() ||
//                                         m_Si_LogicalVolumes[1] == thePostTouchable->GetVolume()->GetLogicalVolume() ) )
//	{
//		G4cout << "Hit Silicon! ID#: " << thePostTouchable->GetVolume()->GetCopyNo() << G4endl;		
//		return thePostTouchable->GetVolume()->GetCopyNo();
//	}
	
	G4int copyIndex = 0;
	G4int parentVolumeIndex = 0;

//  const G4int crystalIndex = thePostTouchable->GetVolume()->GetCopyNo(); //Use if no dead layer
  const G4int crystalIndex = thePostTouchable->GetVolume(1)->GetCopyNo(); // if parent contains the copy number info: aka dead layer
//  G4cout << "crystalIndex: " << crystalIndex << G4endl;//
  
	G4VPhysicalVolume* currentPhysVol = thePostTouchable->GetVolume( parentVolumeIndex );
	
//  if( currentPhysVol->GetLogicalVolume() == m_NaI_LogicalVolumes[4] )
  if( currentPhysVol->GetCopyNo() > 3 )
  {
//    G4cout << "Backcatcher Hit! " << G4endl;
    return currentPhysVol->GetCopyNo();
  }
  
	G4bool done = false;
	const G4int numberModuleTypes = m_ModuleTotal_LogicalVolumes.size();
	G4int i = 0;
	for( i = 0; i < numberModuleTypes; ++i )
	{
		done |= currentPhysVol->GetLogicalVolume() == m_ModuleTotal_LogicalVolumes[i] && currentPhysVol->GetMotherLogical() != 0;
	}
	
//  G4cout << "done: " << (bool) done << G4endl;

	while( ! done )
	{
		parentVolumeIndex++;
		currentPhysVol = thePostTouchable->GetVolume( parentVolumeIndex );
		for( i = 0; i < numberModuleTypes; ++i )
		{
			done |= currentPhysVol->GetLogicalVolume() == m_ModuleTotal_LogicalVolumes[i] && currentPhysVol->GetMotherLogical() != 0;
		}
	}
	
	copyIndex = currentPhysVol->GetCopyNo();// Get Module copy number
//  G4cout << "copyIndex: " << copyIndex << G4endl;
	
//  G4cout << "Module Index: " << copyIndex << G4endl;
  
	return ( 4 * copyIndex + crystalIndex );
}



